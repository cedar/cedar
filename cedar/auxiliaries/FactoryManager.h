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
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/FactoryDerived.h"

// SYSTEM INCLUDES
#include <map>


/*!@todo describe.
 *
 * @todo describe more.
 */
template <class BaseTypePtr>
class cedar::aux::FactoryManager
{
  // this class is a singleton
  friend class cedar::aux::Singleton<FactoryManager<BaseTypePtr> >;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  typedef typename boost::shared_ptr< cedar::aux::Factory<BaseTypePtr> > FactoryTypePtr;
  typedef typename BaseTypePtr::element_type BaseType;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The private constructor.
  FactoryManager()
  {
  }

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  template <class TypePtr>
  bool registerType(std::string specifiedTypeName = "")
  {
    std::string generatedTypeName = cedar::aux::typeToString<typename TypePtr::element_type>();

    // if no type name is supplied, generate type name from actual type
    if (specifiedTypeName.empty())
    {
      specifiedTypeName = cedar::aux::replace(generatedTypeName, "::", ".");
    }

    mTypeNameMapping[generatedTypeName] = specifiedTypeName;

    // check if typename exists
    if (mRegisteredFactories.find(specifiedTypeName) != mRegisteredFactories.end())
    {
      CEDAR_THROW(cedar::aux::DuplicateNameException,
        "A factory already exists for the type name \"" + specifiedTypeName + "\".");
    }

    mRegisteredFactories[specifiedTypeName] = FactoryTypePtr(new cedar::aux::FactoryDerived<BaseTypePtr, TypePtr>());

    return true;
  }

  BaseTypePtr allocate(const std::string& typeName)
  {
    typename std::map<std::string, FactoryTypePtr>::const_iterator iter = mRegisteredFactories.find(typeName);

    if (iter == mRegisteredFactories.end())
    {
      CEDAR_THROW(cedar::aux::UnknownTypeException,
        "No factory is registered for the type name \"" + typeName + "\".");
    }

    return iter->second->allocate();
  }

  std::string getTypeId(BaseTypePtr object)
  {
    std::string generated_type_name = cedar::aux::objectTypeToString(object);

    std::map<std::string, std::string>::iterator iter = mTypeNameMapping.find(generated_type_name);
    if (iter == mTypeNameMapping.end())
    {
      CEDAR_THROW
      (
        cedar::aux::UnknownTypeException,
        "The type name of the object of type \"" + cedar::aux::objectTypeToString(object)
        + "\" could not be determined. This most likely means that the type is not registered with the factory manager "
        + cedar::aux::objectTypeToString(this)
      );
    }

    return iter->second;
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
  std::map<std::string, FactoryTypePtr> mRegisteredFactories;
  std::map<std::string, std::string> mTypeNameMapping;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::FactoryManager

#endif // CEDAR_AUX_FACTORY_MANAGER_H
