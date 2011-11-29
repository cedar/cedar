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

    File:        CameraFrameRate.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description:  Header for CameraFrameRate enum-type class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_FRAMERATE_H
#define CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_FRAMERATE_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/EnumType.h"
#include "namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief Enum class for firewire camera fps setting mapped from dc1394/video.h
 *
 * Use this type for the CameraGrabber::setCameraInitFps() and getCameraInitFps() method.
 *
 * This enumeration is used for non-Format_7 modes. The framerate can be lower than expected if the
 * exposure time is longer than the requested frame period. Framerate can be controlled in a number of
 * other ways: framerate feature, external trigger, software trigger, shutter throttling and packet size
 * (Format_7)
 */
class cedar::dev::sensors::visual::CameraFrameRate
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
public:
  typedef cedar::aux::EnumId Id;
public:
  typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  static void construct();

  static const cedar::aux::EnumBase& type();
  static const cedar::dev::sensors::visual::CameraFrameRate::TypePtr& typePtr();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  static cedar::aux::EnumType<cedar::dev::sensors::visual::CameraFrameRate> mType;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  static const Id FRAMERATE_1_875 =  1;
  static const Id FRAMERATE_3_75 = 3;
  static const Id FRAMERATE_7_5 = 7;
  static const Id FRAMERATE_15 = 15;
  static const Id FRAMERATE_30 = 30;
  static const Id FRAMERATE_60 = 60;
  static const Id FRAMERATE_120 = 120;
  static const Id FRAMERATE_240 = 240;

protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::xxx

#endif // CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_FRAMERATE_H

