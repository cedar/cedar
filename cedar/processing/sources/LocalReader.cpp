/*=============================================================================

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

===============================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        LocalReader.cpp

    Maintainer:  Raul Grieben
    Email:       raul.grienen@ini.ruhr-uni-bochum.de
    Date:        Thu 20 Dez 20181

    Description:

    Credits:

=============================================================================*/

#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/sources/LocalReader.h"
#include "cedar/auxiliaries/net/exceptions.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/sinks/LocalWriter.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/net/exceptions.h"
#include "cedar/version.h"

// SYSTEM INCLUDES
#include <iostream>
#include <vector>


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
      new ElementDeclarationTemplate<cedar::proc::sources::LocalReader>
      (
        "Sources",
        "cedar.processing.sources.LocalReader"
      )
    );
    declaration->setIconPath(":/steps/local_reader.svg");
    declaration->setDescription("Reads a matrix locally. See also the "
                    "LocalWriter step.");

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::sources::LocalReader::LocalReader()
:
cedar::proc::Step(true),
mOutput(new cedar::aux::MatData(cv::Mat())),
// parameters
_mPort(new cedar::aux::StringParameter(this, "port", ""))
{
  // declare all data
  this->declareOutput("output", mOutput);

  //add actions that emits the output properties changed signal

  this->registerFunction
  (
    "emit output properties changed signal",
    boost::bind(&cedar::proc::sources::LocalReader::emitOutputPropertiesChangedSignalOnAction, this),
    false
  );

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sources::LocalReader::reset()
{
  this->connect();
}

void cedar::proc::sources::LocalReader::emitOutputPropertiesChangedSignalOnAction()
{
  this->emitOutputPropertiesChangedSignal("output");
}

void cedar::proc::sources::LocalReader::connect()
{
}

void cedar::proc::sources::LocalReader::onStart()
{
  this->_mPort->setConstant(true);
  this->connect();
}

void cedar::proc::sources::LocalReader::onStop()
{
  this->_mPort->setConstant(false);
}

void cedar::proc::sources::LocalReader::compute(const cedar::proc::Arguments&)
{
    if("" == _mPort->getValue())
    {
        this->setState(cedar::proc::Step::STATE_EXCEPTION, "Local port name is empty!");
        //CEDAR_THROW(cedar::aux::InvalidNameException, "Local port name is empty!");
    }
  else if(cedar::proc::sinks::LocalWriter::mData().count(_mPort->getValue()) > 0)
  {
    cv::Mat old = this->mOutput->getData();
    cedar::aux::ConstMatDataPtr readPtr = cedar::proc::sinks::LocalWriter::mData()[_mPort->getValue()];
    cv::Mat read = readPtr->getData();

    bool changed = (old.type() != read.type() || old.size != read.size);
    this->mOutput->setData(read);
    if (changed)
    {
      this->emitOutputPropertiesChangedSignal("output");
    }
  }
  else
  {
      this->setState(cedar::proc::Step::STATE_EXCEPTION, "Local port does not exist!");
      //CEDAR_THROW(cedar::aux::InvalidNameException, "Local port does not exist!");
  }
}
