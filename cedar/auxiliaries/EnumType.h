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

    File:        EnumType.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 28

    Description: Type class for enums.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_ENUM_TYPE_H
#define CEDAR_AUX_ENUM_TYPE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumBase.h"

// SYSTEM INCLUDES
#include <map>
#include <vector>
#include <string>

/*!@brief A base class for making enums that are more flexible than the standard ones.
 *
 * To make an enum using this class, create a new class that has a static construct function. In the construct function,
 * declare all the enum values belonging to your enum.
 *
 * This is probably best explained with an example:
 *
 * @code
class ExampleEnum
{
  public:
    typedef cedar::aux::EnumId Id;
    typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

    static const Id VALUE1 = 0;
    static const Id VALUE2 = 1;
    static const Id VALUE3 = 2;

    // this function must always be static, void and called construct.
    static void construct()
    {
      mType.type()->def(cedar::aux::Enum(VALUE1, "VALUE1", "What you want to show to the user for value 1"));
      mType.type()->def(cedar::aux::Enum(VALUE2, "VALUE2"));
      mType.type()->def(cedar::aux::Enum(VALUE3, "VALUE3", "I'm different."));
    }

  private:
    static cedar::aux::EnumType<ExampleEnum> mType;
};
 * @endcode
 */
template <class T>
class cedar::aux::EnumType
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  EnumType(const std::string& prefix = "")
  :
  mType(new cedar::aux::EnumBase(prefix))
  {
    T::construct();
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief returns the type / specific enum
  boost::shared_ptr<cedar::aux::EnumBase>& type()
  {
    return this->mType;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // private members
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief the type / specific enum
  boost::shared_ptr<cedar::aux::EnumBase> mType;
}; // class cedar::aux::EnumType

#endif // CEDAR_AUX_ENUM_TYPE_H
