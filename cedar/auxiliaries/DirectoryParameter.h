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

    File:        DirectoryParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_DIRECTORY_PARAMETER_H
#define CEDAR_AUX_DIRECTORY_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/Settings.h"
#include "cedar/defines.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/DirectoryParameter.fwd.h"

// SYSTEM INCLUDES
#include <QDir>
#include <string>


/*!@brief A parameter for directories on the file system.
 */
class cedar::aux::DirectoryParameter : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*!@brief Sets the mode of the path, i.e., is the path treated as a relative or an absolute path?
   */
  enum PathMode
  {
    //! Paths are specified absolutely.
    PATH_MODE_ABSOLUTE,

    //! Paths are specified relative to the application's current working directory.
    PATH_MODE_RELATIVE_TO_WORKING_DIR,

    PATH_MODE_RELATIVE_TO_CURRENT_ARCHITECTURE_DIR
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  DirectoryParameter(cedar::aux::Configurable *pOwner, const std::string& name)
  :
  cedar::aux::Parameter(pOwner, name, false),
  mPathMode(PATH_MODE_ABSOLUTE)
  {
  }

  //!@brief A variant of the standard constructor, adding a default value
  DirectoryParameter(cedar::aux::Configurable *pOwner, const std::string& name, const std::string& defaultValue)
  :
  cedar::aux::Parameter(pOwner, name, true),
  mValue(QString::fromStdString(defaultValue)),
  mDefault(QString::fromStdString(defaultValue)),
  mPathMode(PATH_MODE_ABSOLUTE)
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
  void readFromNode(const cedar::aux::ConfigurationNode& node)
  {
    this->mValue.setPath(QString::fromStdString(node.get_value<std::string>()));
  }

  //!@brief stores a directory as string in a configuration node
  void writeToNode(cedar::aux::ConfigurationNode& root) const
  {
#ifdef CEDAR_PORTABLE
    root.put(this->getName(), this->mValue.path().toStdString());
#else
    root.put(this->getName(), this->mValue.absolutePath().toStdString());
#endif // CEDAR_PORTABLE
  }

  QDir getCurrentArchitectureFileDirectory() const
  {
    auto lastArchitectureFile = cedar::aux::SettingsSingleton::getInstance()->getCurrentArchitectureFileName();
    QDir curArchitectureDir(QString::fromStdString(lastArchitectureFile));
    curArchitectureDir.cdUp(); // Get the folder not the file
    return curArchitectureDir;
  }

  //!@brief sets a new directory from string
  void setValue(const std::string& value, bool lock = false)
  {
    QDir newValue;
    if(this->getPathMode()==PATH_MODE_RELATIVE_TO_CURRENT_ARCHITECTURE_DIR)
    {
      newValue = this->getCurrentArchitectureFileDirectory().filePath(QString::fromStdString(value));
    }
    else
    {
      newValue = QString::fromStdString(value);
    }

    cedar::aux::Parameter::WriteLockerPtr write_locker;
    if (lock)
    {
      this->lockForWrite();
    }
    this->mValue = newValue;
    if (lock)
    {
      this->unlock();
    }
    this->emitChangedSignal();
  }

  //!@brief sets a new directory from QDir
  void setValue(const QDir& value, bool lock = false)
  {
    if (lock)
    {
      this->lockForWrite();
    }
    this->mValue = value;
    if (lock)
    {
      this->unlock();
    }
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
  std::string getPath(bool forceAbsolutePath = false) const
  {
    //All the Grabberclasses expect this filePath, but maybe this could me removed in the long term. The other return values seem to work, but only during initialisation errors are thrown.
    if(forceAbsolutePath)
    {
      return this->mValue.path().toStdString();
    }

    switch (this->mPathMode)
    {
      default:
      case PATH_MODE_ABSOLUTE:
        return this->mValue.absolutePath().toStdString();

      case PATH_MODE_RELATIVE_TO_WORKING_DIR:
        return QDir::current().relativeFilePath(this->mValue.absolutePath()).toStdString();

      case PATH_MODE_RELATIVE_TO_CURRENT_ARCHITECTURE_DIR:
        return this->getCurrentArchitectureFileDirectory().relativeFilePath(this->mValue.absolutePath()).toStdString();
    }
  }

  /*!@brief Sets the path mode.
   */
  void setPathMode(PathMode mode)
  {
    this->mPathMode = mode;
    this->emitChangedSignal();
  }

  PathMode getPathMode()
  {
    return this->mPathMode;
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

  //!@brief The path mode.
  PathMode mPathMode;

}; // class cedar::aux::DirectoryParameter

#endif // CEDAR_AUX_DIRECTORY_PARAMETER_H
