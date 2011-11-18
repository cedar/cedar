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

// LOCAL INCLUDES
#include "cedar/processing/namespace.h"
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/assert.h"

// PROJECT INCLUDES

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
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
public:
  typedef std::map<std::string, cedar::proc::ElementDeclarationPtr> Declarations;
  typedef std::set<std::string> CategoryList;
  typedef std::vector<cedar::proc::ElementDeclarationPtr> CategoryEntries;
  typedef std::map<std::string, CategoryEntries> Categories;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  cedar::proc::ElementPtr allocateClass(const std::string& classId) const;

  void declareClass(cedar::proc::ElementDeclarationPtr pDeclaration);

  const CategoryList& getCategories() const;

  const CategoryEntries& getCategoryEntries(const std::string& category) const;

  cedar::proc::ElementDeclarationPtr getDeclarationOf(ElementPtr object);

  const Declarations& declarations() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  std::map<std::string, cedar::proc::ElementDeclarationPtr> mDeclarations;
  CategoryList mCategories;
  Categories mDeclarationsByCategory;

}; // class cedar::proc::DeclarationRegistry

#endif // CEDAR_PROC_DECLARATION_REGISTRY_H

