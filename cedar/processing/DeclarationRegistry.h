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

    File:        DeclarationRegistry.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 11 18

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_DECLARATION_REGISTRY_H
#define CEDAR_PROC_DECLARATION_REGISTRY_H

// CEDAR INCLUDES
#include "cedar/processing/namespace.h"
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <boost/shared_ptr.hpp>
#include <string>
#include <set>
#include <map>
#include <vector>

/*!@brief This class associates names with pointers to certain objects.
 */
class cedar::proc::DeclarationRegistry
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::Singleton<cedar::proc::DeclarationRegistry>;
  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Map of declaration ids to declaration pointers.
  typedef std::map<std::string, cedar::proc::ElementDeclarationPtr> Declarations;

  //! Set of category names.
  typedef std::set<std::string> CategoryList;

  //! List of element declarations in a category.
  typedef std::vector<cedar::proc::ElementDeclarationPtr> CategoryEntries;

  //! Map of strings to CategoryEntries.
  typedef std::map<std::string, CategoryEntries> Categories;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief   Allocates a class using the declaration identified by the given classId.
   *
   * @throws  cedar::proc::InvalidNameException if no declaration can be found for the given classId.
   */
  cedar::proc::ElementPtr allocateClass(const std::string& classId) const;

  /*!@brief   Adds a class declaration to the registry.
   *
   *          After this call, the declaration will be retrievable by its classId.
   */
  void declareClass(cedar::proc::ElementDeclarationPtr pDeclaration);

  /*!@brief   Returns the list of categories stored in the registry.
   */
  const CategoryList& getCategories() const;

  /*!@brief   Returns the entries for a given category.
   */
  const CategoryEntries& getCategoryEntries(const std::string& category) const;

  /*!@brief   Returns the declaration that generates objects of the same type as the given object.
   */
  cedar::proc::ElementDeclarationPtr getDeclarationOf(cedar::proc::ElementPtr object);

  /*!@brief   Returns the list of declarations in this registry.
   */
  const Declarations& declarations() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief   Constructor. This is private because it follows the singleton pattern.
   */
  DeclarationRegistry();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Member that stores the declarations.
  std::map<std::string, cedar::proc::ElementDeclarationPtr> mDeclarations;

  //! Map of deprecated names to their current ones
  std::map<std::string, std::string> mDeprecatedToCurrentNames;

  //! List of categories.
  CategoryList mCategories;

  //! Association of categories and declarations.
  Categories mDeclarationsByCategory;

}; // class cedar::proc::DeclarationRegistry

#include "cedar/auxiliaries/Singleton.h"

namespace cedar
{
  namespace proc
  {
    CEDAR_INSTANTIATE_PROC_TEMPLATE(cedar::aux::Singleton<cedar::proc::DeclarationRegistry>);

    //!@brief The singleton for the declaration registry.
    typedef cedar::aux::Singleton<cedar::proc::DeclarationRegistry> DeclarationRegistrySingleton;
  }
}

#endif // CEDAR_PROC_DECLARATION_REGISTRY_H
