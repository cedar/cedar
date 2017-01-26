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

    File:        Enum.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 05 27

    Description: Base class for enums.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_ENUM_H
#define CEDAR_AUX_ENUM_H

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/Enum.fwd.h"

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
  //!@brief The standard constructor, needed for std data structures.
  Enum();

  //!@brief constructor with all parameters for this entry
  Enum
  (
    const cedar::aux::EnumId id,
    const std::string& name,
    const std::string& prettyString = "",
    const std::string& explanatoryString = ""
  );

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief comparison operator
  bool operator== (const Enum& other) const;

  //!@brief returns the identification number of this enum entry
  cedar::aux::EnumId id() const;

  //!@brief returns the name of this enum entry
  const std::string& name() const;

  //!@brief returns a beautified name of this enum entry
  const std::string& prettyString() const;

  //!@brief returns an explanatory string for this enum entry
  const std::string& explanatoryString() const;

  //!@brief returns the identification number of this enum entry
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
  //!@brief the id for an unknown entry
  static const cedar::aux::EnumId UNDEFINED = 0xFFFF;
protected:
  // none yet
private:
  //!@brief the identification number of this enum entry
  cedar::aux::EnumId mId;
  //!@brief the name of this enum entry
  std::string mName;
  //!@brief the beautified name of this enum entry
  std::string mPrettyString;
  //!@brief the explanatory string of this enum entry
  std::string mExplanatoryString;
}; // class cedar::aux::Enum

#endif // CEDAR_AUX_ENUM_H
