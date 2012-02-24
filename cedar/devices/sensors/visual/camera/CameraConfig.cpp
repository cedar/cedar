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

    File:        CameraConfig.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 12 01

    Description: Implementation of the @em cedar::devices::visual::CameraConfig class.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/devices/sensors/visual/camera/CameraConfig.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/lexical_cast.hpp> //for reading values from the configfile

using namespace cv;
using namespace cedar::dev::sensors::visual;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
CameraConfig::CameraConfig(
                           //cv::VideoCapture videoCapture,
                           //QReadWriteLockPtr videoCaptureLock,
                           const std::string& configFileName,
                           unsigned int channel,
                           //SupportedPropertiesSet supportedProperties,
                           CameraSettings &camSettings,
                           CameraPropertyValues &camPropertyValues
                        )
:
ConfigurationInterface(configFileName),
mCamSettings(camSettings),
mCamPropertyValues(camPropertyValues)
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraConfig::CameraState] channel "<< channel << " Config-file: " << configFileName << std::endl;
  #endif

    //mVideoCapture = videoCapture;
    //mSupportedProperties = supportedProperties;
    //mpVideoCaptureLock = videoCaptureLock;

    mChannel = channel;
    mChannelPrefix = "ch"+boost::lexical_cast<std::string>(channel)+"_";

    bool result = declareParameter();
    ConfigurationInterface::readOrDefaultConfiguration();
    //result = setAllParametersToCam() && result;


    if (not result )
    {
      std::string err = "[CameraCapabilities::CameraCapabilities] channel "
                        + boost::lexical_cast<std::string>(channel)
                        + " - Critical error in constructor";

      //throwing an exception in a shared-pointer managed class is catched by shared-pointer
      std::cout << err << std::endl;
      CEDAR_THROW(cedar::aux::exc::InitializationException,err);
    }

}

CameraConfig::~CameraConfig()
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraConfig::~CameraState] channel "<< mChannel << " Destroy class" << std::endl;
  #endif
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool CameraConfig::saveConfiguration()
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraConfig::writeConfiguration] channel "<< mChannel << std::endl;
  #endif

  ConfigurationInterface::writeConfiguration();
  return true;
}

 //----------------------------------------------------------------------------------------------------
bool CameraConfig::declareParameter()
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
  result = (addParameter(&mCamSettings.mode, param_name, param_value) == CONFIG_SUCCESS) && result;
  //std::cout << "set videomode to default: " << param_value << std::endl;

  param_value = CameraFrameRate::type().get(CAMERA_DEFAULT_FRAMERATE).name();
  param_name = mChannelPrefix + "cam_framerate";
  result = (addParameter(&mCamSettings.fps, param_name, param_value) == CONFIG_SUCCESS) && result;
  //std::cout << "set FPS to default: " << param_value << std::endl;

  param_value = CameraIsoSpeed::type().get(CAMERA_DEFAULT_ISO_SPEED).name();
  param_name = mChannelPrefix + "cam_iso_speed";
  result = (addParameter(&mCamSettings.iso_speed, param_name, param_value) == CONFIG_SUCCESS) && result;

  // -------------------------------
  // properties
  // -------------------------------
  CameraPropertyValues::iterator it;
  for (it = mCamPropertyValues.begin(); it != mCamPropertyValues.end(); it++)
  {
    double *param_adr = &(it->second);
    cedar::aux::EnumId param_id = static_cast<cedar::aux::EnumId> (it->first);
    std::string param_name = mChannelPrefix + CameraProperty::type().get(param_id).name();
    double param_old_val = it->second;

    //std::stringstream param_name_ch;
    //param_name_ch<<"ch"<<channel<<"_"<<param_name;

    result = (addParameter(param_adr, param_name, CAMERA_PROPERTY_MODE_AUTO) == CONFIG_SUCCESS) && result;

    //save status not supported in our map after configuration interface have modified it
    if (param_old_val == CAMERA_PROPERTY_NOT_SUPPORTED)
    {
      it->second = CAMERA_PROPERTY_NOT_SUPPORTED;
    }

  }
  return result;
}


//----------------------------------------------------------------------------------------------------
/*void CameraConfig::getAllParametersFromCam()
 {
 #ifdef DEBUG_CAMERAGRABBER
 std::cout << "[CameraConfig::getAllParametersFromCam] channel "<< mChannel << std::endl;
 #endif

 //read settings from camera (if set manually)
 unsigned int id_mode = static_cast<unsigned int>(CameraSetting::SETTING_MODE);
 unsigned int act_mode = CameraVideoMode::type().get(mCamSettings.mode).id();

 if (act_mode != CameraVideoMode::MODE_NOT_SET)
 {
 mCamSettings.mode = CameraVideoMode::type().get(getProperty(id_mode).name());
 }

 unsigned int id_fps = static_cast<unsigned int>(CameraSetting::SETTING_FPS);
 unsigned int act_fps = static_cast<unsigned int>(getProperty(id_fps));

 if (mCamSettings.fps != CameraFrameRate::FRAMERATE_NOT_SET)
 {
 mCamSettings.fps = CameraFrameRate::type().get(act_fps).name();
 }

 unsigned int id_iso = static_cast<unsigned int>(CameraSetting::SETTING_ISO_SPEED);
 unsigned int act_iso = static_cast<unsigned int>(getProperty(id_iso));

 if (mCamSettings.iso_speed != CameraIsoSpeed::ISO_NOT_SET)
 {
 mCamSettings.iso_speed = CameraIsoSpeed::type().get(act_iso).name();
 }

 //our map from prop-id to value
 //only supported properties
 CameraPropertyValues::iterator it;
 for ( it=mCamPropertyValues.begin(); it != mCamPropertyValues.end(); it++ )
 {
 //cedar::aux::EnumId prop_id = static_cast<cedar::aux::EnumId>(it->first);
 unsigned int prop_id = it->first;

 //read only if property is set to special mode( which are negative constants)
 if (it->second >= 0 )
 {
 it->second = static_cast<int>(getProperty(prop_id));
 }
 }
 }

   switch (propId)
  {
    case :

    default:

  }

  switch (key) {
    case value:

      break;
    default:
      break;
  }

    //read settings from camera (if set manually)
    unsigned int id_mode = static_cast<unsigned int>(CameraSetting::SETTING_MODE);
    unsigned int act_mode = CameraVideoMode::type().get(mCamSettings.mode).id();

    if (act_mode != CameraVideoMode::MODE_NOT_SET)
    {
      mCamSettings.mode = CameraVideoMode::type().get(getProperty(id_mode).name());
    }

    unsigned int id_fps = static_cast<unsigned int>(CameraSetting::SETTING_FPS);
    unsigned int act_fps = static_cast<unsigned int>(getProperty(id_fps));

    if (mCamSettings.fps != CameraFrameRate::FRAMERATE_NOT_SET)
    {
      mCamSettings.fps = CameraFrameRate::type().get(act_fps).name();
    }

    unsigned int id_iso = static_cast<unsigned int>(CameraSetting::SETTING_ISO_SPEED);
    unsigned int act_iso = static_cast<unsigned int>(getProperty(id_iso));

    if (mCamSettings.iso_speed != CameraIsoSpeed::ISO_NOT_SET)
    {
      mCamSettings.iso_speed = CameraIsoSpeed::type().get(act_iso).name();
    }

    //our map from prop-id to value
    //only supported properties
    CameraPropertyValues::iterator it;
    for ( it=mCamPropertyValues.begin(); it != mCamPropertyValues.end(); it++ )
    {
      //cedar::aux::EnumId prop_id = static_cast<cedar::aux::EnumId>(it->first);
      unsigned int prop_id = it->first;

      //read only if property is set to special mode( which are negative constants)
      if (it->second >= 0 )
      {
        it->second = static_cast<int>(getProperty(prop_id));
      }
    }


 */

