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

    File:        SerializationFormat.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2015 07 20

    Description: Header file for the class cedar::aux::SerializationFormat.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_SERIALIZATION_FORMAT_H
#define CEDAR_AUX_SERIALIZATION_FORMAT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumType.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/SerializationFormat.fwd.h"

// SYSTEM INCLUDES


/*!@brief Enum class for the serialization format of cedar::aux::Data.
 */
class cedar::aux::SerializationFormat
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Type of the enum.
  typedef cedar::aux::EnumId Id;
public:
  //! Pointer to the enumeration type.
  typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Constructs the enumeration values.
  static void construct();

  //! Returns the enum base class.
  static const cedar::aux::EnumBase& type();

  //! Returns a pointer to the enum base class.
  static const cedar::aux::SerializationFormat::TypePtr& typePtr();

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
  //! Write data as CSV.
  static const Id CSV = 0;

  //! Write data in a compact (binary) format.
  static const Id Compact = 1;

protected:
  // none yet
private:
  static cedar::aux::EnumType<cedar::aux::SerializationFormat> mType;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::SerializationFormat

#endif // CEDAR_AUX_SERIALIZATION_FORMAT_H

