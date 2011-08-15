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

    File:        SerialCommunication.h

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: This class provides a string-based communication with an external device using a Serial Port.

    Credits:     Marc Sons (Author of msSerialTransport.h this class is a revised and cedar-compatible version of)

======================================================================================================================*/

#ifndef CEDAR_DEV_COM_SERIAL_COMMUNICATION_H_
#define CEDAR_DEV_COM_SERIAL_COMMUNICATION_H_
//!@todo Serial communication needs implementation for windows.

// LOCAL INCLUDES

#include "devices/com/Communication.h"

// PROJECT INCLUDES

#include "auxiliaries/ConfigurationInterface.h"

// SYSTEM INCLUDES

#ifndef WIN32
  #include <termios.h>
  #include <errno.h>
  #include <fcntl.h>
#else
  // some dummy types for windows.
  struct termios {};
#endif // WIN32
#include <QTime>

/*!@brief This class provides a string-based communication with an external device using a Serial Port.
 *
 * This includes opening and closing the Serial Port as well as sending and receiving strings. Examples for such
 * devices are mobile robots (E-Puck, Khepera). It is also possible to lock the channel to prevent read-/write-errors
 * if multiple threads are accessing the device (Implemented in Communication.h). The parameters of the communication
 * are read from a configuration-file.
 */
class cedar::dev::com::SerialCommunication : public Communication, public cedar::aux::ConfigurationInterface
{

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------

public:

  //!@brief Initiates a new communication with an external device per Serial Port.
  //!@param config Location and name of the Configuration-File to be used.
  SerialCommunication(const std::string config);

  //!@brief Ends the communication with the device and closes the channel.
  ~SerialCommunication();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------

public:

  //!@brief Initializes the communication and opens the channel.
  //!@return 1 if initialization was successful, else 0.
  int init();

  //!@brief The get-function of the initialization-status.
  //!@return true if initialized, else false
  bool isInitialized() const;

  //!@brief The get-function of the File Descriptor.
  //!@return File Descriptor of the communication.
  int getFileDescriptor() const;

  //!@brief The get-function of the communication's name.
  //!@return Name of the communication.
  const std::string& getName() const;

  //!@brief The get-function of the device's path.
  //!@return Path of the Serial Port.
  const std::string& getDevicePath() const;

  //!@brief The get-function of the End-of-Command-String.
  //!@return The End-Of-Command-String.
  const std::string& getEndOfCommandString() const;

  //!@brief The get-function of the C-Flag.
  //!@return The Country-Flag.
  int getCFlag() const;

  //!@brief The get-function of the current Baud Rate.
  //!@return The Baud Rate used by the Serial Port (in Bit/s).
  unsigned int getBaudrate() const;

  //!@brief The get-function of the time-out value.
  //!@return Time in microsecs until current read-/write-operation times out.
  unsigned int getTimeOut() const;

  //!@brief The get-function of the latency.
  //!@return Delay of next operation after send (in microsecs).
  unsigned int getLatency() const;

  /*!@brief Sends a string to the device.
   *@param command The string to be sent.
   *@return Returns 1 if sending was successful and 0 in case of an error.
   */
  int send(const std::string& command);

  /*!@brief Receives a string from the device.
   *@param answer Variable the received string shall be stored in.
   *@return Returns number of received bytes on success and 0 in case of an error.
   */
  int receive(std::string& answer);

  /*!@brief Specifies the new End-Of-Command-String.
   *@param eocString New End-Of-Command-String.
   *
   *The End-Of-Command-String is also read from the configuration-file when the communication is initialized.
   */
  void setEndOfCommandString(const std::string& eocString);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------

protected:

  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------

private:

  //!@brief  Closes the channel.
  void close();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------

public:

  // none yet (hopefully never!)

protected:

  // none yet

private:

  //!@brief File Descriptor of the communication.
  int mFileDescriptor;

  //!@brief Initialization status of the Serial Port.
  // True if SerialCommunication has been initialized, else false.
  bool mInitialized;

  //!@brief Terminal IO.
  struct termios mTerminal;

  //!@brief Help variable for communication settings with OS X.
  unsigned mTimeoutTalk;

  //!@brief Help variable for communication settings with OS X.
  struct termios mOriginalTTYAttrs;

  //!@brief Timer for stopping time of send- and receive-operations.
  QTime mTimer;

  //!@brief Time elapsed since start of send-/receive-operation.
  int mTime;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------

public:

  // none yet (hopefully never!)

protected:

  // none yet

private:

  /*!@brief Name of the communication.
   *Default is "Serial Communication".
   */
  std::string _mName;

  /*!@brief Path of the Serial Port.
   *Default is "/dev/rfcomm0" (Bluetooth).
   */
  std::string _mDevicePath;

  /*!@brief The End-Of-Command-String.
   *Marks the end of the sent/received string. Default is "\r\n" (carriage return + line feed).
   */
  std::string _mEndOfCommandString;

  /*!@brief The Country-Flag.
   *C-Flag is an identifier of the user's country. Set CFlag to 0 if in Germany (Default) or 1 if in USA.
   */
  int _mCFlag;

  /*!@brief The Baudrate which shall be used by the Serial Port (in Bit/s).
   *Set it to 15 for 38400 Bit/s or to 4098 for 115200 Bit/s (Default).
   */
  unsigned int _mBaudrate;

  /*!@brief Time in microsecs until current read-/write-operation times out.
   *The TimeOut prevents deadlocks that occur if the device does not respond. Default is 250000 microsecs.
   */
  unsigned int _mTimeOut;

  /*!@brief Delay of next operation after send (in microsecs).
   *Shouldn't be set too low to prevent reading-/writing-interference. Default is 10000 microsecs.
   */
  unsigned int _mLatency;

  /*!@brief The Debug-Flag.
   *If true, error-messages, sending-/receiving-times and number of sent/received bytes per message are displayed on
   *Console, else not.
   */
  bool _mDebug;

  //!brief The user's Operating System ("Linux" (Default) or "Apple")
  std::string _mOS;

}; // class cedar::dev::com::SerialCommunication

#endif // CEDAR_DEV_COM_SERIAL_COMMUNICATION_H_
