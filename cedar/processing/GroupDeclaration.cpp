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

    File:        GroupDeclaration.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2014 02 14

    Description: Source file for the class cedar::proc::GroupDeclaration.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/GroupDeclaration.h"
#include "cedar/processing/GroupDeclarationManager.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::GroupDeclaration::GroupDeclaration(const std::string& descriptiveName, const cedar::aux::Path& fileName, const std::string& groupName, const std::string& category)
:
cedar::aux::PluginDeclaration(descriptiveName, category),
mFileName(fileName),
mGroupName(groupName)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::GroupDeclaration::declare() const
{
  cedar::proc::GroupDeclarationManagerSingleton::getInstance()->addDeclaration(this->shared_from_this());
}

std::string cedar::proc::GroupDeclaration::getClassName() const
{
  return cedar::aux::PluginDeclaration::getClassName();
}

std::string cedar::proc::GroupDeclaration::getFileName() const
{
  return this->mFileName.absolute(false).toString();
}

std::string cedar::proc::GroupDeclaration::getGroupName() const
{
  return this->mGroupName;
}

std::string cedar::proc::GroupDeclaration::getPluginType() const
{
  return "group template";
}