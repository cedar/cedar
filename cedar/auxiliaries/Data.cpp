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

    File:        Data.cpp


    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 17

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/utilities.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::Data::Data()
:
mpLock(new QReadWriteLock()),
mpeOwner(NULL)
{
}

cedar::aux::Data::~Data()
{
  delete mpLock;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::Data::serialize(std::ostream& /* stream */, cedar::aux::SerializationFormat::Id /* mode */) const
{
  CEDAR_THROW(cedar::aux::NotImplementedException, "serialize function is not implemented for this type of data");
}

void cedar::aux::Data::deserialize(std::istream& /* stream */, cedar::aux::SerializationFormat::Id /* mode */)
{
  CEDAR_THROW(cedar::aux::NotImplementedException, "deserialize function is not implemented for this type of data");
}

void cedar::aux::Data::serializeData(std::ostream& /*stream*/, cedar::aux::SerializationFormat::Id /* mode */) const
{
  CEDAR_THROW(cedar::aux::NotImplementedException, "serializeData function not implemented for this type of data");
}

void cedar::aux::Data::serializeHeader(std::ostream& /*stream*/, cedar::aux::SerializationFormat::Id /* mode */) const
{
  CEDAR_THROW(cedar::aux::NotImplementedException, "serializeHeader function not implemented for this type of data");
}

QReadWriteLock& cedar::aux::Data::getLock()
{
  return *this->mpLock;
}

QReadWriteLock& cedar::aux::Data::getLock() const
{
  return *this->mpLock;
}

cedar::aux::Configurable* cedar::aux::Data::getOwner() const
{
  return this->mpeOwner;
}

void cedar::aux::Data::setOwner(cedar::aux::Configurable* step)
{
  this->mpeOwner = step;
}

void cedar::aux::Data::copyValueFrom(cedar::aux::ConstDataPtr)
{
  CEDAR_THROW
  (
    cedar::aux::NotImplementedException,
    "Value copying is not implemented for this class. This class is: " + cedar::aux::objectTypeToString(this)
  );
}

cedar::aux::DataPtr cedar::aux::Data::clone() const
{
  CEDAR_THROW
  (
    cedar::aux::NotImplementedException,
    "Cloning is not implemented for the  \"" + cedar::aux::objectTypeToString(this) + "\"."
  );
}
