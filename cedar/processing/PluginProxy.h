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

// LOCAL INCLUDES
#include "processing/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <string>

#ifdef WINDOWS
#include <Windows.h>
#endif


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::PluginProxy
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
//#ifdef LINUX
  typedef void (*PluginInterfaceMethod)(cedar::proc::PluginDeclarationPtr);
/*#elif defined WINDOWS
  typedef cedar::proc::PluginDeclarationPtr (*PluginInterfaceMethod)();
#else
#error Implement me for your OS!
#endif*/

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  PluginProxy();
  PluginProxy(const std::string& file);

  //!@brief Destructor
  ~PluginProxy();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void load(const std::string& file);

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
  cedar::proc::PluginDeclarationPtr mDeclaration;
  std::string mFileName;

  //! Handle to the dynamically loaded library.
#ifdef UNIX
  void *mpLibHandle;
#elif defined WINDOWS
  HMODULE mpLibHandle;
#else
#error Implement me for your os!
#endif

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::PluginProxy

#endif // CEDAR_PROC_PLUGIN_PROXY_H

