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

    File:        LibDcBase.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 12 11

    Description: Implementation of the @em LibDcBase class.


    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_LIB_DC1394

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/backends/LibDcBase.h"
#include "cedar/devices/sensors/camera/exceptions.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <sstream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::LibDcBase::LibDcBase()
:
mpFwContext(NULL),
mpCameraList(NULL),
mpCamera(NULL),
mError(DC1394_SUCCESS)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  mpFwContext = dc1394_new();
  if (!mpFwContext)
  {
    CEDAR_THROW
    (
      cedar::dev::sensors::camera::LibDcInitException,
      "cedar::dev::sensors::camera::LibDcBase::LibDcBase(): Couldn't open the firewire bus"
    )
  }
  
  //--------------------------------------------------------------
  // Find cameras
  mError=dc1394_camera_enumerate(mpFwContext, &mpCameraList);

  // if error, clean up
  if (mError < 0 || !mpCameraList ) //|| (unsigned)mpCameraList->num<1)
  {

    if (mpCameraList)
    {
      dc1394_camera_free_list(mpCameraList);
      mpCameraList = NULL;
    }
    dc1394_free (mpFwContext);
    mpFwContext = NULL;
    std::string err_message = "cedar::dev::sensors::camera::LibDcBase::LibDcBase():" + getErrorString() ;
    CEDAR_THROW(cedar::dev::sensors::camera::LibDcInitException, err_message);
  }
}

cedar::dev::sensors::camera::LibDcBase::~LibDcBase()
{
  cleanUp();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::camera::LibDcBase::cleanUp()
{
  if (mpCameraList)
  {
    dc1394_camera_free_list(mpCameraList);
    mpCameraList = NULL;
  }
  if (mpCamera)
  {
    dc1394_iso_release_all(mpCamera);
    dc1394_camera_free(mpCamera);
    mpCamera = NULL;
  }
  if (mpFwContext)
  {
    dc1394_free (mpFwContext);
    mpFwContext = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::camera::LibDcBase::resetFwBus()
{
  if (!mpCamera)
  {
    return false;
  }

  mError = dc1394_reset_bus(mpCamera);
  return (mError == DC1394_SUCCESS);
}


//----------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::camera::LibDcBase::resetCam()
{
  if (!mpCamera)
  {
    return false;
  }

  mError = dc1394_camera_reset(mpCamera);
  return (mError == DC1394_SUCCESS);
}

//----------------------------------------------------------------------------------------------------------------------
std::string cedar::dev::sensors::camera::LibDcBase::getErrorString() const
{
  return dc1394_error_get_string(mError);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int cedar::dev::sensors::camera::LibDcBase::getNumCams()
{
  if (mpCameraList)
  {
    return static_cast<unsigned>(mpCameraList->num);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
uint64_t cedar::dev::sensors::camera::LibDcBase::getCamGuid(unsigned int camNr)
{
  if(camNr >= getNumCams())
  {
    return 0;
  }
  return mpCameraList->ids[camNr].guid;
}


//----------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::camera::LibDcBase::openCamera(uint64_t camGuid)
{
  if (mpCamera)
  {
    closeCamera();
  }
  mpCamera = dc1394_camera_new(mpFwContext, camGuid);
  return (mpCamera != NULL);
}

//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::LibDcBase::closeCamera()
{
  if (mpCamera)
  {
    dc1394_iso_release_all(mpCamera);
    dc1394_camera_free(mpCamera);
    mpCamera = NULL;
  }
}

  
//----------------------------------------------------------------------------------------------------------------------
std::string cedar::dev::sensors::camera::LibDcBase::getAllFeaturesAsString()
{
  // return an empty vector if no camera opened
  if (! mpCamera)
  {
    return "";
  }

  std::stringstream features;

  dc1394featureset_t *cam_features  = new dc1394featureset_t;

  if (cam_features)
  {
    // get features from cam
    mError = dc1394_feature_get_all(mpCamera, cam_features);

    if (mError != DC1394_SUCCESS)
    {
      delete cam_features;
      cleanUp();
      std::string err_message = "cedar::dev::sensors::camera::LibDcBase::getAllFeaturesAsString():" + getErrorString();
      CEDAR_THROW(cedar::dev::sensors::camera::LibDcInitException, err_message);
    }
    // add them to the feature string
    for (int i = 0 ; i < DC1394_FEATURE_NUM; i++)
    {
      features << dc1394_feature_get_string(cam_features->feature[i].id) << std::endl;
    }
    delete cam_features;
  }
  return features.str();
}


//----------------------------------------------------------------------------------------------------------------------
dc1394featureset_t cedar::dev::sensors::camera::LibDcBase::getCamFeatures()
{
  dc1394featureset_t cam_features;

  if (!mpCamera)
  {
    memset(&cam_features,0,sizeof(cam_features));
  }
  
  mError = dc1394_feature_get_all(mpCamera, &cam_features);

  if (mError != DC1394_SUCCESS)
  {
    cleanUp();
    std::string err_message = "cedar::dev::sensors::camera::LibDcBase::getCamFeatures():" + getErrorString();
    CEDAR_THROW(cedar::dev::sensors::camera::LibDcInitException, err_message);
  }
  
  return cam_features;
}

//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::LibDcBase::printAllFeatures()
{
    if (! mpCamera)
    {
      return;
    }
  
  dc1394featureset_t *cam_features  = new dc1394featureset_t;

  if (cam_features)
  {
    mError = dc1394_feature_get_all(mpCamera, cam_features);
    if (mError != DC1394_SUCCESS)
    {
      return;
    }
    
    // get a filehandle to console output
    FILE *con = NULL;
    if  ( (con = fopen( "/dev/stdout" ,"w")) != NULL)
    {                                                    
      fprintf(con,"\n");
      dc1394_camera_print_info(mpCamera,con);
      fprintf(con,"\nCamera features:\n");
      dc1394_feature_print_all(cam_features, con);
      fclose(con);
    }
    delete cam_features;
  }
}

//----------------------------------------------------------------------------------------------------------------------
dc1394video_modes_t cedar::dev::sensors::camera::LibDcBase::getCamVideoModes()
{
  dc1394video_modes_t cam_video_modes;

  if (!mpCamera)
  {
    memset(&cam_video_modes,0,sizeof(cam_video_modes));
  }

  mError = dc1394_video_get_supported_modes(mpCamera, &cam_video_modes);
  
  if (mError != DC1394_SUCCESS)
  {
    cleanUp();
    std::string err_message = "cedar::dev::sensors::camera::LibDcBase::getCamVideoModes():" + getErrorString();
    CEDAR_THROW(cedar::dev::sensors::camera::LibDcInitException, err_message);
  }
  return cam_video_modes;
}

//----------------------------------------------------------------------------------------------------------------------
dc1394framerates_t cedar::dev::sensors::camera::LibDcBase::getCamFramerates
(
  dc1394video_mode_t mode
)
{
  dc1394framerates_t mode_framerates;

  if (!mpCamera)
  {
    memset(&mode_framerates,0,sizeof(mode_framerates));
  }

  mError = dc1394_video_get_supported_framerates(mpCamera,mode, &mode_framerates);
  if (mError != DC1394_SUCCESS)
  {
    cleanUp();
    std::string err_message = "cedar::dev::sensors::camera::LibDcBase::getCamVideoModes():" + getErrorString();
    CEDAR_THROW(cedar::dev::sensors::camera::LibDcInitException, err_message);
  }
  return mode_framerates;
}  
//----------------------------------------------------------------------------------------------------------------------
std::string cedar::dev::sensors::camera::LibDcBase::DC1394FrameRateToString
(
  dc1394framerate_t framerate
) const
{
  std::string framerates[] =
  {
    "DC1394_FRAMERATE_1_875",
    "DC1394_FRAMERATE_3_75",
    "DC1394_FRAMERATE_7_5",
    "DC1394_FRAMERATE_15",
    "DC1394_FRAMERATE_30",
    "DC1394_FRAMERATE_60",
    "DC1394_FRAMERATE_120",
    "DC1394_FRAMERATE_240"
  };

  return framerates[(int)framerate - DC1394_FRAMERATE_MIN];
}

//----------------------------------------------------------------------------------------------------------------------
std::string cedar::dev::sensors::camera::LibDcBase::DC1394VideoModeToString
(
  dc1394video_mode_t mode
) const
{
  std::string modes[]=
  {
    "DC1394_VIDEO_MODE_160x120_YUV444",
    "DC1394_VIDEO_MODE_320x240_YUV422",
    "DC1394_VIDEO_MODE_640x480_YUV411",
    "DC1394_VIDEO_MODE_640x480_YUV422",
    "DC1394_VIDEO_MODE_640x480_RGB8",
    "DC1394_VIDEO_MODE_640x480_MONO8",
    "DC1394_VIDEO_MODE_640x480_MONO16",
    "DC1394_VIDEO_MODE_800x600_YUV422",
    "DC1394_VIDEO_MODE_800x600_RGB8",
    "DC1394_VIDEO_MODE_800x600_MONO8",
    "DC1394_VIDEO_MODE_1024x768_YUV422",
    "DC1394_VIDEO_MODE_1024x768_RGB8",
    "DC1394_VIDEO_MODE_1024x768_MONO8",
    "DC1394_VIDEO_MODE_800x600_MONO16",
    "DC1394_VIDEO_MODE_1024x768_MONO16",
    "DC1394_VIDEO_MODE_1280x960_YUV422",
    "DC1394_VIDEO_MODE_1280x960_RGB8",
    "DC1394_VIDEO_MODE_1280x960_MONO8",
    "DC1394_VIDEO_MODE_1600x1200_YUV422",
    "DC1394_VIDEO_MODE_1600x1200_RGB8",
    "DC1394_VIDEO_MODE_1600x1200_MONO8",
    "DC1394_VIDEO_MODE_1280x960_MONO16",
    "DC1394_VIDEO_MODE_1600x1200_MONO16",
    "DC1394_VIDEO_MODE_EXIF",
    "DC1394_VIDEO_MODE_FORMAT7_0",
    "DC1394_VIDEO_MODE_FORMAT7_1",
    "DC1394_VIDEO_MODE_FORMAT7_2",
    "DC1394_VIDEO_MODE_FORMAT7_3",
    "DC1394_VIDEO_MODE_FORMAT7_4",
    "DC1394_VIDEO_MODE_FORMAT7_5",
    "DC1394_VIDEO_MODE_FORMAT7_6",
    "DC1394_VIDEO_MODE_FORMAT7_7"
  };
  return modes[(int)mode - DC1394_VIDEO_MODE_MIN];
}


//----------------------------------------------------------------------------------------------------------------------
std::string cedar::dev::sensors::camera::LibDcBase::DC1394FeatureToString
(
  dc1394feature_t feature
) const
{
  std::string features[]=
  {
    "DC1394_FEATURE_BRIGHTNESS",
    "DC1394_FEATURE_EXPOSURE",
    "DC1394_FEATURE_SHARPNESS",
    "DC1394_FEATURE_WHITE_BALANCE",
    "DC1394_FEATURE_HUE",
    "DC1394_FEATURE_SATURATION",
    "DC1394_FEATURE_GAMMA",
    "DC1394_FEATURE_SHUTTER",
    "DC1394_FEATURE_GAIN",
    "DC1394_FEATURE_IRIS",
    "DC1394_FEATURE_FOCUS",
    "DC1394_FEATURE_TEMPERATURE",
    "DC1394_FEATURE_TRIGGER",
    "DC1394_FEATURE_TRIGGER_DELAY",
    "DC1394_FEATURE_WHITE_SHADING",
    "DC1394_FEATURE_FRAME_RATE",
    "DC1394_FEATURE_ZOOM",
    "DC1394_FEATURE_PAN",
    "DC1394_FEATURE_TILT",
    "DC1394_FEATURE_OPTICAL_FILTER",
    "DC1394_FEATURE_CAPTURE_SIZE",
    "DC1394_FEATURE_CAPTURE_QUALITY"
  };
  return features[(int)feature - DC1394_FEATURE_MIN];
}

//----------------------------------------------------------------------------------------------------------------------
std::string cedar::dev::sensors::camera::LibDcBase::DC1394FeatureToCameraGrabberPropertyString
(
  dc1394feature_t feature
) const
{
  // CameraGrabber CameraProperty::Id have the same values like their OpenCV counterparts and not dc1394 values
  // so we have to translate to DC1394 features
   
  std::string features[]=
  {
    "PROP_BRIGHTNESS",                    //"DC1394_FEATURE_BRIGHTNESS"
    "PROP_AUTO_EXPOSURE",                 //"DC1394_FEATURE_EXPOSURE"
    "PROP_SHARPNESS",                     //"DC1394_FEATURE_SHARPNESS"
    "PROP_WHITE_BALANCE_BLUE_U",          //"DC1394_FEATURE_WHITE_BALANCE"
    "PROP_HUE",                           //"DC1394_FEATURE_HUE"
    "PROP_SATURATION",                    //"DC1394_FEATURE_SATURATION"
    "PROP_GAMMA",                         //"DC1394_FEATURE_GAMMA"
    "PROP_EXPOSURE",                      //"DC1394_FEATURE_SHUTTER"
    "PROP_GAIN",                          //"DC1394_FEATURE_GAIN"
    "",                                   //"DC1394_FEATURE_IRIS"
    "PROP_FOCUS",                         //"DC1394_FEATURE_FOCUS"
    "PROP_TEMPERATURE",                   //"DC1394_FEATURE_TEMPERATURE"
    "PROP_TRIGGER",                       //"DC1394_FEATURE_TRIGGER"
    "PROP_TRIGGER_DELAY",                 //"DC1394_FEATURE_TRIGGER_DELAY"
    "",                                   //"DC1394_FEATURE_WHITE_SHADING"
    "",                                   //"DC1394_FEATURE_FRAME_RATE"
    "PROP_ZOOM",                          //"DC1394_FEATURE_ZOOM"
    "",                                   //"DC1394_FEATURE_PAN"
    "",                                   //"DC1394_FEATURE_TILT"
    "",                                   //"DC1394_FEATURE_OPTICAL_FILTER"
    "",                                   //"DC1394_FEATURE_CAPTURE_SIZE"
    ""                                    //"DC1394_FEATURE_CAPTURE_QUALITY"
  };
  
  return features[(int)feature - DC1394_FEATURE_MIN];
}
  
#endif// CEDAR_USE_LIB_DC1394
//----------------------------------------------------------------------------------------------------------------------
