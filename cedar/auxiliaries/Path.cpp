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

    File:        Path.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 03 15

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Path.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <boost/filesystem.hpp>


//----------------------------------------------------------------------------------------------------------------------
// static constants
//----------------------------------------------------------------------------------------------------------------------
const std::string cedar::aux::Path::M_PROTOCOL_ABSOLUTE_STR = "absolute";
const std::string cedar::aux::Path::M_PROTOCOL_RESOURCE_STR = "resource";


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::Path::Path()
{
}

cedar::aux::Path::Path(const std::string& path)
{
  this->setPath(path);
}

cedar::aux::Path::Path(const char* path)
{
  this->setPath(std::string(path));
}

cedar::aux::Path::~Path()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

std::string cedar::aux::Path::getFileNameOnly() const
{
  //!@todo If the path exists, check if it is a directory.
  //!@todo Proper exception
  CEDAR_ASSERT(!this->mComponents.empty());

  return this->mComponents.back();
}

bool cedar::aux::Path::exists() const
{
  return boost::filesystem::exists(this->absolute().toString());
}

bool cedar::aux::Path::isDirectory() const
{
  return boost::filesystem::is_directory(this->absolute().toString());
}

cedar::aux::Path cedar::aux::Path::getDirectory() const
{
  if (boost::filesystem::is_directory(this->absolute().toString()))
  {
    return *this;
  }
  else
  {
    // copy this string
    cedar::aux::Path copy = *this;
    // remove last component of the copy
    auto iter = copy.mComponents.end();
    --iter;
    copy.mComponents.erase(iter);
    return copy;
  }
}

cedar::aux::Path cedar::aux::Path::absolute() const
{
  if (this->isResource())
  {
    std::string path = cedar::aux::locateResource(this->toString());
    return cedar::aux::Path(path);
  }

  if (this->isAbsolute())
  {
    return *this;
  }

  // if the path is neither a resource, nor absolute, it should be relative
  return cedar::aux::Path(boost::filesystem::current_path().string() + "/" + this->toString(false));
}

bool cedar::aux::Path::isEmpty() const
{
  return this->mComponents.empty();
}

bool cedar::aux::Path::isResource() const
{
  if (this->mProtocol == cedar::aux::Path::M_PROTOCOL_RESOURCE_STR)
  {
    return true;
  }
  return false;
}

bool cedar::aux::Path::isAbsolute() const
{
  if (this->mProtocol == cedar::aux::Path::M_PROTOCOL_ABSOLUTE_STR)
  {
    return true;
  }
  //!@todo this should probably return true in other circumstances!
  return false;
}

void cedar::aux::Path::setPath(const std::string& path)
{
  enum PARSE_STATE
  {
    STATE_PROTOCOL_OR_PATH,
    STATE_PROTOCOL_OR_PATH_COLON,
    STATE_PROTOCOL_OR_PATH_COLON_SLASH,
    STATE_PATH
  };
  this->mComponents.clear();
  this->mProtocol.clear();

  this->mComponents.resize(1);

  // the path can either start with a specification of the protocol, or be a plain path
  PARSE_STATE state = STATE_PROTOCOL_OR_PATH;
  for (size_t i = 0; i < path.size(); ++i)
  {
    const char& c = path.at(i);
    switch (state)
    {
      case STATE_PROTOCOL_OR_PATH:
      {
        switch (c)
        {
          case '/':
            if (this->mComponents.front().empty())
            {
              this->mProtocol = cedar::aux::Path::M_PROTOCOL_ABSOLUTE_STR;
            }
          case '\\':
            this->mComponents.push_back(std::string());
            state = STATE_PATH;
            break;

          case ':':
            state = STATE_PROTOCOL_OR_PATH_COLON;
            break;

          default:
            this->mComponents.back() += c;
        } // switch (c)
        break;
      } // STATE_PROTOCOL_OR_PATH

      case STATE_PROTOCOL_OR_PATH_COLON:
      {
        switch (c)
        {
          case '/':
          case '\\':
            state = STATE_PROTOCOL_OR_PATH_COLON_SLASH;
            break;

          default:
            CEDAR_THROW(cedar::aux::InvalidPathException, "Cannot parse path \"" + path + "\": expected either '\\' or '/' at character " + cedar::aux::toString(i));
        } // switch (c)
        break;
      } // case STATE_PROTOCOL_OR_PATH_COLON

      case STATE_PROTOCOL_OR_PATH_COLON_SLASH:
      {
        switch (c)
        {
          case '/':
          case '\\':
            // protocol complete -- store accordingly
            this->mProtocol = this->mComponents.front();
            // clear the first string
            this->mComponents.front().clear();

            state = STATE_PATH;
            break;

          default:
            // this is probably some sort of windows path -- but that's ok. append the colon, ignore the slash
            this->mComponents.back() += ':';
            this->mComponents.push_back(std::string());
            this->mComponents.back() += c;
            this->mProtocol = cedar::aux::Path::M_PROTOCOL_ABSOLUTE_STR;
            state = STATE_PATH;
        } // switch (c)
        break;
      } // case STATE_PROTOCOL_OR_PATH_COLON_SLASH

      case STATE_PATH:
      {
        switch (c)
        {
          case '/':
          case '\\':
            this->mComponents.push_back(std::string());
            break;

          default:
            this->mComponents.back() += c;
        }
        break;
      }
    }
  }

  // special things to do at the end
  switch (state)
  {
    case STATE_PROTOCOL_OR_PATH_COLON:
    case STATE_PROTOCOL_OR_PATH_COLON_SLASH:
      // wasn't really a protocol -- add the colon to the path
      this->mComponents.back() += ':';
      this->mProtocol = cedar::aux::Path::M_PROTOCOL_ABSOLUTE_STR;
      break;

    default:
      break;
  }

  // erase empty components
  auto iter = this->mComponents.begin();
  while (iter != this->mComponents.end())
  {
    const std::string& component = *iter;
    if (component.empty())
    {
      iter = this->mComponents.erase(iter);
    }
    else
    {
      ++iter;
    }
  }
}

std::string cedar::aux::Path::toString(bool withProtocol) const
{
  std::string path;

  if (withProtocol)
  {
    if (!this->mProtocol.empty())
    {
      path += this->mProtocol;
      path += "://";
    }
  }

  if
  (
    this->isAbsolute() &&
    (
      this->mComponents.front().empty()
      || this->mComponents.front().at(this->mComponents.front().size() - 1) != ':'
    )
  )
  {
    path += '/';
  }

  for (auto iter = this->mComponents.begin(); iter != this->mComponents.end(); ++iter)
  {
    if (iter != this->mComponents.begin())
    {
      path += '/';
    }

    path += *iter;
  }

  return path;
}

const std::string& cedar::aux::Path::getProtocol() const
{
  return this->mProtocol;
}

cedar::aux::Path cedar::aux::Path::operator+ (const cedar::aux::Path& other) const
{
  cedar::aux::Path combined = *this;
  combined.mComponents.insert(combined.mComponents.end(), other.mComponents.begin(), other.mComponents.end());
  return combined;
}

cedar::aux::Path& cedar::aux::Path::operator= (const std::string& pathStr)
{
  this->setPath(pathStr);
  return *this;
}

cedar::aux::Path& cedar::aux::Path::operator= (const char* pathStr)
{
  this->setPath(std::string(pathStr));
  return *this;
}

cedar::aux::Path cedar::aux::Path::globalCofigurationBaseDirectory()
{
  return cedar::aux::Path(cedar::aux::getUserHomeDirectory() + "/.cedar");
}
