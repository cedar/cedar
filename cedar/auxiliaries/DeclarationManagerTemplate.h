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

    File:        DeclarationManagerTemplate.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 01 23

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_DECLARATION_MANAGER_TEMPLATE_H
#define CEDAR_AUX_DECLARATION_MANAGER_TEMPLATE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/Log.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/DeclarationManagerTemplate.fwd.h"
#include "cedar/auxiliaries/PluginDeclaration.fwd.h"
#include "cedar/auxiliaries/PluginDeclarationTemplate.fwd.h"

// SYSTEM INCLUDES
#include <map>
#include <vector>
#include <set>
#include <string>

/*!@brief A template for manager of plugin declarations.
 *
 *        This class manages a set of cedar::aux::PluginDeclaration instances. It offers functionality for adding and
 *        querying these instances, as well as allocation capabilities for the classes declared therein.
 */
template <class BaseTypePtr>
class cedar::aux::DeclarationManagerTemplate
{
  // this class is a singleton
  friend class cedar::aux::Singleton<cedar::aux::DeclarationManagerTemplate<BaseTypePtr> >;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::FactoryManager<BaseTypePtr> BaseFactoryManager;
  typedef cedar::aux::Singleton<BaseFactoryManager> BaseFactoryManagerSingleton;
  typedef typename cedar::aux::ConstPtrProvider<BaseTypePtr>::ConstBaseTypePtr ConstBaseTypePtr;

public:
  //! Typedef for the PluginDeclaration template containing a BaseType.
  typedef cedar::aux::PluginDeclarationBaseTemplate<BaseTypePtr> BasePluginDeclaration;

  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES(BasePluginDeclaration);
  //!@endcond SKIPPED_DOCUMENTATION

  //! List of base plugin declarations.
  typedef std::vector<ConstBasePluginDeclarationPtr> BasePluginList;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The standard constructor.
  DeclarationManagerTemplate()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Adds the given declaration to this manager.
  void addDeclaration(ConstBasePluginDeclarationPtr declaration)
  {
    mDeclarations.push_back(declaration);

    const std::string& category = declaration->getCategory();
    if (mDeclarationsByCategory.find(category) == mDeclarationsByCategory.end())
    {
      mDeclarationsByCategory[category] = BasePluginList();
    }

    mDeclarationsByCategory[category].push_back(declaration);
  }

  //! Returns the factory manager for this declaration manager.
  boost::shared_ptr<BaseFactoryManager> getFactoryManager() const
  {
    return BaseFactoryManagerSingleton::getInstance();
  }

  //! Allocates an object of the given class.
  BaseTypePtr allocate(const std::string& className) const
  {
    return this->getFactoryManager()->allocate(className);
  }

  /*! Returns the class name used to allocate the given object as a string.
   */
  std::string getTypeId(BaseTypePtr object) const
  {
    return this->getFactoryManager()->getTypeId(object);
  }

  //! Returns the declaration that was used to instantiate the given object.
  cedar::aux::ConstPluginDeclarationPtr getDeclarationOf(ConstBaseTypePtr object) const
  {
    for (auto declaration : this->mDeclarations)
    {
      if (declaration->isObjectInstanceOf(object))
      {
        return declaration;
      }
    }

    CEDAR_THROW(cedar::aux::UnknownTypeException, "Could not find the declaration for the given object.");
  }

  //! Lists all categories stored in this manager.
  std::set<std::string> listCategories() const
  {
    std::set<std::string> result;
    for
    (
      typename std::map<std::string, BasePluginList>::const_iterator iter = this->mDeclarationsByCategory.begin();
      iter != this->mDeclarationsByCategory.end();
      ++iter
    )
    {
      result.insert(iter->first);
    }
    return result;
  }

  //! Returns all entries for the given category.
  const BasePluginList& getCategoryEntries(const std::string& categoryName) const
  {
    typename std::map<std::string, BasePluginList>::const_iterator iter
      = this->mDeclarationsByCategory.find(categoryName);
    if (iter == this->mDeclarationsByCategory.end())
    {
      CEDAR_THROW(cedar::aux::UnknownNameException, "Category \"" + categoryName + " is not known.");
    }
    return iter->second;
  }

  //! Returns the plugin list that contains all declarations in this manager.
  const BasePluginList& getDeclarations() const
  {
    return this->mDeclarations;
  }

  /*! Returns the declaration corresponding to the given class name.
   * @returns A pointer to the declaration, or a null pointer if none was found.
   */
  ConstBasePluginDeclarationPtr getDeclarationNoThrow(const std::string& className) const
  {
    // look through all declarations and see if there is one whose name matches the given one
    for (auto declaration : this->mDeclarations)
    {
      if (declaration->getClassName() == className)
      {
        return declaration;
      }
    }

    // if none was found, return an empty (null) pointer
    return ConstBasePluginDeclarationPtr();
  }

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
  //! List of all declarations.
  BasePluginList mDeclarations;

  //! By-category storage of declarations.
  std::map<std::string, BasePluginList> mDeclarationsByCategory;

}; // class cedar::aux::DeclarationManagerTemplate

#endif // CEDAR_AUX_DECLARATION_MANAGER_TEMPLATE_H

