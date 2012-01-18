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
#include "CameraConfiguration.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::dev::sensors::visual;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
/*
CameraConfiguration::CameraConfiguration(
                                          cv::VideoCapture& videoCapture,
                                          const std::string& configurationFileName,
                                          unsigned long guid
                                        )
{
  //create default capability file name
  std::string capability_file_name = CAMERAGRABBER_CAM_CAPABILITIES_FILENAME(guid);

  bool result = doInit(videoCapture,configurationFileName,capability_file_name);
  if (not result)
  {
    CEDAR_THROW
    (
      cedar::aux::exc::InitializationException,
      "[CameraConfiguration::CameraConfiguration] - Critical error in doInit()"
    );
  }
}
*/
//----------------------------------------------------------------------------------------------------------------------
CameraConfiguration::CameraConfiguration(
                                          cv::VideoCapture videoCapture,
                                          const std::string channelPrefix,
                                          const std::string configurationFileName,
                                          const std::string capabilitiesFileName
                                        )
:
mVideoCapture(videoCapture)
//mConfigurationFileName(configurationFileName),
//mCapabilitiesFileName(capabilitiesFileName)
//mCamCapabilities(NULL),
//mCamConfigFileStorage(NULL)
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraConfiguration::CameraConfiguration] Config-file: " << configurationFileName << std::endl;
    std::cout << "[CameraConfiguration::CameraConfiguration] Capability-file: " << capabilitiesFileName << std::endl;
  #endif

  mConfigurationFileName = configurationFileName;
  mCapabilitiesFileName = capabilitiesFileName;
  mChannelPrefix = channelPrefix;

  //mpCamCapabilities = NULL;
  //mpCamState = NULL;

  try
  {
    //create class for capabilities
    mpCamCapabilities = CameraCapabilitiesPtr(new CameraCapabilities(mCapabilitiesFileName));

    SupportedPropertiesSet supp_prop;
    supp_prop.clear();

    int num_properties = CameraProperty::type().list().size();
    for (int i=0; i<num_properties; i++)
    {
      CameraProperty::Id prop_id = CameraProperty::type().list().at(i).id(); //.id() init exception
      if (mpCamCapabilities->isSupported(prop_id))
      {
        supp_prop.insert(prop_id);
      }
    }
    //and for local storage of properties and settings
    //on intialization: cameraconfiguration will be restored
    mpCamState = CameraStatePtr(new CameraState (videoCapture,mChannelPrefix,supp_prop,mConfigurationFileName));
  }

  //all thrown exceptions catched by the shared_pointer structure in the CameraGrabber class
  //it is not necessary to catch any exception and pass them to CEDAR_THROW
  catch (...)
  {

    CEDAR_THROW
    (
      cedar::aux::exc::InitializationException,
      "[CameraConfiguration::CameraConfiguration] - Critical error in constructor"
    );
  }
}

//----------------------------------------------------------------------------------------------------------------------
CameraConfiguration::~CameraConfiguration()
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraConfiguration::~CameraConfiguration] Destroy class" << std::endl;
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
bool CameraConfiguration::writeConfiguration()
{
  //Camera Capabilities shouldn't be written

  //Save values from the camera
  return mpCamState->writeConfiguration();
}

//--------------------------------------------------------------------------------------------------------------------
int CameraConfiguration::getMinValue(CameraProperty::Id propId)
{
  return mpCamCapabilities->getMinValue(propId);
}

//--------------------------------------------------------------------------------------------------------------------
int CameraConfiguration::getMaxValue(CameraProperty::Id propId)
{
  return mpCamCapabilities->getMaxValue(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraConfiguration::isSupported(CameraProperty::Id propId)
{
  return mpCamCapabilities->isSupported(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraConfiguration::isReadable(CameraProperty::Id propId)
{
  return mpCamCapabilities->isReadable(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraConfiguration::isOnePushCapable(CameraProperty::Id propId)
{
  return mpCamCapabilities->isOnePushCapable(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraConfiguration::isOnOffCapable(CameraProperty::Id propId)
{
  return mpCamCapabilities->isOnOffCapable(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraConfiguration::isAutoCapable(CameraProperty::Id propId)
{
  return mpCamCapabilities->isAutoCapable(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraConfiguration::isManualCapable(CameraProperty::Id propId)
{
  return mpCamCapabilities->isManualCapable(propId);
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraConfiguration::isAbsoluteCapable(CameraProperty::Id propId)
{
  return mpCamCapabilities->isAbsoluteCapable(propId);
}

/*
double CameraConfiguration::getCameraProperty(
                          unsigned int channel,
                          CameraProperty::Id propId
                        )
{
}
                        
                        
bool CameraConfiguration::setCameraProperty(
                        unsigned int channel,
                        CameraProperty::Id propId,
                        double value
                      );
{
}
                      
*/                        


