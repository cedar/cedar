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

    File:        CameraCapabilities.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Header of the @em cedar::dev::sensors::visual::CameraCapabilities class.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "CameraCapabilities.h"
//#include "../../../auxiliaries/exceptions.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
//#include <boost/lexical_cast.hpp> //for reading values from the configfile

using namespace cedar::dev::sensors::visual;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
CameraCapabilities::CameraCapabilities(
                                        const std::string& configFileName
                                      )
:
ConfigurationInterface(configFileName)
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraCapabilities::CameraCapabilities] Config-file: " << configFileName << std::endl;
  #endif

  //for all known properties create a capability structure and store it
  for (unsigned int i=0; i<CameraProperty::type().list().size(); i++)
  {
    CameraPropertyCapability cap;
    cap.propId = CameraProperty::type().list().at(i).id();
    mCamProperties.push_back(cap);
  }

  if (not declareParameters())
  {
    // throwing an exception prevents main-grabber class to clean up => catch this exception
    std::string err = "[CameraCapabilities::CameraCapabilities] - Critical error in constructor";
    CEDAR_THROW
    (
      std::cout << err << std::endl;
      cedar::aux::exc::InitializationException,
      err
    );
  }
  readOrDefaultConfiguration();

  //set properties which are not supported by cedar/opencv to false
  //i.e. ignore config-file values
  for (unsigned int i=0; i<mCamProperties.size(); i++)
  {
    if (    (mCamProperties.at(i).propId == CameraProperty::PROP_TRIGGER)
         || (mCamProperties.at(i).propId == CameraProperty::PROP_TRIGGER_DELAY)
       )
    {
      mCamProperties.at(i).is_supported = false;
      mCamProperties.at(i).is_readable = false;
    }
  }

  //write properties which aren't supported to the properties file
  writeConfiguration();
}

//----------------------------------------------------------------------------------------------------------------------
CameraCapabilities::~CameraCapabilities()
{
  #ifdef DEBUG_CAMERAGRABBER
    std::cout << "[CameraCapabilities::~CameraCapabilities] Destroy class" << std::endl;
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool CameraCapabilities::declareParameters()
{
  bool result = true;

  //prepare all parameters for configuration interface
  for (unsigned int i=0; i<mCamProperties.size(); i++)
  {
    //CameraPropertyCapability& prop = mCamCapabilities.at(i);

    //get id of property number i
    //CameraProperty::Id id = CameraProperty::type().list().at(i).id();
    CameraProperty::Id id = mCamProperties.at(i).propId;

    //property type and name
    //propId = id;
    std::string prop_name = CameraProperty::type().get(id).name() + "_";
    std::string prop_cap;

    //property capability one by one
    prop_cap = prop_name + "max_value";
    result = (addParameter(&mCamProperties.at(i).max_value, prop_cap, 255) == CONFIG_SUCCESS) && result;

    prop_cap = prop_name + "min_value";
    result = (addParameter(&mCamProperties.at(i).min_value, prop_cap, 0) == CONFIG_SUCCESS) && result;

    prop_cap = prop_name + "is_supported";
    result = (addParameter(&mCamProperties.at(i).is_supported, prop_cap, true) == CONFIG_SUCCESS) && result;

    prop_cap = prop_name + "is_readable";
    result = (addParameter(&mCamProperties.at(i).is_readable, prop_cap, true) == CONFIG_SUCCESS) && result;

    prop_cap = prop_name + "is_one_push_capable";
    result = (addParameter(&mCamProperties.at(i).is_one_push_capable, prop_cap, false) == CONFIG_SUCCESS) && result;

    prop_cap = prop_name + "is_on_off_capable";
    result = (addParameter(&mCamProperties.at(i).is_on_off_capable, prop_cap, false) == CONFIG_SUCCESS) && result;

    prop_cap = prop_name + "is_auto_capable";
    result = (addParameter(&mCamProperties.at(i).is_auto_capable, prop_cap, false) == CONFIG_SUCCESS) && result;

    prop_cap = prop_name + "is_manual_capable";
    result = (addParameter(&mCamProperties.at(i).is_manual_capable, prop_cap, true) == CONFIG_SUCCESS) && result;

    prop_cap = prop_name + "is_absolute_capable";
    result = (addParameter(&mCamProperties.at(i).is_absolute_capable, prop_cap, false) == CONFIG_SUCCESS) && result;

  }
  return result;
}

//--------------------------------------------------------------------------------------------------------------------
CameraPropertyCapability&  CameraCapabilities::getCapabilities(CameraProperty::Id propId)
{
  int prop = 0;
  int max_prop = mCamProperties.size();

  while ((mCamProperties.at(prop).propId != propId)&& (prop<max_prop))
//  CameraProperty::Id act_propId = mCamProperties.at(prop).propId;
//  while ((act_propId != propId)&& (prop<max_prop))
  {
//    act_propId = mCamProperties.at(prop).propId;
    prop++;
  }

  //check if found, normally all properties are managed
  if (mCamProperties.at(prop).propId != propId)
  {
    //if not found, this is a critical error
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,
                "[CameraCapabilities::getCapabilities] ERROR: Property not found!");
  }

  return mCamProperties.at(prop);
}

//--------------------------------------------------------------------------------------------------------------------
int CameraCapabilities::getMinValue(CameraProperty::Id propId)
{
  return getCapabilities(propId).min_value;
}

//--------------------------------------------------------------------------------------------------------------------
int CameraCapabilities::getMaxValue(CameraProperty::Id propId)
{
  return getCapabilities(propId).max_value;
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraCapabilities::isSupported(CameraProperty::Id propId)
{
  return getCapabilities(propId).is_supported;
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraCapabilities::isReadable(CameraProperty::Id propId)
{
  return getCapabilities(propId).is_readable;
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraCapabilities::isOnePushCapable(CameraProperty::Id propId)
{
  return getCapabilities(propId).is_one_push_capable;
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraCapabilities::isOnOffCapable(CameraProperty::Id propId)
{
  return getCapabilities(propId).is_on_off_capable;
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraCapabilities::isAutoCapable(CameraProperty::Id propId)
{
  return getCapabilities(propId).is_auto_capable;
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraCapabilities::isManualCapable(CameraProperty::Id propId)
{
  return getCapabilities(propId).is_manual_capable;
}

//--------------------------------------------------------------------------------------------------------------------
bool CameraCapabilities::isAbsoluteCapable(CameraProperty::Id propId)
{
  return getCapabilities(propId).is_absolute_capable;
}

