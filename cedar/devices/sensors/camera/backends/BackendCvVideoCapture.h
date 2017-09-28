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

    File:        BackendCvVideoCapture.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Header for the cedar::dev::sensors::camera::BackendCvVideoCapture class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_CAMERA_BACKEND_CVVIDEOCAPTURE_H
#define CEDAR_DEV_SENSORS_CAMERA_BACKEND_CVVIDEOCAPTURE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/namespace.h"
#include "cedar/devices/sensors/camera/backends/Backend.h"

// SYSTEM INCLUDES


/*!@brief Implements an OpenCV CvVideoCapture-object as camera-backend */
class cedar::dev::sensors::camera::BackendCvVideoCapture
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
  BackendCvVideoCapture(cedar::dev::sensors::camera::Channel* pCameraChannel);

  //!@brief Destructor
  ~BackendCvVideoCapture();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // derived from class Device
  void init();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // derived from class Device
  void applySettingsToCamera();
  void createCaptureObject();

  /*! Updates the properties of the camera.
   */
  void readCameraProperties();

  //!@deprecated Use readCameraProperties() instead.
  CEDAR_DECLARE_DEPRECATED(void getAvailablePropertiesFromCamera())
  {
    this->readCameraProperties();
  }

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

}; // class cedar::dev::sensors::camera::BackendCvVideoCapture

#endif // CEDAR_DEV_SENSORS_CAMERA_BACKEND_CVVIDEOCAPTURE_H

