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

    File:        GroupSink.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2013 11 08

    Description: Source file for the class cedar::proc::sinks::GroupSink.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/sinks/GroupSink.h"
#include "cedar/processing/sources/GroupSource.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/Group.h"
#include "cedar/auxiliaries/Data.h"

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

    ElementDeclarationPtr input_declaration
    (
      new ElementDeclarationTemplate<cedar::proc::sinks::GroupSink>
      (
        "Sinks",
        "cedar.processing.sinks.GroupSink"
      )
    );
    input_declaration->setIconPath(":/steps/group_sink.svg");
    input_declaration->setDescription("Do not use this.");

    input_declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::sinks::GroupSink::GroupSink()
:
mEmptyData(new cedar::aux::Data())
{
  this->setAutoLockInputsAndOutputs(false);
  this->declareInput("input", false);
}

cedar::proc::sinks::GroupSink::~GroupSink()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sinks::GroupSink::validateName(const std::string& newName) const
{
  this->validateNameStringFormat(newName);
  // we don't call the normal validation method here because it checks that this step doesn't have the same name as a
  // group connector, which would lead to trouble as this step needs to have just that.
}

void cedar::proc::sinks::GroupSink::compute(const cedar::proc::Arguments& /*arguments*/)
{
  // nothing to do here!
}

void cedar::proc::sinks::GroupSink::inputConnectionChanged(const std::string& CEDAR_DEBUG_ONLY(inputName))
{
  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "input");

  // since the step may no longer be in a group upon deletion, we may not need to do anything.
  cedar::proc::GroupPtr group = this->getGroup();
  if (!group)
  {
    return;
  }

  if (cedar::aux::ConstDataPtr data = this->getInput("input"))
  {
    group->setOutput(this->getName(), boost::const_pointer_cast<cedar::aux::Data>(data));
    group->emitOutputPropertiesChangedSignal(this->getName());
    // Finally, send data ...
//    this->onTrigger();
  }
  else
  {
    group->setOutput(this->getName(), mEmptyData);
    group->emitOutputPropertiesChangedSignal(this->getName());
  }
}

cedar::aux::DataPtr cedar::proc::sinks::GroupSink::getEmptyData()
{
  return this->mEmptyData;
}
