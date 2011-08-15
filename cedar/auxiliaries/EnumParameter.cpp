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

    File:        EnumParameter.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 28

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "auxiliaries/EnumParameter.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumParameter::EnumParameter(const std::string& name,
                                         boost::shared_ptr<cedar::aux::EnumBase> enumBase)
:
cedar::aux::ParameterBase(name, false),
mEnumDeclaration(enumBase)
{
}

cedar::aux::EnumParameter::EnumParameter(const std::string& name,
                                         boost::shared_ptr<cedar::aux::EnumBase> enumBase,
                                         cedar::aux::EnumId defaultValue)
:
cedar::aux::ParameterBase(name, true),
mDefault(defaultValue),
mEnumDeclaration(enumBase)
{
  this->makeDefault();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::Enum cedar::aux::EnumParameter::get() const
{
  return mEnumDeclaration->get(this->mValue);
}

void cedar::aux::EnumParameter::set(const std::string& enumId)
{
  this->mValue = this->mEnumDeclaration->get(enumId);
  emit parameterChanged();
}

void cedar::aux::EnumParameter::setTo(const cedar::aux::ConfigurationNode& root)
{
  this->mValue = mEnumDeclaration->get(root.get_value<std::string>());
  emit parameterChanged();
}

void cedar::aux::EnumParameter::putTo(cedar::aux::ConfigurationNode& root)
{
  root.put(this->getName(), this->get().name());
}

void cedar::aux::EnumParameter::makeDefault()
{
  this->mValue = this->mDefault;
  emit parameterChanged();
}
