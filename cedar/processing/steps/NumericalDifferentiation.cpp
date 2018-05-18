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

    File:        NumericalDifferentiation.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        

    Description: Source file for the class cedar::proc::steps::NumericalDifferentiation.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/NumericalDifferentiation.h"

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
    new ElementDeclarationTemplate<cedar::proc::steps::NumericalDifferentiation>
    (
      "Numerics",
      "cedar.processing.steps.NumericalDifferentiation"
    )
  );

  declaration->setIconPath(":/steps/numerical_differentiation.svg");
  declaration->setDescription
  (
    "Differentiates the input numerically over time, element-wise. "
    "Uses five point finite backwards difference (using the four past entries and the current one). The delay input is optional and can be used to override the internal estimate of 'dt' which comes from the global clock."
  );

  declaration->declare();

  return true;
}

bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::NumericalDifferentiation::NumericalDifferentiation()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat())),
mOneBack(),
mTwoBack(),
mThreeBack(),
mFourBack()
{
  // declare all data
  cedar::proc::DataSlotPtr input = this->declareInput("input");
  cedar::proc::DataSlotPtr input2 = this->declareInput("delay (optional)", false); // optional

  this->declareOutput("differentiated result", mOutput);

  input->setCheck(cedar::proc::typecheck::IsMatrix());

  mLastTime= cedar::aux::GlobalClockSingleton::getInstance()->getTime();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::NumericalDifferentiation::inputConnectionChanged(const std::string& inputName)
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
    this->mOutput->setData(cv::Mat());
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
    mOneBack= cv::Mat();
    mTwoBack= cv::Mat();
    mThreeBack= cv::Mat();
    mFourBack= cv::Mat();

    this->emitOutputPropertiesChangedSignal("differentiated result");
  }
}

void cedar::proc::steps::NumericalDifferentiation::compute(const cedar::proc::Arguments&)
{
  this->recompute();
}

void cedar::proc::steps::NumericalDifferentiation::reset()
{
  mOneBack= cv::Mat();
  mTwoBack= cv::Mat();
  mThreeBack= cv::Mat();
  mFourBack= cv::Mat();
}

void cedar::proc::steps::NumericalDifferentiation::recompute()
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
  mOneBack= input_mat.clone();

  cedar::unit::Time newtime = cedar::aux::GlobalClockSingleton::getInstance()->getTime();

  double dt;
  boost::shared_ptr< cedar::aux::ConstMatData > delayinput_data;
  cv::Mat delayinput_mat;

  if (!delayinput
      || !(delayinput_data= boost::dynamic_pointer_cast<const cedar::aux::MatData>(delayinput) )
      || (delayinput_mat= delayinput_data->getData()).empty() )
  {
    dt= (newtime - mLastTime) / boost::units::si::second;
  }
  else
  {
    dt= delayinput_mat.at<float>(0,0);
  }

//std::cout << " dt: " << dt << std::endl;  
  mLastTime= newtime;

  if (mOneBack.empty())
  {
    // paranoid?
    if (!out_mat.empty())
    {
      out_mat= cv::Mat::zeros( out_mat.rows, out_mat.cols, CV_32F );
    }
    //out_mat= cv::Mat();
  }
  else if (mTwoBack.empty())
  {
    out_mat= cv::Mat::zeros( mOneBack.rows, mTwoBack.cols, CV_32F );
  }
  else if (mThreeBack.empty())
  {
    // Euler
    //out_mat= - ( mTwoBack - mOneBack ) / dt;
    out_mat= cv::Mat::zeros( mOneBack.rows, mTwoBack.cols, CV_32F );
  }
  else if (mFourBack.empty())
  {
    // Euler
    //out_mat= - ( mTwoBack - mOneBack ) / dt;
    out_mat= cv::Mat::zeros( mOneBack.rows, mTwoBack.cols, CV_32F );
  }
  else
  {
 std::cout << " " <<  ( mTwoBack.at<float>(0,0) - mOneBack.at<float>(0,0) ) << " dt: " << dt << "  div: " << ( mTwoBack.at<float>(0,0) - mOneBack.at<float>(0,0) ) / dt << std::endl;

    // note: mOneBack is the current input, etc. ...
    out_mat = -1.0/(12.0*dt) 
      * ( -25.0 * mOneBack
          + 48.0 * mTwoBack
          - 36.0 * mThreeBack
          + 16.0 * mFourBack
          - 3.0 * five_back );
  }


  this->mOutput->setData(out_mat);
  //this->mOutput->cloneAnnotationsFrom(this->mInput);
}

