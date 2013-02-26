/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        Backend.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Implementation for the cedar::dev::sensors::camera::Backend class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/backends/Backend.h"
#include "cedar/devices/sensors/camera/exceptions.h"
#include "cedar/auxiliaries/ExceptionBase.h"
#include "cedar/auxiliaries/sleepFunctions.h"

// SYSTEM INCLUDES
#include <QWriteLocker>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::Backend::Backend(cedar::dev::sensors::camera::Channel* pCameraChannel)
:
mpCameraChannel(pCameraChannel)
{
}

cedar::dev::sensors::camera::Backend::~Backend()
{
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::camera::Backend::createCaptureBackend()
{
#ifdef DEBUG_CAMERA_GRABBER
  std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif


  // lock
  QWriteLocker videocapture_locking(this->mpCameraChannel->mpVideoCaptureLock);

#ifdef DEBUG_CAMERA_GRABBER
  std::cout << __PRETTY_FUNCTION__ << " Lock: " << this->mpCameraChannel->mpVideoCaptureLock << std::endl;
#endif
  // close old videoCapture device
  this->mpCameraChannel->mVideoCapture = cv::VideoCapture();

  // create cv::VideoCapture
  // on error, a cedar::dev::sensors::camera::CreateBackendException will be thrown
  this->createCaptureObject();

#ifdef DEBUG_CAMERA_GRABBER
  std::cout << __PRETTY_FUNCTION__ << " captue object created" << std::endl;
#endif


  // fill p_capabilities with values (depends on backend and camera if this is necessary at this stage)
  getAvailablePropertiesFromCamera();

#ifdef DEBUG_CAMERA_GRABBER
  std::cout << __PRETTY_FUNCTION__ << " setVideoCaptureObject to channel" << std::endl;
#endif

  // pass the new created capture to the channel structure
  mpCameraChannel->mpProperties->setVideoCaptureObject(mpCameraChannel->mVideoCapture);

#ifdef DEBUG_CAMERA_GRABBER
  std::cout << __PRETTY_FUNCTION__ << " applySettingsToCamera" << std::endl;
#endif

  // apply settings from p_settings structure
  applySettingsToCamera();

#ifdef DEBUG_CAMERA_GRABBER
  std::cout << __PRETTY_FUNCTION__ << " applyStateToCamera" << std::endl;
#endif

  // restore state of the device with the values in p_state
  applyStateToCamera();

#ifdef DEBUG_CAMERA_GRABBER
  std::cout << __PRETTY_FUNCTION__ << " get first image" << std::endl;
#endif

  //get first image
  unsigned int timeout = 0;
  while (! this->mpCameraChannel->mVideoCapture.read(this->mpCameraChannel->mImageMat))
  {
    #ifdef DEBUG_CAMERA_GRABBER
      std::cout << __PRETTY_FUNCTION__ << " Try to get an image" << std::endl;
    #endif
    ++timeout;
    cedar::aux::sleep(cedar::unit::Milliseconds(50));
    if (timeout>100)
    {
      #ifdef DEBUG_CAMERA_GRABBER
        std::cout << __PRETTY_FUNCTION__ << " Timeout on cv::VideoCapture.read()!" << std::endl;
      #endif
      break;
    }
  }

#ifdef DEBUG_CAMERA_GRABBER
  std::cout << __PRETTY_FUNCTION__ << " image grabbed" << std::endl;

  if (this->mpCameraChannel->mImageMat.empty())
  {
    std::cout << __PRETTY_FUNCTION__ << " grabbed image is empty" << std::endl;
  }
#endif

  // unlock done by the QWriteLocker "videocapture_locking" object
}

void cedar::dev::sensors::camera::Backend::applyStateToCamera()
{
#ifdef DEBUG_CAMERA_GRABBER
  std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif

  // disable signals for properties when value is updated with the camera-values
  this->mpCameraChannel->mpProperties->blockSignals(true);

  // apply values to camera
  int num_properties = cedar::dev::sensors::camera::Property::type().list().size();
  for (int i=0; i<num_properties; i++)
  {
    cedar::dev::sensors::camera::Property::Id prop_id
      = cedar::dev::sensors::camera::Property::type().list().at(i).id();

    // get the value from the configuration file or from the parameters
    double value = this->mpCameraChannel->mpProperties->getProperty(prop_id);

    // get property-mode, the real value set depends on the mode!
    cedar::dev::sensors::camera::PropertyMode::Id prop_mode_id;
    prop_mode_id = this->mpCameraChannel->mpProperties->getMode(prop_id);

    switch (prop_mode_id)
    {
    case cedar::dev::sensors::camera::PropertyMode::MANUAL:
      this->setPropertyToCamera(prop_id,value);
      break;

    // if auto: set to auto and get value form camera
    case cedar::dev::sensors::camera::PropertyMode::AUTO:
      this->setPropertyToCamera(prop_id,CAMERA_PROPERTY_MODE_AUTO);

    // if backend_default or mode "auto": get value from camera and disable the value field
    default:  //BACKEND_DEFAULT

      double set_value = this->getPropertyFromCamera(prop_id);
      this->mpCameraChannel->mpProperties->setProperty(prop_id,set_value);
      this->mpCameraChannel->mpProperties->setDefaultValue(prop_id,set_value);
    }
  }

  // allow signal processing for all properties, when changed in the processingGui or from program execution
  this->mpCameraChannel->mpProperties->blockSignals(false);
}


void cedar::dev::sensors::camera::Backend::setPropertyToCamera(unsigned int propertyId, double value)
{
  // no lock needed, the cvVideoCapture object is locked globally
#ifdef DEBUG_CAMERA_GRABBER
  std::string prop_name = cedar::dev::sensors::camera::Property::type().get(propertyId).prettyString();
  std::cout << "setPropertyToCamera "<< prop_name <<"( ID: " << propertyId << ") Value: " << value << std::endl;
#endif
  bool result = true;
  if (this->mpCameraChannel->mVideoCapture.isOpened())
  {
    //set only real values or CAMERA_PROPERTY_MODE_AUTO
    if ( (value != CAMERA_PROPERTY_NOT_SUPPORTED) || (value != CAMERA_PROPERTY_MODE_DEFAULT) )
    {
      // set value
      if (this->mpCameraChannel->mVideoCapture.set(propertyId, value))
      {
        // check if set:
        //if (! this->getPropertyFromCamera(propertyId) == value)
        //{
        //  result = false;
        //}
      }
      else
      {
        result = false;
      }
    }
  }

  // warning, if not set correct
  if (!result)
  {
    std::string prop_name = cedar::dev::sensors::camera::Property::type().get(propertyId).prettyString();
    cedar::aux::LogSingleton::getInstance()->warning
                                             (
                                               "Property " + prop_name
                                               + " couldn't set to " + boost::lexical_cast<std::string>(value),
                                               "cedar::dev::sensors::camera::Backend::setPropertyToCamera()"
                                             );
  }
}

double cedar::dev::sensors::camera::Backend::getPropertyFromCamera(unsigned int propertyId)
{
  if (! this->mpCameraChannel->mVideoCapture.isOpened())
  {
    return CAMERA_PROPERTY_NOT_SUPPORTED;
  }
    
  if (! this->mpCameraChannel->mpProperties->isSupported(propertyId))
  {
    return CAMERA_PROPERTY_NOT_SUPPORTED;
  }

  return this->mpCameraChannel->mVideoCapture.get(propertyId);
}
