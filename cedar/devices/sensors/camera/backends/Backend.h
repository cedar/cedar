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

    File:        Backend.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Header for the cedar::dev::sensors::camera::Backend class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_CAMERA_BACKEND_H
#define CEDAR_DEV_SENSORS_CAMERA_BACKEND_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/namespace.h"
#include "cedar/auxiliaries/SetParameter.h"
#include "cedar/devices/sensors/camera/Properties.h"
#include "cedar/devices/sensors/camera/Channel.h"
#include "cedar/devices/sensors/camera/exceptions.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <QReadWriteLock>


/*!@brief Base class of the misc camera grabber backends.
 *
 * Implements the common features of the backends
 */
class cedar::dev::sensors::camera::Backend
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------


  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The standard constructor.
  Backend(cedar::dev::sensors::camera::Channel* pCameraChannel);

public:
  //!@brief Destructor
  virtual ~Backend();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief This method initializes the backend. It have to be invoked after the creation of the class
   * 
   *  In this method, properties and settings from the backend should be set.
   *  The values could be read probably with the help of external libraries probably in order to read
   *  min/max values or supported features from the wanted Backend and set them in the channel structure.
   *  This method is called before createCaptureBackend() is invoked.
   *
   *  @throw cedar::dev::sensors::camera::CreateBackendException Thrown, if the backend couldn't be created
   *
   */
  virtual void init() = 0;

  /*! @brief Initialization of the class
   *
   *  This function have to be called after the class was created.
   *  @throw cedar::dev::sensors::camera::CreateBackendException This exception is thrown,
   *         if the cameras could not initialized
   */
  void createCaptureBackend();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  /*! Create a new capture - object for the backend
   *
   *   @throw cedar::dev::sensors::camera::CreateBackendException This exception will be thrown, if an error occurs
   */
  virtual void createCaptureObject() = 0;

  //! Apply all Settings to the Camera
  virtual void applySettingsToCamera() = 0;

  //! Apply all Parameters from the GUI to the Camera
  void applyStateToCamera();

  /*! @brief Set a property direct in the cv::VideoCapture class
   *
   *  @param propertyId The OpenCV constants for cv::VideoCapture.set() method
   *  @param value The new value
   */
  void setPropertyToCamera(unsigned int propertyId, double value);


  /*! @brief Get a property directly form the cv::VideoCapture
   *
   *    Use this method only for properties which are not (yet) supported by cedar CameraProperty()
   *    or CameraSetting() class. But be aware, that there is no check if the wanted property is supported
   *    by the used backend
   *
   *  @param propertyId The OpenCV constants for cv::VideoCapture.set() method
   *  @throw VideoCaptureNotOpenedException Throws this exception when the used cv::VideoCapture is not opened.
   *  @throw PropertyNotSupportedException Thrown, when the given property is not supported by the actual used backend
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
  //! @brief The channel structure
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

}; // class cedar::dev::sensors::camera::Backend
#endif // CEDAR_DEV_SENSORS_CAMERA_BACKEND_H

