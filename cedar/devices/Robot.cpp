/*======================================================================================================================

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

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        Robot.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 23

    Description: Manages all components of a robot.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/ObjectMapParameterTemplate.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/ExceptionBase.h"
#include "cedar/devices/Robot.h"
#include "cedar/devices/ComponentSlot.h"
#include "cedar/devices/exceptions.h"

// SYSTEM INCLUDES
#include <cstdio>
#include <map>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
cedar::dev::Robot::Robot(void)
{
}

//! destructor
cedar::dev::Robot::~Robot(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::ComponentPtr cedar::dev::Robot::getComponent(const std::string& componentSlotName) const
{
  return (*_mComponentSlots)[componentSlotName]->getComponent();
}

std::vector<std::string> cedar::dev::Robot::getComponentSlotNames() const
{
  std::vector<std::string> slot_names;

  // for all elements in the _mComponentSlots map
  for
  (
    std::map<std::string, cedar::dev::ComponentSlotPtr>::const_iterator map_entry
      = _mComponentSlots->begin();
    map_entry != _mComponentSlots->end();
    ++map_entry
  )
  {
    // copy the key of the map (i.e., the name of the slot) into the vector
    slot_names.push_back(map_entry->first);
  }

  return slot_names;
}

cedar::dev::ComponentSlotPtr cedar::dev::Robot::getComponentSlot(const std::string& componentSlotName) const
{
  std::map<std::string, cedar::dev::ComponentSlotPtr>::const_iterator slot_it
    = _mComponentSlots->find(componentSlotName);

  if (slot_it == _mComponentSlots->end())
  {
    std::ostringstream exception_message;
    exception_message << "The robot does not have a component slot with the name \""
                      << componentSlotName
                      << "\" .";
    CEDAR_THROW(cedar::dev::ResourceNotAvailableException, exception_message.str());
  }

  return slot_it->second;
}

const std::string& cedar::dev::Robot::getName() const
{
  return this->_mName->getValue();
}
