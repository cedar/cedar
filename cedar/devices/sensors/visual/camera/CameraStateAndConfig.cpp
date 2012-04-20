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

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/camera/CameraStateAndConfig.h"

// SYSTEM INCLUDES
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/round.hpp> //rounding from double to int in cv::VideoCapture get and set methods


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraStateAndConfig::CameraStateAndConfig
(
  cv::VideoCapture videoCapture,
  QReadWriteLock* pVideoCaptureLock,
  unsigned int channel,
  const std::string configurationFileName,
  const std::string capabilitiesFileName
)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);  
  std::stringstream debug_info;
  debug_info << ": Channel "<< channel << " Config-file: " << configurationFileName << std::endl
             << " Channel "<< channel << " Capability-file: " << capabilitiesFileName << std::endl;
                               
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             "CameraStateAndConfig" + debug_info.str(),
                                             "cedar::dev::sensors::visual::CameraStateAndConfig::CameraStateAndConfig()"
                                           );
  mVideoCapture = videoCapture;
  mConfigurationFileName = configurationFileName;
  mCapabilitiesFileName = capabilitiesFileName;
  mpVideoCaptureLock = pVideoCaptureLock;
  mInitialization = true;

  mChannel = channel;
  mChannelPrefix = "ch"+boost::lexical_cast<std::string>(channel)+"_";

  try
  {
    //create class for capabilities of the camera
    mpCamCapabilities = CameraCapabilitiesPtr(new CameraCapabilities(mCapabilitiesFileName));

    //create structure with supported properties and their values
    //to monitor the settings made by the user
    //values doesn't matter, because they will be set in onDeclareParameters
    //in class CameraConfig
    int num_properties = CameraProperty::type().list().size();
    for (int i=0; i<num_properties; i++)
    {
      CameraProperty::Id prop_id = CameraProperty::type().list().at(i).id();
      if (mpCamCapabilities->isSupported(prop_id))
      {
        mCamPropertyValues.insert(std::pair<unsigned int,int>(prop_id,0));
      }
      else
      {
        mCamPropertyValues.insert(std::pair<unsigned int,int>(prop_id,CAMERA_PROPERTY_NOT_SUPPORTED));
      }
    }

    //mCamSettings is initialized in class CameraConfig in method onDeclareParameters
    mpCamConfig = CameraConfigPtr(new CameraConfig(mConfigurationFileName, mChannel, mCamSettings, mCamPropertyValues));

    //now set all restored parameters to the camera
    setAllParametersToCam();
  }
  catch (...)
  {
    CEDAR_THROW
    (
      cedar::aux::InitializationException,
      "[CameraStateAndConfig::CameraStateAndConfig] - Critical error in constructor"
    );
  }

  mInitialization = false;
}

//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraStateAndConfig::~CameraStateAndConfig()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraStateAndConfig::saveConfiguration()
{
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             "CameraStateAndConfig: channel " 
                                               + boost::lexical_cast<std::string>(mChannel),
                                             "cedar::dev::sensors::visual::CameraStateAndConfig::saveConfiguration()"
                                           );

  bool result = true;
  //Camera Capabilities shouldn't be written

  //Save values from the camera
  result = mpCamConfig->saveConfiguration() && result;

  return result;
}


//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraStateAndConfig::setProperty(CameraProperty::Id propId, double value)
{
  int wanted_value = boost::math::iround(value);


  std::string prop_name = CameraProperty::type().get(propId).name();
  std::stringstream info;
  info << "Channel " << mChannel << ": Property \"" << prop_name << "\" ";

  // for cv::VideoCapture.get() we need as an unsigned int
  int prop_id = static_cast<unsigned int>(propId);

  //------------------------------------------------------
  //check if undefined properties
  if ( propId == cedar::aux::Enum::UNDEFINED)
  {
    info << "unknown (ID: "<< prop_id << ")";
    cedar::aux::LogSingleton::getInstance()->warning
                                             (
                                               "CameraStateAndConfig: " + info.str(),
                                               "cedar::dev::sensors::visual::CameraStateAndConfig::setProperty()"
                                             );
    return false;
  }

  //------------------------------------------------------
  //check if property is supported from the cam
  if (!isSupported(propId))
  {
    mCamPropertyValues[propId] = CAMERA_PROPERTY_NOT_SUPPORTED;

    //at initialization time, all properties will be initialized with NOT_SUPPORTED, 
    //this is not a warning
    if (!mInitialization)
    {
      info << "unsupported (ID: "<< prop_id << ")";
      cedar::aux::LogSingleton::getInstance()->warning
                                               (
                                                 "CameraStateAndConfig: " + info.str(),
                                                 "cedar::dev::sensors::visual::CameraStateAndConfig::setProperty()"
                                               );
    }
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
        info << "couldn't set to mode \"AUTO\" (ID: "<< prop_id << ")";
        cedar::aux::LogSingleton::getInstance()->warning
                                                 (
                                                   "CameraStateAndConfig: " + info.str(),
                                                   "cedar::dev::sensors::visual::CameraStateAndConfig::setProperty()"
                                                 );
        return false;
      }
    }
    else
    {
      if (! mInitialization)
      {
        info << "doesn't support mode \"AUTO\"";
        cedar::aux::LogSingleton::getInstance()->warning
                                                 (
                                                   "CameraStateAndConfig: " + info.str(),
                                                   "cedar::dev::sensors::visual::CameraStateAndConfig::setProperty()"
                                                 );        
      }
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
        info << "couldn't set to \"OFF\"";
        cedar::aux::LogSingleton::getInstance()->warning
                                                 (
                                                   "CameraStateAndConfig: " + info.str(),
                                                   "cedar::dev::sensors::visual::CameraStateAndConfig::setProperty()"
                                                 );        
        return false;
      }
    }
    else
    {
      if (!mInitialization)
      {
        info << "doesn't support mode \"ON/OFF\"";
        cedar::aux::LogSingleton::getInstance()->warning
                                                 (
                                                   "CameraStateAndConfig: " + info.str(),
                                                   "cedar::dev::sensors::visual::CameraStateAndConfig::setProperty()"
                                                 );  
      }
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
        info << "couldn't set to mode \"OnePushAuto\"";
        cedar::aux::LogSingleton::getInstance()->warning
                                                 (
                                                   "CameraStateAndConfig: " + info.str(),
                                                   "cedar::dev::sensors::visual::CameraStateAndConfig::setProperty()"
                                                 );  
        return false;
      }
    }
    else
    {
      if (!mInitialization)
      {
        info << "doesn't support mode \"OnePushAuto\"";
        cedar::aux::LogSingleton::getInstance()->warning
                                                 (
                                                   "CameraStateAndConfig: " + info.str(),
                                                   "cedar::dev::sensors::visual::CameraStateAndConfig::setProperty()"
                                                 );  
      }
      return false;
    }
  }
  else
  {
    //manual setting of property value
    if (! (isManualCapable(propId) || isAbsoluteCapable(propId) )  )
    {
      info << "is not able to set values manually";
      cedar::aux::LogSingleton::getInstance()->warning
                                               (
                                                 "CameraStateAndConfig: " + info.str(),
                                                 "cedar::dev::sensors::visual::CameraStateAndConfig::setProperty()"
                                               );  
      return false;
    }

    //check lower and upper boundaries for manual settings
    int max_value = getMaxValue(propId);
    int min_value = getMinValue(propId);

    if ( (wanted_value > max_value) || (wanted_value < min_value))
    {
      info << "has a range of [" << min_value << ","<< max_value << "]. "
           << "Your value of "<< value << " exceeds this boundaries!";
      cedar::aux::LogSingleton::getInstance()->warning
                                               (
                                                 "CameraStateAndConfig: " + info.str(),
                                                 "cedar::dev::sensors::visual::CameraStateAndConfig::setProperty()"
                                               );  
    }

    if (value > max_value)
    {
      wanted_value = max_value;
    }
    else if (value < min_value)
    {
      wanted_value = min_value;
    }

    if (wanted_value != boost::math::iround(value))
    {
      //Firewire only supports integer as property values
      //Other backends may be different
      //std::cout << "[CameraStateAndConfig::setProperty] set " << prop_name << " to " << wanted_value << std::endl;
    }

    //check if already set
    int old_value = boost::math::iround(getProperty(propId));
    if (old_value == wanted_value)
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
    info << "couldn't be set! An unexpected error occured! New property value: "
         << new_value << " (wanted value: " << wanted_value << ")";
    cedar::aux::LogSingleton::getInstance()->error
                                             (
                                               "CameraStateAndConfig: " + info.str(),
                                               "cedar::dev::sensors::visual::CameraStateAndConfig::setProperty()"
                                             );  

    //sync with local storage
    mCamPropertyValues[propId] = getProperty(propId);

    return false;
  }
}

//--------------------------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraStateAndConfig::getProperty(CameraProperty::Id propId)
{
  if (propId == cedar::aux::Enum::UNDEFINED)
  {
    cedar::aux::LogSingleton::getInstance()->warning
                                             (
                                               "CameraStateAndConfig: Undefined property (ID: "
                                                 + boost::lexical_cast<std::string>(propId) + ")",
                                               "cedar::dev::sensors::visual::CameraStateAndConfig::getProperty()"
                                             ); 
    return static_cast<double>(CAMERA_PROPERTY_NOT_SUPPORTED);
  }

  // return value of local storage
  return mCamPropertyValues[propId];
}

//--------------------------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraStateAndConfig::getPropertyValue(CameraProperty::Id propId)
{

  std::string prop_name = CameraProperty::type().get(propId).name();

  // for cv::VideoCapture.get() we need as an unsigned int
  int prop_id = static_cast<unsigned int>(propId);

  if (propId == cedar::aux::Enum::UNDEFINED)
  {
    cedar::aux::LogSingleton::getInstance()->warning
                                             (
                                               "CameraStateAndConfig: Undefined property (ID: "
                                                 + boost::lexical_cast<std::string>(propId) + ")",
                                               "cedar::dev::sensors::visual::CameraStateAndConfig::getPropertyValue()"
                                             ); 
    return false;
  }

  if (isSupported(propId))
  {
    return getCamProperty(prop_id);
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->warning
                                             (
                                               "CameraStateAndConfig: Unsupported property (ID: "
                                                 + boost::lexical_cast<std::string>(propId) + ")",
                                               "cedar::dev::sensors::visual::CameraStateAndConfig::getPropertyValue()"
                                             );  
    return static_cast<double>(CAMERA_PROPERTY_NOT_SUPPORTED);
  }
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraStateAndConfig::setCamProperty(unsigned int propId, double value)
{
  bool result;
  mpVideoCaptureLock->lockForWrite();
  result = mVideoCapture.set(propId, value);
  mpVideoCaptureLock->unlock();
  return result;
}

//----------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraStateAndConfig::getCamProperty(unsigned int propId)
{
  double result;
  mpVideoCaptureLock->lockForWrite();
  result = mVideoCapture.get(propId);
  mpVideoCaptureLock->unlock();
  return result;
}
//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraStateAndConfig::setAllParametersToCam()
{
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             "CameraStateAndConfig: Channel "
                                               + boost::lexical_cast<std::string>(mChannel) + ")",
                                             "cedar::dev::sensors::visual::CameraStateAndConfig::getPropertyValue()"
                                           ); 

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

  //properties for each channel
  //the map from prop-id to value
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
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraStateAndConfig::setSetting(CameraSetting::Id settingId, double value)
{
  unsigned int prop_id = static_cast<unsigned int>(settingId);
  return setCamProperty(prop_id,value);
}

//--------------------------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraStateAndConfig::getSetting(CameraSetting::Id settingId)
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
int cedar::dev::sensors::visual::CameraStateAndConfig::getMinValue(CameraProperty::Id propId)
{
  return mpCamCapabilities->getMinValue(propId);
}

//--------------------------------------------------------------------------------------------------------------------
int cedar::dev::sensors::visual::CameraStateAndConfig::getMaxValue(CameraProperty::Id propId)
{
  return mpCamCapabilities->getMaxValue(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraStateAndConfig::isSupported(CameraProperty::Id propId)
{
  return mpCamCapabilities->isSupported(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraStateAndConfig::isReadable(CameraProperty::Id propId)
{
  return mpCamCapabilities->isReadable(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraStateAndConfig::isOnePushCapable(CameraProperty::Id propId)
{
  return mpCamCapabilities->isOnePushCapable(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraStateAndConfig::isOnOffCapable(CameraProperty::Id propId)
{
  return mpCamCapabilities->isOnOffCapable(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraStateAndConfig::isAutoCapable(CameraProperty::Id propId)
{
  return mpCamCapabilities->isAutoCapable(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraStateAndConfig::isManualCapable(CameraProperty::Id propId)
{
  return mpCamCapabilities->isManualCapable(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraStateAndConfig::isAbsoluteCapable(CameraProperty::Id propId)
{
  return mpCamCapabilities->isAbsoluteCapable(propId);
}



