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

    File:        PluginDeclaration.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 01 18

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/PluginDeclaration.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::PluginDeclaration::PluginDeclaration()
:
mIsDeprecated(false)
{
}

cedar::aux::PluginDeclaration::PluginDeclaration(const std::string& className, const std::string& category)
:
mClassName(className),
mCategory(category),
mIsDeprecated(false)
{
}

cedar::aux::PluginDeclaration::~PluginDeclaration()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

QString cedar::aux::PluginDeclaration::determinedIconPath() const
{
  QResource existance_test(QString::fromStdString(this->getIconPath()));
  if (existance_test.isValid())
  {
    auto icon = QIcon(QString::fromStdString(this->getIconPath()));
    if (icon.isNull())
    {
      return ":/steps/no_icon.svg";
    }
    else
    {
      return QString::fromStdString(this->getIconPath());
    }
  }
  else
  {
    return ":/steps/broken_icon.svg";
  }
}

std::string cedar::aux::PluginDeclaration::getPluginType() const
{
  return std::string();
}

bool cedar::aux::PluginDeclaration::isDeprecatedName(const std::string& name) const
{
  for (size_t i = 0; i < this->mDeprecatedNames.size(); ++i)
  {
    if (this->mDeprecatedNames.at(i) == name)
    {
      return true;
    }
  }

  return false;
}

/*!
 * @brief Returns the class name without the preceding namespace.
 */
std::string cedar::aux::PluginDeclaration::getClassNameWithoutNamespace() const
{
  std::size_t index = this->getClassName().rfind('.');
  if (index != std::string::npos)
  {
    return this->getClassName().substr(index + 1);
  }
  else
  {
    return this->getClassName();
  }
}

/*!
 * @brief Returns the namespace name without the class name.
 */
std::string cedar::aux::PluginDeclaration::getNamespaceName() const
{
  std::size_t index = this->getClassName().rfind('.');
  if (index != std::string::npos)
  {
    return this->getClassName().substr(0, index);
  }
  else
  {
    return this->getClassName();
  }
}

void cedar::aux::PluginDeclaration::read(const cedar::aux::ConfigurationNode& node)
{
  // read custom information
  this->customRead(node);
}

void cedar::aux::PluginDeclaration::customRead(const cedar::aux::ConfigurationNode& /* node */)
{
  // empty default implementation, override in derived classes to read custom information
}

void cedar::aux::PluginDeclaration::setSource(const std::string& source)
{
  this->mSource = source;
}

std::string cedar::aux::PluginDeclaration::getClassName() const
{
  return mClassName;
}
