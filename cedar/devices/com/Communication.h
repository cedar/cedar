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

    File:        Communication.h

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: This class provides a string-based communication with an external device.

    Credits:     Marc Sons (Author of msTransport.h this class is a revised and cedar-compatible version of)

======================================================================================================================*/

#ifndef CEDAR_DEV_COM_COMMUNICATION_H_
#define CEDAR_DEV_COM_COMMUNICATION_H_

// LOCAL INCLUDES

#include "devices/com/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

#include <string>
#include <QReadWriteLock>

/*!@brief This class provides a string-based communication with an external device.
 *
 * This includes opening and closing the channel as well as sending and receiving strings. Examples for external
 * devices communicating per string are mobile robots (E-Puck, Khepera). It is also possible to lock the channel to
 * prevent read-/write-errors if multiple threads are accessing the device.
 */
class cedar::dev::com::Communication
{

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------

public:

  //!@brief Initiates a new communication with an external device.
  Communication();

  //!@brief Ends the communication with the device.
  virtual ~Communication ();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------

public:

  /*!@brief Sends a string to the device.
   *@param command The string to be sent.
   *@return 1 if sending was successful and 0 in case of an error.
   */
  virtual int send(const std::string& command) = 0;

  /*!@brief Receives a string from the device.
   *@param answer Variable the received string shall be stored in.
   *@return number of received bytes on success and 0 in case of an error.
   */
  virtual int receive(std::string& answer) = 0;

  /*!@brief Locks the channel for reading or writing.
   *
   *This function locks the channel for reading or writing operations. If the channel is already locked, the calling
   *thread is blocked until the channel is unlocked again. Always call unlock() after locking the channel!
   */
  void lock();

  //!@brief Unlocks the channel. If the channel is currently not locked, this function has no effect.
  void unlock();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------

protected:

  //!@brief Closes the channel.
  virtual void close() = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------

private:

  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------

public:

  // none yet (hopefully never!)

protected:

  //!@brief Implements the read-/write-lock.
  QReadWriteLock mLock;

private:

  //!@brief Status of the read-/write-lock. True if one thread has currently locked the channel, else false.
  bool mLocked;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------

public:
  // none yet (hopefully never!)

protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::com::Communication

#endif // CEDAR_DEV_COM_COMMUNICATION_H_
