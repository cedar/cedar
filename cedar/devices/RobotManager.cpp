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

    File:        RobotManager.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 02 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/RobotManager.h"
#include "cedar/devices/Robot.h"
#include "cedar/auxiliaries/systemFunctions.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::RobotManager::RobotManager()
{
}

cedar::dev::RobotManager::~RobotManager()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::RobotManager::addRobotName(const std::string& robotName)
{
  if (mRobotInstances.find(robotName) != mRobotInstances.end())
  {
    CEDAR_THROW(cedar::aux::DuplicateNameException, "A robot with the name \"" + robotName + "\" already exists.");
  }

  this->mRobotInstances[robotName] = cedar::dev::RobotPtr(new cedar::dev::Robot());

  this->mRobotNameAddedSignal(robotName);
}

cedar::dev::RobotPtr cedar::dev::RobotManager::getRobot(const std::string& robotName) const
{
  auto iter = this->mRobotInstances.find(robotName);
  if (iter == this->mRobotInstances.end())
  {
    CEDAR_THROW(cedar::aux::UnknownNameException, "No robot of the name \"" + robotName + "\" is known.");
  }

  return iter->second;
}

void cedar::dev::RobotManager::loadRobotConfigurationFromResource
     (
       const std::string& robotName,
       const std::string& resourcePath
     )
{
  if (this->mRobotInstances.find(robotName) == this->mRobotInstances.end())
  {
    CEDAR_THROW(cedar::aux::UnknownNameException, "Cannot find a robot by the name \"" + robotName + "\".");
  }

  mRobotResourceConfigurations[robotName] = resourcePath;
  std::string full_path = cedar::aux::locateResource(resourcePath);

  this->mRobotInstances[robotName]->readJson(full_path);

  this->mRobotConfigurationLoadedSignal(robotName);
}
