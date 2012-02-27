/*======================================================================================================================

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

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        Robot.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 11 08

    Description: Manages all components of a robot and the communication with the hardware.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/robot/Robot.h"
#include "cedar/devices/robot/ComponentNotAvailableException.h"
#include "cedar/auxiliaries/ExceptionBase.h"

// SYSTEM INCLUDES
#include <map>
#include <string>
#include <set>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
cedar::dev::robot::Robot::Robot(void)
{
}

//! destructor
cedar::dev::robot::Robot::~Robot(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

/*! If the component has not been initialized yet, it will be created using the abstract @em createComponent method.
 *  For that reason, the method is not const.
 */
cedar::dev::robot::ComponentPtr cedar::dev::robot::Robot::getComponent(const std::string& rComponentName)
{
  // if the requested component is not available ..
  if(!isComponentAvailable(rComponentName))
  {
    // .. throw an exception
    CEDAR_THROW(cedar::dev::robot::ComponentNotAvailableException,
      "Component with name \"" + rComponentName + "\" does not exist in the robot \"" + _mName + "\".\n");
  }

  // if the component has not been initialized yet ..
  if (mComponents[rComponentName].use_count() == 0)
  {
    // .. and add it to the map of all components.
    mComponents[rComponentName] = createComponent(rComponentName);
  }

  return mComponents[rComponentName];
}

bool cedar::dev::robot::Robot::isComponentAvailable(const std::string& rComponentName) const
{
  // search for the component name in the map
  std::map<std::string, std::set<std::string> >::const_iterator component_names_it
    = _mSubComponentNames.find(rComponentName);

  // return whether the component name was found
  return component_names_it != _mSubComponentNames.end();
}

bool cedar::dev::robot::Robot::isComponentAvailable
     (
       const std::string& rComponentName,
       const std::string& rParentComponentName
     ) const
{
  bool is_available = false;

  // search for the sub components of the parent component
  std::map< std::string, std::set<std::string> >::const_iterator parent_component_it
    = _mSubComponentNames.find(rParentComponentName);

  // if the parent component was not found ..
  if (parent_component_it == _mSubComponentNames.end())
  {
    // .. throw an exception.
    CEDAR_THROW(cedar::dev::robot::ComponentNotAvailableException,
      "Parent component with name \"" + rParentComponentName + "\" does not exist in the robot.\n");
  }

  // get the names of all subcomponents of the parent component
  std::set<std::string> sub_component_names = parent_component_it->second;

  // search for the subcomponent
  std::set<std::string>::const_iterator sub_component_name_it
    = sub_component_names.find(rComponentName);

  // check and save whether the component name was found
  if (sub_component_name_it != sub_component_names.end())
  {
    is_available = true;
  }

  return is_available;
}
