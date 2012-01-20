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

    File:        CameraState.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 12 01

    Description: Implementation of the @em cedar::devices::visual::CameraState class.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "CameraState.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/lexical_cast.hpp> //for reading values from the configfile

using namespace cv;
using namespace cedar::dev::sensors::visual;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
CameraState::CameraState(
                          cv::VideoCapture videoCapture,
                          QReadWriteLockPtr videoCaptureLock,
                          const std::string& channelPrefix,
                          SupportedPropertiesSet supportedProperties,
                          const std::string& configFileName
                        )
:
ConfigurationInterface(configFileName)
//mVideoCapture(videoCapture)
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraState::CameraState] Config-file: " << configFileName << std::endl;
  #endif

    mVideoCapture = videoCapture;
    mChannelPrefix = channelPrefix;
    mSupportedProperties = supportedProperties;
    mpVideoCaptureLock = videoCaptureLock;

    createParameterStorage();
    bool result = declareParameter();
    ConfigurationInterface::readOrDefaultConfiguration();
    result = setAllParametersToCam() && result;


    if (not result )
    {
      std::string err = "[CameraCapabilities::CameraCapabilities] - Critical error in constructor";
      //throwing an exception prevents main-grabber class to clean up => catch this exception
      std::cout << err << std::endl;
      CEDAR_THROW(cedar::aux::exc::InitializationException,err);
    }

}

CameraState::~CameraState()
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraState::~CameraState] Destroy class" << std::endl;
  #endif
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool CameraState::writeConfiguration()
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraState::writeConfiguration]" << std::endl;
  #endif

  getAllParametersFromCam();
  return ConfigurationInterface::writeConfiguration();
}

//----------------------------------------------------------------------------------------------------------------------
// creates the parameter storage needed for configuration interface
 void CameraState::createParameterStorage()
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
 bool CameraState::declareParameter()
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
   result = (addParameter(&mCamSettings.mode,param_name,param_value) == CONFIG_SUCCESS) && result;
   //std::cout << "set videomode to default: " << param_value << std::endl;

   param_value = CameraFrameRate::type().get(CAMERA_DEFAULT_FRAMERATE).name();
   param_name = mChannelPrefix + "cam_framerate";
   result = (addParameter(&mCamSettings.fps,param_name,param_value) == CONFIG_SUCCESS) && result;
   //std::cout << "set FPS to default: " << param_value << std::endl;

   param_value = CameraIsoSpeed::type().get(CAMERA_DEFAULT_ISO_SPEED).name();
   param_name = mChannelPrefix + "cam_iso_speed";
   result = (addParameter(&mCamSettings.iso_speed,param_name,param_value) == CONFIG_SUCCESS) && result;
   // -------------------------------
   CameraPropertyValues::iterator it;
   for ( it=mCamPropertyValues.begin(); it != mCamPropertyValues.end(); it++ )
   {
     int *param_adr = &(it->second);
     cedar::aux::EnumId param_id = static_cast<cedar::aux::EnumId>(it->first);
     std::string param_name = mChannelPrefix+CameraProperty::type().get(param_id).name();

     //std::stringstream param_name_ch;
     //param_name_ch<<"ch"<<channel<<"_"<<param_name;

     result = (addParameter(param_adr, param_name, CAMERA_PROPERTY_MODE_AUTO) == CONFIG_SUCCESS) && result;
   }
   return result;
}
 //----------------------------------------------------------------------------------------------------
 bool CameraState::setAllParametersToCam()
 {
   #ifdef DEBUG_CAMERAGRABBER
     std::cout << "[CameraState::setAllParametersToCam]" << std::endl;
   #endif


   //all parameters are read from config-file and stored in mCamPropertyValues,mCamSettings
   //all values in the configuration file have to be valid!!!

   //---------------------------------------------
   // Settings
   //---------------------------------------------
   CameraVideoMode::Id id_mode = CameraVideoMode::type().get(mCamSettings.mode).id();
   if (id_mode == cedar::aux::Enum::UNDEFINED)
   {
     id_mode = CameraVideoMode::MODE_NOT_SET;
   }
   //only set it if a mode is given. Otherwise use the default auto-mode from OpenCv
   if (id_mode != CameraVideoMode::MODE_NOT_SET )
   {
     setProperty(static_cast<unsigned int>(CameraSetting::SETTING_MODE),static_cast<unsigned int>(id_mode));
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
   setProperty(static_cast<unsigned int>(CameraSetting::SETTING_FPS), static_cast<unsigned int>(id_fps));

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
   setProperty(static_cast<unsigned int>(CameraSetting::SETTING_ISO_SPEED), static_cast<unsigned int>(id_iso));

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
       setProperty(prop_id,prop_val);
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
 void CameraState::getAllParametersFromCam()
 {
    #ifdef DEBUG_CAMERAGRABBER
      std::cout << "[CameraState::getAllParametersFromCam]" << std::endl;
    #endif

   //read settings from camera
   unsigned int id_mode = static_cast<unsigned int>(CameraSetting::SETTING_MODE);
   unsigned int act_mode = static_cast<unsigned int>(getProperty(id_mode));
   mCamSettings.mode = CameraVideoMode::type().get(act_mode).name();

   unsigned int id_fps = static_cast<unsigned int>(CameraSetting::SETTING_FPS);
   unsigned int act_fps = static_cast<unsigned int>(getProperty(id_fps));
   mCamSettings.fps = CameraFrameRate::type().get(act_fps).name();

   unsigned int id_iso = static_cast<unsigned int>(CameraSetting::SETTING_ISO_SPEED);
   unsigned int act_iso = static_cast<unsigned int>(getProperty(id_iso));
   mCamSettings.iso_speed = CameraIsoSpeed::type().get(act_iso).name();

   //our map from prop-id to value
   //only supported properties
   CameraPropertyValues::iterator it;
   for ( it=mCamPropertyValues.begin(); it != mCamPropertyValues.end(); it++ )
   {
     //cedar::aux::EnumId prop_id = static_cast<cedar::aux::EnumId>(it->first);
     unsigned int prop_id = it->first;
     //int old_prop_val = it->second;
     it->second = static_cast<int>(getProperty(prop_id));
   }
}

 //----------------------------------------------------------------------------------------------------
 bool CameraState::setProperty(unsigned int prop_id, double value)
 {
    #ifdef DEBUG_CAMERAGRABBER
      std::cout << "[CameraState::setProperty] prop_id " << prop_id
                << " value " << boost::lexical_cast<std::string>(value)
                << std::endl;
    #endif

   bool result;
   mpVideoCaptureLock->lockForWrite();
   result = mVideoCapture.set(prop_id,value);
   mpVideoCaptureLock->unlock();

   return result;
 }

 //----------------------------------------------------------------------------------------------------
 double CameraState::getProperty(unsigned int prop_id)
 {
    #ifdef DEBUG_CAMERAGRABBER
      std::cout << "[CameraState::getProperty] prop_id " << prop_id;
    #endif
   double value;
   mpVideoCaptureLock->lockForWrite();
   value = mVideoCapture.get(prop_id);
   mpVideoCaptureLock->unlock();

    #ifdef DEBUG_CAMERAGRABBER
       std::cout  << " value " << boost::lexical_cast<std::string>(value)
       << std::endl;
    #endif

   return value;
 }
