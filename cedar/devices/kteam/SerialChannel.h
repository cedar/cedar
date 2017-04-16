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
    Date:        2012 11 26

    Description: Channel to KTEAM serial devices.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KTEAM_SERIAL_CHANNEL_H
#define CEDAR_DEV_KTEAM_SERIAL_CHANNEL_H

// CEDAR INCLUDES
#include "cedar/devices/kteam/namespace.h"
#include "cedar/devices/SerialChannel.h"

// SYSTEM INCLUDES


/*!@brief Channel to KTEAM serial devices.
 *
 */
class cedar::dev::kteam::SerialChannel : public cedar::dev::SerialChannel
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested classes
  //--------------------------------------------------------------------------------------------------------------------
public:
  class ConnectionTimeOutException : public cedar::aux::ExceptionBase {};

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief After opening the channel, the read buffer needs to be cleaned from the status message sent by the robot.
  virtual void postOpenHook();

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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet
}; // class cedar::dev::kteam::SerialChannel
#endif // CEDAR_DEV_KTEAM_SERIAL_CHANNEL_H
