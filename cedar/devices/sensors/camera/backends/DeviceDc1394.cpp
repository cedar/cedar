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

    File:        CameraDeviceDc1394.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Implementation for the cedar::dev::sensors::camera::DeviceDc1394 class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_LIB_DC1394

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/backends/DeviceDc1394.h"
#include "cedar/devices/sensors/camera/backends/LibDcBase.h"
#include "cedar/devices/sensors/camera/exceptions.h"

// SYSTEM INCLUDES
//#include <boost/lexical_cast.hpp>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::DeviceDc1394::DeviceDc1394
(
  cedar::dev::sensors::camera::Channel* pCameraChannel
)
:
cedar::dev::sensors::camera::Device::Device(pCameraChannel),
mpLibDcInterface(new cedar::dev::sensors::camera::LibDcBase)
{
}


cedar::dev::sensors::camera::DeviceDc1394::~DeviceDc1394()
{
}


void cedar::dev::sensors::camera::DeviceDc1394::initDevice()
{
  if (!openLibDcCamera())
  {
    //CEDAR_THROW(cedar::dev::sensors::camera::LibDcInitException,"cedar::dev::sensors::camera::DeviceDc1394::initDevice()");
    return;
  }

//  std::cout << "DC1394: initDevice. Camera successfully opened through libdc" << std::endl;
  //set available properties and their values

  getFeaturesFromLibDc();
  getGrabModesFromLibDc();

//  std::cout << "Set Grabmode to "
//    << cedar::dev::sensors::camera::VideoMode::type().get(mpCameraChannel->_mpGrabMode->getValue()).prettyString()
//    << std::endl;
  getFrameRatesFromLibDc(mpCameraChannel->_mpGrabMode->getValue()); //id?????


  //set first available mode
//  unsigned int set_video_mode_id = 0;
//  cedar::dev::sensors::camera::VideoMode::Id set_video_mode;
//  set_video_mode = static_cast<cedar::dev::sensors::camera::VideoMode::Id>(cam_video_modes.modes[set_video_mode_id]);
//  mpCameraChannel->_mpGrabMode->setValue(set_video_mode);
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
//step 1
bool cedar::dev::sensors::camera::DeviceDc1394::createCaptureDevice()
{
  if (mpLibDcInterface)
  {
    // cleanup libdc
    mpLibDcInterface->closeCamera();
    mpLibDcInterface->cleanUp();
    mpLibDcInterface.reset();
  }

  //open camera with openCV backend
  cv::VideoCapture capture(mpCameraChannel->getCameraId());
  if(capture.isOpened())
  {
    mpCameraChannel->mVideoCapture = capture;
    return true;
  }

  return false;
}


void cedar::dev::sensors::camera::DeviceDc1394::getGrabModesFromLibDc()
{
  dc1394video_modes_t cam_video_modes = mpLibDcInterface->getCamVideoModes();
  int num_modes = cam_video_modes.num;
//  std::cout << "Camera Video Modes:" << std::endl;
  mpCameraChannel->_mpGrabMode->disableAll();
  for (int i=0; i<num_modes; i++)
  {
//    std::cout << " - " << cam_video_modes.modes[i]
//                       <<" : "
//                       << mpLibDcInterface->DC1394VideoModeToString(cam_video_modes.modes[i])
//                       << " VideoMode::prettyString: "
//                       << cedar::dev::sensors::camera::VideoMode::type().get(cam_video_modes.modes[i]).prettyString()
//                       << std::endl;


   // LibDc constants for the grabmodes are identical to VideoMode::Id
   cedar::dev::sensors::camera::VideoMode::Id mode_id
     = static_cast<cedar::dev::sensors::camera::VideoMode::Id>(cam_video_modes.modes[i]);
   if (mode_id <= cedar::dev::sensors::camera::VideoMode::FW_EXIF)
   {
     mpCameraChannel->_mpGrabMode->enable(mode_id);
//     std::cout << "enable " << cedar::dev::sensors::camera::VideoMode::type().get(cam_video_modes.modes[i]).prettyString() << std::endl;
   }
  }

  //std::cout << "---modes finished---" << std::endl;

}


void cedar::dev::sensors::camera::DeviceDc1394::getFrameRatesFromLibDc
(
  cedar::dev::sensors::camera::VideoMode::Id modeId
)
{
  // LibDc constants for the grab-modes are identical to VideoMode::Id
  int mode_id = static_cast<int>(modeId);
  dc1394video_modes_t cam_video_modes = mpLibDcInterface->getCamVideoModes();

//  std::string video_mode_str = mpLibDcInterface->DC1394VideoModeToString(cam_video_modes.modes[set_video_mode_id]);
//  std::cout << "\n - Video mode "
//            << cam_video_modes.modes[set_video_mode_id]
//            <<" : "
//            << video_mode_str
//            << std::endl;

  int libdc_mode_index = -1;
  for (unsigned int i = 0; i < cam_video_modes.num; ++i)
  {
    if (cam_video_modes.modes[i] == mode_id)
    {
      libdc_mode_index = i;
      break;
    }
  }

  CEDAR_ASSERT(libdc_mode_index != -1);
  dc1394framerates_t mode_framerates = mpLibDcInterface->getCamFramerates(cam_video_modes.modes[libdc_mode_index]);

  // LibDc framerate id's are not identical to FrameRate::Id
  // search every FrameRate::Id in the LibDc structures, and set if available

  int num_cedar_framerates = cedar::dev::sensors::camera::FrameRate::type().list().size();
  for (int rate = 0; rate < num_cedar_framerates; ++rate)
  {
    cedar::dev::sensors::camera::FrameRate::Id rate_id
      = cedar::dev::sensors::camera::FrameRate::type().list().at(rate).id();

    int libdc_framerate_index = -1;
    int num_libdc_framerates = mode_framerates.num;
    for (int i=0; i<num_libdc_framerates; i++)
    {

      cedar::dev::sensors::camera::FrameRate::Id test_framerate_id = 10000;
      switch (mode_framerates.framerates[i])
      {
        using cedar::dev::sensors::camera::FrameRate;
        case DC1394_FRAMERATE_1_875: test_framerate_id = FrameRate::FPS_2 ; break;
        case DC1394_FRAMERATE_3_75: test_framerate_id = FrameRate::FPS_4; break;
        case DC1394_FRAMERATE_7_5: test_framerate_id = FrameRate::FPS_8; break;
        case DC1394_FRAMERATE_15: test_framerate_id = FrameRate::FPS_15; break;
        case DC1394_FRAMERATE_30: test_framerate_id = FrameRate::FPS_30; break;
        case DC1394_FRAMERATE_60: test_framerate_id = FrameRate::FPS_60; break;
        case DC1394_FRAMERATE_120: test_framerate_id = FrameRate::FPS_120; break;
        case DC1394_FRAMERATE_240: test_framerate_id = FrameRate::FPS_240; break;
      }

      //test if the right framerate found
      if (rate_id == test_framerate_id)
      {
        libdc_framerate_index = i;
        break;
      }
    }

    if (libdc_framerate_index == -1)
    {
      mpCameraChannel->_mpFPS->disable(rate_id);
    }
    else
    {
//      std::string dc_name = mpLibDcInterface->DC1394FrameRateToString(mode_framerates.framerates[libdc_framerate_index]);
//      std::string frame_name = cedar::dev::sensors::camera::FrameRate::type().get(rate_id).prettyString();
//
//      std::cout << "DC1394 ID " << (mode_framerates.framerates[libdc_framerate_index])
//                << " name " << dc_name
//                << " FrameRate::Id " << rate_id << "; name "<< frame_name << std::endl;
       mpCameraChannel->_mpFPS->enable(rate_id);
    }


  }
}

bool cedar::dev::sensors::camera::DeviceDc1394::openLibDcCamera()
{
  unsigned int camera_id = mpCameraChannel->getCameraId();
  unsigned int bus_id = 0;
  uint64_t cam_guid = 0;

  // get uid and guid from settings and camera
  if (this->mpCameraChannel->getByGuid())
  {

    //search all cams, i.e. all bus-ids
    cedar::aux::LogSingleton::getInstance()->debugMessage
                                            (
                                              "Searching for camera with GUID "
                                               + boost::lexical_cast<std::string>(camera_id) + " on the bus...",
                                              "cedar::dev::sensors::camera::Grabber::onInit()"
                                            );

    unsigned int cams_on_bus = mpLibDcInterface->getNumCams();

    if (cams_on_bus < 1)
    {
      cedar::aux::LogSingleton::getInstance()->error
                                               (
                                                 "No cameras on the firewire bus!",
                                                 "cedar::dev::sensors::camera::Grabber::onInit()"
                                               );
      return false;
    }

    // search for the right guid, so open it one by one
    bool cam_found = false;

    for (unsigned int cam=0; cam < cams_on_bus; ++cam)
    {
      //only lower 32bit of guid
      cam_guid = mpLibDcInterface->getCamGuid(cam);
      unsigned int guid = (unsigned int)(cam_guid&0x00000000FFFFFFFF);

      cedar::aux::LogSingleton::getInstance()->debugMessage
                                              (
                                                "Found: at Bus-Id " + boost::lexical_cast<std::string>(cam)
                                                  + ": GUID " + boost::lexical_cast<std::string>(guid),
                                                "cedar::dev::sensors::camera::Grabber::onInit()"
                                              );
      cam_found = (guid == camera_id);
      if (cam_found)
      {
        bus_id = cam;
        break;
      }
    }

    //test if found
    if (!cam_found)
    {
      cedar::aux::LogSingleton::getInstance()->error
                                               (
                                                 "No cameras with GUID "+boost::lexical_cast<std::string>(camera_id)
                                                 +" not found on the firewire bus!",
                                                 "cedar::dev::sensors::camera::Grabber::onInit()"
                                               );
      return false;
    }

    //found, the right camera is the camera with the index before
    //std::cout << "Camera found at Bus-ID: " << bus_id << std::endl;

  } //byGuid
  else
  {
    //by bus-id from settings and guid from camera
    bus_id = camera_id;
    cam_guid = mpLibDcInterface->getCamGuid(camera_id);

    unsigned int guid = (unsigned int)(cam_guid&0x00000000FFFFFFFF);
    cedar::aux::LogSingleton::getInstance()->debugMessage
                                            (
                                              "Open camera with bus Id " + boost::lexical_cast<std::string>(bus_id)
                                                + ": GUID " + boost::lexical_cast<std::string>(guid),
                                              "cedar::dev::sensors::camera::Grabber::onInit()"
                                            );
  }


  // open camera with libdc to get the capabilities
  return mpLibDcInterface->openCamera(cam_guid);
}

void cedar::dev::sensors::camera::DeviceDc1394::getFeaturesFromLibDc()
{
  // ***************************************
  // get all features from cam
  // ***************************************
  dc1394featureset_t cam_features = mpLibDcInterface->getCamFeatures();

  //get all settings for all properties from camera
  int num_properties = cedar::dev::sensors::camera::Property::type().list().size();
  for (int prop = 0; prop < num_properties; ++prop)
  {
    cedar::dev::sensors::camera::Property::Id prop_id
      = cedar::dev::sensors::camera::Property::type().list().at(prop).id();

    //the class which handles the actual property
    cedar::dev::sensors::camera::CamPropertyPtr p_prop = mpCameraChannel->mpProperties->getPropertyClass(prop_id);
    p_prop->doNotHandleEvents = true;

    //search for current property in the feature-list
    int libdc_feature_index = -1;
    for (int i = 0 ; i < DC1394_FEATURE_NUM; i++)
    {
      cedar::dev::sensors::camera::Property::Id test_id = CAMERA_PROPERTY_NOT_SUPPORTED;

      // check if cedar/opencv/firewire support this feature
      switch (cam_features.feature[i].id)
      {
        using cedar::dev::sensors::camera::Property;
        case DC1394_FEATURE_BRIGHTNESS: test_id = Property::PROP_BRIGHTNESS; break;
        case DC1394_FEATURE_EXPOSURE: test_id = Property::PROP_EXPOSURE; break;
        case DC1394_FEATURE_SHARPNESS: test_id = Property::PROP_SHARPNESS; break;
        case DC1394_FEATURE_WHITE_BALANCE: test_id = Property::PROP_WHITE_BALANCE_BLUE_U; break;
        case DC1394_FEATURE_HUE: test_id = Property::PROP_HUE; break;
        case DC1394_FEATURE_SATURATION: test_id = Property::PROP_SATURATION; break;
        case DC1394_FEATURE_GAMMA: test_id = Property::PROP_GAMMA; break;
        case DC1394_FEATURE_GAIN: test_id = Property::PROP_GAIN; break;
        case DC1394_FEATURE_FOCUS: test_id = Property::PROP_FOCUS; break;
        case DC1394_FEATURE_ZOOM: test_id = Property::PROP_ZOOM; break;
        case DC1394_FEATURE_SHUTTER: test_id = Property::PROP_AUTO_EXPOSURE; break;
        case DC1394_FEATURE_TEMPERATURE: test_id = Property::PROP_TEMPERATURE; break;
        default: ;
      }

      //test if the right property found
      if (prop_id == test_id)
      {
        libdc_feature_index = i;
        break;
      }
    }

    if (libdc_feature_index == -1)
    {
      //property not supported from backend, so disable it
      p_prop->mSupported=false;
    }
    else
    {
      //actual property found in supported list of properties from camera

//      std::string feature_name
//        = mpLibDcInterface->DC1394FeatureToCameraGrabberPropertyString(cam_features.feature[libdc_feature_index].id);
//      std::string prop_name = cedar::dev::sensors::camera::Property::type().get(prop_id).prettyString();
//
//      std::cout << "DC1394_ID " << (cam_features.feature[libdc_feature_index].id) << " feature-name " << feature_name
//                << " Property::Id " << prop_id << "; prop-name "<< prop_name << std::endl;

      dc1394feature_info_t& feature = cam_features.feature[libdc_feature_index];

      // test if available and readable in the libdc backend
      if (feature.available && feature.readout_capable)
      {
        // enable property CamProperty class
        p_prop->mSupported=true;

        // activate available modes:
        unsigned int num_modes = feature.modes.num;
//        std::cout << "Available modes : " << num_modes << std::endl;

        p_prop->mAutoCapable = false;
        p_prop->mManualCapable = false;

        for (unsigned int i=0; i<num_modes;++i)
        {
          if (feature.modes.modes[i] == DC1394_FEATURE_MODE_AUTO)
          {
//            std::cout << "Mode " << i << ": Auto" << std::endl;
            p_prop->mAutoCapable = true;
          }
          else if (feature.modes.modes[i] == DC1394_FEATURE_MODE_MANUAL)
          {
//            std::cout << "Mode " << i << ": Manual" << std::endl;
            p_prop->mManualCapable = true;
          }
         //onePushAuto not supported
        }
        // check mode and set appropriate in the gui
        // possibilities: default, automatic, manual
        if (feature.current_mode == DC1394_FEATURE_MODE_AUTO )
        {
          //set gui to automatic
          p_prop->mpPropertyMode->setValue(cedar::dev::sensors::camera::PropertyMode::AUTO);
        }
        else
        {
          //set gui to default
          p_prop->mpPropertyMode->setValue(cedar::dev::sensors::camera::PropertyMode::BACKEND_DEFAULT);
        }

//        std::cout << "set min " << feature.min << " and max " << feature.max << " values" << std::endl;
        //set maximum and minimum values
        p_prop->mpPropertyValue->setMaximum(static_cast<double>(feature.max));
        p_prop->mpPropertyValue->setMinimum(static_cast<double>(feature.min));


        //set value in gui
//        std::cout << "value: " << feature.value << std::endl;
        p_prop->mDefaultValue = static_cast<double>(feature.value);
        p_prop->mpPropertyValue->setValue(static_cast<double>(feature.value));


      }
      else
      {
        // disable property
        p_prop->mSupported=false;
      }
    }
    p_prop->updateGuiElements();
    p_prop->doNotHandleEvents = false;
  }

}


void cedar::dev::sensors::camera::DeviceDc1394::setProperties()
{
//  int num_properties = cedar::dev::sensors::camera::Property::type().list().size();
//  for (int i=0; i<num_properties; i++)
//  {
//    cedar::dev::sensors::camera::Property::Id prop_id
//      = cedar::dev::sensors::camera::Property::type().list().at(i).id();
//
//
//    // ignore unsupported features
//    bool is_supported = true;
//    bool is_readable = true;
//
//    if (    (prop_id == cedar::dev::sensors::camera::Property::PROP_TRIGGER)
//         || (prop_id == cedar::dev::sensors::camera::Property::PROP_TRIGGER_DELAY)
//       )
//    {
//      is_supported = false;
//      is_readable = false;
//    }
//
//    std::string prop_name = cedar::dev::sensors::camera::Property::type().list().at(i).prettyString();
//
//    //@todo: get min/max values from device !!!
//
//    mpCameraChannel->mpProperties->getPropertyClass(prop_name);

    //adjust values with the right ones from the cam






  //adjust values of the settings available

  /*
  cedar::dev::sensors::camera::SettingsSet& settings = mpSettings->getSettings();
  settings.get().clear();


  //create structure with settings and their values
  int num_settings = cedar::dev::sensors::camera::Setting::type().list().size();
  for (int i=0; i<num_settings; i++)
  {
    cedar::dev::sensors::camera::Setting::Id setting_id
      = cedar::dev::sensors::camera::Setting::type().list().at(i).id();

    std::string setting_name = cedar::dev::sensors::camera::Setting::type().list().at(i).prettyString();

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





//  }


}


// step 3
void cedar::dev::sensors::camera::DeviceDc1394::applySettingsToCamera()
{

}

// step 4
void cedar::dev::sensors::camera::DeviceDc1394::applyStateToCamera()
{

}

#endif // CEDAR_USE_LIB_DC1394
