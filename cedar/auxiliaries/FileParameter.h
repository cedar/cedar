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
#include "cedar/auxiliaries/Settings.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/FileParameter.fwd.h"

// SYSTEM INCLUDES
#include <QDir>
#include <string>

/*!@brief A parameter for directories on the file system.
 *
 *@todo Why doesn't this inherit ParameterTemplate<cedar::aux::Path>?
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
  FileParameter(cedar::aux::Configurable *pOwner = nullptr, const std::string& name = "", Mode mode = READ);

  //!@brief A variant of the standard constructor, adding a default value
  FileParameter(cedar::aux::Configurable *pOwner, const std::string& name, Mode mode, const std::string& defaultValue);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief reads a directory from a configuration node
  void readFromNode(const cedar::aux::ConfigurationNode& node);

  //!@brief stores a directory as string in a configuration node
  void writeToNode(cedar::aux::ConfigurationNode& root) const;

  //! Copies the value from another (file) parameter.
  void copyValueFrom(cedar::aux::ConstParameterPtr other);

  //! Checks if the value of the given parameter can be copied into this one.
  bool canCopyFrom(cedar::aux::ConstParameterPtr other ) const;

  //!@brief sets a new directory from string
  void setValue(const std::string& value);

  //!@brief sets a new directory from QDir
  void setValue(const QDir& value);

  //!@brief sets directory to default value
  void makeDefault();

  //!@brief get the directory
  const QDir& getValue() const;

  /*!@brief Returns the path stored in the parameter.
   *
   * @remarks This is an alias for calling getValue().path().toStdString(). See the documentation for QDir for details.
   */
  std::string getPath(bool forceAbsolutePath = false) const;

  /*!@brief Returns the mode of the file parameter.
   */
  cedar::aux::FileParameter::Mode getMode() const;

  /*!@brief Sets the path mode.
   */
  void setPathMode(cedar::aux::FileParameter::PathMode mode);

  cedar::aux::FileParameter::PathMode getPathMode();

  void setPathModeConstant(bool isConstant);

  bool isPathModeConstant();

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
  QDir getCurrentArchitectureFileDirectory() const;

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

  //!@brief The path mode.
  PathMode mPathMode;

  bool mConstantPathMode;

}; // class cedar::aux::DirectoryParameter

#endif // CEDAR_AUX_FILE_PARAMETER_H
