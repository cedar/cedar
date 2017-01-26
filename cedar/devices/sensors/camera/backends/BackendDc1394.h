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

    File:        CameraBackendDc1394.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Header for the cedar::dev::sensors::camera::BackendDc1394 class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_CAMERA_BACKEND_DC1394_H
#define CEDAR_DEV_SENSORS_CAMERA_BACKEND_DC1394_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_LIB_DC1394

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/namespace.h"
#include "cedar/devices/sensors/camera/backends/Backend.h"

// SYSTEM INCLUDES



/*!@brief Base class of the misc camera grabber backends.
 *
 * Implements the common features of a camera device
 */
class cedar::dev::sensors::camera::BackendDc1394
:
public cedar::dev::sensors::camera::Backend
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  BackendDc1394
  (
    cedar::dev::sensors::camera::Channel* pCameraChannel
  );

  //!@brief Destructor
  ~BackendDc1394();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! does the backend initialization
  void init();

  //! update settings from gui
  //void updateSettings();

  //!@brief Enable/disable framerates for the current selected frame mode
  void updateFps();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  // derived from class Backend
  void applySettingsToCamera();
  void createCaptureObject();

  /*! @brief Opens the wanted camera with libDc methods
   *
   *  The wanted camera is determind from the camereaId of the channel (set in the constructor or in the gui)
   *  @throw cedar::dev::sensors::camera::LibDcCameraNotFoundException Thrown, if the wanted or no camera is not found
   *  @throw cedar::dev::sensors::camera::LibDcException Thrown, if the camera couldn't be opened by
   *         the firewire backend
   */
  void openLibDcCamera();

  //! @brief Get all features of the opened camera from libdc
  void readFeaturesFromLibDc();

  /*! @brief Get all framerates and enable them in the enum-class, disable all others
   *  @param modeId The grabbing mode for the framerates (framerate selection depends on actual used grabbing mode)
   */
  void readFrameRatesFromLibDc(cedar::dev::sensors::camera::VideoMode::Id modeId);

  //! @brief Get all available modes and enable them in the enum-class, disable all others and set mode to "AUTO"
  void readGrabModesFromLibDc();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*! @brief Search the Firewire-Bus for the camera with the given GUID and return the bus-ID
   *  @param guid The GUID of the camera to search
   *  @throw  cedar::dev::sensors::camera::LibDcCameraNotFoundException Thrown, if the wanted camera is not found
   *  @return The Bus-ID of the Camera.
   */
  unsigned int getBusIdFromGuid(unsigned int guid);



  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! @brief The firewire interface for available settings and properties from camera
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

}; // class cedar::dev::sensors::camera::BackendDc1394

#endif // defined CEDAR_USE_LIB_DC1394

#endif // CEDAR_DEV_SENSORS_CAMERA_BACKEND_DC1394_H

