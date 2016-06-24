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
  mReadPortName(new cedar::aux::StringParameter(this, "readPortName", "defaultRead")),
  mWritePortName(new cedar::aux::StringParameter(this, "writePortName", "defaultWrite"))
{

}

  ~YarpChannel()
  {
    prepareChannelDestructAbsolutelyRequired();
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  T read(const std::string& port)
  {
    std::cout << "Read from the YarpChannel. Port: "<< port << std::endl;
    try
    {
      std::cout<< "Find Reader"<<std::endl;
      auto iter = mReaderMap.find(port);
      if (iter != mReaderMap.end() && iter->second)
      {
        std::cout<<"Try Reading Value with instatiated reader"<< std::endl;
        T readValue = iter->second->read();
        std::cout<<" Read Value with instatiated reader: "<< readValue << std::endl;
        return readValue;
      }
      else
      {
        // try to open it once more!
        std::cout<< "Read Else"<<std::endl;
        iter->second = TypeReaderPtr(new TypeReader(port));
        std::cout<<"Initialized Reader"<< std::endl;
        T readValue = iter->second->read();
        return readValue;
//      CEDAR_THROW(cedar::dev::CommunicationException, "port \"" + port + "\" is unknown");
      }
    }
    catch (cedar::aux::net::NetUnexpectedDataException& exc)
    {
      std::cout<< "Throw NetUnexpectedDataException"<<std::endl;
      CEDAR_THROW(cedar::dev::IgnoreCommunicationException, "no data");
      // this is ok, happens when reader is faster than writer
    }
    catch (cedar::aux::net::NetWaitingForWriterException &e)
    {
      std::cout<< "Throw NetWaitingForWriterException"<<std::endl;
      CEDAR_THROW(cedar::dev::IgnoreCommunicationException, "writer is missing");
    }
    catch (cedar::aux::net::NetMissingRessourceException &e)
    {
      std::cout<< "Throw NetMissingRessourceException"<<std::endl;
      CEDAR_THROW(cedar::dev::CommunicationException, "yarp is down");
    }
    CEDAR_THROW(cedar::aux::ExceptionBase, "read failed because of an undefined error");
    std::cout<< "Throw ExceptionBase"<<std::endl;

  }

  void write(T value, const std::string& port)
  {
    std::cout << "Write to the YarpChannel. Port: "<< port << " Value: "<< value <<std::endl;
    if (mWriterMap.find(port) != mWriterMap.end())
    {
      std::cout << "1"<< std::endl;
      try
      {
        std::cout << "2: mWriterMap Size: "<< mWriterMap.size() << " Count: "<< mWriterMap.count(port)<< std::endl;
        TypeWriterPtr myWriter = mWriterMap.at(port);
        std::cout<< " 2B: myWriter exists " <<std::endl;
        myWriter->write(value);
        std::cout << "3"<< std::endl;
      }
      catch (cedar::aux::net::NetMissingRessourceException &e)
      {
        std::cout << "4"<< std::endl;
        CEDAR_THROW(cedar::dev::CommunicationException, "yarp is down");
      }
      std::cout << "5"<< std::endl;
    }
    else
    {
      std::cout << "6"<< std::endl;
      CEDAR_THROW(cedar::dev::CommunicationException, "port is not open");
    }
    std::cout << "7"<< std::endl;
  }

  void addReaderPort(const std::string& port)
  {
    std::cout << "Add Reader Port: " << port << std::endl;
    if (mReaderMap.find(port) == mReaderMap.end())
    {
      this->mReaderMap[port] = TypeReaderPtr();
    }
    else
    { //TODO: exception
      std::cout << "Reader not added: " << port << std::endl;
      CEDAR_ASSERT(false);
    }
  }

  void addWriterPort(const std::string& port)
  {
    std::cout << "Add Writer Port: " << port << std::endl;
    if (mWriterMap.find(port) == mWriterMap.end())
    {
      this->mWriterMap[port] = TypeWriterPtr(new TypeWriter(port));
    }
    else
    { //TODO: exception
      std::cout << "Writer not added: " << port << std::endl;
      CEDAR_ASSERT(false);
    }
  }

//--------------------------------------------------------------------------------------------------------------------
// protected methods
//--------------------------------------------------------------------------------------------------------------------
protected:
  void openHook()
  {
    std::cout << "Started the OpenHook" << std::endl;
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
    }
  }

  void closeHook()
  {
    std::cout << "Started the CloseHook" << std::endl;
    if (isOpen())
    {
      std::cout << "CloseHook closes!" << std::endl;
      mReaderMap.clear();
      mWriterMap.clear();
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
