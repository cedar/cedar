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

    File:        System.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 26

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/System.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <cstdlib>

#if defined LINUX || defined APPLE
#include <stdlib.h>
#endif

#ifdef WINDOWS
#include <Shlobj.h>
#include <comutil.h>

#pragma comment(lib, "comsuppw")
#endif

// INTERNALS HEADER
#define CEDAR_INTERNAL
#include "cedar/internals.h"
#undef CEDAR_INTERNAL

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::System::openCrashFile(std::ofstream& stream, std::string& crash_file)
{
  crash_file = cedar::aux::System::getUserHomeDirectory() + "/.cedar/crashes/";
  boost::filesystem::create_directories(crash_file);
  const boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
  crash_file += "processingIde.";
  crash_file += boost::posix_time::to_iso_string(now);
  crash_file += ".stacktrace";
  stream.open(crash_file.c_str());
}

std::string cedar::aux::System::getUserHomeDirectory()
{
#ifdef UNIX
  std::string homedir = getenv("HOME");
  return homedir;
#elif defined WINDOWS
  LPWSTR path = NULL;
  if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Profile, KF_FLAG_CREATE, 0, &path)))
  {
    _bstr_t bstr_path(path);
    std::string ret(static_cast<char*>(bstr_path));
    CoTaskMemFree(path);
    return ret;
  }
  else
  {
    //!@todo handle errors
  }
  return "";
#else
#error Implement me for this OS!
#endif
}

std::string cedar::aux::System::getUserApplicationDataDirectory()
{
#ifdef UNIX
  return cedar::aux::System::getUserHomeDirectory();
#elif defined WINDOWS
  LPWSTR path = NULL;
  if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, 0, &path)))
  {
    _bstr_t bstr_path(path);
    std::string ret(static_cast<char*>(bstr_path));
    CoTaskMemFree(path);
    return ret;
  }
  else
  {
    //!@todo handle errors
  }
  return "";
#else
#error Implement me for this OS!
#endif
}

std::string cedar::aux::System::locateResource(const std::string& resourcePath)
{
  std::string in_home = CEDAR_HOME_DIRECTORY "/resources/" + resourcePath;
  std::string in_install = CEDAR_RESOURCE_INSTALL_DIR "/" + resourcePath;
  std::string cedar_resource_path;
  char *p_resource_path = getenv("CEDAR_RESOURCE_PATH");
  if (p_resource_path)
  {
    cedar_resource_path = p_resource_path;
  }
  
  if (boost::filesystem::exists(resourcePath))
  {
    cedar::aux::LogSingleton::getInstance()->systemInfo("Found resource \"" + resourcePath + "\" locally.", "cedar::aux::System::locateResource");
    return resourcePath;
  }
  
  if (!cedar_resource_path.empty())
  {
    // this bool is used to avoid notifying about using the environment variable every time this function is used.
    static bool notified_about_this = false;
    if (!notified_about_this)
    {
      cedar::aux::LogSingleton::getInstance()->systemInfo("Using CEDAR_RESOURCE_PATH for finding resources.", "cedar::aux::System::locateResource");
      notified_about_this = true;
    }
    std::string path = cedar_resource_path + "/" + resourcePath;
    if (boost::filesystem::exists(path))
    {
      cedar::aux::LogSingleton::getInstance()->systemInfo("Found resource \"" + resourcePath + "\" at \"" + path + "\".", "cedar::aux::System::locateResource");
      return path;
    }
  }
  
  if (boost::filesystem::exists(in_home))
  {
    cedar::aux::LogSingleton::getInstance()->systemInfo("Found resource \"" + resourcePath + "\" at \"" + in_home + "\".", "cedar::aux::System::locateResource");
    return in_home;
  }
  if (boost::filesystem::exists(in_install))
  {
    cedar::aux::LogSingleton::getInstance()->systemInfo("Found resource \"" + resourcePath + "\" at \"" + in_install + "\".", "cedar::aux::System::locateResource");
    return in_install;
  }
  
  CEDAR_THROW(cedar::aux::ResourceNotFoundException, "The resource \"" + resourcePath + "\" could not be found.");
}

