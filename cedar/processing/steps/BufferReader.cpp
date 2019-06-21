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

    File:        BufferReader.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2017 05 12

    Description: Source file for the class cedar::proc::steps::BufferReader.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/BufferReader.h"

// CEDAR INCLUDES
#include <cedar/processing/Group.h>
#include <cedar/processing/exceptions.h>
#include <cedar/auxiliaries/stringFunctions.h>
#include <cedar/auxiliaries/MatData.h>
#include <cedar/auxiliaries/Log.h>
#include <cedar/processing/ElementDeclaration.h>


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
    new ElementDeclarationTemplate<cedar::proc::steps::BufferReader>
    (
      "Programming",
      "cedar.processing.steps.BufferReader"
    )
  );

  declaration->setIconPath(":/steps/buffer_reader.svg");
  declaration->setDescription
  (
    "A step that reads a buffer from a neural field and outputs it. NOTE: the syntax to adress buffers is to separate elements via dots, example: 'Neural Field 2.activation'."
  );

  declaration->declare();

  return true;
}

bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::BufferReader::BufferReader()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat())),
mPath(new cedar::aux::StringParameter(this, "buffer path", ""))
{
  // declare all data
  this->declareOutput("buffer value", cedar::aux::DataPtr(new cedar::aux::Data()));
  QObject::connect(this->mPath.get(), SIGNAL(valueChanged()), this, SLOT(bufferPathChanged()));

  this->connectToGroupChanged(boost::bind(&cedar::proc::steps::BufferReader::bufferPathChanged, this));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::BufferReader::compute(const cedar::proc::Arguments&)
{
}

void cedar::proc::steps::BufferReader::bufferPathChanged()
{
  std::string step_path;
  std::string buffer_path;
  const std::string& path = this->getPath();

  try
  {
    cedar::aux::splitLast(path, ".", step_path, buffer_path);
  }
  catch (const cedar::aux::InvalidNameException&)
  {
    if (path.length())
    {
      cedar::aux::LogSingleton::getInstance()->error
      (
        "BufferReader did not find '"
        + path + "'. SYNTAX: stepname.buffername"
        "void utilities::BufferReader::bufferPathChanged()",
        this->getName()
      );
    }
    return;
  }

  if (!this->getGroup())
  {
    return;
  }

  cedar::proc::ConnectablePtr connectable;
  try
  {
    connectable = this->getGroup()->getElement<cedar::proc::Connectable>(step_path);
  }
  catch (const cedar::aux::InvalidNameException&)
  {
    if (step_path.length() 
        && buffer_path.length())
    {
      cedar::aux::LogSingleton::getInstance()->error
      (
        "BufferReader could not find the step: '"
        + step_path + "'.",
        "void utilities::BufferReader::bufferPathChanged()",
        this->getName()
      );
    }
    return;
  }

  try
  {
    cedar::aux::ConstDataPtr buffer = connectable->getBuffer(buffer_path);
    this->setOutput("buffer value", boost::const_pointer_cast<cedar::aux::Data>(buffer));
  }
  catch (const cedar::aux::InvalidNameException&)
  {
    // scrolls too much
#if 0    
    cedar::aux::LogSingleton::getInstance()->error
    (
      "BufferReader could not find the buffer: '"
      + buffer_path + "' in that step.",
      "void utilities::BufferReader::bufferPathChanged()",
      this->getName()
    );
#endif
    return;
  }

  cedar::aux::LogSingleton::getInstance()->message
  (
    "Buffer successfully stolen.",
    "void utilities::BufferReader::bufferPathChanged()",
    this->getName()
  );
}

void cedar::proc::steps::BufferReader::afterArchitectureLoaded()
{
  bufferPathChanged();
}


