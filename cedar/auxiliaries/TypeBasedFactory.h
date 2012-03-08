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

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/FactoryDerived.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/utilities.h"

// SYSTEM INCLUDES
#include <vector>
#include <typeinfo>


/*!@brief A factory that creates objects based on types.
 *
 * @todo This concept needs far more explanation.
 */
template
<
  class KeyBaseType,
  class ValueBaseType,
  typename KeySmartPointerType, // = boost::shared_ptr<KeyBaseType>
  typename ValueSmartPointerType // = boost::shared_ptr<ValueBaseType>
>
class cedar::aux::TypeBasedFactory
{
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Type for a smart pointer to the key type.
  typedef KeySmartPointerType KeyBaseTypePtr;

  //! Type for the factory.
  typedef boost::shared_ptr<cedar::aux::Factory<ValueSmartPointerType> > FactoryPtr;

private:
  //! A pair linking a type info to a factory.
  typedef std::pair<const std::type_info*, FactoryPtr> Pair;

  //! Type for the list of pairs.
  typedef std::vector<Pair> KeyTypes;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Adds an association into the factory.
   *
   *        After calling this function, the type Key can be used to create objects of the type Value.
   */
  template <class Key, class Value>
  void add()
  {
    const std::type_info* type = &typeid(Key);
    FactoryPtr factory(new cedar::aux::FactoryDerived<boost::shared_ptr<ValueBaseType>, boost::shared_ptr<Value> >());
    mKeyTypes.push_back(Pair(type, factory));
  }

  /*!@brief Returns the factory associated with the type of the object pointed to by the parameter.
   */
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
    message += cedar::aux::objectTypeToString(pointer);
    message += ") is not handled in the TypeBasedFactory.";
    CEDAR_THROW(cedar::aux::UnknownTypeException, message);
    return FactoryPtr();
  }

  /*!@brief Checks, whether the factory is empty.
   *
   * @returns True, if no associations are stored in this factory, false otherwise.
   */
  bool empty() const
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
  //! List of associations for this factory.
  KeyTypes mKeyTypes;

}; // class cedar::aux::TypeBasedFactory

#endif // CEDAR_AUX_TYPE_BASED_FACTORY_H
