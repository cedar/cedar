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

    File:        Device.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Header for the cedar::dev::sensors::camera::Device class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_CAMERA_DEVICE_H
#define CEDAR_DEV_SENSORS_CAMERA_DEVICE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/namespace.h"
#include "cedar/auxiliaries/SetParameter.h"
#include "cedar/devices/sensors/camera/Properties.h"
#include "cedar/devices/sensors/camera/Channel.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <QReadWriteLock>


/*!@brief Base class of the misc camera grabber backends.
 *
 * Implements the common features of a camera device
 */
class cedar::dev::sensors::camera::Device
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------


  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The standard constructor.
  Device(cedar::dev::sensors::camera::Channel* pCameraChannel);

public:
  //!@brief Destructor
  virtual ~Device();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //! does the backend initialization
  virtual void initDevice();


  bool init();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //! Set all properties to the camera
  virtual void setProperties() = 0;

  //! Create a new Capture device
  virtual bool createCaptureDevice() = 0;

  //! Apply all Settings to the Camera
  virtual void applySettingsToCamera() = 0;

  //! Apply all Parameters to the Camera
  virtual void applyStateToCamera() = 0;


  /*! @brief Set a property direct in the cv::VideoCapture class
   *
   *  @param propertyId The OpenCV constants for cv::VideoCapture.set() method
   *  @param value The new value
   *  @return Boolean value, that indicates if the value is properly set
   */
  bool setPropertyToCamera(unsigned int propertyId, double value);


  /*! @brief Get a property directly form the cv::VideoCapture
   *
   *    Use this method only for properties which are not (yet) supported by cedar CameraProperty()
   *    or CameraSetting() class. But be aware, that there is no check if the wanted property is supported
   *    by the used backend
   *
   *  @param propertyId The OpenCV constants for cv::VideoCapture.set() method
   *  @return Value, that indicates the exit-state of cv::VideoCapture.set()
   */
  double getPropertyFromCamera(unsigned int propertyId);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! The channel structure
  cedar::dev::sensors::camera::Channel* mpCameraChannel;

private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::sensors::camera::Device

/*
// The typedefs for the manager of the Devices
#include "cedar/auxiliaries/FactoryManager.h"

namespace cedar
{
  namespace dev
  {
    namespace sensors
    {
      namespace visual
      {
        //!@brief The manager of all sigmoind instances
        typedef cedar::aux::FactoryManager<DevicePtr> DeviceManager;

#ifdef MSVC
#ifdef CEDAR_LIB_EXPORTS_DEV
        // dllexport
        template class __declspec(dllexport) cedar::aux::Singleton<DeviceManager>;
#else // CEDAR_LIB_EXPORTS_DEV
      // dllimport
        extern template class __declspec(dllimport) cedar::aux::Singleton<DeviceManager>;
#endif // CEDAR_LIB_EXPORTS_DEV
#endif // MSVC

        //!@brief The singleton object of the TransferFunctionFactory.
        typedef cedar::aux::Singleton<DeviceManager> DeviceManagerSingleton;
      }
    }
  }
}
*/
#endif // CEDAR_DEV_SENSORS_CAMERA_DEVICE_H

