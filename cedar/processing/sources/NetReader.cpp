/*=============================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
s
    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        NetReader.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Thu 03 Nov 2011

    Description:

    Credits:

=============================================================================*/

#include "cedar/configuration.h"
#ifdef CEDAR_USE_YARP

// LOCAL INCLUDES
#include "cedar/processing/sources/NetReader.h"
#include "cedar/auxiliaries/net/exceptions.h"

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
      new ElementDeclarationTemplate<cedar::proc::sources::NetReader>
      (
        "Sources",
        "cedar.processing.sources.NetReader"
      )
    );
    input_declaration->setIconPath(":/steps/net_reader.svg");
    input_declaration->setDescription("Reads a matrix from a yarp port.");
    cedar::proc::DeclarationRegistrySingleton::getInstance()->declareClass(input_declaration);

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::sources::NetReader::NetReader()
:
mOutput(new cedar::aux::MatData(cv::Mat())),
// outputs
mReader()
// parameters
{
  // declare all data
  this->declareOutput("output", mOutput);

  // the Reader will be instantiated on the first read
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sources::NetReader::onStart()
{
  // instantiate the reader, if not yet done
  if (!mReader)
  {
    try 
    {
      mReader= boost::shared_ptr< cedar::aux::net::Reader< cedar::aux::MatData::DataType > >(new cedar::aux::net::Reader< cedar::aux::MatData::DataType >("DEMOCHANNEL"));
      // TODO: make channel configurable
    }
    catch ( cedar::aux::net::NetWaitingForWriterException &e )
    {
      // the writer hasnt declared the channel yet ... abort, better luck
      // on the next compute()
      return;
    }
    catch ( cedar::aux::net::NetMissingRessourceException &e )
    {
      // somehow YARP doesnt work ... :( typically fatal.
      // TODO: what to do?
      throw( e ); // lets try this ...
    }
  }
}

void cedar::proc::sources::NetReader::onStop()
{
  mReader.reset();
}

void cedar::proc::sources::NetReader::compute(const cedar::proc::Arguments&)
{
  if (!mReader)
    return;

  // read from net and set data
  try
  {
    this->mOutput->setData( mReader->read() );
  }
  catch(cedar::aux::net::NetWaitingForWriterException& e)
  {
    // no writer instantiated yet? ignore
    // CHANGE NOTHING
    return;
  }
  catch (cedar::aux::net::NetUnexpectedDataException& e)
  {
    // communication problem? ignore
    // CHANGE NOTHING
    return;
  }

}

#endif

