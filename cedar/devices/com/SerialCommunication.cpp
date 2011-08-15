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

// LOCAL INCLUDES

#include  "SerialCommunication.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


using namespace cedar::dev::com;

#define clear_my(var, mask)    var &= (~(mask))
#define set(var, mask)      var |= (mask)

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

SerialCommunication::SerialCommunication(const std::string config) : cedar::aux::ConfigurationInterface(config)
{
  mInitialized = false;
  init();
}

SerialCommunication::~SerialCommunication()
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

bool SerialCommunication::isInitialized() const
{
  return mInitialized;
}

int SerialCommunication::getFileDescriptor() const
{
  return mFileDescriptor;
}

const std::string& SerialCommunication::getName() const
{
  return _mName;
}

const std::string& SerialCommunication::getDevicePath() const
{
  return _mDevicePath;
}

const std::string& SerialCommunication::getEndOfCommandString() const
{
  return _mEndOfCommandString;
}

int SerialCommunication::getCFlag() const
{
  return _mCFlag;
}

unsigned int SerialCommunication::getBaudrate() const
{
  return _mBaudrate;
}

unsigned int SerialCommunication::getTimeOut() const
{
  return _mTimeOut;
}

unsigned int SerialCommunication::getLatency() const
{
  return _mLatency;
}

int SerialCommunication::init()
{
  if (mInitialized)
  {
    if (_mDebug)
    {
      std::cout << "SerialCommunication: Initialization failed (already initialized)\n";
    }
    return 0;
  }

  // initialization of members
  mFileDescriptor = 0;
  mInitialized = false;
  mTimeoutTalk = 0;
  _mName = "";
  _mDevicePath = "";
  _mBaudrate = 0;
  _mEndOfCommandString = "";
  _mCFlag = 0;
  _mTimeOut = 0;
  _mLatency = 0;
  _mDebug = false;
  mTime  = 0;
  _mOS = "";

  // reading parameters from configuration-file
  addParameter(&_mName, "Name", "Serial Communication");
  addParameter(&_mDevicePath, "DevicePath", "/dev/rfcomm0");
  addParameter(&_mBaudrate, "BaudRate", 4098);
  addParameter(&_mEndOfCommandString, "EndOfCommandString", "\r\n");
  addParameter(&_mCFlag, "CFlag", 0);
  addParameter(&_mTimeOut, "TimeOut", 250000);
  addParameter(&_mLatency, "Latency", 10000);
  addParameter(&_mDebug, "Debug", false);
  addParameter(&_mOS, "OperatingSystem", "Linux");
  this->readOrDefaultConfiguration();

  setEndOfCommandString(_mEndOfCommandString); // sets end-of-command-string

#ifndef WIN32
  // initialize communication on Linux
  if(_mOS == "Linux") //!@todo Switch this to a #ifdef block?
  {
    mFileDescriptor = open(_mDevicePath.c_str(), O_RDWR | O_NOCTTY);

    if (mFileDescriptor == -1)
    {
      if (_mDebug)
      {
        std::cout << "SerialCommunication: Error opening Port '" << _mDevicePath << "' on Linux\n";
      }
      return 0;
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
    switch (_mCFlag)
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

    cfsetospeed(&mTerminal, _mBaudrate);
    cfsetispeed(&mTerminal, _mBaudrate);
    mTerminal.c_cc[VMIN] = 0;
    mTerminal.c_cc[VTIME] = 0;
    tcsetattr(mFileDescriptor, TCSANOW, &mTerminal);
  }

  // initialize communication on Mac

  else if(_mOS == "Apple")
  {
    // mPortId="/dev/tty.PL2303-0000101D";
    mFileDescriptor = open(_mDevicePath.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (mFileDescriptor < 0)
    {
      if (_mDebug)
      {
        std::cout << "SerialCommunication: Error opening Port '" << _mDevicePath << "' on Mac.\n";
      }
      return 0;
    }

    if (fcntl(mFileDescriptor, F_SETFL, 0) < 0)
    {
      if (_mDebug)
      {
        printf("SerialCommunication: Error clearing O_NDELAY %s - %s(%d).\n",
               _mDevicePath.c_str(), strerror(errno), errno);
      }
      return 0;
    }

    // Get the current options and save them for later reset.
    if (tcgetattr(mFileDescriptor, &mOriginalTTYAttrs) < 0)
    {
      if (_mDebug)
      {
        printf("SerialCommunication: Error getting tty attributes %s - %s(%d).\n",
               _mDevicePath.c_str(), strerror(errno), errno);
      }
      return 0;
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
    mTerminal.c_ispeed = _mBaudrate; //38400
    mTerminal.c_ospeed = _mBaudrate; //38400
    clear_my(mTerminal.c_cflag, CSIZE|PARENB); // Control modes
    set(mTerminal.c_cflag, CS8 | CSTOPB);

    // Set the options.
    if (tcsetattr(mFileDescriptor, TCSANOW, &mTerminal) < 0)
    {
      if (_mDebug)
      {
        printf("SerialCommunication: Error setting tty attributes %s - %s(%d).\n",
               _mDevicePath.c_str(), strerror(errno), errno);
      }
      return 0;
    }
  }

  else
  {
    if (_mDebug)
    {
      std::cout << "SerialCommunication: Error opening Port (OS unknown)\n";
    }
    return 0;
  }

  tcflush(mFileDescriptor, TCIOFLUSH);
  mInitialized = true;
  if (_mDebug)
  {
    std::cout << "SerialCommunication: Port '" << _mDevicePath << "' initialized\n";
  }
  return 1;
#else // WIN32
  return 0;
#endif // WIN32
}

int SerialCommunication::send(const std::string& command)
{
#ifndef WIN32
  if (!mInitialized)
  {
    if (_mDebug)
    {
      std::cout << "SerialCommunication: Error Sending Data (Port not initialized)\n";
    }
    return 0;
  }

  if (_mDebug)
  {
    mTimer.start();
  }

  std::ostringstream stream;
  int s = 0; // status-information

  stream << command << _mEndOfCommandString;
  s = write(mFileDescriptor, stream.str().c_str(), stream.str().length());

  if (s < 0) // error errno occured
  {
    if (_mDebug)
    {
      std::cout << "SerialCommunication: Error Sending Data (" << strerror(errno) << ")\n";
    }
    tcflush(mFileDescriptor, TCOFLUSH);
    return 0;
  }
  if (_mDebug)
  {
    mTime = mTimer.elapsed();
    std::cout << "SerialCommunication: Sending Data Successful (" << s << " Bytes written to '"
              << _mDevicePath << "')\n" << "Sending-Time: " << mTime << " ms\n";
  }

  usleep(_mLatency); // Delay following operations
  return 1;
#else // WIN32
  return 0;
#endif // WIN32
}

int SerialCommunication::receive(std::string& answer)
{
#ifndef WIN32
  if (!mInitialized)
  {
    if (_mDebug)
    {
      std::cout << "SerialCommunication: Error Receiving Data (Port not initialized)\n";
    }
    return 0;
  }

  if (_mDebug)
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
    tv.tv_usec = _mTimeOut;
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
        if (_mDebug)
        {
          std::cout << "SerialCommunication: Error Receiving Data (" << strerror(errno) << ")\n";
        }
        tcflush(mFileDescriptor, TCIFLUSH);
        return 0;
      }
    }
    else if (s==0)  //select: timeOut
    {
      if (_mDebug)
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
        if (_mDebug)
        {
          std::cout << "SerialCommunication: Error Receiving Data (" << strerror(errno) << ")\n";
        }
        tcflush(mFileDescriptor, TCIFLUSH);
        return 0;
      }
    }
    e = ans.str().find(_mEndOfCommandString); //search End-Of-Command-String, e = -1 if not found
  } while (e < 0); //End-Of-Command-String not found

  answer = ans.str(); // write received string to desired location
  answer.resize(e); // delete End-Of-Command-String

  if (_mDebug)
  {
    mTime = mTimer.elapsed();
    std::cout << "SerialCommunication: Receiving Data Successful ("
              << e << " Byte(s) read from '" << _mDevicePath << "')\n" << "Receiving-Time: " << mTime << " ms\n";
  }
  return e;
#else
  return 0;
#endif // WIN32
}

void SerialCommunication::setEndOfCommandString(const std::string& eocString)
{
  _mEndOfCommandString = eocString;
}

void SerialCommunication::close()
{
#ifndef WIN32
  int s = ::close(mFileDescriptor);
  if (_mDebug)
  {
    if (s == 0)
    {
      std::cout << "Serial Communication: Port '" << _mDevicePath << "' closed\n";
    }
    else
    {
      std::cout << "Serial Communication: Error Closing Port '" << _mDevicePath << "'\n";
    }
  }
#endif // WIN32
}
