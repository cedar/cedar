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

#ifdef WINDOWS
#include <Windows.h>
#endif


/*!@brief A class that encapsulates the OS dependent functionality for dynamically loading libraries.
 */
class cedar::proc::PluginProxy
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef void (*PluginInterfaceMethod)(cedar::proc::PluginDeclarationPtr);

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
  cedar::proc::PluginDeclarationPtr getDeclaration();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief search known directories for this plugin
  std::string findPluginFile(const std::string& file);

#ifdef WINDOWS
  std::string getLastError();
#endif

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief plugin declaration
  cedar::proc::PluginDeclarationPtr mDeclaration;
  //!@brief file path to plugin
  std::string mFileName;

  //! Handle to the dynamically loaded library.
#ifdef UNIX
  void *mpLibHandle;
#elif defined WINDOWS
  HMODULE mpLibHandle;
#else
#error Implement me for your os!
#endif
}; // class cedar::proc::PluginProxy

#endif // CEDAR_PROC_PLUGIN_PROXY_H

