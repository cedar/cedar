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

    File:        SerialChannel.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2013 02 05

    Description: Channel to serial devices, based on Boost ASIO.

    Credits:     Based on the class TimeoutSerial, written by Terraneo Federico.
                 http://gitorious.org/serial-port/serial-port/trees/master/2_with_timeout

======================================================================================================================*/

#ifndef CEDAR_DEV_SERIAL_CHANNEL_H
#define CEDAR_DEV_SERIAL_CHANNEL_H

// CEDAR INCLUDES
#include "cedar/devices/Channel.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/TimeParameter.h"

// FORWARD DECLARATIONS
#include "cedar/devices/SerialChannel.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#ifndef Q_MOC_RUN
  #include <boost/utility.hpp>
  #include <boost/asio.hpp>
#endif // Q_MOC_RUN
#include <string>
#include <QReadWriteLock>


//!@brief Channel to serial devies, based on Boost ASIO.
class cedar::dev::SerialChannel : public QObject, public cedar::dev::Channel
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Exception that notifies the user if something goes wrong during writing.
  class WriteException : public cedar::aux::ExceptionBase {};
  class BoostException : public cedar::aux::ExceptionBase {};

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The default constructor.
  SerialChannel();

  //!@brief Destructor.
  ~SerialChannel();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Opens the channel.
  virtual void openHook();

  //!@brief  Closes the channel.
  virtual void closeHook();

  virtual bool isOpen() const;

  //!@brief Returns the device path and name of the serial port.
  const std::string& getDevicePath() const;

  //!@brief Returns the string that signals the end of commands.
  const std::string& getCommandDelimiter() const;

  //!@brief Sets the string that signals the end of commands.
  void setCommandDelimiter(const std::string& commandDelimiter);

  //!@brief Returns the baud rate (Bit/s) set for the serial port.
  unsigned int getBaudRate() const;

  //!@brief Returns the timeout in seconds.
  const cedar::unit::Time& getTimeout() const;

  /*!@brief Writes a string to the serial port.
   * Always supply commands without the trailing command delimiter, as it is automatically added.
   */
  void write(std::string command);

  //!@brief Reads a string from the serial port.
  std::string read();

  /*!@brief Writes a string and reads the answer. While doing so, the channel is locked.
   * Always supply commands without the trailing command delimiter, as it is automatically added.
   */
  std::string writeAndReadLocked(const std::string& command);
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Reads parameters from a configuration node.
  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  //!@brief Sets up the reading process.
  void setupRead();

  /*!@brief Handler that is called whenever a read operation was successful.
   * In a successful read, the command delimiter is reached before the timeout expires.
   */
  void readCompleted(const boost::system::error_code& error, const size_t bytesTransferred);

  //!@brief Handler that is called whenever the timeout expires on read operations.
  void timeoutExpired(const boost::system::error_code& error);

private slots:
  //!@brief Constructs the actual command delimiter from an escaped version that the user inputs.
  void updateCommandDelimiter();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! string that signals the end of commands
  std::string mCommandDelimiter;
  //! IO service object
  boost::asio::io_service mIoService;
  //! serial port object
  boost::asio::serial_port mPort;
  //! timer for the timeout
  boost::asio::deadline_timer mTimer;
  //! buffer for received data
  boost::asio::streambuf mReadData;
  //! number of received bytes
  size_t mBytesTransferred;

  //! possible results of receiving an answer
  enum ReadResult
  {
    readInProgress,
    readSuccess,
    readError,
    readTimeoutExpired
  };

  //! current status of the read operation
  enum ReadResult mReadResult;

  QReadWriteLock mLock;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  /*!@brief Path and name of the serial port.
   * Default is "/dev/rfcomm0" (Bluetooth).
   */
  cedar::aux::StringParameterPtr _mDevicePath;

  /*!@brief Marks the end of sent and received strings.
   * Default is "\\r\\n" (escaped carriage return + escaped line feed).
   */
  cedar::aux::StringParameterPtr _mEscapedCommandDelimiter;

  /*!@brief The baud rate (Bit/s) used for communicating with the serial port.
   *        Default is 115200 Bit/s.
   */
  cedar::aux::UIntParameterPtr _mBaudRate;

  /*!@brief Time (s) until the current read or write operation times out.
   * Default is 0.25 s (250 ms).
   */
  cedar::aux::TimeParameterPtr _mTimeout;

}; // class cedar::dev::SerialChannel
#endif // CEDAR_DEV_SERIAL_CHANNEL_H
