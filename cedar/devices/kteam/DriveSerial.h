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

    File:        DriveSerial.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 26

    Description: Add description.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KTEAM_DRIVE_SERIAL_H
#define CEDAR_DEV_KTEAM_DRIVE_SERIAL_H

// CEDAR INCLUDES
#include "cedar/devices/kteam/Drive.h"
#include "cedar/devices/namespace.h"
#include "cedar/devices/kteam/namespace.h"

// SYSTEM INCLUDES
#include <vector>


/*!@brief Add description.
 */
class cedar::dev::kteam::DriveSerial : public cedar::dev::kteam::Drive
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor
  DriveSerial();

  //!@brief Constructor taking an externally created channel
  DriveSerial(cedar::dev::kteam::SerialChannelPtr channel);

  ~DriveSerial();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns the current encoder value of the left and right wheel.
  cv::Mat retrieveEncoders() const;

  /*!@brief Sets the encoder values of both wheels.
   * @param[in] encoders encoder value for the left and right wheel
   */
  void sendEncoders(const cv::Mat &mat);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! @see Base class.
  virtual void sendMovementCommand();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void init();

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
  //! string representing the "set speed" command
  cedar::aux::StringParameterPtr _mCommandSetSpeed;
  //! string representing the "set encoder" command
  cedar::aux::StringParameterPtr _mCommandSetEncoder;
  //! string representing the "get encoder" command
  cedar::aux::StringParameterPtr _mCommandGetEncoder;

private:
  // none yet
}; // class cedar::dev::kteam::DriveSerial

#endif // CEDAR_DEV_KTEAM_DRIVE_H
