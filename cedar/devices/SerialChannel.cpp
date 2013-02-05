/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
    Date:        2012 04 13

    Description: This class provides a string-based communication with an external device using a Serial Port.

    Credits:     Andre Bartel, Marc Sons

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/exceptions.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/devices/SerialChannel.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES
#include <cstdio>
#include <QTime>
#include <locale>

#ifndef CEDAR_OS_WINDOWS
  #include <termios.h>
  #include <unistd.h>
#endif // CEDAR_OS_WINDOWS


#define clear_my(var, mask)    var &= (~(mask))
#define set(var, mask)      var |= (mask)

#undef DEBUG_VERBOSE

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::SerialChannel::SerialChannel()
:
mFileDescriptor(0),
_mDevicePath(new cedar::aux::StringParameter(this, "device path", "/dev/rfcomm0")),
_mEndOfCommandString(new cedar::aux::StringParameter(this, "end of command string", "\\r\\n")),
_mCountryFlag(new cedar::aux::IntParameter(this, "country flag", 0, 0, 1)),
_mBaudrate(new cedar::aux::UIntParameter(this, "baud rate", 4098, 0, 1000000)),
_mTimeOut(new cedar::aux::UIntParameter(this, "time out", 250000, 0, 1000000)),
_mLatency(new cedar::aux::UIntParameter(this, "latency", 10000, 0, 1000000))
{
  QObject::connect(_mEndOfCommandString.get(), SIGNAL(valueChanged()),
                   this, SLOT(updateTranslatedEndOfCommandString()));
}

cedar::dev::SerialChannel::~SerialChannel()
{
  close();

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


int cedar::dev::SerialChannel::getFileDescriptor() const
{
  return mFileDescriptor;
}

const std::string& cedar::dev::SerialChannel::getDevicePath() const
{
  return _mDevicePath->getValue();
}

const std::string& cedar::dev::SerialChannel::getEndOfCommandString() const
{
  return _mEndOfCommandString->getValue();
}

int cedar::dev::SerialChannel::getCountryFlag() const
{
  return _mCountryFlag->getValue();
}

unsigned int cedar::dev::SerialChannel::getBaudrate() const
{
  return _mBaudrate->getValue();
}

unsigned int cedar::dev::SerialChannel::getTimeOut() const
{
  return _mTimeOut->getValue();
}

unsigned int cedar::dev::SerialChannel::getLatency() const
{
  return _mLatency->getValue();
}

void cedar::dev::SerialChannel::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  // read the configuration
  this->Configurable::readConfiguration(node);
}

void cedar::dev::SerialChannel::checkIfOpen() const
{
  if (this->mFileDescriptor == 0)
  {
    CEDAR_THROW(cedar::aux::InitializationException, "Serial port not opened.");
  }
}

std::string cedar::dev::SerialChannel::sendAndReceiveLocked(const std::string& command)
{
  QWriteLocker lock(&(this->mLock));
  this->send(command);
  return this->receive();
}

void cedar::dev::SerialChannel::send(const std::string& command)
{
#ifdef CEDAR_OS_WINDOWS
#pragma message ("cedar::dev::SerialChannel::send() not implemented for Windows.")
#else

  checkIfOpen();

#ifdef DEBUG
  QTime timer;
  timer.start();
#endif

  std::ostringstream stream;
  // status information about the serial transmission
  int status = 0;

  stream << command << mTranslatedEndOfCommandString;
  status = write(mFileDescriptor, stream.str().c_str(), stream.str().length());

  if (status < 0)
  {
    tcflush(mFileDescriptor, TCOFLUSH);

    std::string exception_message = "Error sending data (" + cedar::aux::toString(errno) + ")";
    CEDAR_THROW(cedar::dev::SerialCommunicationException, exception_message);
  }

#ifdef DEBUG_VERBOSE
  std::ostringstream message;
  message << "Successfully sent command '"
          << command
          << "'"
          << "("
          << status
          << " bytes written to '"
          << getDevicePath()
          << "'), "
          << "sending time: "
          << timer.elapsed()
          << " ms";

  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    message.str(),
    "cedar::dev::SerialChannel",
    "Successfully sent data"
  );
#endif

  // delay the following operations
  cedar::aux::usleep(_mLatency->getValue());
#endif // CEDAR_OS_WINDOWS
}

std::string cedar::dev::SerialChannel::receive()
{
  // the answer received
  std::string answer;

#ifdef CEDAR_OS_WINDOWS
#pragma message ("cedar::dev::SerialChannel::receive() not implemented for Windows.")
#else

  checkIfOpen();

#ifdef DEBUG
  QTime timer;
  timer.start();
#endif

  // the last read char
  char char_read;
  // the complete string
  std::ostringstream ans;
  // status-information
  int status = 0;
  // number of bytes read
  int read_bytes = 0;

  fd_set read_fds;
  struct timeval tv;

  FD_ZERO(&read_fds);
  FD_SET(mFileDescriptor, &read_fds);

  do
  {
    tv.tv_sec = 0;
    tv.tv_usec = _mTimeOut->getValue();
    // check if the channel is ready
    status = select(FD_SETSIZE, &read_fds, NULL, NULL, &tv);

    // channel is ready
    if (status > 0)
    {
      // read 1 byte
      status = read(mFileDescriptor, &char_read, 1);

      // reading successful
      if (status > 0)
      {
        ans << char_read;
      }
      // reading failed
      else
      {
        tcflush(mFileDescriptor, TCIFLUSH);

        std::string exception_message = "Error sending data (" + cedar::aux::toString(errno) + ")";
        CEDAR_THROW(cedar::dev::SerialCommunicationException, exception_message);
      }
    }
    // select: time out
    else if (status == 0)
    {
      tcflush(mFileDescriptor, TCIFLUSH);
      CEDAR_THROW(cedar::dev::SerialCommunicationException, "Error: Time out while receiving data");
    }
    // select: error
    else if (status < 0)
    {
      // error is not an interrupt
      if (errno != EINTR)
      {
        tcflush(mFileDescriptor, TCIFLUSH);

        std::string exception_message = "Error sending data (" + cedar::aux::toString(errno) + ")";
        CEDAR_THROW(cedar::dev::SerialCommunicationException, exception_message);
      }
    }

    //search for end-of-command string, read_bytes = -1 if it is not found
    read_bytes = ans.str().find(mTranslatedEndOfCommandString);
  }
  while (read_bytes < 0); // end-of-command string not found

  // write received string to desired location
  answer = ans.str();
  // delete end-of-command string
  answer.resize(read_bytes);

#ifdef DEBUG_VERBOSE
  std::ostringstream message;
  message << "Successfully received data ("
          << read_bytes
          << " Byte(s) read from '"
          << getDevicePath()
          << "')\n"
          << "Receiving time: "
          << timer.elapsed()
          << " ms\n";

  cedar::aux::LogSingleton::getInstance()->message
  (
    message.str(),
    "cedar::dev::SerialChannel",
    "Successfully received data"
  );
#endif
#endif

  return answer;
}

void cedar::dev::SerialChannel::setEndOfCommandString(const std::string& eocString)
{
  mTranslatedEndOfCommandString = eocString;
  mTranslatedEndOfCommandString = cedar::aux::replace(mTranslatedEndOfCommandString, "\\r", "\r");
  mTranslatedEndOfCommandString = cedar::aux::replace(mTranslatedEndOfCommandString, "\\n", "\n");
}

void cedar::dev::SerialChannel::updateTranslatedEndOfCommandString()
{
  this->setEndOfCommandString(_mEndOfCommandString->getValue());
}

void cedar::dev::SerialChannel::openHook()
{
  //!@todo There is a lot of duplicate code here for mac and linux -- make this less redundant

  //!@todo Do we still need this? Not sure why this is done here.
  setEndOfCommandString(_mEndOfCommandString->getValue());

  if (this->mFileDescriptor > 0)
  {
    return;
  }

  // Parameters cannot be changed while the device is open
  _mDevicePath->setConstant(true);
  _mEndOfCommandString->setConstant(true);
  _mCountryFlag->setConstant(true);
  _mBaudrate->setConstant(true);
  _mTimeOut->setConstant(true);
  _mLatency->setConstant(true);

#ifdef CEDAR_OS_WINDOWS
#pragma message ("cedar::dev::SerialChannel::open() not implemented for Windows.")
#else
  // open channel on Linux
#if defined CEDAR_OS_LINUX
  mFileDescriptor = ::open(getDevicePath().c_str(), O_RDWR | O_NOCTTY);

  if (mFileDescriptor == -1)
  {
    std::string exception_message = "Error opening serial port " + getDevicePath() + " on Linux";
    CEDAR_THROW(cedar::dev::SerialCommunicationException, exception_message);
  }

  // save current modem settings
  tcgetattr(mFileDescriptor, &mTerminal);

  // ignore bytes with parity errors and make terminal raw and dumb
  mTerminal.c_iflag = IGNPAR | IGNBRK;

  // raw output
  mTerminal.c_oflag = 0;

  // do not echo characters
  mTerminal.c_lflag = 0;

  // set bps rate, hardware flow control, and 8n2 (8 bit, no parity, 2 stop bit)
  switch (_mCountryFlag->getValue())
  {
    case 0:
    {
      mTerminal.c_cflag = CLOCAL | CREAD | CSTOPB | CS8;
      break;
    }
    case 1:
    {
      mTerminal.c_cflag = CLOCAL | CREAD | CS8;
      break;
    }
    default:
    {
      CEDAR_THROW
      (
        cedar::dev::SerialCommunicationException,
        "Wrong country flag (set flag to 0 for Germany or 1 for USA.)"
      );
    }
  }

  //@todo what is going on here?
  cfsetospeed(&mTerminal, _mBaudrate->getValue());
  cfsetispeed(&mTerminal, _mBaudrate->getValue());
  mTerminal.c_cc[VMIN] = 0;
  mTerminal.c_cc[VTIME] = 0;
  tcsetattr(mFileDescriptor, TCSANOW, &mTerminal);


  // openialize channel on Mac
#elif defined CEDAR_OS_APPLE
  mFileDescriptor = ::open(getDevicePath().c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

  if (mFileDescriptor < 0)
  {
    CEDAR_THROW(cedar::dev::SerialCommunicationException, "Error opening serial port " + getDevicePath() + " on Mac");
  }

  if (fcntl(mFileDescriptor, F_SETFL, 0) < 0)
  {
    std::ostringstream exception_message;
    exception_message << "Error clearing O_NDELAY"
                      << getDevicePath()
                      << " - "
                      << strerror(errno)
                      << "("
                      << errno
                      << ")";
    CEDAR_THROW(cedar::dev::SerialCommunicationException, exception_message.str());
  }

  // get the current options and save them for later reset
  if (tcgetattr(mFileDescriptor, &mOriginalTTYAttrs) < 0)
  {
    std::ostringstream exception_message;
    exception_message << "Error getting tty attribues"
                      << getDevicePath()
                      << " - "
                      << strerror(errno)
                      << "("
                      << errno
                      << ")";
    CEDAR_THROW(cedar::dev::SerialCommunicationException, exception_message.str());
  }

  // set raw input; timeout: 1 s
  // these options are documented in the man page for termios
  mTerminal = mOriginalTTYAttrs;
  mTerminal.c_cflag |= (CLOCAL | CREAD);
  mTerminal.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  mTerminal.c_oflag &= ~OPOST;
  mTerminal.c_cc[VMIN] = 0;
  mTimeoutTalk = 30;
  mTerminal.c_cc[VTIME] = 0; // mTimeoutTalk;
  mTerminal.c_ispeed = _mBaudrate->getValue();
  mTerminal.c_ospeed = _mBaudrate->getValue();
  clear_my(mTerminal.c_cflag, CSIZE|PARENB); // control modes
  set(mTerminal.c_cflag, CS8 | CSTOPB);

  // set the options
  if (tcsetattr(mFileDescriptor, TCSANOW, &mTerminal) < 0)
  {
    std::ostringstream exception_message;
    exception_message << "Error getting tty attribues"
                      << getDevicePath()
                      << " - "
                      << strerror(errno)
                      << "("
                      << errno
                      << ")";
    CEDAR_THROW(cedar::dev::SerialCommunicationException, exception_message.str());
  }
#else
  CEDAR_THROW(cedar::dev::UnknownOperatingSystemException, "Error opening serial port - unknown operating system.");
#endif

  tcflush(mFileDescriptor, TCIOFLUSH);

  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully opened port" + getDevicePath(),
    "cedar::dev::SerialChannel",
    "Serial channel opened"
  );
#endif
}

void cedar::dev::SerialChannel::closeHook()
{
#ifdef CEDAR_OS_WINDOWS
#pragma message ("cedar::dev::SerialChannel::close() not implemented for Windows.")
#else
  int status = ::close(mFileDescriptor);

  if (status != 0)
  {
    std::string exception_message = "Error closing port '" + getDevicePath() + "'";
    CEDAR_THROW(cedar::dev::SerialCommunicationException, exception_message);
  }

  mFileDescriptor = 0;
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Closing Port",
    "cedar::dev::SerialChannel",
    "Serial channel closed"
  );

  // Parameters cannot be changed while the device is open
  _mDevicePath->setConstant(false);
  _mEndOfCommandString->setConstant(false);
  _mCountryFlag->setConstant(false);
  _mBaudrate->setConstant(false);
  _mTimeOut->setConstant(false);
  _mLatency->setConstant(false);
#endif // CEDAR_OS_WINDOWS
}
