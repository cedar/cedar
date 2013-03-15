/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
  T read()
  {
    if (mReader)
    {
      try
      {
        return mReader->read();
      }
      catch (cedar::aux::net::NetUnexpectedDataException& exc)
      {
        CEDAR_THROW(cedar::dev::CommunicationException, "port is not open");
      }
      catch (cedar::aux::net::NetWaitingForWriterException& exc)
      {
        CEDAR_THROW(cedar::dev::CommunicationException, "port is not open");
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
  void write(T value)
  {
    if (mWriter)
    {
      try
      {
        mWriter->write(value);
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

  void setReaderPort(const std::string& port)
  {
    this->mReaderPort = port;
  }

  void setWriterPort(const std::string& port)
  {
    this->mWriterPort = port;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void openHook()
  {
    if (!mIsOpen)
    {
      if (mReaderPort != "")
      {
        try
        {
          mReader = TypeReaderPtr(new TypeReader(mReaderPort));
        }
        catch (cedar::aux::net::NetWaitingForWriterException& exc)
        {
          // that's ok
        }
      }
      if (mWriterPort != "")
      {
        mWriter = TypeWriterPtr(new TypeWriter(mWriterPort));
      }
      mIsOpen = true;
    }
  }

  void closeHook()
  {
    if (mIsOpen)
    {
      mReader.reset();
      mWriter.reset();
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
  TypeReaderPtr mReader;
  TypeWriterPtr mWriter;
  std::string mReaderPort;
  std::string mWriterPort;
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

