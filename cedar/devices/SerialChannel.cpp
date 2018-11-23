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

    File:        SerialChannel.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2013 02 05

    Description: Channel to serial devices, based on Boost ASIO.

    Credits:     Based on the class TimeoutSerial, written by Terraneo Federico.
                 http://gitorious.org/serial-port/serial-port/trees/master/2_with_timeout

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/exceptions.h"
#include "cedar/devices/SerialChannel.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/TimeParameter.h"
#include "cedar/auxiliaries/StringParameter.h"

// SYSTEM INCLUDES
#include <boost/utility.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <istream>
#include <sstream>


#undef DEBUG_VERBOSE

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::SerialChannel::SerialChannel()
:
mCommandDelimiter("\r\n"),
mIoService(),
mPort(mIoService),
mTimer(mIoService),
_mDevicePath(new cedar::aux::StringParameter(this, "device path", "/dev/rfcomm0")),
_mEscapedCommandDelimiter(new cedar::aux::StringParameter(this, "escaped command delimiter", "\\r\\n")),
_mBaudRate(new cedar::aux::UIntParameter(this, "baud rate", 115200, 0, 8000000)),
_mTimeout
(
  new cedar::aux::TimeParameter
  (
    this,
    "time out",
    0.25 * cedar::unit::seconds,
    0.0 * cedar::unit::seconds,
    1000.0 * cedar::unit::seconds
  )
)
{
  // whenever the user changes the (escaped) command delimiter, the unescaped version needs to be updated accordingly
  QObject::connect(_mEscapedCommandDelimiter.get(), SIGNAL(valueChanged()),
                   this, SLOT(updateCommandDelimiter()));

  // update the command delimiter once to initialize
  updateCommandDelimiter();
}

cedar::dev::SerialChannel::~SerialChannel()
{
  if (isOpen())
  {
    close();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::dev::SerialChannel::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  this->Configurable::readConfiguration(node);
}

const std::string& cedar::dev::SerialChannel::getDevicePath() const
{
  return _mDevicePath->getValue();
}

const std::string& cedar::dev::SerialChannel::getCommandDelimiter() const
{
  return _mEscapedCommandDelimiter->getValue();
}

unsigned int cedar::dev::SerialChannel::getBaudRate() const
{
  return _mBaudRate->getValue();
}

const cedar::unit::Time& cedar::dev::SerialChannel::getTimeout() const
{
  return _mTimeout->getValue();
}

std::string cedar::dev::SerialChannel::writeAndReadLocked(const std::string& command)
{
  QWriteLocker lock(&(this->mLock));
  this->write(command);
  return this->read();
}

void cedar::dev::SerialChannel::write(std::string command)
{
  CEDAR_ASSERT(this->isOpen());

  // append the command delimiter to the sent command
  command.append(mCommandDelimiter);
  try
  {
    boost::asio::write(mPort, boost::asio::buffer(command.c_str(), command.size()));
  }
  catch (const std::exception& e)
  {
    CEDAR_THROW(cedar::dev::SerialChannel::WriteException, "Error during writing. Error is: " + std::string(e.what()));
  }

#ifdef DEBUG_VERBOSE
  std::ostringstream message;
  message << "Successfully sent command '"
          << command
          << "'"
          << " to serial port '"
          << getDevicePath()
          << "'";

  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    message.str(),
    "cedar::dev::SerialChannel",
    "Successfully sent data"
  );
#endif
}

void cedar::dev::SerialChannel::setupRead()
{
  // set up an asynchronous read:
  // as soon as the command delimiter is found in the input,
  // the method cedar::dev::SerialChannel::readCompleted is called to handle the incoming answer
  boost::asio::async_read_until
  (
    mPort,
    mReadData,
    mCommandDelimiter,
    boost::bind
    (
      &cedar::dev::SerialChannel::readCompleted,
      this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred
    )
  );
}

std::string cedar::dev::SerialChannel::read()
{
  try
  {
    // set the current reading status to "in progress"
    mReadResult = readInProgress;
    // counter for the number of transferred bytes
    mBytesTransferred = 0;

    // set up the read process
    setupRead();

    // start the timer for the timeout
    boost::posix_time::seconds timeout_boost_seconds(static_cast<long>(getTimeout() / cedar::unit::Time(1.0 * cedar::unit::second)));
    mTimer.expires_from_now(boost::posix_time::time_duration(timeout_boost_seconds));
    // wait for the timeout to expire and call cedar::dev::SerialChannel::timeoutExpired when it does
    mTimer.async_wait(boost::bind(&cedar::dev::SerialChannel::timeoutExpired, this, boost::asio::placeholders::error));

    // Run, Forrest, Run!
    for (;;)
    {
      // start the reading process
      // run_one() will block until the delimiter is found, in which case readCompleted() is called,
      // or the timeout expires, in which case timeoutExpired() is called
      mIoService.run_one();

      switch(mReadResult)
      {
        // if the read was successful
        case readSuccess:
        {
          mTimer.cancel();
          // from the counted number of bytes, substract the size of the command delimiter
          mBytesTransferred -= mCommandDelimiter.size();
          std::istream input_stream(&mReadData);
          // allocate a string for the answer
          std::string answer(mBytesTransferred, '\0');
          // fill the string from the buffer
          input_stream.read(&answer[0], mBytesTransferred);
          // remove the delimiter from the stream
          input_stream.ignore(mCommandDelimiter.size());

          #ifdef DEBUG_VERBOSE
            std::ostringstream message;
            message << "Successfully received data ("
                    << mBytesTransferred
                    << " Byte(s) read from '"
                    << getDevicePath()
                    << "')\n";

            cedar::aux::LogSingleton::getInstance()->message
            (
              message.str(),
              "cedar::dev::SerialChannel",
              "Successfully received data"
            );
          #endif

          return answer;
        }
        // if the timeout expired
        case readTimeoutExpired:
        {
          mPort.cancel();
          CEDAR_THROW(cedar::dev::TimeoutException, "Timeout expired on receiving data on the serial channel.");
          continue;
        }
        // if an error occurred on read
        case readError:
        {
          mTimer.cancel();
          mPort.cancel();

          std::ostringstream message;
          message << "Boost system error while receiving data on the serial channel. Error code: "
                  << boost::system::error_code();
          CEDAR_THROW(cedar::dev::SerialCommunicationException, message.str());
        }
        // if the reading is still in progress
        case readInProgress:
        {
          // remain in the loop
        }
      }
    }
  }
  catch (const boost::system::system_error& e)
  {
    CEDAR_THROW(BoostException, "A boost exception occurred: " + std::string(e.what()));
  }
}

void cedar::dev::SerialChannel::readCompleted
(
  const boost::system::error_code& error,
  const size_t bytesTransferred
)
{
  if (!error)
  {
    // set the current state of the read operation to "successful"
    mReadResult = readSuccess;
    // store the number of transferred bytes
    this->mBytesTransferred = bytesTransferred;

    return;
  }

  // in case an asynchronous operation is canceled due to a timeout,
  // each OS seems to have its own way to react
  #if defined CEDAR_OS_WINDOWS

  // Windows spits out error 995
  if (error.value() == 995)
  {
    return;
  }

  #elif defined CEDAR_OS_APPLE

  // MacOS spits out error 45
  if (error.value() == 45)
  {
    // Bug in OS X, it might be necessary to repeat the setup
    // http://osdir.com/ml/lib.boost.asio.user/2008-08/msg00004.html
    // non-existant: setupReceiver();

    return;
  }

  #elif defined CEDAR_OS_LINUX

  // Linux spits out error 125
  if (error.value() == 125)
  {
    return;
  }

  #endif

  mReadResult = readError;
}

void cedar::dev::SerialChannel::timeoutExpired(const boost::system::error_code& error)
{
  if (!error && mReadResult == readInProgress)
  {
    mReadResult = readTimeoutExpired;
  }
}

void cedar::dev::SerialChannel::setCommandDelimiter(const std::string& commandDelimiter)
{
  mCommandDelimiter = commandDelimiter;
  mCommandDelimiter = cedar::aux::replace(mCommandDelimiter, "\\r", "\r");
  mCommandDelimiter = cedar::aux::replace(mCommandDelimiter, "\\n", "\n");
}

void cedar::dev::SerialChannel::updateCommandDelimiter()
{
  this->setCommandDelimiter(_mEscapedCommandDelimiter->getValue());
}

bool cedar::dev::SerialChannel::isOpen() const
{
  return mPort.is_open();
}

void cedar::dev::SerialChannel::openHook()
{
  QWriteLocker lock(&(this->mLock));
  // the following parameters are not to be changed while the port is open
  _mDevicePath->setConstant(true);
  _mEscapedCommandDelimiter->setConstant(true);
  _mBaudRate->setConstant(true);
  _mTimeout->setConstant(true);

  if (this->isOpen())
  {
    this->close();
  }

  // open the serial port
  //!@todo Handle errors during opening
  boost::system::error_code error;
  error = mPort.open(_mDevicePath->getValue(), error);

  if (error)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Error opening serial port: " + std::string(error.message()),
      CEDAR_CURRENT_FUNCTION_NAME,
      "Serial channel error"
    );
  }

  error = mPort.set_option(boost::asio::serial_port_base::baud_rate(_mBaudRate->getValue()), error);
  if (error)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Error setting baud rate: " + std::string(error.message()),
      CEDAR_CURRENT_FUNCTION_NAME,
      "Serial channel error"
    );
  }

  if (mPort.is_open())
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Successfully opened port " + getDevicePath(),
      CEDAR_CURRENT_FUNCTION_NAME,
      "Serial channel opened"
    );
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Port " + getDevicePath() + " not opened.",
      CEDAR_CURRENT_FUNCTION_NAME,
      "Serial channel error"
    );
  }
}

void cedar::dev::SerialChannel::closeHook()
{
  QWriteLocker lock(&(this->mLock));
  if (!isOpen())
  {
    return;
  }

  // close the serial port
  mPort.close();

  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Closing Port",
    "cedar::dev::SerialChannel",
    "Serial channel closed"
  );

  // parameters can now be changed again
  _mDevicePath->setConstant(false);
  _mEscapedCommandDelimiter->setConstant(false);
  _mBaudRate->setConstant(false);
  _mTimeout->setConstant(false);
}
