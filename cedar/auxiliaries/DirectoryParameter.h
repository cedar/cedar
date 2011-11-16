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
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/namespace.h"

// SYSTEM INCLUDES
#include <QDir>
#include <string>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::aux::DirectoryParameter : public cedar::aux::Parameter
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
  cedar::aux::Parameter(pOwner, name, false)
  {
  }

  //!@brief A variant of the standard constructor, adding a default value
  DirectoryParameter(cedar::aux::Configurable *pOwner, const std::string& name, const std::string& defaultValue)
  :
  cedar::aux::Parameter(pOwner, name, true),
  mDefault(QString::fromStdString(defaultValue))
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
  //!@brief reads a directory from a configuration node
  void setTo(const cedar::aux::ConfigurationNode& node)
  {
    this->mValue.setPath(QString::fromStdString(node.get_value<std::string>()));
  }

  //!@brief stores a directory as string in a configuration node
  void putTo(cedar::aux::ConfigurationNode& root)
  {
    root.put(this->getName(), this->mValue.absolutePath().toStdString());
  }

  //!@brief sets a new directory from string
  void set(const std::string& value)
  {
    this->mValue.setPath(QString::fromStdString(value));
    emit valueChanged();
  }

  //!@brief sets a new directory from QDir
  void set(const QDir& value)
  {
    this->mValue = value;
    emit valueChanged();
  }

  //!@brief sets directory to default value
  void makeDefault()
  {
    this->set(this->mDefault);
  }

  //!@brief get the directory
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
  //!@brief a directory
  QDir mValue;
  //!@brief a default directory
  QDir mDefault;

}; // class cedar::aux::DirectoryParameter

#endif // CEDAR_AUX_DIRECTORY_PARAMETER_H

