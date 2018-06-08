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

    You should have received a copy of the GNU Lesser General Public License
    along with cedar. If not, see <http://www.gnu.org/licenses/>.

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        LinearLateralShift.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        

    Description: Source file for the class cedar::proc::steps::LinearLateralShift.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/LinearLateralShift.h"

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
    new ElementDeclarationTemplate<cedar::proc::steps::LinearLateralShift>
    (
      "DFT",
      "cedar.processing.steps.LinearLateralShift"
    )
  );

  declaration->setIconPath(":/steps/linear_lateralshift.svg");
  declaration->setDescription
  (
    "Translates a 1D feature space representation by means of a linear function: a + bx"
  );

  declaration->declare();

  return true;
}

bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::LinearLateralShift::LinearLateralShift()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat())),
//mSize(new cedar::aux::UIntParameter(this, "size", 50, 1, 5000)),
//mLowerLimit(new cedar::aux::DoubleParameter(this, "lower limit")),
//mUpperLimit(new cedar::aux::DoubleParameter(this, "upper limit")),
mA(new cedar::aux::DoubleParameter(this, "offset a", 0.0)),
mB(new cedar::aux::DoubleParameter(this, "factor b", 1.0))
{
  // declare all data
  cedar::proc::DataSlotPtr input = this->declareInput("input");
  this->declareOutput("output", mOutput);

  input->setCheck(cedar::proc::typecheck::IsMatrix());

  QObject::connect(mA.get(), SIGNAL(valueChanged()), this, SLOT(parametersChanged()), Qt::DirectConnection);
  QObject::connect(mB.get(), SIGNAL(valueChanged()), this, SLOT(parametersChanged()), Qt::DirectConnection);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::LinearLateralShift::inputConnectionChanged(const std::string& inputName)
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
      this->mOutput->setData(cv::Mat());
    }

    // Make a copy to create a matrix of the same type, dimensions, ...
    this->recompute();
  }

  if (output_changed)
  {
    this->emitOutputPropertiesChangedSignal("output");
  }
}

void cedar::proc::steps::LinearLateralShift::compute(const cedar::proc::Arguments&)
{
  this->recompute();
}

void cedar::proc::steps::LinearLateralShift::recompute()
{
  auto input = getInput("input");

  if (!input)
    return;

  auto input_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input);

  if (!input_data)
    return;

  cv::Mat input_mat = input_data->getData();

  if (input_mat.empty())
    return;

  int siz = static_cast<int>( input_mat.rows );

  cv::Mat output_mat;

  if (!this->mOutput)
  {
    output_mat= cv::Mat(siz, 1, CV_32F );
  }
  else
  {
    output_mat= mOutput->getData();
  }

  if (output_mat.empty()
      || output_mat.rows != input_mat.rows
      || output_mat.cols != input_mat.cols)
  {
    output_mat= cv::Mat(siz, 1, CV_32F );
  }


  // shift:

  //auto lower = mLowerLimit->getValue();
  //auto upper = mUpperLimit->getValue();

  int i= 0;
  int index_in_last= -1;
  float last_value;

  last_value= input_mat.at<float>(0,0);

  for(; i < siz; i++)
  {
    int index_in;

    index_in= round( ( static_cast<float>(i) - this->mA->getValue() ) 
                     / this->mB->getValue() );

    if (index_in < siz
        && index_in >= 0
        && index_in_last < index_in)
    {
      last_value= input_mat.at<float>(index_in, 0);
      output_mat.at<float>(i, 0)= last_value;
      index_in_last= index_in;
    }
    else
    {
      output_mat.at<float>(i, 0)= last_value;
    }
  }

  this->mOutput->setData( output_mat );
}


void cedar::proc::steps::LinearLateralShift::parametersChanged()
{
  this->recompute();
}

