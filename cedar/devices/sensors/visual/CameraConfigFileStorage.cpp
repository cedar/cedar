/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        <filename>

    Maintainer:  <first name> <last name>
    Email:       <email address>
    Date:        <creation date YYYY MM DD>

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "CameraConfigFileStorage.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/lexical_cast.hpp> //for reading values from the configfile

using namespace cv;
using namespace cedar::dev::sensors::visual;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
CameraConfigFileStorage::CameraConfigFileStorage(
                                                  cv::VideoCapture& videoCapture,
                                                  const std::string& channelPrefix,
                                                  SupportedPropertiesSet supportedProperties,
                                                  const std::string& configFileName
                                                )
:
ConfigurationInterface(configFileName),
mVideoCapture(videoCapture)
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraConfigFileStorage::CameraConfigFileStorage] Config-file: " << configFileName << std::endl;
  #endif

    //mVideoCapture = videoCapture;
    mChannelPrefix = channelPrefix;
    mSupportedProperties = supportedProperties;

    createParameterStorage();
    bool result = declareParameter();
    ConfigurationInterface::readOrDefaultConfiguration();
    result = result && setAllParametersToCam();


    if (not result )
    {
      std::string err = "[CameraCapabilities::CameraCapabilities] - Critical error in constructor";
      //throwing an exception prevents main-grabber class to clean up => catch this exception
      std::cout << err << std::endl;
      CEDAR_THROW(cedar::aux::exc::InitializationException,err);
    }

}

CameraConfigFileStorage::~CameraConfigFileStorage()
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraConfigFileStorage::~CameraConfigFileStorage] Destroy class" << std::endl;
  #endif
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool CameraConfigFileStorage::writeConfiguration()
{
  getAllParametersFromCam();
  return ConfigurationInterface::writeConfiguration();
}

//----------------------------------------------------------------------------------------------------------------------
// creates the parameter storage needed for configuration interface
 void CameraConfigFileStorage::createParameterStorage()
 {
   //create and insert only supported properties,
   //value dosn't matter, because it will be set in onDeclareParameters
   SupportedPropertiesSet::iterator it;
   for (it = mSupportedProperties.begin(); it != mSupportedProperties.end();it++)
   {
     unsigned int id = static_cast<unsigned int>(*it);
     mCamPropertyValues.insert(PROPERTY_VALUE_PAIR(id, 0));
   }

   //mCamSettings is initialized on readOrDefaultConfiguration
 }

 //----------------------------------------------------------------------------------------------------
 bool CameraConfigFileStorage::declareParameter()
 {
   //map local buffer of camera properties and settings
   //to the ConfigurationInterface class
   std::string param_value;
   std::string param_name;
   bool result = true;

   // -------------------------------
   // settings
   // -------------------------------
   param_value = CameraVideoMode::type().get(CameraVideoMode::MODE_NOT_SET).name();
   param_name = mChannelPrefix + "cam_mode";
   result = result && (addParameter(&mCamSettings.mode,param_name,param_value) == CONFIG_SUCCESS);
   //std::cout << "set videomode to default: " << param_value << std::endl;

   param_value = CameraFrameRate::type().get(CAMERA_DEFAULT_FRAMERATE).name();
   param_name = mChannelPrefix + "cam_framerate";
   result = result && (addParameter(&mCamSettings.fps,param_name,param_value) == CONFIG_SUCCESS);
   //std::cout << "set FPS to default: " << param_value << std::endl;

   param_value = CameraIsoSpeed::type().get(CAMERA_DEFAULT_ISO_SPEED).name();
   param_name = mChannelPrefix + "cam_iso_speed";
   result = result && (addParameter(&mCamSettings.iso_speed,param_name,param_value) == CONFIG_SUCCESS);
   //std::cout << "set ISO-speed to default: " << param_value << std::endl;


   // -------------------------------
   //properties
   // -------------------------------
   CameraPropertyValues::iterator it;
   for ( it=mCamPropertyValues.begin(); it != mCamPropertyValues.end(); it++ )
   {
     int *param_adr = &(it->second);
     cedar::aux::EnumId param_id = static_cast<cedar::aux::EnumId>(it->first);
     std::string param_name = mChannelPrefix+CameraProperty::type().get(param_id).name();

     //std::stringstream param_name_ch;
     //param_name_ch<<"ch"<<channel<<"_"<<param_name;

     result = result && (addParameter(param_adr, param_name, CAMERA_PROPERTY_MODE_AUTO) == CONFIG_SUCCESS);
   }
   return result;
}
 //----------------------------------------------------------------------------------------------------
 bool CameraConfigFileStorage::setAllParametersToCam()
 {
   #ifdef DEBUG_CAMERAGRABBER
     std::cout << "[CameraConfigFileStorage::setAllParametersToCam]" << std::endl;
   #endif


   //all parameters are readed from config-file and stored in mCamPropertyValues,mCamSettings
   //all values in the configuration file have to be valid!!!

   //---------------------------------------------
   // Settings
   //---------------------------------------------
   CameraVideoMode::Id id_mode = CameraVideoMode::type().get(mCamSettings.mode).id();
   if (id_mode == cedar::aux::Enum::UNDEFINED)
   {
     id_mode = CameraVideoMode::MODE_NOT_SET;
   }
   //only set it if a mode is given. Otherwise use auto-mode from OpenCv
   if (id_mode != CameraVideoMode::MODE_NOT_SET )
   {
     mVideoCapture.set(static_cast<unsigned int>(CameraSetting::SETTING_MODE),static_cast<unsigned int>(id_mode));
   }

   CameraFrameRate::Id id_fps = CameraFrameRate::type().get(mCamSettings.fps).id();
   if (id_fps == cedar::aux::Enum::UNDEFINED)
   {
     //perhaps it is an integer?
     try
     {
         int fps = boost::lexical_cast<int>(mCamSettings.fps);

         //yes: set it to the nearest possible value
         int num_fps_settings = CameraFrameRate::type().list().size();

         int min_diff = INT_MAX;
         for (int i=0; i<num_fps_settings; i++)
         {
           int act_fps = CameraFrameRate::type().list().at(i);
           int diff = abs(act_fps - fps);
           if ( diff < min_diff)
           {
             min_diff = diff;
             id_fps = act_fps;
           }
         }
     }
     catch( boost::bad_lexical_cast const& )
     {
       //no integer, use default
       id_fps = CAMERA_DEFAULT_FRAMERATE;
     }
   }
   mVideoCapture.set(static_cast<unsigned int>(CameraSetting::SETTING_FPS), static_cast<unsigned int>(id_fps));

   //---------------------------------------------
   CameraIsoSpeed::Id id_iso = CameraIsoSpeed::type().get(mCamSettings.iso_speed).id();
   if (id_iso == cedar::aux::Enum::UNDEFINED)
   {
     //perhaps it is an integer?
     try
     {
         int iso = boost::lexical_cast<int>(mCamSettings.iso_speed);

         //yes: set it to the nearest possible value
         int num_iso_settings = CameraIsoSpeed::type().list().size();

         int min_diff = INT_MAX;
         for (int i=0; i<num_iso_settings; i++)
         {
           int act_iso = CameraIsoSpeed::type().list().at(i);
           int diff = abs(act_iso - iso);

           if ( diff < min_diff )
           {
             min_diff=diff;
             id_iso = act_iso;
           }
         }
     }
     catch( boost::bad_lexical_cast const& )
     {
       //no integer, use default
       id_iso = CAMERA_DEFAULT_ISO_SPEED;
     }
   }
   mVideoCapture.set(static_cast<unsigned int>(CameraSetting::SETTING_ISO_SPEED), static_cast<unsigned int>(id_iso));

   //---------------------------------------------
   //properties for each channel
   //our map from prop-id to value
   CameraPropertyValues::iterator it;

   for ( it=mCamPropertyValues.begin(); it != mCamPropertyValues.end(); it++ )
   {
     unsigned int prop_id = it->first;
     int prop_val = it->second;

     if (prop_val != CAMERA_PROPERTY_NOT_SUPPORTED)
     {
       mVideoCapture.set(prop_id,prop_val);
     }
   }
   //values set in camera, but there they aren't taken by the camera
   //this gives the default-values created by the cv::CvCapture class
   /*#ifdef DEBUG_CAMERAGRABBER
      std::cout << "\n[CameraGrabber::setAllParametersToCam]" << channel << std::endl;
      printAllSettings(channel);
      printAllProperties(channel);
      std::cout << "\n[CameraGrabber::setAllParametersToCam]" << channel << std::endl;
   #endif
   */
     return true;
 }


 //----------------------------------------------------------------------------------------------------
 void CameraConfigFileStorage::getAllParametersFromCam()
 {
   //read settings from camera
   unsigned int id_mode = static_cast<unsigned int>(CameraSetting::SETTING_MODE);
   unsigned int act_mode = static_cast<unsigned int>(mVideoCapture.get(id_mode));
   mCamSettings.mode = CameraVideoMode::type().get(act_mode).name();

   unsigned int id_fps = static_cast<unsigned int>(CameraSetting::SETTING_FPS);
   unsigned int act_fps = static_cast<unsigned int>(mVideoCapture.get(id_fps));
   mCamSettings.fps = CameraFrameRate::type().get(act_fps).name();

   unsigned int id_iso = static_cast<unsigned int>(CameraSetting::SETTING_ISO_SPEED);
   unsigned int act_iso = static_cast<unsigned int>(mVideoCapture.get(id_iso));
   mCamSettings.iso_speed = CameraIsoSpeed::type().get(act_iso).name();

   //std::string config = mCamSettings.capability_config_file_name;
/*
   #ifdef DEBUG_CAMERAGRABBER
     std::cout << "[CameraConfigFileStorage::getAllParametersFromCam] Read videomode of: " << mode << std::endl;
     std::cout << "[CameraConfigFileStorage::getAllParametersFromCam] Read FPS of: " << fps << std::endl;
     std::cout << "[CameraConfigFileStorage::getAllParametersFromCam] Read ISO-speed of: " << iso << std::endl;
     unsigned int width = mVideoCapture.get(channel, CameraSetting::SETTING_FRAME_WIDTH);
     unsigned int height = mVideoCapture.get(channel, CameraSetting::SETTING_FRAME_HEIGHT);
     std::cout << "[CameraConfigFileStorage::getAllParametersFromCam] Read frame width of: " << width << std::endl;
     std::cout << "[CameraConfigFileStorage::getAllParametersFromCam] Read frame height of: " << height << std::endl;
     std::cout << "[CameraConfigFileStorage::getAllParametersFromCam] Camera configuration file: " << config << std::endl;
   #endif
*/

   //our map from prop-id to value
   //only supported properties
   CameraPropertyValues::iterator it;
   for ( it=mCamPropertyValues.begin(); it != mCamPropertyValues.end(); it++ )
   {
     //cedar::aux::EnumId prop_id = static_cast<cedar::aux::EnumId>(it->first);
     unsigned int prop_id = it->first;
     //int old_prop_val = it->second;
     it->second = static_cast<int>(mVideoCapture.get(prop_id));
   }
   /*
   #ifdef DEBUG_CAMERAGRABBER
     std::cout << "[CameraConfigFileStorage::getAllParametersFromCam] All Properties of camera: "<< std::endl;
     //printAllProperties(channel);
   #endif

  */
}

 /*
    bool result = false;
  cedar::aux::Enum val;

  //save setting to internal buffer for ConfigurationInterface
  switch ( setting )
  {
    case CameraSetting::SETTING_FPS:
      //fractional fps are coded as integers
      val = CameraFrameRate::type().get((unsigned int) value);
      if (val != cedar::aux::Enum::UNDEFINED)
      {
        mCamSettings.at(channel).fps = val.name();
        result=true;
      }
      break;

    case CameraSetting::SETTING_MODE:
      val = CameraVideoMode::type().get((int)value);
      if (val != cedar::aux::Enum::UNDEFINED)
      {
        mCamSettings.at(channel).mode = val.name();
        result=true;
      }
      break;

    case CameraSetting::SETTING_ISO_SPEED:
      val = CameraIsoSpeed::type().get((int)value);
      if (val != cedar::aux::Enum::UNDEFINED)
      {
        mCamSettings.at(channel).iso_speed = val.name();
        result=true;
      }
      break;
  }
  #ifdef ENABLE_GRABBER_WARNING_OUTPUT
    if ( !result )
    {
       std::cout << "[CameraGrabber::setCameraSetting] Couldn't set setting "
                 << CameraSetting::type().get(setting).name()
                 << " to " << value << std::endl;
       return false;
    }
  #endif
  */

