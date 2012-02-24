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

    File:        CameraStateAndConfig.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 12 01

    Description: Implementation of the @em cedar::devices::visual::CameraStateAndConfig class.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/devices/sensors/visual/camera/CameraStateAndConfig.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/round.hpp> //rounding from double to int in cv::VideoCapture get and set methods


using namespace cedar::dev::sensors::visual;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
CameraStateAndConfig::CameraStateAndConfig(
                                          cv::VideoCapture videoCapture,
                                          QReadWriteLockPtr videoCaptureLock,
                                          unsigned int channel,
                                          const std::string configurationFileName,
                                          const std::string capabilitiesFileName
                                        )
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraStateAndConfig::CameraConfiguration] channel "<< channel
              << " Config-file: " << configurationFileName << std::endl;
    std::cout << "[CameraStateAndConfig::CameraConfiguration] channel "<< channel
              << " Capability-file: " << capabilitiesFileName << std::endl;
  #endif

  mVideoCapture = videoCapture;
  mConfigurationFileName = configurationFileName;
  mCapabilitiesFileName = capabilitiesFileName;
  mpVideoCaptureLock = videoCaptureLock;

  mChannel = channel;
  mChannelPrefix = "ch"+boost::lexical_cast<std::string>(channel)+"_";

  try
  {
    //create class for capabilities of the camera
    mpCamCapabilities = CameraCapabilitiesPtr(new CameraCapabilities(mCapabilitiesFileName));

    //create structure with supported properties and their values
    //to monitor the settings made by the user
    //values dosn't matter, because thezy will be set in onDeclareParameters
    //in class CameraConfig
    int num_properties = CameraProperty::type().list().size();
    for (int i=0; i<num_properties; i++)
    {
      CameraProperty::Id prop_id = CameraProperty::type().list().at(i).id(); //.id() init exception
      if (mpCamCapabilities->isSupported(prop_id))
      {
        mCamPropertyValues.insert(PROPERTY_VALUE_PAIR(prop_id,0));
      }
      else
      {
        mCamPropertyValues.insert(PROPERTY_VALUE_PAIR(prop_id,CAMERA_PROPERTY_NOT_SUPPORTED));
      }
    }


    //mCamSettings is initialized on readOrDefaultConfiguration
    //and for local storage of properties and settings
    mpCamConfig = CameraConfigPtr(new CameraConfig
                                      (
                                         mConfigurationFileName,
                                         mChannel,
                                         mCamSettings,
                                         mCamPropertyValues
                                       )
                                  );

    //now set all restored parameters to the camera
    setAllParametersToCam();
  }
  catch (...)
  {

    CEDAR_THROW
    (
      cedar::aux::exc::InitializationException,
      "[CameraStateAndConfig::CameraStateAndConfig] - Critical error in constructor"
    );
  }


}

//----------------------------------------------------------------------------------------------------------------------
CameraStateAndConfig::~CameraStateAndConfig()
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraStateAndConfig::~CameraStateAndConfig] channel "<< mChannel << " Destroy class" << std::endl;
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
bool CameraStateAndConfig::saveConfiguration()
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[CameraStateAndConfig::writeConfiguration] channel " << mChannel <<  std::endl;
  #endif

  bool result = true;
  //Camera Capabilities shouldn't be written

  //Save values from the camera
  result = mpCamConfig->saveConfiguration() && result;

  #ifdef DEBUG_CAMERAGRABBER
    std::cout<<"[CameraStateAndConfig::writeConfiguration] channel "<< mChannel << " result: "
             << std::boolalpha << result << std::noboolalpha << std::endl;
  #endif

  return result;
}


//--------------------------------------------------------------------------------------------------------------------
bool CameraStateAndConfig::setProperty(CameraProperty::Id propId, double value)
{

  int wanted_value = boost::math::iround(value);// static_cast<int>(value);

  #ifdef ENABLE_GRABBER_WARNING_OUTPUT
    std::string prop_name = CameraProperty::type().get(propId).name();
  #endif

  // for cv::VideoCapture.get() we need as an unsigned int
  int prop_id = static_cast<unsigned int>(propId);

  //------------------------------------------------------
  //check if undefined properties
  if ( propId == cedar::aux::Enum::UNDEFINED)
  {
    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraStateAndConfig::setCameraProperty] Couldn't set undefined property \""
                << prop_name << "\" (ID: "<< prop_id << ")" << std::endl;
    #endif

    return false;
  }

  //------------------------------------------------------
  //check if property is supported from the cam
  if (!isSupported(propId))
  {
    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraStateAndConfig::setCameraProperty] Property \"" << prop_name
                << "\" unsupported on channel " << mChannel << std::endl;
    #endif

    mCamPropertyValues[propId] = CAMERA_PROPERTY_NOT_SUPPORTED;
    return false;
  }

  //------------------------------------------------------
  //check property modes
  if (wanted_value == CAMERA_PROPERTY_MODE_AUTO)
  {
    if (isAutoCapable(propId))
    {
      if (setCamProperty(prop_id, static_cast<double>(CAMERA_PROPERTY_MODE_AUTO)))
      {
        mCamPropertyValues[propId] = CAMERA_PROPERTY_MODE_AUTO;
        return true;
      }
      else
      {
        #ifdef ENABLE_GRABBER_WARNING_OUTPUT
        std::cout << "[CameraStateAndConfig::setCameraProperty] Channel "<< mChannel << ": Property \"" << prop_name
                  << "\" couldn't set to mode \"auto\""  << std::endl;
        #endif
        return false;
      }
    }
    else
    {
      #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraStateAndConfig::setCameraProperty] Channel "<< mChannel << ": Property \"" << prop_name
                << "\" doesn't support \"CAMERA_PROPERTY_MODE_AUTO\""  << std::endl;
      #endif
      return false;
    }
  }
  else if (wanted_value == CAMERA_PROPERTY_MODE_OFF)
  {
    if (isOnOffCapable(propId))
    {
      if (setCamProperty(prop_id, static_cast<double>(CAMERA_PROPERTY_MODE_OFF)))
      {
        mCamPropertyValues[propId] = CAMERA_PROPERTY_MODE_OFF;
        return true;
      }
      else
      {
        #ifdef ENABLE_GRABBER_WARNING_OUTPUT
        std::cout << "[CameraStateAndConfig::setProperty] Channel "<< mChannel << ": Property \"" << prop_name
                  << "\" couldn't switched off"  << std::endl;
        #endif
        return false;
      }
    }
    else
    {
      #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraStateAndConfig::setProperty] Channel "<< mChannel << ": Property \"" << prop_name
                << "\" is not on/off capable"  << std::endl;
      #endif
      return false;
    }
  }
  else if (wanted_value == CAMERA_PROPERTY_MODE_ONE_PUSH_AUTO)
  {
    if (isOnePushCapable(propId))
    {
      if (setCamProperty(prop_id, static_cast<double>(CAMERA_PROPERTY_MODE_ONE_PUSH_AUTO)))
      {
        mCamPropertyValues[propId] = CAMERA_PROPERTY_MODE_ONE_PUSH_AUTO;
        return true;
      }
      else
      {
        #ifdef ENABLE_GRABBER_WARNING_OUTPUT
        std::cout << "[CameraStateAndConfig::setProperty] Channel "<< mChannel << ": Property \"" << prop_name
                  << "\" couldn't set to mode \"OnePushAuto\""  << std::endl;
        #endif
        return false;
      }
    }
    else
    {
      #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraStateAndConfig::setProperty] Channel "<< mChannel << ": Property \"" << prop_name
                << "\" is not \"one push auto\" capable"  << std::endl;
      #endif
      return false;
    }
  }
  else
  {
    //manual setting of property value
    if (! (isManualCapable(propId) || isAbsoluteCapable(propId) )  )
    {
      #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraStateAndConfig::setProperty] Channel "<< mChannel << ": Property \"" << prop_name
                << "\" couldn't set to manual mode"  << std::endl;
      #endif
      return false;
    }

    //check lower and upper boundaries for manual settings
    int max_value = getMaxValue(propId);
    int min_value = getMinValue(propId);

    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      if ( (wanted_value > max_value) || (wanted_value < min_value))
      {
          std::cout << "[CameraStateAndConfig::setProperty] Channel "<< mChannel << ": Property \"" << prop_name
                    << "\" has a range of [" << min_value << ","<< max_value << "]" << std::endl;
          std::cout << "[CameraStateAndConfig::setProperty] Your value of "<< value
                    << " exceeds this boundaries" << std::endl;
      }
    #endif

    if (value > max_value)
    {
      wanted_value = max_value;
    }
    else if (value < min_value)
    {
      wanted_value = min_value;
    }

    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      if (wanted_value != boost::math::iround(value))
      {
        std::cout << "[CameraStateAndConfig::setProperty] set " << prop_name << " to " << wanted_value << std::endl;
      }
    #endif


    //check if already set
    int old_value = boost::math::iround(getProperty(propId));
    if ( old_value == wanted_value)
    {
      return true;
    }

    //not set, set it
    int new_value = 0;
    //if (mVideoCaptures.at(channel).set(propId, static_cast<double>(wanted_value))) asdf
    if (setCamProperty(prop_id,value) )
    {
      //and check if successful
      new_value = boost::math::iround(getPropertyValue(propId));

      //sync with local storage
      mCamPropertyValues[propId] = new_value;

      if (new_value == wanted_value)
      {
        return true;
      }
    }

    // an error occurred
    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraStateAndConfig::setProperty] Couldn't set Property "
                << prop_name
                << " to " << value << ". " << std::endl;
    #endif

    //sync with local storage
    mCamPropertyValues[propId] = getProperty(propId);


    return false;
  }





}

//--------------------------------------------------------------------------------------------------------------------
double CameraStateAndConfig::getProperty(CameraProperty::Id propId)
{

  if (propId == cedar::aux::Enum::UNDEFINED)
  {
    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraStateAndConfig::getProperty] Undefined property \""
                << CameraProperty::type().get(propId).name()
                << "\" (ID: "
                << boost::lexical_cast<int>(propId)
                << ")" << std::endl;
    #endif

    return static_cast<double>(CAMERA_PROPERTY_NOT_SUPPORTED);
  }

  // return value of local storage
  return mCamPropertyValues[propId];
}

//--------------------------------------------------------------------------------------------------------------------
double CameraStateAndConfig::getPropertyValue(CameraProperty::Id propId)
{
  #ifdef ENABLE_GRABBER_WARNING_OUTPUT
    std::string prop_name = CameraProperty::type().get(propId).name();
  #endif

  // for cv::VideoCapture.get() we need as an unsigned int
  int prop_id = static_cast<unsigned int>(propId);

  if (propId == cedar::aux::Enum::UNDEFINED)
  {
    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraStateAndConfig::getPropertyValue] Undefined property \""
                << prop_name << "\" (ID: "<< prop_id << ")" << std::endl;
    #endif

    return false;
  }


  if (isSupported(propId))
  {
    return getCamProperty(prop_id);
  }
  else
  {
    #ifdef ENABLE_GRABBER_WARNING_OUTPUT
      std::cout << "[CameraStateAndConfig::getPropertyValue] Unsupported property \""
                << prop_name << "\" (ID: "<< prop_id << ")" << std::endl;
    #endif
    return static_cast<double>(CAMERA_PROPERTY_NOT_SUPPORTED);
  }
}

//----------------------------------------------------------------------------------------------------
bool CameraStateAndConfig::setCamProperty(unsigned int propId, double value)
{
  bool result;
  mpVideoCaptureLock->lockForWrite();
  result = mVideoCapture.set(propId, value);
  mpVideoCaptureLock->unlock();
  return result;
}

//----------------------------------------------------------------------------------------------------
double CameraStateAndConfig::getCamProperty(unsigned int propId)
{
  double result;
  mpVideoCaptureLock->lockForWrite();
  result = mVideoCapture.get(propId);
  mpVideoCaptureLock->unlock();
  return result;
}
//----------------------------------------------------------------------------------------------------
bool CameraStateAndConfig::setAllParametersToCam()
{
#ifdef DEBUG_CAMERAGRABBER
  std::cout << "[CameraState::setAllParametersToCam] channel " << mChannel << std::endl;
#endif

  //all parameters are already read from config-file and stored in mCamPropertyValues,mCamSettings
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
  if (id_mode != CameraVideoMode::MODE_NOT_SET)
  {
    setCamProperty(static_cast<unsigned int> (CameraSetting::SETTING_MODE), static_cast<unsigned int> (id_mode));
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
      for (int i = 0; i < num_fps_settings; i++)
      {
        int act_fps = CameraFrameRate::type().list().at(i);
        int diff = abs(act_fps - fps);
        if (diff < min_diff)
        {
          min_diff = diff;
          id_fps = act_fps;
        }
      }
    } catch (boost::bad_lexical_cast const&)
    {
      //no integer, use default
      id_fps = CameraFrameRate::FRAMERATE_NOT_SET;
    }
  }

  if (id_fps != CameraFrameRate::FRAMERATE_NOT_SET)
  {
    setCamProperty(static_cast<unsigned int> (CameraSetting::SETTING_FPS), static_cast<unsigned int> (id_fps));
  }

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
      for (int i = 0; i < num_iso_settings; i++)
      {
        int act_iso = CameraIsoSpeed::type().list().at(i);
        int diff = abs(act_iso - iso);

        if (diff < min_diff)
        {
          min_diff = diff;
          id_iso = act_iso;
        }
      }
    } catch (boost::bad_lexical_cast const&)
    {
      //no integer, use default
      id_iso = CameraIsoSpeed::ISO_NOT_SET;
    }
  }

  if (id_iso != CameraIsoSpeed::ISO_NOT_SET)
  {
    setCamProperty(static_cast<unsigned int> (CameraSetting::SETTING_ISO_SPEED), static_cast<unsigned int> (id_iso));
  }

  //---------------------------------------------
  //properties for each channel
  //our map from prop-id to value
  CameraPropertyValues::iterator it;

  for (it = mCamPropertyValues.begin(); it != mCamPropertyValues.end(); it++)
  {
    unsigned int prop_id = it->first;
    int prop_val = it->second;

    if (  (prop_val != CAMERA_PROPERTY_NOT_SUPPORTED) ) //|| (prop_val != CAMERA_PROPERTY_MODE_AUTO) )
    {
      //with recheck in case something wrong in the config-file
      setProperty(prop_id, prop_val);
      //without recheck
      //setCamProperty(prop_id,prop_val);
    }
  }
  //values set in camera, but there they aren't taken by the camera
  //this gives the default-values created by the cv::CvCapture class
  /*#ifdef DEBUG_CAMERAGRABBER
   std::cout << "\n[CameraStateAndConfig::setAllParametersToCam]" << channel << std::endl;
   printAllSettings(channel);
   printAllProperties(channel);
   std::cout << "\n[CameraStateAndConfig::setAllParametersToCam]" << channel << std::endl;
   #endif
   */
  return true;
}


//--------------------------------------------------------------------------------------------------------------------
bool CameraStateAndConfig::setSetting(CameraSetting::Id settingId, double value)
{
  unsigned int prop_id = static_cast<unsigned int>(settingId);
  return setCamProperty(prop_id,value);
}

//--------------------------------------------------------------------------------------------------------------------
double CameraStateAndConfig::getSetting(CameraSetting::Id settingId)
{
  switch (settingId)
  {
  case CameraSetting::SETTING_FPS :
    return static_cast<double>(CameraFrameRate::type().get(mCamSettings.fps).id());
  case CameraSetting::SETTING_ISO_SPEED :
    return static_cast<double>(CameraIsoSpeed::type().get(mCamSettings.iso_speed).id());
  case CameraSetting::SETTING_MODE :
    return static_cast<double>(CameraVideoMode::type().get(mCamSettings.mode).id());
  case CameraSetting::SETTING_FRAME_HEIGHT :
    return getCamProperty(CameraSetting::SETTING_FRAME_HEIGHT);
  case CameraSetting::SETTING_FRAME_WIDTH :
    return getCamProperty(CameraSetting::SETTING_FRAME_WIDTH);
  default :
    return CAMERA_PROPERTY_NOT_SUPPORTED;
  }
}

//--------------------------------------------------------------------------------------------------------------------
int CameraStateAndConfig::getMinValue(CameraProperty::Id propId)
{
  return mpCamCapabilities->getMinValue(propId);
}

//--------------------------------------------------------------------------------------------------------------------
int CameraStateAndConfig::getMaxValue(CameraProperty::Id propId)
{
  return mpCamCapabilities->getMaxValue(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraStateAndConfig::isSupported(CameraProperty::Id propId)
{
  return mpCamCapabilities->isSupported(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraStateAndConfig::isReadable(CameraProperty::Id propId)
{
  return mpCamCapabilities->isReadable(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraStateAndConfig::isOnePushCapable(CameraProperty::Id propId)
{
  return mpCamCapabilities->isOnePushCapable(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraStateAndConfig::isOnOffCapable(CameraProperty::Id propId)
{
  return mpCamCapabilities->isOnOffCapable(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraStateAndConfig::isAutoCapable(CameraProperty::Id propId)
{
  return mpCamCapabilities->isAutoCapable(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraStateAndConfig::isManualCapable(CameraProperty::Id propId)
{
  return mpCamCapabilities->isManualCapable(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraStateAndConfig::isAbsoluteCapable(CameraProperty::Id propId)
{
  return mpCamCapabilities->isAbsoluteCapable(propId);
}



