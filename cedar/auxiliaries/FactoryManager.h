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

    File:        FactoryManager.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 02 16

    Description: Manager for factories.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_FACTORY_MANAGER_H
#define CEDAR_AUX_FACTORY_MANAGER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/FactoryDerived.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/boostConstPointerHelper.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/FactoryManager.fwd.h"

// SYSTEM INCLUDES
#include <map>
#include <set>
#include <vector>
#include <string>


/*!@brief A manager of factories.
 *
 * @tparam BaseTypePtr The type of pointer returned by the managed factories.
 */
template <class BaseTypePtr>
class cedar::aux::FactoryManager
{
  // this class is a singleton
  friend class cedar::aux::Singleton<FactoryManager<BaseTypePtr> >;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef typename boost::shared_ptr< cedar::aux::Factory<BaseTypePtr> > FactoryTypePtr;
  typedef typename BaseTypePtr::element_type BaseType;
  typedef const typename BaseTypePtr::element_type ConstBaseType;
  typedef typename cedar::aux::ConstPtrProvider<BaseTypePtr>::ConstBaseTypePtr ConstBaseTypePtr;

  typedef std::map<std::string, std::vector<FactoryTypePtr> > CategoryMap;

  struct FactoryRecord
  {
    FactoryTypePtr factory;
    bool deprecated;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The private constructor.
  FactoryManager()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief register a new type at the factory manager
  template <class TypePtr>
  bool registerType(const std::string& specifiedTypeName = std::string())
  {
    std::string used_type_name = specifiedTypeName;
    // if no type name is supplied, generate type name from actual type
    if (specifiedTypeName.empty())
    {
      used_type_name = this->generateTypeName<TypePtr>();
    }

    // store mapping from type name to specified name
    mTypeNameMapping[this->getTypeKey<TypePtr>()] = used_type_name;

    // check if typename exists
    if (mRegisteredFactories.find(used_type_name) != mRegisteredFactories.end())
    {
      CEDAR_THROW(cedar::aux::DuplicateNameException,
        "A factory already exists for the type name \"" + used_type_name + "\".");
    }

    FactoryTypePtr factory(new cedar::aux::FactoryDerived<BaseTypePtr, TypePtr>());
    FactoryRecord factory_record;
    factory_record.factory = factory;
    factory_record.deprecated = false;
    mRegisteredFactories[used_type_name] = factory_record;

    return true;
  }

  //! Deprecates the given class.
  template <class TypePtr>
  void deprecate()
  {
    std::string generated_name = this->getTypeKey<TypePtr>();
    auto name_iter = mTypeNameMapping.find(generated_name);
    if (name_iter == mTypeNameMapping.end())
    {
      CEDAR_THROW(cedar::aux::UnknownTypeException, "No factory is known for the type \"" + generated_name + "\".");
    }

    const std::string& name = name_iter->second;

    // check if typename exists
    auto record_iter = mRegisteredFactories.find(name);
    if (record_iter == mRegisteredFactories.end())
    {
      CEDAR_THROW(cedar::aux::UnknownTypeException, "The type \"" + name + "\" is not registered.");
    }

    record_iter->second.deprecated = true;
  }

  //! Adds a deprecated name for the given type.
  template <class TypePtr>
  void addDeprecatedName(const std::string& deprecatedName)
  {
    std::string class_id = this->generateTypeName<TypePtr>();
    this->addDeprecatedName(class_id, deprecatedName);
  }

  //! Adds a deprecated name for the given class id.
  void addDeprecatedName(const std::string classId, const std::string& deprecatedName)
  {
    auto iter = this->mDeprecatedNames.find(deprecatedName);
    // check if the deprecated name exists
    if (iter != this->mDeprecatedNames.end())
    {
      // if it is different from the new one
      if (iter->second != classId)
      {
        CEDAR_THROW
        (
          cedar::aux::DuplicateNameException,
          "The deprecated name \"" + deprecatedName + "\" is already used for a different type."
        );
      }
    }
    this->mDeprecatedNames[deprecatedName] = classId;
  }

  //! Converts the given class's type to a string.
  template <class TypePtr>
  std::string generateTypeName() const
  {
    return cedar::aux::replace(this->getTypeKey<TypePtr>(), "::", ".");
  }

  //!@brief allocate a new object of the given type
  BaseTypePtr allocate(const std::string& typeName)
  {
    auto iter = mRegisteredFactories.find(typeName);

    if (iter == mRegisteredFactories.end())
    {
      auto depr_iter = this->mDeprecatedNames.find(typeName);
      if (depr_iter != this->mDeprecatedNames.end())
      {
        const std::string deprecated_name = depr_iter->first;
        const std::string new_name = depr_iter->second;

        cedar::aux::LogSingleton::getInstance()->warning
        (
          "Allocating object with deprecated type name \"" + deprecated_name + "\". New name is: \"" + new_name + "\".",
          "cedar::aux::FactoryManager::allocate(const std::string& typeName)"
        );

        return this->allocate(new_name);
      }
      else
      {
        CEDAR_THROW
        (
          cedar::aux::UnknownTypeException,
          "No factory is registered for the type name \"" + typeName + "\"."
        );
      }
    }

    auto factory_record = iter->second;
    if (factory_record.deprecated)
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Allocating deprecated type \"" + typeName + "\".",
        "cedar::aux::FactoryManager::allocate(const std::string& typeName)"
      );
    }

    return factory_record.factory->allocate();
  }

  //!@brief look up the type id of an object using a smart pointer
  const std::string& getTypeId(ConstBaseTypePtr object) const
  {
    return this->getTypeId(object.get());
  }

  //!@brief look up the type id of an object using a raw pointer
  const std::string& getTypeId(ConstBaseType* pObject) const
  {
    std::string generated_type_name = cedar::aux::objectTypeToString(pObject);

    std::map<std::string, std::string>::const_iterator iter = mTypeNameMapping.find(generated_type_name);
    if (iter == mTypeNameMapping.end())
    {
      CEDAR_THROW
      (
        cedar::aux::UnknownTypeException,
        "The type name of the object of type \"" + generated_type_name
        + "\" could not be determined. This most likely means that the type is not registered with the factory manager "
        + cedar::aux::objectTypeToString(this)
      );
    }

    return iter->second;
  }

  //!@brief list all types registered at the factory manager
  void listTypes(std::vector<std::string>& types) const
  {
    for(auto iter = this->mRegisteredFactories.begin(); iter != this->mRegisteredFactories.end(); ++iter)
    {
      types.push_back(iter->first);
    }
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
  template <typename TypePtr>
  std::string getTypeKey() const
  {
    return cedar::aux::typeToString<typename TypePtr::element_type>();
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  std::map<std::string, FactoryRecord> mRegisteredFactories;

  std::map<std::string, std::string> mTypeNameMapping;

  //! map from deprecated name to new name
  std::map<std::string, std::string> mDeprecatedNames;

}; // class cedar::aux::FactoryManager

#endif // CEDAR_AUX_FACTORY_MANAGER_H
