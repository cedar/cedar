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

    File:        GroupDeclarationManager.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2014 02 14

    Description: Source file for the class cedar::proc::GroupDeclarationManager.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/GroupDeclarationManager.h"
#include "cedar/processing/GroupDeclaration.h"
#include "cedar/processing/Group.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::GroupDeclarationManager::GroupDeclarationManager()
{
}

cedar::proc::GroupDeclarationManager::~GroupDeclarationManager()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::GroupDeclarationManager::addDeclaration(cedar::proc::ConstGroupDeclarationPtr declaration)
{
  if (this->mDeclarations.find(declaration->getClassName()) == this->mDeclarations.end())
  {
    this->mDeclarations[declaration->getClassName()] = declaration;
  }
  else
  {
    CEDAR_THROW
    (
      cedar::aux::NotFoundException,
      "A group declaration with name " + declaration->getClassName() + " already exists."
    );
  }
}

cedar::proc::ElementPtr cedar::proc::GroupDeclarationManager::addGroupTemplateToGroup(const std::string& templateName, cedar::proc::GroupPtr base, bool makeLink) const
{
  auto iter = this->mDeclarations.find(templateName);
  if (iter != this->mDeclarations.end())
  {
    cedar::proc::ConstGroupDeclarationPtr declaration = this->mDeclarations.find(templateName)->second;
    try
    {
      if (makeLink)
      {
        return base->createLinkedTemplate(declaration->getGroupName(), declaration->getFileName(), templateName);
      }
      else
      {
        return base->importGroupFromFile(declaration->getGroupName(), declaration->getFileName());
      }
    }
    catch (cedar::aux::NotFoundException& exc)
    {
      try
      {
        return base->importStepFromFile(declaration->getGroupName(), declaration->getFileName());
      }
      catch (cedar::aux::NotFoundException& exc)
      {
        CEDAR_THROW
        (
          cedar::aux::NotFoundException,
          "A group declaration with name " + templateName + " could not be found."
        );
      }
    }
  }
  else
  {
    CEDAR_THROW
    (
      cedar::aux::NotFoundException,
      "A group declaration with name " + templateName + " could not be found."
    );
  }
}

const cedar::proc::GroupDeclarationManager::GroupDeclarationMap& cedar::proc::GroupDeclarationManager::getDefinitions() const
{
  return this->mDeclarations;
}

cedar::proc::ConstGroupDeclarationPtr cedar::proc::GroupDeclarationManager::getDeclaration(const std::string& name) const
{
  auto decl = this->getDeclarationNoThrow(name);
  if (decl)
  {
    return decl;
  }
  else
  {
    CEDAR_THROW(cedar::aux::NotFoundException, "Could not find groul template declaration with name \"" + name + "\".");
  }
}

cedar::proc::ConstGroupDeclarationPtr cedar::proc::GroupDeclarationManager::getDeclarationNoThrow(const std::string& name) const
{
  for (const auto& name_declaration_pair : this->mDeclarations)
  {
    if (name_declaration_pair.first == name)
    {
      return name_declaration_pair.second;
    }
  }
  return cedar::proc::ConstGroupDeclarationPtr();
}
