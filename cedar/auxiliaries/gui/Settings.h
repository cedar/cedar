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

    File:        Settings.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 09 14

    Description: Header file for the gui settings in cedar's auxiliaries library.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_SETTINGS_H
#define CEDAR_AUX_GUI_SETTINGS_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/DirectoryParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/StringParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/Settings.fwd.h"

// SYSTEM INCLUDES
#include <string>


/*!@brief User interface settings for widgets in the aux namespace.
 *
 *        This class provides (singleton) access to settings that concern the user interface classes located in the
 *        cedar::aux namespace.
 *
 *        It takes care of storing and restoring the configuration to and from a file located in the user's home
 *        directory.
 */
class cedar::aux::gui::Settings : public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::Singleton<cedar::aux::gui::Settings>;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The standard constructor.
  Settings();

public:
  //!@brief Destructor
  ~Settings();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief loads the UI settings
  void load();

  //!@brief saves the UI settings
  void save();

  //!@brief returns the last directory, from which a plugin was loaded
  cedar::aux::DirectoryParameterPtr lastPluginLoadDialogLocation() const;

  //! Sets the last directory from which a plugin was loaded.
  void setLastPluginDialogLocation(const std::string& location);

  //! Disables writing.
  void disableWriting(bool disable = true)
  {
    this->mWritingDisabled = disable;
  }

  //! Whether or not memory output is generated.
  unsigned int getMaximumNumberOfLogEntries() const;

  //! Returns the maximal number of entries allowed for the log entry.
  cedar::aux::UIntParameterPtr getMaximalNumberOfLogEntriesParameter() const;

  //! Returns the default plot to be opened for 2d mat data.
  std::string getDefault2dMatDataPlot() const;

  //! Sets the default plot to be opened for 2d mat data.
  void setDefault2dMatDataPlot(const std::string& plotClass);





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
  //! Disables writing of the properties; this is useful for unit tests that shouldn't alter the configuration.
  bool mWritingDisabled;



  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief Parameter representing the maximum number of log entries
  cedar::aux::UIntParameterPtr _mMaximumNumberOfLogEntries;

  //!@brief Directory, where the PluginLoadDialog is supposed to open.
  cedar::aux::DirectoryParameterPtr _mPluginLoadDialogLocation;

  //!@brief Directory, where the PluginLoadDialog is supposed to open.
  cedar::aux::StringParameterPtr _mDefaultMatDataPlot;

}; // class cedar::proc::gui::Settings

namespace cedar
{
  namespace aux
  {
    namespace gui
    {
      //! Singleton for the settings object.
      typedef cedar::aux::Singleton<cedar::aux::gui::Settings> SettingsSingleton;
    }
  }
}

CEDAR_AUX_EXPORT_SINGLETON(cedar::aux::gui::Settings);

#endif // CEDAR_AUX_GUI_SETTINGS_H

