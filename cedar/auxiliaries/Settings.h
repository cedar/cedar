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

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Settings();

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

  bool getMemoryDebugOutput();

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

private:
  // none yet

}; // class cedar::aux::Settings

namespace cedar
{
  namespace aux
  {
#ifdef MSVC
#ifdef CEDAR_LIB_EXPORTS_AUX
    // dllexport
    template class __declspec(dllexport) cedar::aux::Singleton<cedar::aux::Settings>;
#else // CEDAR_LIB_EXPORTS_AUX
    // dllimport
    extern template class __declspec(dllimport) cedar::aux::Singleton<cedar::aux::Settings>;
#endif // CEDAR_LIB_EXPORTS_AUX
#endif // MSVC

    //!@brief The singleton instance of the auxiliaries settings.
    typedef cedar::aux::Singleton<cedar::aux::Settings> SettingsSingleton;
  }
}

#endif // CEDAR_AUX_SETTINGS_H
