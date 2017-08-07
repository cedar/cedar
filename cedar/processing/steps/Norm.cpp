/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Norm.cpp

    Maintainer:  Nico Kuerschner
    Email:       Nico.Kuerschner@ini.rub.de
    Date:        2016 12 08

    Description: Source file for the class cedar::proc::steps::Norm.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/Norm.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/IsMatrix.h"
#include "cedar/processing/ElementDeclaration.h"

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
      new ElementDeclarationTemplate<cedar::proc::steps::Norm>
      (
        "Algebra",
        "cedar.processing.Norm"
      )
    );
    declaration->setIconPath(":/steps/norm.svg");
    declaration->setDescription
    (
      "Calculates the norm of an input vector."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::Norm::Norm()
    :
    mOutput(new cedar::aux::MatData(cv::Mat(1, 1, CV_32F)))
{  
  this->declareOutput("output", mOutput);
  cedar::proc::DataSlotPtr input = this->declareInput("input");
  input->setCheck(cedar::proc::typecheck::IsMatrix());
}

cedar::proc::steps::Norm::~Norm()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::steps::Norm::compute(const cedar::proc::Arguments&)
{
  mOutput->getData().at<float>(0, 0) = cv::norm(this->mInput->getData());
}

void cedar::proc::steps::Norm::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == "input")
  {
    mInput = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
}
