/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        systemFunctions.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 26

    Description: Functions that depend on the operating system.

    Credits:

======================================================================================================================*/


// CEDAR INCLUDES
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/date_time.hpp>
  #include <boost/filesystem.hpp>
#endif
#include <cstdlib>

#if defined CEDAR_COMPILER_GCC
#include <stdlib.h>
#endif // CEDAR_COMPILER_GCC

#ifdef CEDAR_COMPILER_MSVC
  #include <comutil.h>
  #include <Shlobj.h>
  #pragma comment(lib, "comsuppw")
#endif // CEDAR_COMPILER_MSVC

#ifdef CEDAR_OS_WINDOWS
#include <Windows.h>
#endif // CEDAR_OS_WINDOWS

// INTERNALS HEADER
#define CEDAR_INTERNAL
#include "cedar/internals.h"
#undef CEDAR_INTERNAL

// ---------------------------------------------------------------------------------------------------------------------
//  general methods
// ---------------------------------------------------------------------------------------------------------------------

void cedar::aux::openCrashFile(std::ofstream& stream, std::string& crash_file)
{
  crash_file = cedar::aux::getUserHomeDirectory() + "/.cedar/crashes/";
  boost::filesystem::create_directories(crash_file);
  const boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
  crash_file += "cedar.";
  crash_file += boost::posix_time::to_iso_string(now);
  crash_file += ".stacktrace";
  stream.open(crash_file.c_str());
}

std::string cedar::aux::getUserHomeDirectory()
{
#ifdef CEDAR_OS_UNIX
  std::string homedir = getenv("HOME");
  return homedir;
#elif defined CEDAR_OS_WINDOWS
#ifdef CEDAR_COMPILER_GCC
  std::string homedir = getenv("USERPROFILE");
  return homedir;
#else // CEDAR_COMPILER_GCC
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
#endif // CEDAR_COMPILER_GCC
#else
#error Implement me for this OS!
#endif // CEDAR_OS_UNIX
}

std::string cedar::aux::getUserApplicationDataDirectory()
{
#ifdef CEDAR_OS_UNIX
  return cedar::aux::getUserHomeDirectory();
#elif defined CEDAR_OS_WINDOWS

#ifdef CEDAR_COMPILER_GCC
  std::string homedir = getenv("APPDATA");
  return homedir;
#else // CEDAR_COMPILER_GCC
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
    CEDAR_THROW(cedar::aux::UnknownNameException, "Could not find application data directory. Error: " + cedar::aux::windows::getLastError());
  }
  return "";
#endif // CEDAR_COMPILER_GCC
#else // CEDAR_OS_WINDOWS
#error Implement me for this OS!
#endif // CEDAR_OS_WINDOWS
}
std::vector<std::string> cedar::aux::listResourcePaths()
{
  std::vector<std::string> paths;

  paths.push_back(boost::filesystem::path(boost::filesystem::current_path()).string());

  paths.push_back(CEDAR_HOME_DIRECTORY "/resources/");

  char *p_resource_path = getenv("CEDAR_RESOURCE_PATH");
  if (p_resource_path)
  {
    std::string cedar_resource_path = p_resource_path;

    if (!cedar_resource_path.empty())
    {
      static bool notified_about_this = false;
      if (!notified_about_this)
      {
        cedar::aux::LogSingleton::getInstance()->systemInfo
        (
          "Using CEDAR_RESOURCE_PATH \"" + cedar_resource_path + "\"for finding resources.",
          "std::vector<std::string> cedar::aux::listResourcePaths()"
        );
        notified_about_this = true;
      }

      paths.push_back(cedar_resource_path);
    }
  }

  paths.push_back(CEDAR_HOME_DIRECTORY "/resources/");
  paths.push_back(CEDAR_RESOURCE_INSTALL_DIR "/");

  return paths;
}

std::string cedar::aux::locateResource(const std::string& resourcePath, bool showInLog)
{
  bool is_directory = false;
  std::vector<std::string> paths = cedar::aux::listResourcePaths();
  for (auto path_iter = paths.begin(); path_iter != paths.end(); ++path_iter)
  {
    std::string path = (*path_iter) + "/" + resourcePath;
    if (boost::filesystem::exists(path))
    {
      if (boost::filesystem::is_regular_file(path))
      {
        if (showInLog)
        {
          cedar::aux::LogSingleton::getInstance()->systemInfo
          (
            "Found resource \"" + resourcePath + "\" at \"" + path + "\".",
            "std::string cedar::aux::locateResource(const std::string&)"
          );
        }
        return path;
      }
      else
      {
        is_directory = true;
      }
    }
  }

  if (is_directory)
  {
    CEDAR_THROW
    (
      cedar::aux::ResourceNotFoundException,
      "The resource \"" + resourcePath + "\" could not be found: all instances seem to be directories."
    );
  }
  else
  {
    CEDAR_THROW(cedar::aux::ResourceNotFoundException, "The resource \"" + resourcePath + "\" could not be found.");
  }
}



// ---------------------------------------------------------------------------------------------------------------------
//  Windows specific
// ---------------------------------------------------------------------------------------------------------------------

#ifdef CEDAR_OS_WINDOWS

std::string cedar::aux::windows::getLastError()
{
  LPVOID lpMsgBuf;
  DWORD dw = GetLastError();

  FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    dw,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPTSTR)&lpMsgBuf,
    0, NULL);

  std::string error((char*)lpMsgBuf);

  LocalFree(lpMsgBuf);
  return error;
}

#endif //def CEDAR_OS_WINDOWS
