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

    File:        DataPath.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2015 01 23

    Description: Source file for the class cedar::proc::DataPath.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/DataPath.h"
#include "cedar/processing/GroupPath.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::DataPath::DataPath(cedar::proc::GroupPath pathToElement, cedar::proc::DataRole::Id role, const std::string& dataName)
{
  this->setDataPath(pathToElement, role, dataName);
}

cedar::proc::DataPath::DataPath(const std::string& path)
{
  this->parseFromString(path);
}

cedar::proc::DataPath::~DataPath()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::GroupPath cedar::proc::DataPath::getPathToElement() const
{
  return this->mPathToElement;
}

cedar::proc::DataRole::Id cedar::proc::DataPath::getDataRole() const
{
  return this->mDataRole;
}

std::string cedar::proc::DataPath::getDataName() const
{
  return this->mDataName;
}

void cedar::proc::DataPath::parseFromString(const std::string& dataPath)
{
  std::string element_with_type;
  std::string element;
  std::string role;
  std::string data_name;

  // split data name
  cedar::aux::splitLast(dataPath, ".", element_with_type, data_name);
  this->mDataName = data_name;

  // split data role
  cedar::aux::splitLast(element_with_type, "[", element, role);
  if (!role.empty() && role.at(role.size() - 1) == ']')
  {
    role = role.substr(0, role.size() - 1);
  }
  this->mDataRole = cedar::proc::DataRole::type().get(role).id();

  // the rest is the path
  this->mPathToElement = element;
}

std::string cedar::proc::DataPath::toString() const
{
  std::string role = cedar::proc::DataRole::type().get(this->mDataRole).name();
  return this->mPathToElement.toString() + "[" + role + "]" + "." + this->mDataName;
}

void cedar::proc::DataPath::setDataPath(cedar::proc::GroupPath pathToElement, cedar::proc::DataRole::Id role, const std::string& dataName)
{
  this->mPathToElement = pathToElement;
  this->mDataRole = role;
  this->mDataName = dataName;
}
