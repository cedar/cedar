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

    File:        CSVToPhoneme.cpp

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de
    Date:        2022 11 20

    Description: Source file for the class cedar::proc::steps::CSVToPhoneme.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/CSVToPhoneme.h"

// CEDAR INCLUDES
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
            new ElementDeclarationTemplate<cedar::proc::steps::CSVToPhoneme>
                    (
                        "Programming",
                        "cedar.processing.steps.CSVToPhoneme"
                    )
    );

    declaration->setIconPath("");
    declaration->setDescription
    (
            ""
    );

    declaration->declare();

    return true;
  }
  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::CSVToPhoneme::CSVToPhoneme():
_mInput(cedar::aux::MatDataPtr()),
_mOutputs(1, cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat::zeros(1,1, CV_32F)))),
_mElapsedTime(0.0),
_mIsLogging(false),
_mHasLogged(false),
_mCSVPath(new cedar::aux::FileParameter(this,"CSV file",cedar::aux::FileParameter::Mode::READ,
                                        "example.txt")),
_mLogThreshold(new cedar::aux::DoubleParameter(this, "log threshold", 0.5)),
_mOutputDimension (new cedar::aux::UIntParameter(this, "dimensionality", 1,1,255))
{
  this->declareInput("input", false);

  this->declareOutput("output 0", _mOutputs.front());

  QObject::connect(_mOutputDimension.get(), SIGNAL(valueChanged()), this, SLOT(vectorDimensionChanged()));
}

cedar::proc::steps::CSVToPhoneme::~CSVToPhoneme()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
// The arguments are unused here
void cedar::proc::steps::CSVToPhoneme::compute(const cedar::proc::Arguments &)
{

}

void cedar::proc::steps::CSVToPhoneme::vectorDimensionChanged()
{
//  std::cout << "hu" << std::endl;
//  unsigned newsize = _mOutputDimension->getValue();
//  if (newsize == _mInputs.size())
//  {
//    return;
//  }
//  else if (newsize < _mInputs.size())
//  {
//    //delete unused slots
//    for (unsigned i=newsize; i< _mInputs.size(); i++)
//    {
//      removeInputSlot(makeSlotName(i));
//    }
//  }
//  else if (newsize > _mInputs.size())
//  {
//    //declare new input slots
//    for (unsigned i= _mInputs.size(); i<newsize; i++)
//    {
//      declareInput(makeSlotName(i), false);
//    }
//  }
//  //resize inputs vector
//  _mInputs.resize(newsize);
//  //resize the output matrix
//  _mOutput->getData() = cv::Mat::zeros(newsize, 1, _mOutput->getData().type());
//
//  emitOutputPropertiesChangedSignal("output");
//  onTrigger();
}

std::string cedar::proc::steps::CSVToPhoneme::makeSlotName(const int i)
{
  std::stringstream s;
  s << "input " << i;
  return s.str();
}