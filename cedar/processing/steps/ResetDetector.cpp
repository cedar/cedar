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

    File:        ResetDetector.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2017 12 04

    Description: Source file for the class cedar::proc::steps::ResetDetector.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/ResetDetector.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/IsMatrix.h"
#include <cedar/processing/ElementDeclaration.h>

// SYSTEM INCLUDES
#include <math.h>
#include <iostream>

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
    new ElementDeclarationTemplate<cedar::proc::steps::ResetDetector>
    (
      "Programming",
      "cedar.processing.steps.ResetDetector"
    )
  );

  //declaration->setIconPath(":/steps/noop.svg");
  declaration->setDescription
  (
    "Emits a scalar one iff a reset was detected and a zero otherwise. "
    "Resets occur either when the architecture resets if no input is provided, "
    "or, if the optional input is provided, when any element in that "
    "input tensor changes(!) from zero to non-zero, i.e. a Boost signal is "
    "connected and it gets "
    "activated."
  );

  declaration->declare();

  return true;
}

bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ResetDetector::ResetDetector()
:
// outputs
mOutputReset(new cedar::aux::MatData(cv::Mat::ones(1,1, CV_32F))),
mBoostWasActive(false)
{
  // declare all data
  cedar::proc::DataSlotPtr input1 = this->declareInput("boost signal (optional)", false);

  this->declareOutput("reset detected", mOutputReset);

  input1->setCheck(cedar::proc::typecheck::IsMatrix());
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::ResetDetector::inputConnectionChanged(const std::string&)
{
  mBoostWasActive= false;
  this->recompute(false);
}

void cedar::proc::steps::ResetDetector::compute(const cedar::proc::Arguments&)
{
  this->recompute(false);
}

void cedar::proc::steps::ResetDetector::recompute(bool reset)
{
  auto input_matrix= getInput("boost signal (optional)");

  if (input_matrix) // if there is an input it overrides the reset
  {
    reset= false;

    auto data_matrix = boost::dynamic_pointer_cast
                           <const cedar::aux::MatData>(input_matrix);
    if (data_matrix)
    {
      if (!data_matrix->isEmpty())
      {
        cv::Mat mat_matrix= data_matrix->getData();

        int nonzeros= cv::countNonZero( mat_matrix );
        if (nonzeros > 0)
        {
          if (!mBoostWasActive)
          {
            reset= true;
            mBoostWasActive= true; // only reset once
          }
        }
        else
        {
          mBoostWasActive= false;
        }
      }
    }
  }

  cv::Mat& out1= mOutputReset->getData();
  if (out1.empty())
  {
    out1= cv::Mat::zeros(1,1,CV_32F);
  }

  float old_val= out1.at<float>(0,0);
  float new_val= ( reset ? 1.0 : 0.0 );

  out1.at<float>(0,0)= new_val;
  if (old_val != new_val)
  {
    this->emitOutputPropertiesChangedSignal("reset detected");
  }
}

void cedar::proc::steps::ResetDetector::reset()
{
  mBoostWasActive= false;
  recompute( true );  // trigger the reset
  recompute( false ); // revert back to no-reset. 
                      // if there is no input, there is no tick and we
                      // wont revert, otherwise
}


