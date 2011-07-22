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

    File:        PluginProxy.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 22

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "processing/PluginProxy.h"
#include "processing/exceptions.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <dlfcn.h>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::PluginProxy::PluginProxy()
:
mpLibHandle(NULL)
{
}

cedar::proc::PluginProxy::PluginProxy(const std::string& file)
:
mpLibHandle(NULL)
{
  this->load(file);
}

cedar::proc::PluginProxy::~PluginProxy()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::PluginProxy::load(const std::string& file)
{
  this->mFileName = file;

  this->mpLibHandle = dlopen(this->mFileName.c_str(), RTLD_NOW);
  if (!this->mpLibHandle)
  {
    CEDAR_THROW(cedar::proc::PluginException, "Could not load plugin: dlopen failed.");
  }

  PluginInterfaceMethod interface = NULL;
  interface = (PluginInterfaceMethod) (dlsym(this->mpLibHandle, "pluginDeclaration"));
  if (!interface)
  {
    CEDAR_THROW(cedar::proc::PluginException, "Error loading interface function: dlsym returned NULL.");
  }

  //@todo this might segfault if the function pointer points to a bad function; handle this somehow.
  this->mDeclaration = (*interface)();
}

cedar::proc::PluginDeclarationPtr cedar::proc::PluginProxy::getDeclaration()
{
  return this->mDeclaration;
}
