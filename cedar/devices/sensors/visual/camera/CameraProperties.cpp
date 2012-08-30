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

    File:        CameraBackendType.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Implementation for the cedar::dev::sensors::visual::CameraDevice class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/camera/CameraProperties.h"

#include "cedar/devices/sensors/visual/camera/enums/CameraIsoSpeed.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraProperty.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraVideoMode.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraFrameRate.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraSetting.h"
#include "cedar/devices/sensors/visual/camera/enums/DeBayerFilter.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraProperties::CameraProperties(cedar::aux::Configurable *pOwner)
:
cedar::aux::Configurable()
{
  // create new node for properties in the configuration
  cedar::aux::ConfigurablePtr properties(new cedar::aux::Configurable());
  pOwner->addConfigurableChild("properties", properties);

  mpPropertiesList = cedar::dev::sensors::visual::CameraProperties::CamPropertiesMapPtr
                     (
                       new cedar::dev::sensors::visual::CameraProperties::CamPropertiesMap
                     );

  // Add all properties to the properties-list
  int num_properties = cedar::dev::sensors::visual::CameraProperty::type().list().size();
  for (int i=0; i<num_properties; i++)
  {
    cedar::dev::sensors::visual::CameraProperty::Id prop_id
      = cedar::dev::sensors::visual::CameraProperty::type().list().at(i).id();

    // ignore unsupported features
    bool is_supported = true;
    bool is_readable = true;

    if (    (prop_id == cedar::dev::sensors::visual::CameraProperty::PROP_TRIGGER)
         || (prop_id == cedar::dev::sensors::visual::CameraProperty::PROP_TRIGGER_DELAY)
       )
    {
      is_supported = false;
      is_readable = false;
    }

    //get name of property and create it with default value
    std::string prop_name = cedar::dev::sensors::visual::CameraProperty::type().list().at(i).prettyString();

    cedar::dev::sensors::visual::CamPropertyPtr p_prop(new cedar::dev::sensors::visual::CamProperty
                                                         (
                                                           prop_id,
                                                           prop_name,
                                                           (double) INT_MIN,
                                                           (double) INT_MAX,
                                                           128.f,
                                                           is_supported,
                                                           is_readable,
                                                           false,
                                                           true
                                                         )
                                                      );

    mpPropertiesList->insert(std::pair<unsigned int, CamPropertyPtr>(prop_id,p_prop));
    properties->addConfigurableChild(prop_name, p_prop);

  }
}

cedar::dev::sensors::visual::CameraProperties::~CameraProperties()
{
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
//cedar::dev::sensors::visual::CameraPropertyMap& cedar::dev::sensors::visual::CameraProperties::getPropertiesList()
//{
//  return mPropertiesList;
//}

//void cedar::dev::sensors::visual::CameraProperties::setPropertiesList
//( 
  //cedar::dev::sensors::visual::CameraPropertyMap propertiesList
//)
//{
  //mPropertiesList.clear();
  //mPropertiesList = propertiesList;
//}


//bool cedar::dev::sensors::visual::CameraProperties::setProperty(unsigned int property_id,double value)
//{
//  //set the value with respect to concurrent access
//
//  //get old value
//  //set new value only if it is different
//  //check if value is set to the right value
//  //update setting-mode
//  //update property-mode
//  return false;
//}
//
//double cedar::dev::sensors::visual::CameraProperties::getProperty(unsigned int property)
//{
//
//}






bool cedar::dev::sensors::visual::CameraProperties::setPropertyRawValue(unsigned int property_id,double value)
{
  //lock videocapture

  //set property

  //unlock videocapture

  return false;
}


double cedar::dev::sensors::visual::CameraProperties::getPropertyRawValue(unsigned int property_id)
{
  return 0.0f;
}


cedar::dev::sensors::visual::CamPropertyPtr cedar::dev::sensors::visual::CameraProperties::getPropertyPtr
(
  CameraProperty::Id propId
)
{
  //return mPropertiesList.find(static_cast<unsigned int>(propId))->second;

  //create empty CamProperty
  //cedar::dev::sensors::visual::CamPropertyPtr val(new cedar::dev::sensors::visual::CamProperty());
  //return val;
}


cedar::dev::sensors::visual::CameraPropertyMode::Id cedar::dev::sensors::visual::CameraProperties::getMode
(
  cedar::dev::sensors::visual::CameraProperty::Id propId
)
{

}

void cedar::dev::sensors::visual::CameraProperties::setMode
(
  cedar::dev::sensors::visual::CameraProperty::Id propId,
  cedar::dev::sensors::visual::CameraPropertyMode::Id mode
)
{

}

double cedar::dev::sensors::visual::CameraProperties::getMinValue(CameraProperty::Id propId)
{

}

double cedar::dev::sensors::visual::CameraProperties::getMaxValue(CameraProperty::Id propId)
{

}

bool cedar::dev::sensors::visual::CameraProperties::isSupported(CameraProperty::Id propId)
{

}

bool cedar::dev::sensors::visual::CameraProperties::isReadable(CameraProperty::Id propId)
{

}

/*
bool cedar::dev::sensors::visual::CameraProperties::isOnOffCapable(CameraProperty::Id propId)
{

}
*/

bool cedar::dev::sensors::visual::CameraProperties::isAutoCapable(CameraProperty::Id propId)
{

}

bool cedar::dev::sensors::visual::CameraProperties::isManualCapable(CameraProperty::Id propId)
{

}
