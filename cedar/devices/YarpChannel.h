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
#include "cedar/devices/YarpChannel.fwd.h"
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
template<typename T>
class cedar::dev::YarpChannel : public cedar::dev::NetworkChannel
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  typedef cedar::aux::net::Reader<T> TypeReader;
  typedef cedar::aux::net::Writer<T> TypeWriter;
  CEDAR_GENERATE_POINTER_TYPES(TypeReader);
  CEDAR_GENERATE_POINTER_TYPES(TypeWriter);

public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------

  YarpChannel()
:
  mIsOpen(false),
  mReadPortName(new cedar::aux::StringParameter(this, "readPortName", "defaultRead")),
  mWritePortName(new cedar::aux::StringParameter(this, "writePortName", "defaultWrite"))
  {

  }

  ~YarpChannel()
  {
    if (isOpen())
    {
      close();
    }
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  T read(const std::string& port)
  {
      auto iter = mReaderMap.find(port);
      if (iter != mReaderMap.end())
      {
        try
        {
          if(iter->second)
          {
            T readValue = iter->second->read();
            return readValue;
          }
          else
          {
            //a previous NetWaitingForWriterException makes us do this
            //std::cout<< "Try to read from port: "<< port <<" , but there was no reader yet. Created one!"<<std::endl;
            iter->second = TypeReaderPtr(new TypeReader(port));
            T readValue = iter->second->read();
            return readValue;
          }
        }
        catch (cedar::aux::net::NetUnexpectedDataException& exc)
        {
          // this is ok, happens when reader is faster than writer
          CEDAR_THROW(cedar::dev::IgnoreCommunicationException, "no data");
        }
        catch (cedar::aux::net::NetWaitingForWriterException &e)
        {
          // that's ok,we'll establish that one once a reader is available, but reset the pointer for now.
          iter->second = nullptr;
          CEDAR_THROW(cedar::dev::IgnoreCommunicationException, "writer is missing");
        }
        catch (cedar::aux::net::NetMissingRessourceException &e)
        {
          CEDAR_THROW(cedar::dev::CommunicationException, "yarp is down");
        }
      }
    CEDAR_THROW(cedar::aux::ExceptionBase, "read failed because of an undefined error");
  }

  void write(T value, const std::string& port)
  {
    auto iter = mWriterMap.find(port);
    if (iter != mWriterMap.end())
    {
      if(iter->second)
      {
        try
        {
          iter->second->write(value);
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
  }

  void addReaderPort(const std::string& port)
  {
    //One could check here, if the channel is open and instantiate an open port in that case
    if (mReaderMap.find(port) == mReaderMap.end())
    {
      this->mReaderMap[port] = nullptr;
    }
  }

  void addWriterPort(const std::string& port)
  {
    //One could check here, if the channel is open and instantiate an open port in that case
    if (mWriterMap.find(port) == mWriterMap.end())
    {
      this->mWriterMap[port] = nullptr;
    }
  }

//--------------------------------------------------------------------------------------------------------------------
// protected methods
//--------------------------------------------------------------------------------------------------------------------
protected:
  void openHook()
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
            // that's ok,we'll establish that one once a reader is available, but reset the pointer for now.
            it->second = nullptr;
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

  void closeHook()
  {
    //Remove all Ports, but keep portname info
    for (auto it = mWriterMap.begin(); it != mWriterMap.end(); ++it)
    {
      it->second = nullptr;
    }

    for (auto it = mReaderMap.begin(); it != mReaderMap.end(); ++it)
    {
      it->second = nullptr;
    }

    mIsOpen = false;
  }

  bool isOpen() const
  {
    return mIsOpen;
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
  cedar::aux::StringParameterPtr mReadPortName;
  cedar::aux::StringParameterPtr mWritePortName;

};
// class cedar::dev::YarpChannel
#endif // CEDAR_USE_YARP
#endif // CEDAR_DEV_YARP_CHANNEL_H
