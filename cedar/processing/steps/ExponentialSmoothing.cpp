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

    File:        ExponentialSmoothing.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        

    Description: Source file for the class cedar::proc::steps::ExponentialSmoothing.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/ExponentialSmoothing.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/IsMatrix.h"
#include <cedar/processing/ElementDeclaration.h>

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
    new ElementDeclarationTemplate<cedar::proc::steps::ExponentialSmoothing>
    (
      "Numerics",
      "cedar.processing.steps.ExponentialSmoothing"
    )
  );

  declaration->setIconPath(":/steps/exponential_smoothing.svg");
  declaration->setDescription
  (
    "Implements element-wise exponential smoothing (over time) and "
    "thus smoothes your incoming inputs. Holt (i.e. double exponential) method. TODO: implement optional triple or single exponential methods."
  );

  declaration->declare();

  return true;
}

bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ExponentialSmoothing::ExponentialSmoothing()
:
cedar::proc::Step(true),
// outputs
mOutput(new cedar::aux::MatData(cv::Mat())),
mDataEstimate(),
mTrendEstimate(),
mDataSmoothingFactor(new cedar::aux::DoubleParameter(this, "data smoothing factor", 0.1, 0.0, 1.0)),
mTrendSmoothingFactor(new cedar::aux::DoubleParameter(this, "trend smoothing factor", 0.1, 0.0, 1.0))
{
  // declare all data
  cedar::proc::DataSlotPtr input = this->declareInput("input");
  this->declareOutput("forecast", mOutput);

  input->setCheck(cedar::proc::typecheck::IsMatrix());
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::ExponentialSmoothing::inputConnectionChanged(const std::string& inputName)
{
  // TODO: you may want to replace this code by using a cedar::proc::InputSlotHelper

  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "input");

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
    this->recompute();
  }

  if (output_changed)
  {
    mDataEstimate= cv::Mat();
    mTrendEstimate= cv::Mat();
    this->emitOutputPropertiesChangedSignal("forecast");
  }
}

void cedar::proc::steps::ExponentialSmoothing::compute(const cedar::proc::Arguments&)
{
  this->recompute();
}

void cedar::proc::steps::ExponentialSmoothing::reset()
{
  mDataEstimate= cv::Mat();
  mTrendEstimate= cv::Mat();
}

void cedar::proc::steps::ExponentialSmoothing::recompute()
{
  auto input = getInput("input");

  if (!input)
    return;

  auto data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input);

  if (!data)
    return;

  cv::Mat input_mat = data->getData();

  cv::Mat out_mat;

  // first time step:
  if (mDataEstimate.empty())
  {
    mDataEstimate= input_mat.clone(); // prepare

    out_mat= input_mat.clone(); // no filtering yet
  }
  else if (mTrendEstimate.empty())
  {
    // prepare:
    mTrendEstimate= input_mat - mDataEstimate;
    mDataEstimate= input_mat.clone();

    out_mat= input_mat.clone(); // no filtering yet
  }
  else
  {
    auto alpha = mDataSmoothingFactor->getValue();
    auto gamma = mTrendSmoothingFactor->getValue();

    // smoothed data:
    cv::Mat last_data_estimate = mDataEstimate.clone();
    cv::Mat new_data_estimate;

    // best trend estimate (i.e. where the data is going ...)
    cv::Mat last_trend_estimate = mTrendEstimate.clone();
    cv::Mat new_trend_estimate;

    // mix of new data and last forecast:
    new_data_estimate= alpha * input_mat
                        + ( 1 - alpha ) 
                          // and the last forecast:
                          * ( last_data_estimate
                             + last_trend_estimate ); 

    // mix of new and last trend estimates:
    new_trend_estimate= gamma * ( new_data_estimate - last_data_estimate )
                        + ( 1 - gamma ) * last_trend_estimate;

    // the new forecast:
    out_mat = new_data_estimate + new_trend_estimate;

    mDataEstimate= new_data_estimate;
    mTrendEstimate= new_trend_estimate;
  }


  this->mOutput->setData(out_mat);
  //this->mOutput->cloneAnnotationsFrom(this->mInput);
}

