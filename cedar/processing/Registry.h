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

    File:        Registry.h

    Maintainer:  <first name> <last name>
    Email:       <email address>
    Date:        <creation date YYYY MM DD>

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_REGISTRY_H
#define CEDAR_PROC_REGISTRY_H

// LOCAL INCLUDES
#include "processing/namespace.h"
#include "processing/exceptions.h"
#include "auxiliaries/macros.h"
#include "auxiliaries/LoopedThread.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/shared_ptr.hpp>
#include <string>
#include <set>
#include <map>
#include <vector>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
template <class T, class T_Declaration>
class cedar::proc::Registry
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
public:
  friend class cedar::proc::Manager;
  friend class cedar::proc::Network;

  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
public:
   typedef boost::shared_ptr<T> ObjectPointer;
   typedef boost::shared_ptr<T_Declaration> DeclarationPointer;
   typedef std::map<std::string, DeclarationPointer> Declarations;

   typedef std::set<std::string> CategoryList;
   typedef std::vector<DeclarationPointer> CategoryEntries;
   typedef std::map<std::string, CategoryEntries> Categories;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void registerObject(ObjectPointer object)
  {
    if (this->mObjects.find(object->getName()) != this->mObjects.end())
    {
      CEDAR_THROW(cedar::proc::InvalidNameException, "Duplicate object entry: " + object->getName());
    }
    mObjects[object->getName()] = object;
  }

  void renameObject(const std::string& oldName, const std::string& newName)
  {
    ObjectPointer object = this->get(oldName);

    mObjects.erase(mObjects.find(oldName));

    mObjects[newName] = object;
    object->setName(newName);
  }

  void removeObject(const std::string& name)
  {
    ObjectPointer object = this->get(name);

    mObjects.erase(mObjects.find(name));
  }

  ObjectPointer get(const std::string& name)
  {
    typename std::map<std::string, ObjectPointer>::iterator iter = this->mObjects.find(name);
    if (iter != this->mObjects.end())
    {
      return iter->second;
    }
    else
    {
      CEDAR_THROW(cedar::proc::InvalidNameException, "Unknown object: " + name);
      return ObjectPointer();
    }
  }

  ObjectPointer createInstance(const std::string& classId, const std::string& name)
  {
    ObjectPointer object = this->allocateClass(classId);
    object->setName(name);
    this->registerObject(object);
    return object;
  }

  void declareClass(DeclarationPointer pDeclaration)
  {
    const std::string& class_id = pDeclaration->getClassId();
    if (this->mDeclarations.find(class_id) != this->mDeclarations.end())
    {
      CEDAR_THROW(cedar::proc::InvalidNameException, "Duplicate class declaration: " + class_id);
    }
    this->mDeclarations[class_id] = pDeclaration;

    // manage category
    typename Categories::iterator category_iter;
    category_iter = this->mDeclarationsByCategory.find(pDeclaration->getCategory());
    if (category_iter == this->mDeclarationsByCategory.end())
    {
      this->mCategories.insert(pDeclaration->getCategory());
      this->mDeclarationsByCategory[pDeclaration->getCategory()] = CategoryEntries();
      category_iter = this->mDeclarationsByCategory.find(pDeclaration->getCategory());
      CEDAR_DEBUG_ASSERT(category_iter != this->mDeclarationsByCategory.end());
    }
    category_iter->second.push_back(pDeclaration);
  }

  const CategoryList& getCategories() const
  {
    return this->mCategories;
  }

  const CategoryEntries& getCategoryEntries(const std::string& category) const
  {
    typename Categories::const_iterator it = this->mDeclarationsByCategory.find(category);
    if (it != this->mDeclarationsByCategory.end())
    {
      return this->mDeclarationsByCategory.find(category)->second;
    }
    else
    {
      CEDAR_THROW(cedar::proc::InvalidCategoryException, category + " is not a valid category");
    }
  }

  DeclarationPointer getDeclarationOf(ObjectPointer object)
  {
    for (typename std::map<std::string, DeclarationPointer>::iterator iter = this->mDeclarations.begin();
         iter != mDeclarations.end();
         ++iter)
    {
      if (iter->second->isObjectInstanceOf(object))
      {
        return iter->second;
      }
    }
    // if this point is reached, no declaration was found - throw
    CEDAR_THROW(cedar::proc::MissingDeclarationException, "cannot find a declaration that fits the given object");
  }

  bool testExists(const std::string& name) const
  {
    return this->mObjects.find(name) != this->mObjects.end();
  }

  const Declarations& declarations() const
  {
    return this->mDeclarations;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  ObjectPointer allocateClass(const std::string& classId) const
  {
    typename std::map<std::string, DeclarationPointer>::const_iterator iter;
    iter = mDeclarations.find(classId);

    if (iter != mDeclarations.end())
    {
      return iter->second->getObjectFactory()->allocate();
    }
    // if this point is reached, no factory could be found for the given class id - throw
    CEDAR_THROW(cedar::proc::InvalidNameException, "Cannot allocate object of class \""
                                                   + classId + "\": class unknown.")
  }

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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  std::map<std::string, DeclarationPointer> mDeclarations;
  std::map<std::string, ObjectPointer> mObjects;

  CategoryList mCategories;
  Categories mDeclarationsByCategory;

}; // class cedar::proc::Registry

#endif // CEDAR_PROC_REGISTRY_H

