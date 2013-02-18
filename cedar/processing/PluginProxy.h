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

    File:        PluginProxy.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_PLUGIN_PROXY_H
#define CEDAR_PROC_PLUGIN_PROXY_H

// CEDAR INCLUDES
#include "cedar/processing/namespace.h"

// SYSTEM INCLUDES
#include <string>

#ifdef CEDAR_OS_WINDOWS
#include <Windows.h>
#endif // CEDAR_OS_WINDOWS

/*!@brief A class that encapsulates the OS dependent functionality for dynamically loading libraries.
 *
 * @todo In the long run, this should be moved into aux, along with proc::Manager as a more generalized PluginManager.
 */
class cedar::proc::PluginProxy
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef void (*PluginInterfaceMethod)(cedar::aux::PluginDeclarationListPtr);

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  PluginProxy();
  //!@brief Some other constructor.
  PluginProxy(const std::string& file);

  //!@brief Destructor
  ~PluginProxy();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief loaded a shared/dynamic library from a file path
  void load(const std::string& file);

  //!@brief get declaration of this proxy
  cedar::aux::PluginDeclarationListPtr getDeclaration();

  /*!@brief Returns the canonical name of a plugin based on its filepath
   */
  static std::string getPluginNameFromPath(const std::string& path);

#ifdef CEDAR_OS_UNIX
  static void abortHandler(int signal);
#endif // CEDAR_OS_UNIX

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief search known directories for this plugin.
  std::string findPluginFile(const std::string& file) const;

  //!@brief Searches for the plugin description file.
  std::string findPluginDescription(const std::string& plugin_path) const;

#ifdef CEDAR_OS_WINDOWS
  std::string getLastError();
#endif // CEDAR_OS_WINDOWS

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief plugin declaration
  cedar::aux::PluginDeclarationListPtr mDeclaration;
  //!@brief file path to plugin
  std::string mFileName;

  //! Handle to the dynamically loaded library.
#ifdef CEDAR_OS_UNIX
  void *mpLibHandle;

  /*! The plugin that is currently being loaded -- used to report to the user if a SIGABRT was caught during plugin
   *  loading
   */
  static std::string mPluginBeingLoaded;
#elif defined CEDAR_OS_WINDOWS
  HMODULE mpLibHandle;
#else
#error Implement me for your os!
#endif
}; // class cedar::proc::PluginProxy

#endif // CEDAR_PROC_PLUGIN_PROXY_H

