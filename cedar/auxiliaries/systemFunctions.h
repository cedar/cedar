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

    File:        systemFunctions.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 26

    Description: Functions that depend on the operating system.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_SYSTEM_FUNCTIONS_H
#define CEDAR_AUX_SYSTEM_FUNCTIONS_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/lib.h"

// SYSTEM INCLUDES
#include <string>
#include <fstream>
#include <vector>


namespace cedar
{
  namespace aux
  {
    //!@brief try to get the home directory from standard environment variables
    CEDAR_AUX_LIB_EXPORT  std::string getUserHomeDirectory();

    //!@brief try to get the directory in which application data of cedar is stored
    CEDAR_AUX_LIB_EXPORT std::string getUserApplicationDataDirectory();
  
    /*!@brief   Finds the path to the resource.
     *
     *          This function locates cedar resources. These resources are usually stored in cedar's resource directory.
     *
     * @param   resourcePath The path to the resource, relative to cedar's resource directory.
     * @param   showInLog Flag deciding whether this function writes log messages where resources were found.
     *
     * @remarks The function looks for the resource in the following order:
     *          First, the function looks in the current path.
     *          Then, if the environment-variable CEDAR_RESOURCE_PATH is set, the paths stored in it are searched.
     *          Next, the function looks in ${CEDAR_HOME}, i.e., the directory in which cedar was originally compiled.
     *          Lastly, the function looks in the cedar install directory.
     *
     */
    CEDAR_AUX_LIB_EXPORT std::string locateResource(const std::string& resourcePath, bool showInLog = false);

    /*!@brief   Lists all the paths that are searched for resources.
     *
     *          Paths will be searched in their order in the vector, i.e., the system first looks in paths[0], then
     *          paths[1] and so on.
     */
    CEDAR_AUX_LIB_EXPORT std::vector<std::string> listResourcePaths();

    /*!@brief This function opens a crash report file in a standardized location.
     */
    CEDAR_AUX_LIB_EXPORT void openCrashFile(std::ofstream& stream, std::string& fileName);

#ifdef CEDAR_OS_WINDOWS

    namespace windows
    {
      CEDAR_AUX_LIB_EXPORT std::string getLastError();
    }

#endif // CEDAR_OS_WINDOWS

    /*! Returns a string identifying the cedar configuration.
     */
    CEDAR_AUX_LIB_EXPORT std::string getCedarConfigurationInfo(const std::string& separator, const std::string& lineEnd);
  }
} // namespace cedar::aux

#endif // CEDAR_AUX_SYSTEM_FUNCTIONS_H
