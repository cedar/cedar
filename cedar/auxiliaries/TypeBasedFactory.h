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

    File:        TypeBasedFactory.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 03 09

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_TYPE_BASED_FACTORY_H
#define CEDAR_AUX_TYPE_BASED_FACTORY_H

// LOCAL INCLUDES
#include "auxiliaries/namespace.h"
#include "auxiliaries/FactoryDerived.h"
#include "auxiliaries/exceptions.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <vector>
#include <typeinfo>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
template <class KeyBaseType, class ValueBaseType>
class cedar::aux::TypeBasedFactory
{
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
public:
  typedef boost::shared_ptr<KeyBaseType> KeyBaseTypePtr;
  typedef boost::shared_ptr<cedar::aux::Factory<ValueBaseType> > FactoryPtr;

private:
  typedef std::pair<const std::type_info*, FactoryPtr> Pair;
  typedef std::vector<Pair> KeyTypes;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  template <class Key, class Value>
  void add()
  {
    const std::type_info* type = &typeid(Key);
    FactoryPtr factory(new cedar::aux::FactoryDerived<ValueBaseType, Value>());
    mKeyTypes.push_back(Pair(type, factory));
  }

  FactoryPtr get(KeyBaseTypePtr pointer)
  {
    for(typename KeyTypes::iterator iter = this->mKeyTypes.begin(); iter != this->mKeyTypes.end(); ++iter)
    {
      if (*(iter->first) == typeid(*pointer.get()))
      {
        return iter->second;
      }
    }
    std::string message = "Type of the base pointer (";
    message += typeid(pointer.get()).name();
    message += ") is not handled in the TypeBasedFactory.";
    CEDAR_THROW(cedar::aux::UnknownTypeException, message);
    return FactoryPtr();
  }

  bool empty()
  {
    return this->mKeyTypes.empty();
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
  KeyTypes mKeyTypes;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::TypeBasedFactory

#endif // CEDAR_XXX_XXX_H

