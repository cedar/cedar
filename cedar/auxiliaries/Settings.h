/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Settings.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2013 06 21

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_SETTINGS_H
#define CEDAR_AUX_SETTINGS_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/BoolParameter.h"

// SYSTEM INCLUDES

/*!@brief A singleton class for storing user-specific parameters related to the processing framework.
 *
 * @todo  Write a widget for these settings.
 */
class cedar::aux::Settings : public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // friend
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::Singleton<cedar::aux::Settings>;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The standard constructor.
  Settings();

public:
  //!@brief The destructor.
  ~Settings();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Loads the settings from a file in the user's home directory.
   */
  void load();

  /*!@brief Saves the settings to a file in the user's home directory.
   */
  void save();

  //! Whether or not memory output is generated.
  bool getMemoryDebugOutput();

  //!@brief Returns the directory where the recorder will save the recorded files.
  std::string getRecorderOutputDirectory() const;

  //! Returns the parameter that contains the recorder's output directory.
  cedar::aux::DirectoryParameterPtr getRecorderWorkspaceParameter() const;

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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Parameter representing the plugin workspace.
  cedar::aux::BoolParameterPtr _mMemoryDebugOutput;

  //!@brief Parameter representing where recorded files will be saved.
  cedar::aux::DirectoryParameterPtr mRecorderWorkspace;

private:
  // none yet

}; // class cedar::aux::Settings

CEDAR_AUX_SINGLETON(Settings);

#endif // CEDAR_AUX_SETTINGS_H
