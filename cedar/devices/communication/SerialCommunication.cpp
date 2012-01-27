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

    File:        SerialCommunication.cpp

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: This class provides a string-based communication with an external device using a Serial Port.

    Credits:     Marc Sons (Author of msSerialTransport.cpp this class is a revised and cedar-compatible version of)

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/communication/SerialCommunication.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES
#include <cstdio>

#define clear_my(var, mask)    var &= (~(mask))
#define set(var, mask)      var |= (mask)

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::com::SerialCommunication::SerialCommunication()
:
mOldConfigMode(false),
_mName(new cedar::aux::StringParameter(this, "Name", "Serial Communication")),
_mDevicePath(new cedar::aux::StringParameter(this, "DevicePath", "/dev/rfcomm0")),
_mEndOfCommandString(new cedar::aux::StringParameter(this, "EndOfCommandString", "\\r\\n")),
_mCFlag(new cedar::aux::IntParameter(this, "CFlag", 0, 0, 1)),
_mBaudrate(new cedar::aux::UIntParameter(this, "BaudRate", 4098, 0, 1000000)),
_mTimeOut(new cedar::aux::UIntParameter(this, "TimeOut", 250000, 0, 1000000)),
_mLatency(new cedar::aux::UIntParameter(this, "Latency", 10000, 0, 1000000)),
_mDebug(new cedar::aux::BoolParameter(this, "Debug", false)),
_mOS(new cedar::aux::StringParameter(this, "OperatingSystem", "Linux"))
{
  // currently, paramters for this class cannot be changed online.
  _mName->setConstant(true);
  _mDevicePath->setConstant(true);
  _mEndOfCommandString->setConstant(true);
  _mCFlag->setConstant(true);
  _mBaudrate->setConstant(true);
  _mTimeOut->setConstant(true);
  _mLatency->setConstant(true);
  _mDebug->setConstant(true);
  _mOS->setConstant(true);

  mInitialized = false;
}


cedar::dev::com::SerialCommunication::SerialCommunication(const std::string& config_file)
:
mOldConfigMode(true),
_mName(new cedar::aux::StringParameter(this, "Name", "Serial Communication")),
_mDevicePath(new cedar::aux::StringParameter(this, "DevicePath", "/dev/rfcomm0")),
_mEndOfCommandString(new cedar::aux::StringParameter(this, "EndOfCommandString", "\\r\\n")),
_mCFlag(new cedar::aux::IntParameter(this, "CFlag", 0, 0, 1)),
_mBaudrate(new cedar::aux::UIntParameter(this, "BaudRate", 4098, 0, 1000000)),
_mTimeOut(new cedar::aux::UIntParameter(this, "TimeOut", 250000, 0, 1000000)),
_mLatency(new cedar::aux::UIntParameter(this, "Latency", 10000, 0, 1000000)),
_mDebug(new cedar::aux::BoolParameter(this, "Debug", false)),
_mOS(new cedar::aux::StringParameter(this, "OperatingSystem", "Linux"))
{
  // currently, paramters for this class cannot be changed online.
  _mName->setConstant(true);
  _mDevicePath->setConstant(true);
  _mEndOfCommandString->setConstant(true);
  _mCFlag->setConstant(true);
  _mBaudrate->setConstant(true);
  _mTimeOut->setConstant(true);
  _mLatency->setConstant(true);
  _mDebug->setConstant(true);
  _mOS->setConstant(true);

  mInitialized = false;

  this->readOldConfig(config_file);
  // this has to be called manually because the virtual function readConfiguration is not called
//  this->init();
}

cedar::dev::com::SerialCommunication::~SerialCommunication()
{
  if (mInitialized)
  {
    if (_mDebug)
    {
      std::cout << "SerialCommunication: Closing Port\n";
    }
    close();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::dev::com::SerialCommunication::isInitialized() const
{
  return mInitialized;
}

int cedar::dev::com::SerialCommunication::getFileDescriptor() const
{
  return mFileDescriptor;
}

const std::string& cedar::dev::com::SerialCommunication::getName() const
{
  return _mName->getValue();
}

const std::string& cedar::dev::com::SerialCommunication::getDevicePath() const
{
  return _mDevicePath->getValue();
}

const std::string& cedar::dev::com::SerialCommunication::getEndOfCommandString() const
{
  return _mEndOfCommandString->getValue();
}

int cedar::dev::com::SerialCommunication::getCFlag() const
{
  return _mCFlag->getValue();
}

unsigned int cedar::dev::com::SerialCommunication::getBaudrate() const
{
  return _mBaudrate->getValue();
}

unsigned int cedar::dev::com::SerialCommunication::getTimeOut() const
{
  return _mTimeOut->getValue();
}

unsigned int cedar::dev::com::SerialCommunication::getLatency() const
{
  return _mLatency->getValue();
}

void cedar::dev::com::SerialCommunication::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  // read the configuration
  this->Configurable::readConfiguration(node);

  this->init();
}

void cedar::dev::com::SerialCommunication::init()
{
  // initialize
  if (mInitialized)
  {
    if (_mDebug->getValue())
    {
      std::cout << "SerialCommunication: Initialization failed (already initialized)\n";
    }
    return;
  }

  if (mOldConfigMode)
  {
    std::cout << "Warning: you are using the old config interface. Please switch to the new one." << std::endl;
  }

  setEndOfCommandString(_mEndOfCommandString->getValue()); // sets end-of-command-string

#ifndef WIN32
  // initialize communication on Linux
  if(_mOS->getValue() == "Linux") //!@todo Switch this to a #ifdef block
  {
    mFileDescriptor = open(_mDevicePath->getValue().c_str(), O_RDWR | O_NOCTTY);

    if (mFileDescriptor == -1)
    {
      if (_mDebug->getValue())
      {
        std::cout << "SerialCommunication: Error opening Port '" << _mDevicePath << "' on Linux\n";
      }
      return; //@todo Throw an exception
    }

    tcgetattr(mFileDescriptor, &mTerminal); // save current modem settings

    // Ignore bytes with parity errors and make terminal raw and dumb.
    mTerminal.c_iflag = IGNPAR | IGNBRK;

    // Raw output.
    mTerminal.c_oflag = 0;

    // Don't echo characters
    mTerminal.c_lflag = 0;

    // Set bps rate and hardware flow control and
    // 8n2 (8 bit, no parity, 2 stop bit).
    switch (_mCFlag->getValue())
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
        std::cout << "Wrong CFlag (Set CFlag to 0 for Germany or 1 for USA.)\n";
        break;
      }
    }

    cfsetospeed(&mTerminal, _mBaudrate->getValue());
    cfsetispeed(&mTerminal, _mBaudrate->getValue());
    mTerminal.c_cc[VMIN] = 0;
    mTerminal.c_cc[VTIME] = 0;
    tcsetattr(mFileDescriptor, TCSANOW, &mTerminal);
  }

  // initialize communication on Mac

  else if(_mOS->getValue() == "Apple")
  {
    // mPortId="/dev/tty.PL2303-0000101D";
    mFileDescriptor = open(_mDevicePath->getValue().c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (mFileDescriptor < 0)
    {
      if (_mDebug->getValue())
      {
        std::cout << "SerialCommunication: Error opening Port '" << _mDevicePath << "' on Mac.\n";
      }
      return; //!@todo Throw exception
    }

    if (fcntl(mFileDescriptor, F_SETFL, 0) < 0)
    {
      if (_mDebug->getValue())
      {
        printf("SerialCommunication: Error clearing O_NDELAY %s - %s(%d).\n",
               _mDevicePath->getValue().c_str(), strerror(errno), errno);
      }
      return; //!@todo Throw an exception.
    }

    // Get the current options and save them for later reset.
    if (tcgetattr(mFileDescriptor, &mOriginalTTYAttrs) < 0)
    {
      if (_mDebug->getValue())
      {
        printf("SerialCommunication: Error getting tty attributes %s - %s(%d).\n",
               _mDevicePath->getValue().c_str(), strerror(errno), errno);
      }
      return; //!@todo Throw exception.
    }

    // Set raw input, one second timeout.
    // These options are documented in the man page for termios.
    mTerminal = mOriginalTTYAttrs;
    mTerminal.c_cflag |= (CLOCAL | CREAD);
    mTerminal.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    mTerminal.c_oflag &= ~OPOST;
    mTerminal.c_cc[VMIN] = 0;
    mTimeoutTalk = 30;
    mTerminal.c_cc[VTIME] = 0; //mTimeoutTalk;
    mTerminal.c_ispeed = _mBaudrate->getValue(); //38400
    mTerminal.c_ospeed = _mBaudrate->getValue(); //38400
    clear_my(mTerminal.c_cflag, CSIZE|PARENB); // Control modes
    set(mTerminal.c_cflag, CS8 | CSTOPB);

    // Set the options.
    if (tcsetattr(mFileDescriptor, TCSANOW, &mTerminal) < 0)
    {
      if (_mDebug->getValue())
      {
        printf("SerialCommunication: Error setting tty attributes %s - %s(%d).\n",
               _mDevicePath->getValue().c_str(), strerror(errno), errno);
      }
      return; //!@todo Throw exception.
    }
  }

  else
  {
    if (_mDebug->getValue())
    {
      std::cout << "SerialCommunication: Error opening Port (OS unknown)\n";
    }
    return; //!@todo Throw exception.
  }

  tcflush(mFileDescriptor, TCIOFLUSH);
  mInitialized = true;
  if (_mDebug->getValue())
  {
    std::cout << "SerialCommunication: Port '" << _mDevicePath->getValue() << "' initialized\n";
  }
#else // WIN32
  return 0;
#endif // WIN32
}

int cedar::dev::com::SerialCommunication::send(const std::string& command)
{
#ifndef WIN32
  if (!mInitialized)
  {
    if (_mDebug->getValue())
    {
      std::cout << "SerialCommunication: Error Sending Data (Port not initialized)\n";
    }
    return 0;
  }

  if (_mDebug->getValue())
  {
    mTimer.start();
  }

  std::ostringstream stream;
  int s = 0; // status-information

  stream << command << mTranslatedEndOfCommandString;
  s = write(mFileDescriptor, stream.str().c_str(), stream.str().length());

  if (s < 0) // error errno occured
  {
    if (_mDebug->getValue())
    {
      std::cout << "SerialCommunication: Error Sending Data (" << strerror(errno) << ")\n";
    }
    tcflush(mFileDescriptor, TCOFLUSH);
    return 0;
  }
  if (_mDebug->getValue())
  {
    mTime = mTimer.elapsed();
    std::cout << "SerialCommunication: Sending Data Successful (" << s << " Bytes written to '"
              << _mDevicePath << "')\n" << "Sending-Time: " << mTime << " ms\n";
  }

  usleep(_mLatency->getValue()); // Delay following operations
  return 1;
#else // WIN32
  return 0;
#endif // WIN32
}

int cedar::dev::com::SerialCommunication::receive(std::string& answer)
{
#ifndef WIN32
  if (!mInitialized)
  {
    if (_mDebug->getValue())
    {
      std::cout << "SerialCommunication: Error Receiving Data (Port not initialized)\n";
    }
    return 0;
  }

  if (_mDebug->getValue())
  {
    mTimer.start();
  }

  char char_read; // the last read char
  std::ostringstream ans; //the complete string
  int s = 0; //status-information
  int e = 0; //number of read bytes
  fd_set read_fds;
  struct timeval tv;

  FD_ZERO(&read_fds);
  FD_SET(mFileDescriptor, &read_fds);

  do
  {
    tv.tv_sec = 0;
    tv.tv_usec = _mTimeOut->getValue();
    s = select(FD_SETSIZE, &read_fds, NULL, NULL, &tv); //check if channel is ready

    if (s>0) // channel ready
    {
      s = read(mFileDescriptor, &char_read, 1); //read 1 byte
      if (s>0) // reading successful
      {
        ans << char_read;
      }
      else //reading failed
      {
        if (_mDebug->getValue())
        {
          std::cout << "SerialCommunication: Error Receiving Data (" << strerror(errno) << ")\n";
        }
        tcflush(mFileDescriptor, TCIFLUSH);
        return 0;
      }
    }
    else if (s==0)  //select: timeOut
    {
      if (_mDebug->getValue())
      {
        std::cout << "SerialCommunication: Error Receiving Data (TimeOut)\n";
      }
      tcflush(mFileDescriptor, TCIFLUSH);
      return 0;
    }
    else if (s<0) //select: error
    {
      if (errno != EINTR) // error is not an interrupt
      {
        if (_mDebug->getValue())
        {
          std::cout << "SerialCommunication: Error Receiving Data (" << strerror(errno) << ")\n";
        }
        tcflush(mFileDescriptor, TCIFLUSH);
        return 0;
      }
    }
    e = ans.str().find(mTranslatedEndOfCommandString); //search End-Of-Command-String, e = -1 if not found
  } while (e < 0); //End-Of-Command-String not found

  answer = ans.str(); // write received string to desired location
  answer.resize(e); // delete End-Of-Command-String

  if (_mDebug->getValue())
  {
    mTime = mTimer.elapsed();
    std::cout << "SerialCommunication: Receiving Data Successful ("
              << e << " Byte(s) read from '" << _mDevicePath->getValue() << "')\n" << "Receiving-Time: " << mTime << " ms\n";
  }
  return e;
#else
  return 0;
#endif // WIN32
}

void cedar::dev::com::SerialCommunication::setEndOfCommandString(const std::string& eocString)
{
  mTranslatedEndOfCommandString = eocString;
  mTranslatedEndOfCommandString = cedar::aux::replace(mTranslatedEndOfCommandString, "\\r", "\r");
  mTranslatedEndOfCommandString = cedar::aux::replace(mTranslatedEndOfCommandString, "\\n", "\n");
  _mEndOfCommandString->setValue(eocString);
}

void cedar::dev::com::SerialCommunication::close()
{
#ifndef WIN32
  int s = ::close(mFileDescriptor);
  if (_mDebug->getValue())
  {
    if (s == 0)
    {
      std::cout << "Serial Communication: Port '" << _mDevicePath->getValue() << "' closed\n";
    }
    else
    {
      std::cout << "Serial Communication: Error Closing Port '" << _mDevicePath->getValue() << "'\n";
    }
  }
#endif // WIN32
}
