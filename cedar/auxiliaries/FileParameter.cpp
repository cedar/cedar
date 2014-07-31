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

    File:        FileParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 05

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/FileParameter.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::FileParameter::FileParameter
(
  cedar::aux::Configurable *pOwner,
  const std::string& name,
  cedar::aux::FileParameter::Mode mode
)
:
cedar::aux::Parameter(pOwner, name, false),
mMode(mode),
mPathMode(cedar::aux::FileParameter::PATH_MODE_ABSOLUTE)
{
}

cedar::aux::FileParameter::FileParameter
(
  cedar::aux::Configurable *pOwner,
  const std::string& name,
  cedar::aux::FileParameter::Mode mode,
  const std::string& defaultValue
)
:
cedar::aux::Parameter(pOwner, name, true),
mValue(QString::fromStdString(defaultValue)),
mDefault(QString::fromStdString(defaultValue)),
mMode(mode),
mPathMode(cedar::aux::FileParameter::PATH_MODE_ABSOLUTE)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::FileParameter::readFromNode(const cedar::aux::ConfigurationNode& node)
{
  this->mValue.setPath(QString::fromStdString(node.get_value<std::string>()));
}

void cedar::aux::FileParameter::writeToNode(cedar::aux::ConfigurationNode& root) const
{
  root.put(this->getName(), this->getPath());
}

void cedar::aux::FileParameter::setValue(const std::string& value)
{
  this->mValue = QString::fromStdString(value);

  this->emitChangedSignal();
}

void cedar::aux::FileParameter::setValue(const QDir& value)
{
  this->mValue = value;

  this->emitChangedSignal();
}

void cedar::aux::FileParameter::makeDefault()
{
  this->setValue(this->mDefault);
}

const QDir& cedar::aux::FileParameter::getValue() const
{
  return this->mValue;
}

std::string cedar::aux::FileParameter::getPath() const
{
  switch (this->mPathMode)
  {
    default:
    case PATH_MODE_ABSOLUTE:
      return this->mValue.path().toStdString();

    case PATH_MODE_RELATIVE_TO_WORKING_DIR:
      return QDir::current().relativeFilePath(this->mValue.absolutePath()).toStdString();
  }
}

cedar::aux::FileParameter::Mode cedar::aux::FileParameter::getMode() const
{
  return this->mMode;
}

void cedar::aux::FileParameter::setPathMode(cedar::aux::FileParameter::PathMode mode)
{
  this->mPathMode = mode;
}
