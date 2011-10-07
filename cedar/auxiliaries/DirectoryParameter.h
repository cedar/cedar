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

    File:        DirectoryParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_DIRECTORY_PARAMETER_H
#define CEDAR_AUX_DIRECTORY_PARAMETER_H

// LOCAL INCLUDES
#include "auxiliaries/ParameterBase.h"
#include "auxiliaries/namespace.h"

// SYSTEM INCLUDES
#include <QDir>
#include <string>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::aux::DirectoryParameter : public cedar::aux::ParameterBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  DirectoryParameter(cedar::aux::Configurable *pOwner, const std::string& name)
  :
  cedar::aux::ParameterBase(pOwner, name, false)
  {
  }

  DirectoryParameter(cedar::aux::Configurable *pOwner, const std::string& name, const std::string& defaultValue)
  :
  cedar::aux::ParameterBase(pOwner, name, true),
  mDefault(defaultValue.c_str())
  {
  }

  //!@brief Destructor
  ~DirectoryParameter()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void setTo(const cedar::aux::ConfigurationNode& node)
  {
    this->mValue.setPath(QString(node.get_value<std::string>().c_str()));
  }

  void putTo(cedar::aux::ConfigurationNode& root)
  {
    root.put(this->getName(), this->mValue.absolutePath().toStdString());
  }

  void set(const std::string& value)
  {
    this->mValue.setPath(QString(value.c_str()));
    emit valueChanged();
  }

  void set(const QDir& value)
  {
    this->mValue = value;
    emit valueChanged();
  }

  void makeDefault()
  {
    this->set(this->mDefault);
  }

  const QDir& get() const
  {
    return this->mValue;
  }

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
protected:
  // none yet
private:
  QDir mValue;
  QDir mDefault;

}; // class cedar::aux::DirectoryParameter

#endif // CEDAR_AUX_DIRECTORY_PARAMETER_H

