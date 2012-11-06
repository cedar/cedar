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

    File:        CameraDeviceDc1394.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Header for the cedar::dev::sensors::camera::DeviceDc1394 class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_CAMERA_DEVICE_DC1394_H
#define CEDAR_DEV_SENSORS_CAMERA_DEVICE_DC1394_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_LIB_DC1394

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/namespace.h"
#include "cedar/devices/sensors/camera/backends/Device.h"

// SYSTEM INCLUDES



/*!@brief Base class of the misc camera grabber backends.
 *
 * Implements the common features of a camera device
 */
class cedar::dev::sensors::camera::DeviceDc1394
:
public cedar::dev::sensors::camera::Device
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  DeviceDc1394
  (
    cedar::dev::sensors::camera::Channel* pCameraChannel
  );

  //!@brief Destructor
  ~DeviceDc1394();

  void initDevice();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void setProperties();
  void applySettingsToCamera();
  bool createCaptureDevice();
  void applyStateToCamera();

  /*! @brief Opens the wanted camera with libDc methods
   *
   *  The wanted camera is determind from the camereaId of the channel (set in the constructor or in the gui)
   *  @returns True, if camera was successfully opened, otherwise false
   */
  bool openLibDcCamera();

  //! get all features from cam
  void getFeaturesFromLibDc();

  /*! get all framerates and enable them in the enum-class, disable all others
   *  \param modeId The grabbing mode for the framerates (framerate selection depends on actual used grabbing mode)
   */
  void getFrameRatesFromLibDc(cedar::dev::sensors::camera::VideoMode::Id modeId);

  //! get all available modes and enable them in the enum-class, disable all others
  void getGrabModesFromLibDc();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet


  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! firewire interface for available settings and properties from camera
  cedar::dev::sensors::camera::LibDcBasePtr mpLibDcInterface;

private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::sensors::camera::DeviceDc1394

#endif // defined CEDAR_USE_LIB_DC1394

#endif // CEDAR_DEV_SENSORS_CAMERA_DEVICE_DC1394_H

