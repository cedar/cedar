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

    File:        Enum.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 05 27

    Description: Base class for enums.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "Enum.h"

// SYSTEM INCLUDES


cedar::aux::Enum::Enum()
{
}

cedar::aux::Enum::Enum
(
  const cedar::aux::EnumId id,
  const std::string& name,
  const std::string& prettyString,
  const std::string& explanatoryString
)
:
mId(id),
mName(name),
mPrettyString(prettyString),
mExplanatoryString(explanatoryString)
{
}

bool cedar::aux::Enum::operator== (const cedar::aux::Enum& other) const
{
  return this->mId == other.mId;
}

cedar::aux::EnumId cedar::aux::Enum::id() const
{
  return this->mId;
}

const std::string& cedar::aux::Enum::name() const
{
  return this->mName;
}

const std::string& cedar::aux::Enum::prettyString() const
{
  if (!this->mPrettyString.empty())
  {
    return this->mPrettyString;
  }
  else
  {
    return this->mName;
  }
}

const std::string& cedar::aux::Enum::explanatoryString() const
{
  return this->mExplanatoryString;
}

cedar::aux::Enum::operator cedar::aux::EnumId () const
{
  return this->mId;
}
