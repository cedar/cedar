/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumParameter.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumParameter::EnumParameter(cedar::aux::Configurable *pOwner,
                                         const std::string& name,
                                         boost::shared_ptr<cedar::aux::EnumBase> enumBase)
:
cedar::aux::Parameter(pOwner, name, false),
mEnumDeclaration(enumBase)
{
}

cedar::aux::EnumParameter::EnumParameter(cedar::aux::Configurable *pOwner,
                                         const std::string& name,
                                         boost::shared_ptr<cedar::aux::EnumBase> enumBase,
                                         cedar::aux::EnumId defaultValue)
:
cedar::aux::Parameter(pOwner, name, true),
mDefault(defaultValue),
mEnumDeclaration(enumBase)
{
  this->makeDefault();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::EnumParameter::disable(cedar::aux::EnumId value)
{
  this->mDisabledValues.insert(value);
  if (!this->isEnabled(this->mValue))
  {
    this->selectFirstEnabled();
  }
}

void cedar::aux::EnumParameter::enable(cedar::aux::EnumId value)
{
  std::set<cedar::aux::EnumId>::iterator iter = this->mDisabledValues.find(value);
  if (iter != this->mDisabledValues.end())
  {
    this->mDisabledValues.erase(iter);
  }

  // if the current value is one of the disabled ones (this can happen, e.g., if all are disabled) select a new one
  if (!this->isEnabled(this->mValue))
  {
    this->selectFirstEnabled();
  }
}

void cedar::aux::EnumParameter::selectFirstEnabled()
{
  for (size_t i = 0; i < this->mEnumDeclaration->list().size(); ++i)
  {
    if (this->isEnabled(this->mEnumDeclaration->list().at(i)))
    {
      this->setValue(this->mEnumDeclaration->list().at(i).name());
      break;
    }
  }
}

void cedar::aux::EnumParameter::setEnabled(cedar::aux::EnumId value, bool enabled)
{
  if (enabled)
  {
    this->enable(value);
  }
  else
  {
    this->disable(value);
  }
}

void cedar::aux::EnumParameter::enableAll()
{
  this->mDisabledValues.clear();
}

void cedar::aux::EnumParameter::disableAll()
{
  for (size_t i = 0; i < this->mEnumDeclaration->list().size(); ++i)
  {
    this->disable(this->mEnumDeclaration->list().at(i));
  }
}

bool cedar::aux::EnumParameter::isEnabled(cedar::aux::EnumId value) const
{
  return this->mDisabledValues.find(value) == this->mDisabledValues.end();
}

cedar::aux::Enum cedar::aux::EnumParameter::getValue() const
{
  return this->mEnumDeclaration->get(this->mValue);
}

void cedar::aux::EnumParameter::setValue(cedar::aux::EnumId enumId, bool lock)
{
  if (lock)
  {
    this->lockForRead();
  }
  this->mValue = enumId;
  if (lock)
  {
    this->unlock();
  }
  this->emitChangedSignal();
}

void cedar::aux::EnumParameter::setValue(const std::string& enumId, bool lock)
{
  this->setValue(this->mEnumDeclaration->get(enumId), lock);
}

void cedar::aux::EnumParameter::readFromNode(const cedar::aux::ConfigurationNode& root)
{
  this->mValue = mEnumDeclaration->get(root.get_value<std::string>());
  this->emitChangedSignal();
}

void cedar::aux::EnumParameter::writeToNode(cedar::aux::ConfigurationNode& root) const
{
  root.put(this->getName(), this->getValue().name());
}

void cedar::aux::EnumParameter::makeDefault()
{
  this->mValue = this->mDefault;
  this->emitChangedSignal();
}
