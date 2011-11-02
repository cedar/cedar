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

    File:        Enum.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 05 27

    Description: Base class for enums.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_ENUM_H
#define CEDAR_AUX_ENUM_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <map>
#include <vector>
#include <string>

/*!@brief A class representing entries in an enumeration.
 */
class cedar::aux::Enum
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Enum();

  Enum(const cedar::aux::EnumId id, const std::string& name, const std::string& prettyString = "");

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  bool operator== (const Enum& other);

  cedar::aux::EnumId id() const;

  const std::string& name() const;

  const std::string& prettyString() const;

  operator cedar::aux::EnumId () const;

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
public:
  static const cedar::aux::EnumId UNDEFINED = 0xFFFF;
protected:
  // none yet
private:
  cedar::aux::EnumId mId;

  std::string mName;

  std::string mPrettyString;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::Enum

#endif // CEDAR_AUX_ENUM_H
