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

    File:        SerializationFormat.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2015 07 20

    Description: Source file for the class cedar::aux::SerializationFormat.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/auxiliaries/SerializationFormat.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::aux::SerializationFormat>
  cedar::aux::SerializationFormat::mType("cedar::aux::SerializationFormat::");

#ifndef CEDAR_COMPILER_MSVC
const cedar::aux::SerializationFormat::Id cedar::aux::SerializationFormat::CSV;
const cedar::aux::SerializationFormat::Id cedar::aux::SerializationFormat::Compact;
#endif // CEDAR_COMPILER_MSVC

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::SerializationFormat::construct()
{
  mType.type()->def(cedar::aux::Enum(cedar::aux::SerializationFormat::CSV, "CSV", "CSV"));
  mType.type()->def(cedar::aux::Enum(cedar::aux::SerializationFormat::Compact, "Compact", "Compact"));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

const cedar::aux::EnumBase& cedar::aux::SerializationFormat::type()
{
  return *cedar::aux::SerializationFormat::mType.type();
}

const cedar::aux::SerializationFormat::TypePtr& cedar::aux::SerializationFormat::typePtr()
{
  return cedar::aux::SerializationFormat::mType.type();
}
