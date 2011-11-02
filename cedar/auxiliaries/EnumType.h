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

    File:        EnumType.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 28

    Description: Type class for enums.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_ENUM_TYPE_H
#define CEDAR_AUX_ENUM_TYPE_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/EnumBase.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <map>
#include <vector>
#include <string>

/*!@brief A base class for making enums that are more flexible than the standard ones.
 *
 * To make an enum using this class, create a new class that has a static construct function. In the construct function,
 * declare all the enum values belonging to your enum.
 * @todo explain this better and add example code
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
  boost::shared_ptr<cedar::aux::EnumBase>& type()
  {
    return this->mType;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // private members
  //--------------------------------------------------------------------------------------------------------------------
private:
  boost::shared_ptr<cedar::aux::EnumBase> mType;
}; // class cedar::aux::EnumType

#endif // CEDAR_AUX_ENUM_TYPE_H
