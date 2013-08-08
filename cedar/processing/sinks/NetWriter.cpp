/*=============================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        NetWriter.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Sun 06 Nov 2011

    Description:

    Credits:

=============================================================================*/

#include "cedar/configuration.h"
#ifdef CEDAR_USE_YARP

// CEDAR INCLUDES
#include "cedar/processing/sinks/NetWriter.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/net/exceptions.h"
#include "cedar/auxiliaries/assert.h"

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

    ElementDeclarationPtr input_declaration
    (
      new ElementDeclarationTemplate<cedar::proc::sinks::NetWriter>
      (
        "Sinks",
        "cedar.processing.sinks.NetWriter"
      )
    );
    input_declaration->setIconPath(":/steps/net_writer.svg");
    input_declaration->setDescription("Writes incoming matrices to the local network.");

    input_declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::sinks::NetWriter::NetWriter()
:
// outputs
mInput(new cedar::aux::MatData(cv::Mat())),
mWriter(),
_mPort(new cedar::aux::StringParameter(this, "port", 
                                       "CEDAR/" 
                                       + cedar::aux::versionNumberToString(CEDAR_VERSION)
                                       + "/MISC" ))
{
  // declare all data
  this->declareInput("input");
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sinks::NetWriter::onStart()
{
  _mPort->setConstant(true);

  this->connect();
}

void cedar::proc::sinks::NetWriter::connect()
{
  // instantiate the reader, if not yet done
  if (!mWriter)
  {
    try 
    {
      mWriter
        = boost::shared_ptr<cedar::aux::net::Writer<cedar::aux::MatData::DataType> >
          (
            new cedar::aux::net::Writer<cedar::aux::MatData::DataType>(this->getPort())
          );
    }
    catch (cedar::aux::net::NetMissingRessourceException& e)
    {
      // somehow YARP doesnt work ... :( typically fatal.
      this->setState(cedar::proc::Step::STATE_EXCEPTION, "Network communication exception: " + e.exceptionInfo());
      throw (e); // lets try this ...
    }
  }
}

void cedar::proc::sinks::NetWriter::onStop()
{
  mWriter.reset();
  _mPort->setConstant(false);
}

void cedar::proc::sinks::NetWriter::reset()
{
  mWriter.reset();
  this->connect();
}

void cedar::proc::sinks::NetWriter::compute(const cedar::proc::Arguments&)
{
  if (!mWriter)
  {
    return;
  }

  // write it over the channel
  try
  {
    mWriter->write(mInput->getData());
  }
  catch (cedar::aux::net::NetMissingRessourceException& e)
  {
    // somehow YARP doesnt work ... :( typically fatal.
    this->setState(cedar::proc::Step::STATE_EXCEPTION, "Network communication exception: " + e.exceptionInfo());
    throw (e); // lets try this ...
  }
}

cedar::proc::DataSlot::VALIDITY cedar::proc::sinks::NetWriter::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr CEDAR_DEBUG_ONLY(slot),
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  // First, let's make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(slot->getName() == "input")

  if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
  {
    const cv::Mat& matref= mat_data->getData();

    if (matref.cols <= 0
        || matref.rows <= 0)
    {
      // we don't support n-Dim at the moment
      return cedar::proc::DataSlot::VALIDITY_ERROR;
    }

    // Mat data is accepted.
    return cedar::proc::DataSlot::VALIDITY_VALID;
  }
  else
  {
    // Everything else is rejected.
    return cedar::proc::DataSlot::VALIDITY_ERROR;
  }
}

void cedar::proc::sinks::NetWriter::inputConnectionChanged(const std::string& inputName)
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

#endif // CEDAR_USE_YARP
