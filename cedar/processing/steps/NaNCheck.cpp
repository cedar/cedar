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

    File:        NaNCheck.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2017 12 04

    Description: Source file for the class cedar::proc::steps::NaNCheck.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/NaNCheck.h"
#include "cedar/processing/gui/stepViews/NaNCheckView.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/IsMatrix.h"
#include <cedar/processing/ElementDeclaration.h>
#include <cedar/auxiliaries/BoolParameter.h>
#include <cedar/auxiliaries/BoolParameter.h>

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
    new ElementDeclarationTemplate<cedar::proc::steps::NaNCheck, cedar::proc::gui::NaNCheckView>
    (
      "Programming",
      "cedar.processing.steps.NaNCheck"
    )
  );

  declaration->setIconPath(":/steps/nan_check.svg");
  declaration->setDescription
  (
    "If any entry in 'input' is NaN OR Infinite OR empty, give a visual clue. "
    "The "
    "visual clue will remain until you reset the step.\n"
    "You can also determine how to replace the input 'input' by "
    "the optional input 'replacement'.\n"
    "You can also select a subset of the above checking conditions."
  );

  declaration->declare();

  return true;
}

bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::NaNCheck::NaNCheck()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat())),
mCaughtNaN(false),
mCaughtInf(false),
mCaughtEmpty(false),
mCheckForNaN(new cedar::aux::BoolParameter(this,"check for NaN?", true)),
mCheckForInf(new cedar::aux::BoolParameter(this,"check for Inf?", true)),
mCheckForEmpty(new cedar::aux::BoolParameter(this,"check for Empty?", true)),
mAutoReset(new cedar::aux::BoolParameter(this,"automatically reset", false))
{
  // declare all data
  cedar::proc::DataSlotPtr input = this->declareInput("input");
  cedar::proc::DataSlotPtr input2 = this->declareInput("replacement (optional)", false); // optional
  this->declareOutput("output", mOutput);

  input->setCheck(cedar::proc::typecheck::IsMatrix());
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::NaNCheck::inputConnectionChanged(const std::string& inputName)
{
  // TODO: you may want to replace this code by using a cedar::proc::InputSlotHelper

  if (inputName != "input")
    return;

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

    // Make a copy to create a matrix of the same type, dimensions, ...
    this->recompute();
  }

  if (output_changed)
  {
    this->emitOutputPropertiesChangedSignal("output");
  }
}

void cedar::proc::steps::NaNCheck::compute(const cedar::proc::Arguments&)
{
  this->recompute();
}

void cedar::proc::steps::NaNCheck::recompute()
{
  auto input = getInput("input");

  if (!input)
    return;

  auto data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input);

  if (!data)
    return;

  auto mat= data->getData();
 
  bool replace_me = false;
  bool caught_one = false;

  if (mat.empty())
  {
    if (mCheckForEmpty->getValue())
    {
      caught_one= true;
      setCaughtEmpty(true);
    }
  }
  else
  {
    float matnorm; // quick version to find a nan
    
    matnorm= cv::norm( mat );

    if (mCheckForNaN->getValue()
        && std::isnan( matnorm ))
    {
      caught_one= true;
      setCaughtNaN(true);
    }
    else if (mCheckForInf->getValue()
         && std::isinf( matnorm ))
    {
      caught_one= true;
      setCaughtInf(true);
    }
  }

  if (caught_one)
  {
    replace_me= true;

    auto input2 = getInput("replacement (optional)");

    if (!input2)
    {
      replace_me= false;
    }
    else
    {
      auto data2 = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input2);

      if (!data2
          || data2->isEmpty())
      {
        replace_me= false;
      }
      else
      {
        this->mOutput->setData( data2->getData() );
      }
    }
  }
  else
  {
    if (mAutoReset->getValue())
    {
      if (getCaughtNaN() || getCaughtInf() || getCaughtEmpty())
      {
        resetStates();
      }
    }
  }

  if (!replace_me) // yes, checking again because this may have changed
  {
    this->mOutput->setData( mat );
  }
}


bool cedar::proc::steps::NaNCheck::getCaughtNaN() const
{
  return mCaughtNaN;
}

void cedar::proc::steps::NaNCheck::setCaughtNaN(bool b)
{
  mCaughtNaN= b;
  emit caughtNaNChangedSignal();
}

bool cedar::proc::steps::NaNCheck::getCaughtInf() const
{
  return mCaughtInf;
}

void cedar::proc::steps::NaNCheck::setCaughtInf(bool b)
{
  mCaughtInf= b;
  emit caughtNaNChangedSignal();
}

bool cedar::proc::steps::NaNCheck::getCaughtEmpty() const
{
  return mCaughtEmpty;
}

void cedar::proc::steps::NaNCheck::setCaughtEmpty(bool b)
{
  mCaughtEmpty= b;
  emit caughtNaNChangedSignal();
}

void cedar::proc::steps::NaNCheck::resetStates()
{
  setCaughtNaN(false);
  setCaughtInf(false);
  setCaughtEmpty(false);
}

void cedar::proc::steps::NaNCheck::reset()
{
  resetStates();
}

void cedar::proc::steps::NaNCheck::onStart()
{
  resetStates();
}

