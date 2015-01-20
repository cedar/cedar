/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        YarpChannel.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2013 03 06

    Description: Communication channel for a component or device over Yarp.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_YARP_CHANNEL_H
#define CEDAR_DEV_YARP_CHANNEL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_YARP

// CEDAR INCLUDES
#include "cedar/auxiliaries/net/Reader.h"
#include "cedar/auxiliaries/net/Writer.h"
#include "cedar/devices/namespace.h"
#include "cedar/devices/exceptions.h"
#include "cedar/devices/NetworkChannel.h"

// SYSTEM INCLUDES
#include <map>

/*!@brief Communication channel for a component or device over Yarp.
 *
 * @todo describe more.
 */
template <typename T>
class cedar::dev::YarpChannel : public cedar::dev::NetworkChannel
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  typedef cedar::aux::net::Reader<T> TypeReader;
  typedef cedar::aux::net::Writer<T> TypeWriter;
  CEDAR_GENERATE_POINTER_TYPES(TypeReader);
  CEDAR_GENERATE_POINTER_TYPES(TypeWriter);
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  T read(const std::string& port)
  {
    auto iter = mReaderMap.find(port);
    if (iter != mReaderMap.end() && iter->second)
    {
      try
      {
        return iter->second->read();
      }
      catch (cedar::aux::net::NetUnexpectedDataException& exc)
      {
        // this is ok, happens when reader is faster than writer
        return T();
      }
      catch (cedar::aux::net::NetMissingRessourceException &e)
      {
        CEDAR_THROW(cedar::dev::CommunicationException, "yarp is down");
      }
    }
    else
    {
      // try to open it once more!
      iter->second = TypeReaderPtr(new TypeReader(port));
      return this->read(port);
//      CEDAR_THROW(cedar::dev::CommunicationException, "port \"" + port + "\" is unknown");
    }
  }

  void write(T value, const std::string& port)
  {
    if (mWriterMap.find(port) != mWriterMap.end())
    {
      try
      {
        mWriterMap[port]->write(value);
      }
      catch (cedar::aux::net::NetMissingRessourceException &e)
      {
        CEDAR_THROW(cedar::dev::CommunicationException, "yarp is down");
      }
    }
    else
    {
      CEDAR_THROW(cedar::dev::CommunicationException, "port is not open");
    }
  }

  bool isOpen() const
  {
    return this->mIsOpen;
  }

  void addReaderPort(const std::string& port)
  {
    if (mReaderMap.find(port) == mReaderMap.end())
    {
      this->mReaderMap[port] = TypeReaderPtr();
    }
    else
    { //TODO: exception
      CEDAR_ASSERT(false);
    }
  }

  void addWriterPort(const std::string& port)
  {
    if (mWriterMap.find(port) == mWriterMap.end())
    {
      this->mWriterMap[port] = TypeWriterPtr();
    }
    else
    { //TODO: exception
      CEDAR_ASSERT(false);
    }
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void openHook()
  {
//    if (!mIsOpen)
    {
      for (auto it = mReaderMap.begin(); it != mReaderMap.end(); ++it)
      {
        const std::string& port = it->first;
        if (!it->second) // no object instance allocated before
        {
          try
          {
            it->second = TypeReaderPtr(new TypeReader(port));
          }
          catch (cedar::aux::net::NetWaitingForWriterException& exc)
          {
            // that's ok, but reset pointer
            it->second = TypeReaderPtr();
          }
        }
      }
      for (auto it = mWriterMap.begin(); it != mWriterMap.end(); ++it)
      {
        const std::string& port = it->first;
        if (!it->second) // no object instance allocated before
        {
          it->second = TypeWriterPtr(new TypeWriter(port));
        }
      }
      mIsOpen = true;
    }
  }

  void closeHook()
  {
    if (mIsOpen)
    {
      mReaderMap.clear();
      mWriterMap.clear();
      mIsOpen = false;
    }
  }

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
  std::map<std::string, TypeReaderPtr> mReaderMap;
  std::map<std::string, TypeWriterPtr> mWriterMap;
  bool mIsOpen;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

}; // class cedar::dev::YarpChannel
#endif // CEDAR_USE_YARP

#endif // CEDAR_DEV_YARP_CHANNEL_H

