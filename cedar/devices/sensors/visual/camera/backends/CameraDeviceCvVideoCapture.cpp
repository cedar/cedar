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

    File:        CameraDeviceCvVideoCapture.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Implementation for the cedar::dev::sensors::visual::CameraDeviceCvVideoCapture class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/camera/backends/CameraDeviceCvVideoCapture.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::CameraDeviceCvVideoCapture
(
  cedar::dev::sensors::visual::CameraChannelPtr pCameraChannel
)
:
cedar::dev::sensors::visual::CameraDevice::CameraDevice(pCameraChannel)
{
}


cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::~CameraDeviceCvVideoCapture()
{
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//void cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::createPropertyAndSetting()
//{
//}


//first step
void cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::fillCapabilities()
{
//  cedar::dev::sensors::visual::CameraPropertiesSet& properties = mpCameraChannel->mpProperties->getProperties();
//  properties.get().clear();
//
//  //create structure with supported properties and their values
//  int num_properties = cedar::dev::sensors::visual::CameraProperty::type().list().size();
//  for (int i=0; i<num_properties; i++)
//  {
//    cedar::dev::sensors::visual::CameraProperty::Id prop_id
//      = cedar::dev::sensors::visual::CameraProperty::type().list().at(i).id();
//
//    std::string prop_name = cedar::dev::sensors::visual::CameraProperty::type().list().at(i).prettyString();
//
//    //@todo: get min/max values from device !!!
//
//    cedar::dev::sensors::visual::CamPropertyPtr p_prop(new cedar::dev::sensors::visual::CamProperty
//                                                         (
//                                                           prop_id,
//                                                           prop_name,
//                                                           0.f,
//                                                           1024.f,
//                                                           128.f,
//                                                           true,
//                                                           true,
//                                                           false,
//                                                           true
//                                                         )
//                                                      );
//    properties.insert(p_prop);
//  }

  /*
  cedar::dev::sensors::visual::CameraSettingsSet& settings = mpSettings->getSettings();
  settings.get().clear();


  //create structure with settings and their values
  int num_settings = cedar::dev::sensors::visual::CameraSetting::type().list().size();
  for (int i=0; i<num_settings; i++)
  {
    cedar::dev::sensors::visual::CameraSetting::Id setting_id
      = cedar::dev::sensors::visual::CameraSetting::type().list().at(i).id();

    std::string setting_name = cedar::dev::sensors::visual::CameraSetting::type().list().at(i).prettyString();

    //@todo: get min/max values from device !!!

    cedar::dev::sensors::visual::CamSettingPtr p_prop(new cedar::dev::sensors::visual::CamSetting
                                                         (
                                                           setting_id,
                                                           setting_name
                                                         )
                                                      );
    settings.insert(p_prop);
  }
  */

}

//2. step
bool cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::createCaptureDevice()
{
  //

  std::cout << "Create camera with cv::VideoCapture Backend\n"
      << "BusId:" << mpCameraChannel->_mpBusId->getValue() << "\n"
      << "Guid:" << mpCameraChannel->_mpGuid->getValue() << "\n"
      << "ByGuid:" << mpCameraChannel->_mpByGuid->getValue() << "\n"
      << std::endl;

  cv::VideoCapture capture(mpCameraChannel->_mpBusId->getValue());
  if(capture.isOpened())
  {
    mpCameraChannel->mVideoCapture = capture;
    return true;
  }
  return false;
}

//3. step
void cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::applySettingsToCamera()
{

}

//4. step
void cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::applyStateToCamera()
{

}
