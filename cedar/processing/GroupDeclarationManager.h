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

    File:        GroupDeclarationManager.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2014 02 14

    Description: Header file for the class cedar::proc::GroupDeclarationManager.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GROUP_DECLARATION_MANAGER_H
#define CEDAR_PROC_GROUP_DECLARATION_MANAGER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Singleton.h"

// FORWARD DECLARATIONS
#include "cedar/processing/GroupDeclarationManager.fwd.h"
#include "cedar/processing/GroupDeclaration.fwd.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/processing/Element.fwd.h"

// SYSTEM INCLUDES
#include <map>

/*!@brief Manages declarations of group templates.
 */
class cedar::proc::GroupDeclarationManager
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::Singleton<cedar::proc::GroupDeclarationManager>;

public:
  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
  //! a map of template names to group template definitions
  typedef std::map<std::string, cedar::proc::ConstGroupDeclarationPtr> GroupDeclarationMap;
private:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief The standard constructor.
  GroupDeclarationManager();

public:
  //!@brief Destructor
  virtual ~GroupDeclarationManager();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief add a new declaration to the manager
  void addDeclaration(cedar::proc::ConstGroupDeclarationPtr declaration);

  /*! Adds a group template to the given group.
   *
   * @param templateName Name of the template to add.
   * @param base Group to which the template is added.
   * @param makeLink Whether the created group should be a link. Linked groups are added read-only and load the
   *                 corresponding template from its file every time the base group is loaded.
   * @return A pointer to the added group.
   */
  cedar::proc::ElementPtr addGroupTemplateToGroup
  (
    const std::string& templateName,
    cedar::proc::GroupPtr base,
    bool makeLink = false
  )
  const;

  //!@brief get all definitions
  const GroupDeclarationMap& getDefinitions() const;

  //! Gets a declaration given a name. Throws a cedar::aux::NotFoundException when the name cannot be found.
  cedar::proc::ConstGroupDeclarationPtr getDeclaration(const std::string& name) const;

  //! Gets a declaration given a name. Returns a null pointer if the name cannot be found.
  cedar::proc::ConstGroupDeclarationPtr getDeclarationNoThrow(const std::string& name) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! List of all declarations.
  GroupDeclarationMap mDeclarations;

}; // class cedar::proc::GroupDeclarationManager

CEDAR_PROC_SINGLETON(GroupDeclarationManager);

#endif // CEDAR_PROC_GROUP_DECLARATION_MANAGER_H

