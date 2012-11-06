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

    File:        CameraBackendType.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Implementation for the cedar::dev::sensors::camera::Device class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/backends/Device.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::Device::Device(cedar::dev::sensors::camera::Channel* pCameraChannel)
:
mpCameraChannel(pCameraChannel)
{
}

cedar::dev::sensors::camera::Device::~Device()
{
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::Device::initDevice()
{

}

bool cedar::dev::sensors::camera::Device::init()
{
  bool result = true;

  // lock
  this->mpCameraChannel->mpVideoCaptureLock->lockForWrite();

  // close old videoCapture device
  this->mpCameraChannel->mVideoCapture = cv::VideoCapture();
//  this->mpCameraChannel->mVideoCapture.release();

  // create cv::VideoCapture
  result = this->createCaptureDevice();

  if (!result)
  {
    return false;
  }

  // fill p_capabilities with the right values (depends on backend and camera)
  this->setProperties();

  // pass the new created capture to the channel structure
  mpCameraChannel->mpProperties->setVideoCaptureObject(mpCameraChannel->mVideoCapture);

  // apply settings from p_settings structure
  this->applySettingsToCamera();

  // restore state of the device with the values in p_state
  this->applyStateToCamera();

  // unlock
  this->mpCameraChannel->mpVideoCaptureLock->unlock();

  return true;
}


bool cedar::dev::sensors::camera::Device::setPropertyToCamera(unsigned int propertyId, double value)
{
  // no lock needed, the cvVideoCapture object is globally locked
  std::string prop_name = cedar::dev::sensors::camera::Property::type().get(propertyId).prettyString();
  std::cout << "setPropertyToCamera "<< prop_name <<"( ID: " << propertyId << ") Value: " << value << std::endl;
  bool result = false;
  if (this->mpCameraChannel->mVideoCapture.isOpened())
  {
    //set only real values or CAMERA_PROPERTY_MODE_AUTO
    if ( (value != CAMERA_PROPERTY_NOT_SUPPORTED) || (value != CAMERA_PROPERTY_MODE_DEFAULT) )
    {
      result = this->mpCameraChannel->mVideoCapture.set(propertyId, value);
    }
  }

//  // check if set
//  if (result)
//  {
//    if (this->getPropertyFromCamera(propertyId) == value)
//    {
//      return true;
//    }
//  }
//
//  std::string prop_name = cedar::dev::sensors::camera::Property::type().get(propertyId).prettyString();
//  cedar::aux::LogSingleton::getInstance()->warning
//                                           (
//                                             "property " + prop_name
//                                             + " couldn't set to " + boost::lexical_cast<std::string>(value),
//                                             "cedar::dev::sensors::camera::Device::setPropertyToCamera()"
//                                           );
  return result;
}

double cedar::dev::sensors::camera::Device::getPropertyFromCamera(unsigned int propertyId)
{
  double result = CAMERA_PROPERTY_NOT_SUPPORTED;

  if (this->mpCameraChannel->mVideoCapture.isOpened())
  {
    if (this->mpCameraChannel->mpProperties->isSupported(propertyId))
    {
      result = this->mpCameraChannel->mVideoCapture.get(propertyId);
    }
    else
    {
      result = CAMERA_PROPERTY_NOT_SUPPORTED;
    }
  }
  return result;
}
