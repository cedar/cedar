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

    File:        FileParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 01 18

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_FILE_PARAMETER_H
#define CEDAR_AUX_FILE_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/namespace.h"

// SYSTEM INCLUDES
#include <QDir>
#include <string>

/*!@brief A parameter for directories on the file system.
 */
class cedar::aux::FileParameter : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The mode of the file.
   *
   * This mode is used to declare what you intend to do with this file, i.e., read it or write it. Mainly, this is used
   * to decide whether to show an "open file" or a "save file" dialog.
   */
  enum Mode
  {
    READ,
    WRITE
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  FileParameter(cedar::aux::Configurable *pOwner, const std::string& name, Mode mode)
  :
  cedar::aux::Parameter(pOwner, name, false),
  mMode(mode)
  {
  }

  //!@brief A variant of the standard constructor, adding a default value
  FileParameter(cedar::aux::Configurable *pOwner, const std::string& name, Mode mode, const std::string& defaultValue)
  :
  cedar::aux::Parameter(pOwner, name, true),
  mValue(QString::fromStdString(defaultValue)),
  mDefault(QString::fromStdString(defaultValue)),
  mMode(mode)
  {
  }

  //!@brief Destructor
  ~FileParameter()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief reads a directory from a configuration node
  void readFromNode(const cedar::aux::ConfigurationNode& node)
  {
    this->mValue.setPath(QString::fromStdString(node.get_value<std::string>()));
  }

  //!@brief stores a directory as string in a configuration node
  void writeToNode(cedar::aux::ConfigurationNode& root) const
  {
    root.put(this->getName(), this->mValue.absolutePath().toStdString());
  }

  //!@brief sets a new directory from string
  void setValue(const std::string& value)
  {
    this->mValue.setPath(QString::fromStdString(value));
    this->emitChangedSignal();
  }

  //!@brief sets a new directory from QDir
  void setValue(const QDir& value)
  {
    this->mValue = value;
    this->emitChangedSignal();
  }

  //!@brief sets directory to default value
  void makeDefault()
  {
    this->setValue(this->mDefault);
  }

  //!@brief get the directory
  const QDir& getValue() const
  {
    return this->mValue;
  }

  /*!@brief Returns the path stored in the parameter.
   *
   * @remarks This is an alias for calling getValue().path().toStdString(). See the documentation for QDir for details.
   */
  std::string getPath()
  {
    return this->mValue.path().toStdString();
  }

  CEDAR_DECLARE_DEPRECATED(void set(const std::string& value))
  {
    this->setValue(value);
  }

  CEDAR_DECLARE_DEPRECATED(void set(const QDir& value))
  {
    this->setValue(value);
  }

  CEDAR_DECLARE_DEPRECATED(const QDir& get())
  {
    return this->getValue();
  }

  Mode getMode() const
  {
    return this->mMode;
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
  //!@brief The selected file path.
  QDir mValue;

  //!@brief The default file.
  QDir mDefault;

  //!@brief Mode of the file
  Mode mMode;
}; // class cedar::aux::DirectoryParameter

#endif // CEDAR_AUX_FILE_PARAMETER_H