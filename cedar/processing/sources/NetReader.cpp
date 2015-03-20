/*=============================================================================

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

// CEDAR INCLUDES
#include "cedar/processing/sources/NetReader.h"
#include "cedar/auxiliaries/net/exceptions.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
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
      new ElementDeclarationTemplate<cedar::proc::sources::NetReader>
      (
        "Sources",
        "cedar.processing.sources.NetReader"
      )
    );
    declaration->setIconPath(":/steps/net_reader.svg");
    declaration->setDescription("Reads a matrix from the local network.");

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::sources::NetReader::NetReader()
:
cedar::proc::Step(true),
mOutput(new cedar::aux::MatData(cv::Mat())),
// outputs
mReader(),
// parameters
_mPort(new cedar::aux::StringParameter(this, "port",
                                       "CEDAR/" 
                                       + cedar::aux::versionNumberToString(CEDAR_VERSION)
                                       + "/MISC" ))
{
  // declare all data
  this->declareOutput("output", mOutput);

  //add actions that emits the output properties changed signal
  this->registerFunction
  (
    "emit output properties changed signal",
    boost::bind(&cedar::proc::sources::NetReader::emitOutputPropertiesChangedSignalOnAction, this),
    false
  );

  // the Reader will be instantiated on the first read
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sources::NetReader::reset()
{
  mReader.reset();
  this->connect();
}

void cedar::proc::sources::NetReader::emitOutputPropertiesChangedSignalOnAction()
{
  this->emitOutputPropertiesChangedSignal("output");
}

void cedar::proc::sources::NetReader::connect()
{
  // instantiate the reader, if not yet done
  if (!mReader)
  {

    try 
    {
      mReader
        = boost::shared_ptr< cedar::aux::net::Reader< cedar::aux::MatData::DataType > >
          (
            new cedar::aux::net::Reader< cedar::aux::MatData::DataType >(this->getPort())
          );
    }
    catch ( cedar::aux::net::NetWaitingForWriterException &e )
    {
      // the writer hasnt declared the channel yet ... abort, better luck
      // on the next compute()
      this->setState( cedar::proc::Triggerable::STATE_EXCEPTION,
                      "The writer hasn't initialized this port/channel, yet. "
                      "Please check whether the name is correct and it "
                      "is running, then "
                      "select reset in the context menu." );
        // TODO: would be nice to have a state for temporarily disabling
      return;
    }
    catch (cedar::aux::net::NetMissingRessourceException &e)
    {
      // somehow YARP doesnt work ... :( typically fatal.
      throw e; // lets try this ...
    }
  }
}

void cedar::proc::sources::NetReader::onStart()
{
  this->_mPort->setConstant(true);

  this->connect();
}

void cedar::proc::sources::NetReader::onStop()
{
  this->_mPort->setConstant(false);
  mReader.reset();
}

void cedar::proc::sources::NetReader::compute(const cedar::proc::Arguments&)
{
  if (!mReader)
    return;

  // read from net and set data
  try
  {
    cv::Mat old = this->mOutput->getData();
    cv::Mat read = mReader->read();
    bool changed = (old.type() != read.type() || old.size != read.size);
    this->mOutput->setData(read);

    if (changed)
    {
      this->emitOutputPropertiesChangedSignal("output");
    }
  }
  catch (cedar::aux::net::NetWaitingForWriterException& e)
  {
    // no writer instantiated yet? ignore
    // CHANGE NOTHING
    return;
  }
  catch (cedar::aux::net::NetNoNewDataException& e)
  {
    // no new data has been sent. ignore
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

#endif // CEDAR_USE_YARP
