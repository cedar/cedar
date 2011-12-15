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

    File:        FrameworkSettings.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 26

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_FRAMEWORK_SETTINGS_H
#define CEDAR_PROC_FRAMEWORK_SETTINGS_H

// CEDAR INCLUDES
#include "cedar/processing/namespace.h"
#include "cedar/auxiliaries/Configurable.h"

// SYSTEM INCLUDES
#include <set>


/*!@brief A singleton class for storing user-specific parameters related to the processing framework.
 *
 * @todo  Write a widget for these settings.
 */
class cedar::proc::FrameworkSettings : public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  FrameworkSettings();

  //!@brief The destructor.
  ~FrameworkSettings();

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

  /*!@brief Adds a plugin to the list of plugins known by the processing framework.
   */
  void addKnownPlugin(const std::string& file);

  /*!@brief Returns the set of plugins known by the processing framework.
   */
  const std::set<std::string>& getKnownPlugins() const;

  /*!@brief Returns the set of plugin directories known by the processing framework.
   */
  const std::set<std::string>& getPluginDirectories() const;

  /*!@brief Returns the plugin workspace directory.
   *
   *        This is the first directoriy searched for a plugin.
   */
  std::string getPluginWorkspace() const;

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
  cedar::aux::DirectoryParameterPtr mPluginWorkspace;

  //!@brief List of directories to use when looking for plugins.
  cedar::aux::StringSetParameterPtr mPluginIncludeDirectories;

  //!@brief List of known plugins.
  cedar::aux::StringSetParameterPtr mKnownPlugins;

private:
  // none yet

}; // class cedar::proc::FrameworkSettings

#endif // CEDAR_PROC_FRAMEWORK_SETTINGS_H
