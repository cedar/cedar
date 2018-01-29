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
#include "cedar/auxiliaries/FactoryDerived.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/utilities.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/TypeBasedFactory.fwd.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>
#include <typeinfo>


/*!@brief A factory that creates objects based on types.
 */
template
<
  typename KeyBasePtr,
  typename ValueBasePtr
>
class cedar::aux::TypeBasedFactory
{
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief type of the key
  typedef typename KeyBasePtr::element_type KeyBaseType;
  //!@brief type of the value
  typedef typename ValueBasePtr::element_type ValueBaseType;

  //! Type for the factory.
  typedef boost::shared_ptr<cedar::aux::Factory<ValueBasePtr> > FactoryPtr;

private:
  //! Struct for the entries in the factory.
  struct Entry
  {
    virtual ~Entry()
    {
    }

    //! Type info for identifying exact matches of types.
    const std::type_info* mTypeInfo;

    //! The factory for this type.
    FactoryPtr mFactory;

    virtual bool matches(KeyBasePtr pointer) const
    {
      return *this->mTypeInfo == typeid(*pointer);
    }
  };

  CEDAR_GENERATE_POINTER_TYPES(Entry);

  template <typename T>
  struct DerivableEntry : public Entry
  {
    bool matches(KeyBasePtr pointer) const
    {
      return static_cast<bool>(boost::dynamic_pointer_cast<T>(pointer));
    }
  };


  //! Type for the list of pairs.
  typedef std::vector<EntryPtr> KeyTypes;

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
  bool add()
  {
    EntryPtr entry(new Entry());
    entry->mTypeInfo = &typeid(Key);
    entry->mFactory = FactoryPtr
                      (
                        new cedar::aux::FactoryDerived<boost::shared_ptr<ValueBaseType>, boost::shared_ptr<Value> >()
                      );
    mKeyTypes.push_back(entry);

    // this always returns true to allow the use of static initalization for registering types.
    return true;
  }

  /*!@brief Adds an association into the factory.
   *
   *        After calling this function, objects of classes inheriting the type Key can be used to create objects of the
   *        type Value.
   */
  template <class Key, class Value>
  bool addDerived()
  {
    DerivableEntry<Key> *entry = new DerivableEntry<Key>();
    entry->mTypeInfo = &typeid(Key);
    entry->mFactory = FactoryPtr
                      (
                        new cedar::aux::FactoryDerived<boost::shared_ptr<ValueBaseType>, boost::shared_ptr<Value> >()
                      );
    mKeyTypes.push_back(EntryPtr(entry));

    // this always returns true to allow the use of static initalization for registering types.
    return true;
  }

  /*!@brief Returns the factory associated with the type of the object pointed to by the parameter.
   */
  FactoryPtr get(KeyBasePtr pointer)
  {
    for(typename KeyTypes::iterator iter = this->mKeyTypes.begin(); iter != this->mKeyTypes.end(); ++iter)
    {
      if ((*iter)->matches(pointer))
      {
        return (*iter)->mFactory;
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
