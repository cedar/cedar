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

    File:        CameraBackendDc1394.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2012 07 04

    Description:  Implementation for the cedar::dev::sensors::camera::BackendDc1394 class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_LIB_DC1394

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/backends/BackendDc1394.h"
#include "cedar/devices/sensors/camera/backends/LibDcBase.h"
#include "cedar/devices/sensors/camera/exceptions.h"

// SYSTEM INCLUDES
#include <iostream>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::BackendDc1394::BackendDc1394
(
  cedar::dev::sensors::camera::Channel* pCameraChannel
)
:
cedar::dev::sensors::camera::Backend(pCameraChannel),
mpLibDcInterface(new cedar::dev::sensors::camera::LibDcBase)
{
}


cedar::dev::sensors::camera::BackendDc1394::~BackendDc1394()
{
}


void cedar::dev::sensors::camera::BackendDc1394::init()
{
  // open libdc
  try
  {
    this->openLibDcCamera();
  }
  catch(cedar::dev::sensors::camera::LibDcCameraNotFoundException& e)
  {
    CEDAR_THROW(cedar::dev::sensors::camera::CreateBackendException,e.getMessage());
  }
  catch(cedar::dev::sensors::camera::LibDcException& e)
  {
    CEDAR_THROW(cedar::dev::sensors::camera::CreateBackendException,e.getMessage());
  }

  //read features
  this->readFeaturesFromLibDc();
  this->readGrabModesFromLibDc();
  this->readFrameRatesFromLibDc(mpCameraChannel->_mpGrabMode->getValue());
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::camera::BackendDc1394::updateFps()
{
  this->readFrameRatesFromLibDc(mpCameraChannel->_mpGrabMode->getValue());
}


void cedar::dev::sensors::camera::BackendDc1394::createCaptureObject()
{
#ifdef DEBUG_CAMERA_GRABBER
  std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif

  // the BusID or the GUID
  unsigned int cam_id = mpCameraChannel->getCameraId();
  unsigned int bus_id = 0;

  // if GUID, search for bus-ID
  if (mpCameraChannel->getByGuid())
  {
    try
    {
      bus_id = this->getBusIdFromGuid(cam_id);
    }
    catch(cedar::dev::sensors::camera::LibDcCameraNotFoundException& e)
    {
      CEDAR_THROW(cedar::dev::sensors::camera::CreateBackendException,e.getMessage());
    }
  }
  else
  {
    bus_id = cam_id;
  }

  if (mpLibDcInterface)
  {
    // cleanup libdc
    mpLibDcInterface->closeCamera();
    mpLibDcInterface->cleanUp();
    mpLibDcInterface.reset();
  }

  //open camera with OpenCv VideoCapture class
  cv::VideoCapture capture(bus_id);

  // throw an exception if not successful
  if(!capture.isOpened())
  {
    std::string msg = "Error: Couldn't create capture object with camera from Bus-ID "
                        + cedar::aux::toString(bus_id);
    CEDAR_THROW(cedar::dev::sensors::camera::CreateBackendException,msg);
  }

  mpCameraChannel->mVideoCapture = capture;
}


void cedar::dev::sensors::camera::BackendDc1394::readGrabModesFromLibDc()
{
  //only invoked at the creation of the backend

#ifdef DEBUG_CAMERA_GRABBER
  std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif

  //save old state and restore at the end of this method
  cedar::dev::sensors::camera::VideoMode::Id used_mode_id = mpCameraChannel->_mpGrabMode->getValue();

  dc1394video_modes_t cam_video_modes = mpLibDcInterface->getCamVideoModes();

  // disable event-handling from the grab-mode parameter
  mpCameraChannel->_mpGrabMode->blockSignals(true);

  // enable only available grab-modes
  mpCameraChannel->_mpGrabMode->disableAll();
  mpCameraChannel->_mpGrabMode->enable(cedar::dev::sensors::camera::VideoMode::MODE_NOT_SET);

  int num_modes = cam_video_modes.num;
  //  std::cout << "Camera Video Modes:" << std::endl;
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

  //restore previous used mode
  if (mpCameraChannel->_mpGrabMode->isEnabled(used_mode_id))
  {
#ifdef DEBUG_CAMERA_GRABBER
  std::cout << "\tRestore previous mode. ID: "<< used_mode_id << std::endl;
#endif
    mpCameraChannel->_mpGrabMode->enable(used_mode_id);
  }
  else
  {
    mpCameraChannel->_mpGrabMode->setValue(cedar::dev::sensors::camera::VideoMode::MODE_NOT_SET);
#ifdef DEBUG_CAMERA_GRABBER
  std::cout << "\tSet frame mode to auto." << std::endl;
#endif
  }
  mpCameraChannel->_mpGrabMode->blockSignals(false);
}


void cedar::dev::sensors::camera::BackendDc1394::readFrameRatesFromLibDc
(
  cedar::dev::sensors::camera::VideoMode::Id modeId
)
{
#ifdef DEBUG_CAMERA_GRABBER
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "\tVideo mode: " << modeId << std::endl;
#endif

  // save old state for restoring at the end
  cedar::dev::sensors::camera::FrameRate::Id used_fps = mpCameraChannel->_mpFPS->getValue();

  if (modeId == cedar::dev::sensors::camera::VideoMode::MODE_NOT_SET)
  {
    mpCameraChannel->_mpFPS->enableAll();
    return;
  }

  // LibDc constants for the grab-modes are identical to VideoMode::Id
  int mode_id = static_cast<int>(modeId);
  dc1394video_modes_t cam_video_modes = mpLibDcInterface->getCamVideoModes();

//#ifdef DEBUG_CAMERA_GRABBER
//  std::string video_mode_str = mpLibDcInterface->DC1394VideoModeToString(cam_video_modes.modes[mode_id]);
//  std::cout << "\tVideo mode (modeId " << mode_id << "): "
//            << cam_video_modes.modes[mode_id]
//            <<" : "
//            << video_mode_str
//            << std::endl;
//#endif

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

    //mode available?
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

  //always enable mode AUTO
  mpCameraChannel->_mpFPS->enable(cedar::dev::sensors::camera::FrameRate::FPS_NOT_SET);

  //restore previous used fps settings (if available)
  if (mpCameraChannel->_mpFPS->isEnabled(used_fps))
  {
    mpCameraChannel->_mpFPS->setValue(used_fps);
  }
  else
  {
    mpCameraChannel->_mpFPS->setValue(cedar::dev::sensors::camera::FrameRate::FPS_NOT_SET);
  }
}

void cedar::dev::sensors::camera::BackendDc1394::openLibDcCamera()
{
  unsigned int camera_id = mpCameraChannel->getCameraId();
  unsigned int bus_id = 0;
  uint64_t cam_guid = 0;

  unsigned int cams_on_bus = mpLibDcInterface->getNumCams();

  if (cams_on_bus < 1)
  {
    std::string msg = "No cameras on the firewire bus!";
    CEDAR_THROW(cedar::dev::sensors::camera::LibDcCameraNotFoundException,msg);
  }

  // get uid and guid from settings and camera
  if (this->mpCameraChannel->getByGuid())
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
                                            (
                                              "Searching for camera with GUID "
                                               + cedar::aux::toString(camera_id) + " on the bus...",
                                              "cedar::dev::sensors::camera::BackendDc1394::openLibDcCamera()"
                                            );

    //throws an exception if not found
    bus_id = getBusIdFromGuid(camera_id);

    cedar::aux::LogSingleton::getInstance()->debugMessage
                                            (
                                              "Found cam at Bus-Id " + cedar::aux::toString(bus_id),
                                              "cedar::dev::sensors::camera::BackendDc1394::openLibDcCamera()"
                                            );

  }
  else
  {
    //by bus-id from settings and guid from camera
    bus_id = camera_id;
  }

  // guid is needed to open the camera wit libdc
  cam_guid = mpLibDcInterface->getCamGuid(bus_id);

  unsigned int guid = static_cast<unsigned int>(cam_guid & 0x00000000FFFFFFFF);
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                          (
                                            "Open camera with bus Id " + cedar::aux::toString(bus_id)
                                              + ": GUID " + cedar::aux::toString(guid),
                                            "cedar::dev::sensors::camera::BackendDc1394::openLibDcCamera()"
                                          );
  // open camera with libdc to get the capabilities
  if (!mpLibDcInterface->openCamera(cam_guid))
  {
    std::string msg = "Could not open camera with BusID " + cedar::aux::toString(bus_id)
                         + " and GUID " + cedar::aux::toString(guid);
    CEDAR_THROW(cedar::dev::sensors::camera::LibDcException,msg)
  }
}

unsigned int cedar::dev::sensors::camera::BackendDc1394::getBusIdFromGuid(unsigned int guid)
{
  unsigned int cams_on_bus = mpLibDcInterface->getNumCams();

  if (cams_on_bus < 1)
  {
    std::string msg = "No cameras on the firewire bus";
    CEDAR_THROW(cedar::dev::sensors::camera::LibDcCameraNotFoundException,msg);
  }

  // search all cameras for the one with the given guid
  for (unsigned int bus_id = 0; bus_id < cams_on_bus; ++bus_id)
  {
    //only lower 32bit of guid
    uint64_t cam_guid64 = mpLibDcInterface->getCamGuid(bus_id);
    unsigned int cam_guid = static_cast<unsigned int>(cam_guid64 & 0x00000000FFFFFFFF);

    // if found, return bus-id
    if (guid == cam_guid)
    {
      return bus_id;
    }
  }

  // not found, throw an exception
  std::string msg = "Camera with GUID " + cedar::aux::toString(guid) + " not found on the firewire bus";
  CEDAR_THROW(cedar::dev::sensors::camera::LibDcCameraNotFoundException,msg);
}

void cedar::dev::sensors::camera::BackendDc1394::readFeaturesFromLibDc()
{
  // get all features from cam
  dc1394featureset_t cam_features = mpLibDcInterface->getCamFeatures();

  //synchronize every feature (i.e. property) with the representing object
  int num_properties = cedar::dev::sensors::camera::Property::type().list().size();
  for (int prop = 0; prop < num_properties; ++prop)
  {
    cedar::dev::sensors::camera::Property::Id prop_id
      = cedar::dev::sensors::camera::Property::type().list().at(prop).id();

    //the object which handles the actual property
    cedar::dev::sensors::camera::CamPropertyPtr p_prop = mpCameraChannel->mpProperties->getPropertyObject(prop_id);
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

        p_prop->mAutoCapable = false;
        p_prop->mManualCapable = false;

        for (unsigned int i=0; i<num_modes;++i)
        {
          if (feature.modes.modes[i] == DC1394_FEATURE_MODE_AUTO)
          {
            p_prop->mAutoCapable = true;
          }
          else if (feature.modes.modes[i] == DC1394_FEATURE_MODE_MANUAL)
          {
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

        //set maximum and minimum values
        p_prop->mpPropertyValue->setMaximum(static_cast<double>(feature.max));
        p_prop->mpPropertyValue->setMinimum(static_cast<double>(feature.min));

        //set value in gui
        p_prop->mDefaultValue = static_cast<double>(feature.value);
        p_prop->mpPropertyValue->setValue(static_cast<double>(feature.value));
      }
      else
      {
        // disable property
        p_prop->mSupported=false;
      }
    }
    p_prop->update();
    p_prop->doNotHandleEvents = false;
  }
}


void cedar::dev::sensors::camera::BackendDc1394::applySettingsToCamera()
{
#ifdef DEBUG_CAMERA_GRABBER
  std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif

  //frame mode
  cedar::dev::sensors::camera::VideoMode::Id video_mode_id = mpCameraChannel->getVideoMode();
  if (video_mode_id != cedar::dev::sensors::camera::VideoMode::MODE_NOT_SET)
  {
#ifdef DEBUG_CAMERA_GRABBER
    std::cout << "Set VideoMode" << std::endl;
#endif
    double value = static_cast<double>(video_mode_id);
    this->setPropertyToCamera(cedar::dev::sensors::camera::Setting::MODE,value);
  }

  //framerate
  cedar::dev::sensors::camera::FrameRate::Id framerate_id = mpCameraChannel->getFramerate();
  if (framerate_id != cedar::dev::sensors::camera::FrameRate::FPS_NOT_SET)
  {
#ifdef DEBUG_CAMERA_GRABBER
    std::cout << "Set FrameRate" << std::endl;
#endif
    double value = static_cast<double>(framerate_id);
    this->setPropertyToCamera(cedar::dev::sensors::camera::Setting::FPS,value);
  }

  //iso-speed
  cedar::dev::sensors::camera::IsoSpeed::Id iso_speed_id = mpCameraChannel->getIsoSpeed();
  if (iso_speed_id != cedar::dev::sensors::camera::IsoSpeed::ISO_NOT_SET)
  {
#ifdef DEBUG_CAMERA_GRABBER
    std::cout << "Set IsoSpeed" << std::endl;
#endif
    double value = static_cast<double>(iso_speed_id);
    this->setPropertyToCamera(cedar::dev::sensors::camera::Setting::ISO_SPEED,value);
  }

}
#endif // CEDAR_USE_LIB_DC1394
