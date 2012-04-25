/*=============================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        NetWriterSink.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Sun 06 Nov 2011 11:44:14 PM CET

    Description:

    Credits:

=============================================================================*/

#include "cedar/configuration.h"
#ifdef CEDAR_USE_YARP

// LOCAL INCLUDES
#include "cedar/processing/steps/NetWriter.h"

// PROJECT INCLUDES
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/net/exceptions.h"

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
      new ElementDeclarationTemplate<cedar::proc::steps::NetWriterSink>
      (
        "Utilities",
        "cedar.processing.steps.NetWriter"
      )
    );
    input_declaration->setIconPath(":/steps/net_writer.svg");
    cedar::proc::DeclarationRegistrySingleton::getInstance()->declareClass(input_declaration);

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::NetWriterSink::NetWriterSink()
:
// outputs
mInput(new cedar::aux::MatData(cv::Mat())),
mpWriter(NULL)
// parameter
{
  // declare all data
  this->declareInput("input");

}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::NetWriterSink::onStart()
{
  // instantiate the reader, if not yet done
  if (mpWriter == NULL)
  {
    try 
    {
      mpWriter= new cedar::aux::net::Writer< cedar::aux::MatData::DataType >("DEMOCHANNEL");
      // TODO: make channel configurable
    }
    catch ( cedar::aux::net::NetMissingRessourceException &e )
    {
      // somehow YARP doesnt work ... :( typically fatal.
      // TODO: what to do?
      throw( e ); // lets try this ...
    }
  }
}

void cedar::proc::steps::NetWriterSink::onStop()
{
  if (mpWriter != NULL)
    delete mpWriter;
  mpWriter= NULL;
}


void cedar::proc::steps::NetWriterSink::compute(const cedar::proc::Arguments&)
{
  if (mpWriter == NULL)
    return;

  // write it over the channel
  try
  {
    mpWriter->write( mInput->getData() );
  }
  catch ( cedar::aux::net::NetMissingRessourceException &e )
  {
    // somehow YARP doesnt work ... :( typically fatal.
    // TODO: what to do?
    throw( e ); // lets try this ...
  }
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::NetWriterSink::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr CEDAR_DEBUG_ONLY(slot),
                                  cedar::aux::DataPtr data
                                ) const
{
  // First, let's make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(slot->getName() == "input")

  if (boost::shared_dynamic_cast<cedar::aux::MatData>(data))
  {
    cv::Mat &matref= boost::shared_dynamic_cast<cedar::aux::MatData>(data)->getData();

    if (matref.cols <= 0
        || matref.rows <= 0)
    {
      // we dont support n-Dim at the moment
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

void cedar::proc::steps::NetWriterSink::inputConnectionChanged(const std::string& inputName)
{
  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "input");

  // Assign the input to the member. This saves us from casting in every computation step.
  this->mInput = boost::shared_dynamic_cast<const cedar::aux::MatData>(this->getInput(inputName));
  // This should always work since other types should not be accepted.
  CEDAR_DEBUG_ASSERT(this->mInput);


  // Finally, send data ...
  this->onTrigger();
}

#endif // CEDAR_USE_YARP
