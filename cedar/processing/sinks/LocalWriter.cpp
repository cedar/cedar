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

    File:        LocalWriter.cpp

    Maintainer:  Raul Grieben
    Email:       raul.grienen@ini.ruhr-uni-bochum.de
    Date:        Thu 20 Dez 2018

    Description:

    Credits:

=============================================================================*/

#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/sinks/LocalWriter.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/version.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES
#include <QReadLocker>
#include <QWriteLocker>
#include <iostream>
#include <vector>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr input_declaration
    (
      new ElementDeclarationTemplate<cedar::proc::sinks::LocalWriter>
      (
        "Sinks",
        "cedar.processing.sinks.LocalWriter"
      )
    );
    input_declaration->setIconPath(":/steps/local_writer.svg");
    input_declaration->setDescription("Forwards a tensor to a "
                          "'LocalReader' step inside the same local "
                          "cedar instance. "
                          "See also the LocalReader step.");

    input_declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

QReadWriteLock *cedar::proc::sinks::LocalWriter::mpDataLock= NULL;

cedar::proc::sinks::LocalWriter::LocalWriter()
:
cedar::proc::Step(true),
// outputs
mInput(new cedar::aux::MatData(cv::Mat())),
_mPort(new cedar::aux::StringParameter(this, "port", "default local port"))
{
  cedar::proc::sinks::LocalWriter::mpDataLock= new QReadWriteLock();

  // declare all data
  this->declareInput("input");
  _mPort->setValidator(boost::bind(&cedar::proc::sinks::LocalWriter::validatePortName, this, _1));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sinks::LocalWriter::onStart()
{
  if (!this->mInput)
    return;

  _mPort->setConstant(true);
    if("" != oldName && oldName != _mPort->getValue())
    {
        if(getPortCount(oldName) > 0)
        {
            QWriteLocker lock( mpDataLock ); 
            accessDataUnlocked().erase(oldName);
            lock.unlock();
        }
        oldName = "";
    }

    if("" != _mPort->getValue())
    {
        this->setMatrix( _mPort->getValue(), this->mInput->getData() );
        oldName = _mPort->getValue();
    }

  this->connect();
}

void cedar::proc::sinks::LocalWriter::connect()
{

}

void cedar::proc::sinks::LocalWriter::onStop()
{
  _mPort->setConstant(false);
}

void cedar::proc::sinks::LocalWriter::reset()
{
  this->connect();
}

void cedar::proc::sinks::LocalWriter::compute(const cedar::proc::Arguments&)
{
    if("" == _mPort->getValue())
    {
        this->setState(cedar::proc::Step::STATE_EXCEPTION, "Local port name is empty!");
        //CEDAR_THROW(cedar::aux::InvalidNameException, "Local port name is empty!");
    }
    else
    {
      this->setMatrix( _mPort->getValue(), this->mInput->getData() );
    }
}

cedar::proc::DataSlot::VALIDITY cedar::proc::sinks::LocalWriter::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr CEDAR_DEBUG_ONLY(slot),
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  // First, let's make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(slot->getName() == "input")

  if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
  {
    return cedar::proc::DataSlot::VALIDITY_VALID;
  }
  else
  {
    return cedar::proc::DataSlot::VALIDITY_ERROR;
  }
}

void cedar::proc::sinks::LocalWriter::inputConnectionChanged(const std::string& inputName)
{
  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "input");

  // Assign the input to the member. This saves us from casting in every computation step.
  this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));

  if (!this->mInput)
  {
    return;
  }

  // Finally, send data ...
  this->onTrigger();
}

void cedar::proc::sinks::LocalWriter::validatePortName(const std::string& portName) const
{
  if(getPortCount( portName ) > 0)
  {
    CEDAR_THROW(cedar::aux::ValidationFailedException, "Port name already in use.");
  }
  if (portName.find_first_of(" \r\n") != std::string::npos)
  {
    CEDAR_THROW(cedar::aux::ValidationFailedException, "Port names may not contain whitespace characters.");
  }
}

void cedar::proc::sinks::LocalWriter::setMatrix(const std::string &key, const cv::Mat &mat)
{
  QWriteLocker lock( mpDataLock );
  accessDataUnlocked()[ key ]= mat.clone();
}

cv::Mat cedar::proc::sinks::LocalWriter::getMatrix(const std::string &key)
{
  QReadLocker lock( mpDataLock ); // locking for multi-threaded writers/readers
  auto data= accessDataUnlocked();

  // there is no guarantee that the key already exists
  auto found= data.find( key );
  if  (found == data.end())
    return cv::Mat();

  auto res= found->second;

  return res.clone(); // cloning is important since the cv::Mats share 
                      // their memory
}

unsigned int cedar::proc::sinks::LocalWriter::getPortCount(const std::string &key)
{
  QReadLocker lock( mpDataLock );
  return accessDataUnlocked().count( key );
}

