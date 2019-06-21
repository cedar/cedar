/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
    This file is part of cedar.

    cedar is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    cedar is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a clone of the GNU Lesser General Public License
    along with cedar. If not, see <http://www.gnu.org/licenses/>.

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        NumericalIntegration.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        

    Description: Source file for the class cedar::proc::steps::NumericalIntegration.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/NumericalIntegration.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/IsMatrix.h"
#include <cedar/processing/ElementDeclaration.h>
#include "cedar/auxiliaries/GlobalClock.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
bool declare()
{
  using cedar::proc::ElementDeclarationPtr;
  using cedar::proc::ElementDeclarationTemplate;

  ElementDeclarationPtr declaration
  (
    new ElementDeclarationTemplate<cedar::proc::steps::NumericalIntegration>
    (
      "Numerics",
      "cedar.processing.steps.NumericalIntegration"
    )
  );

  declaration->setIconPath(":/steps/numerical_integration.svg");
  declaration->setDescription
  (
    "Integrates the input numerically over time, element-wise.\n"
    "Uses Euler Step as a default. "
    "Optionally, you can use a BDF5 (backwards differentiation formula fifth order) method which will yield better results than the Euler step, especially for stiff systems.\n"
    "The initial state input is also optional and will be used when initialzing the state if available. If not, a zero valued tensor of appropriate size will be the initial state. "
    "You can choose to re-initialize the values on an architecture reset via the corresponding parameter.\n"
    "The delay input is optional and can be used to override the internal estimate of 'dt', the time step, which comes from the global clock. (Hint: There is a buffer in the Field stept which will yield the accurate deltaT that was used for that field, since that will differ if architectures get large. So use this to get precice intergration regarding the Field as source of a signal.). "
  );

  declaration->declare();

  return true;
}

bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::NumericalIntegration::NumericalIntegration()
:
cedar::proc::Step(true),
// outputs
mOutput(new cedar::aux::MatData(cv::Mat())),
mOneBack(),
mTwoBack(),
mThreeBack(),
mFourBack(),
mInitializeOnReset(new cedar::aux::BoolParameter(this, "initialize on reset", true)),
mUseBDF5(new cedar::aux::BoolParameter(this, "use BDF5", false))
{
  // declare all data
  cedar::proc::DataSlotPtr input = this->declareInput("input");
  cedar::proc::DataSlotPtr input2 = this->declareInput("initial state (optional)", false); // optional
  cedar::proc::DataSlotPtr input3 = this->declareInput("delay (optional)", false); // optional

  this->declareOutput("integration result", mOutput);

  input->setCheck(cedar::proc::typecheck::IsMatrix());

  mLastTime= cedar::aux::GlobalClockSingleton::getInstance()->getTime();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::steps::NumericalIntegration::reinitialize()
{
  auto initialinput= getInput("initial state (optional)");
  boost::shared_ptr< cedar::aux::ConstMatData > initialinput_data;
  cv::Mat initialinput_mat;

  if (!initialinput
      || !(initialinput_data= boost::dynamic_pointer_cast<const cedar::aux::MatData>(initialinput) )
      || (initialinput_mat= initialinput_data->getData()).empty() )
  {
    int rows, cols;

    rows= 0;
    cols= 0;

    auto input = getInput("input");
    cv::Mat input_mat;
    if (input)
    {
      auto data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input);
      if (data)
      {
        cv::Mat input_mat;
        input_mat = data->getData();
        if (!input_mat.empty())
        {
          rows= input_mat.rows;
          cols= input_mat.cols;
        }
      }
    }

    mLastState= cv::Mat::zeros( rows, cols, CV_32F );
  }
  else
  {
    mLastState= initialinput_mat.clone();
  }

  mOneBack= cv::Mat();
  mTwoBack= cv::Mat();
  mThreeBack= cv::Mat();
  mFourBack= cv::Mat();
}

void cedar::proc::steps::NumericalIntegration::reset()
{
  if (mInitializeOnReset->getValue())
  {
    reinitialize();
  }
}

void cedar::proc::steps::NumericalIntegration::inputConnectionChanged(const std::string& inputName)
{
  // TODO: you may want to replace this code by using a cedar::proc::InputSlotHelper

  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  //CEDAR_DEBUG_ASSERT(inputName == "input");
  if (inputName != "input")
    return; // do nothing for delay

  // Assign the input to the member. This saves us from casting in every computation step.
  this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));

  bool output_changed = false;
  if (!this->mInput)
  {
    // no input -> no output
    // quickfix: this crashes plots: this->mOutput->setData(cv::Mat());
    output_changed = true;
  }
  else
  {
    // Let's get a reference to the input matrix.
    const cv::Mat& input = this->mInput->getData();

    // check if the input is different from the output
    if (input.type() != this->mOutput->getData().type() || input.size != this->mOutput->getData().size)
    {
      output_changed = true;
    }

    // Make a clone to create a matrix of the same type, dimensions, ...
    this->recompute();
  }

  if (output_changed)
  {
    reinitialize();

    this->emitOutputPropertiesChangedSignal("integration result");
  }
}

void cedar::proc::steps::NumericalIntegration::compute(const cedar::proc::Arguments&)
{
  this->recompute();
}

void cedar::proc::steps::NumericalIntegration::recompute()
{
  auto input = getInput("input");
  auto delayinput= getInput("delay (optional)");

  if (!input)
    return;

  auto data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input);

  if (!data)
    return;

  cv::Mat input_mat = data->getData();

  cv::Mat out_mat;

  cv::Mat five_back;

  five_back= mFourBack;
  mFourBack= mThreeBack;
  mThreeBack= mTwoBack;
  mTwoBack= mOneBack;
  mOneBack= mLastState.clone(); // !! not the input !!

  cedar::unit::Time newtime = cedar::aux::GlobalClockSingleton::getInstance()->getTime();

  double dt;
  boost::shared_ptr< cedar::aux::ConstMatData > delayinput_data;
  cv::Mat delayinput_mat;

  if (!delayinput
      || !(delayinput_data= boost::dynamic_pointer_cast<const cedar::aux::MatData>(delayinput) )
      || (delayinput_mat= delayinput_data->getData()).empty() )
  {
    if (mLastTime / boost::units::si::second == 0.0
        || mOneBack.empty())
    {
      dt= 0.0;
    }
    else
    {
      dt= (newtime - mLastTime) / boost::units::si::second;
    }
  }
  else
  {
    dt= delayinput_mat.at<float>(0,0);
  }

  mLastTime= newtime;

  if (dt < 0)
  {
    // architecture reset
    return; // ignore here, will treat in reset()
  }

  if (mUseBDF5->getValue())
  {
    if (mOneBack.empty())
    {
      reinitialize();
      out_mat= mLastState.clone();
    }
    else if (mTwoBack.empty())
    {
      // use Euler:
      out_mat= mLastState
               + dt * mOneBack;
    }
    else if (mThreeBack.empty())
    {
      // use Euler:
      out_mat= mLastState
               + dt * mOneBack;
    }
    else if (mFourBack.empty())
    {
      // use Euler:
      out_mat= mLastState
               + dt * mOneBack;
    }
    else
    {
      // BDF 5
      out_mat= ( 300.0 * mOneBack
                    -300.0 * mTwoBack
                    + 200.0 * mThreeBack
                    -75.0 * mFourBack
                    + 12.0 * five_back
                    + 60.0 * dt * input_mat)
                  / 137.0;
    }
  }
  else
  {
    if (mOneBack.empty())
    {
      reinitialize();
    }
    else
    {
      // use Euler:
      out_mat = mLastState 
                + dt * input_mat;
    }
  }


  if (!out_mat.empty())
  {
    mLastState= out_mat.clone();
  }

  this->mOutput->setData(out_mat);
  //this->mOutput->cloneAnnotationsFrom(this->mInput);
}

