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
    "Differentiates the input numerically over time, element-wise.\n"
    "Uses Euler Step OR five point Finite Backwards Difference (using the four past entries and the current one), selectable via parameter.\n"
    "The delay input is optional and can be used to override the internal estimate of 'dt', the time-step, which by default comes from the global clock. This will be relevant for scenarios where architectures are large but require exact results.\n"
    "To avoid big jumps in the output in corner cases, adjust the parameters 'ignore time steps smaller than' and 'ignore time steps larger than', which will "
    "prompt resetting of the algorithm in such cases."
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
cedar::proc::Step(true),
// outputs
mOutput(new cedar::aux::MatData(cv::Mat())),
mOneBack(),
mTwoBack(),
mThreeBack(),
mFourBack(),
mUseFivePoints(new cedar::aux::BoolParameter(this, "use five points", false)),
mIgnoreTimeStepsSmallerThan(new cedar::aux::DoubleParameter(this, "ignore time steps smaller than", 0.002, 0.0, 10000.0)),
mIgnoreTimeStepsLargerThan(new cedar::aux::DoubleParameter(this, "ignore time steps larger than", 1.0, 0.0, 10000.0))
{
  // declare all data
  cedar::proc::DataSlotPtr input = this->declareInput("input");
  cedar::proc::DataSlotPtr input2 = this->declareInput("delay (optional)", false); // optional

  this->declareOutput("differentiated result", mOutput);
  mIgnoreTimeStepsSmallerThan->markAdvanced(true);
  mIgnoreTimeStepsLargerThan->markAdvanced(true);

  input->setCheck(cedar::proc::typecheck::IsMatrix());

  mLastTime= cedar::aux::GlobalClockSingleton::getInstance()->getTime();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::NumericalDifferentiation::inputConnectionChanged(const std::string& inputName)
{
//std::cout << "  inputConnectionChanged" << std::endl;
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
    //this->mOutput->setData(cv::Mat());
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
    this->recompute(true);
  }

  if (output_changed)
  {
    reinitialize();

    this->emitOutputPropertiesChangedSignal("differentiated result");
  }
}

void cedar::proc::steps::NumericalDifferentiation::reinitialize()
{
//std::cout << "  reinitialize" << std::endl;
  mOneBack= cv::Mat();
  mTwoBack= cv::Mat();

  if (mUseFivePoints->getValue())
  {
    mThreeBack= cv::Mat();
    mFourBack= cv::Mat();
  }

  auto input = getInput("input");
  auto delayinput= getInput("delay (optional)");

  if (!input)
    return;

  auto data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input);

  if (!data)
    return;

  cv::Mat input_mat = data->getData();
  if (input_mat.empty())
    return;

//std::cout << "  setting output to zero" << std::endl;
  cv::Mat out_mat= cv::Mat::zeros( input_mat.rows, input_mat.cols, CV_32F );
  this->mOutput->setData(out_mat);
}

void cedar::proc::steps::NumericalDifferentiation::compute(const cedar::proc::Arguments&)
{
//std::cout << "  compute" << std::endl;
  this->recompute(false);
}

void cedar::proc::steps::NumericalDifferentiation::reset()
{
//std::cout << "\n\n\n  RESET: " << std::endl;
  reinitialize();
}

void cedar::proc::steps::NumericalDifferentiation::recompute(bool force_reinit)
{
//std::cout << "  recompute" << std::endl;
  auto input = getInput("input");
  auto delayinput= getInput("delay (optional)");

  if (!input)
    return;

  auto data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input);

  if (!data)
    return;

  cv::Mat input_mat = data->getData();

  cv::Mat out_mat;

  cedar::unit::Time newtime = cedar::aux::GlobalClockSingleton::getInstance()->getTime();

  double dt;
  boost::shared_ptr< cedar::aux::ConstMatData > delayinput_data;
  cv::Mat delayinput_mat;

  if (!delayinput
      || !(delayinput_data= boost::dynamic_pointer_cast<const cedar::aux::MatData>(delayinput) )
      || (delayinput_mat= delayinput_data->getData()).empty() )
  {
    if (cedar::aux::math::isZero(mLastTime / boost::units::si::second)
        || mOneBack.empty())
    {
//std::cout << " setting dt to zero" << std::endl;    
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

//std::cout << " dt: " << dt << std::endl;  
  mLastTime= newtime;

  if (force_reinit
      || dt < 0.0)
  {
//std::cout << "  force reinitialize ... " << std::endl;
    // architecture reset
    reinitialize(); // reinit here so to avoid spikes when mFourBack is not cleared
    return; // ignore here, will treat in reset()
  }
//std::cout << "  dt since last: " << dt << std::endl;

  // this avoids big numerical errors:
  //   avoid dividing by very small numbers!
  //   avoid making big numerial errors after pausing the architecture or
  //     lagging
  if (cedar::aux::math::isZero(dt)
      || dt < mIgnoreTimeStepsSmallerThan->getValue() )

  {
    mOneBack= input_mat.clone();
    return;
  }
  else if (dt > mIgnoreTimeStepsLargerThan->getValue() ) 
  {
    mOneBack= input_mat.clone();
    return;
  }

  cv::Mat five_back;

  five_back= mFourBack;
  mFourBack= mThreeBack;
  mThreeBack= mTwoBack;
  mTwoBack= mOneBack;
  mOneBack= input_mat.clone();

  if (mUseFivePoints->getValue())
  {
    if (mOneBack.empty())
    {
//std::cout << " A" << std::endl;    
      // paranoid?
      if (!out_mat.empty())
      {
        out_mat= cv::Mat::zeros( out_mat.rows, out_mat.cols, CV_32F );
      }
      //out_mat= cv::Mat();
    }
    else if (mTwoBack.empty())
    {
//std::cout << " B" << std::endl;    
      out_mat= cv::Mat::zeros( mOneBack.rows, mTwoBack.cols, CV_32F );
    }
    else if (mThreeBack.empty())
    {
//std::cout << " C" << std::endl;    
      // fall back to Euler
      out_mat= - ( mTwoBack - mOneBack ) / dt;
//std::cout << "      " << mTwoBack << " " << mOneBack << " dt: " << dt << std::endl;
      out_mat= cv::Mat::zeros( mOneBack.rows, mTwoBack.cols, CV_32F );
    }
    else if (mFourBack.empty())
    {
//std::cout << " D" << std::endl;   
//std::cout << "      " << mTwoBack << " " << mOneBack << " dt: " << dt << std::endl;
      // Euler
      out_mat= - ( mTwoBack - mOneBack ) / dt;
      out_mat= cv::Mat::zeros( mOneBack.rows, mTwoBack.cols, CV_32F );
    }
    else
    {
//std::cout << " E" << std::endl;    
//std::cout << "   " << mOneBack << " two: " << mTwoBack << " three: " << mThreeBack << "  four: " << mFourBack  << " five: " << five_back << std::endl;
      // use finite difference 5th order
  // std::cout << " " <<  ( mTwoBack.at<float>(0,0) - mOneBack.at<float>(0,0) ) << " dt: " << dt << "  div: " << ( mTwoBack.at<float>(0,0) - mOneBack.at<float>(0,0) ) / dt << std::endl;

      // note: mOneBack is the current input, etc. ...
      out_mat = -1.0/(12.0*dt) 
        * ( -25.0 * mOneBack
            + 48.0 * mTwoBack
            - 36.0 * mThreeBack
            + 16.0 * mFourBack
            - 3.0 * five_back );
    }
  }
  else
  {
    // use Euler:
//std::cout << "     using Euler ... " << std::endl;
    if (mOneBack.empty())
    {
//std::cout << "         A " << std::endl;      
      // paranoid?
      if (!out_mat.empty())
      {
//std::cout << "         a " << std::endl;      
        out_mat= cv::Mat::zeros( out_mat.rows, out_mat.cols, CV_32F );
      }
      //out_mat= cv::Mat();
    }
    else if (mTwoBack.empty())
    {
//std::cout << "         B " << std::endl;      
      out_mat= cv::Mat::zeros( mOneBack.rows, mTwoBack.cols, CV_32F );
    }
    else
    {
//std::cout << "         C " << std::endl;      
      // use Euler
      out_mat= - ( mTwoBack - mOneBack ) / dt;
//std::cout << "      " << mTwoBack <<  " , " << mOneBack << " dt: " << dt;      
    }
  }

  this->mOutput->setData(out_mat);
  //this->mOutput->cloneAnnotationsFrom(this->mInput);
}

