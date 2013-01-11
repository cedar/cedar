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

    Description:  Implementation for the cedar::dev::sensors::camera::Device class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/Properties.h"
#include "cedar/devices/sensors/camera/enums/IsoSpeed.h"
#include "cedar/devices/sensors/camera/enums/Property.h"
#include "cedar/devices/sensors/camera/enums/VideoMode.h"
#include "cedar/devices/sensors/camera/enums/FrameRate.h"
#include "cedar/devices/sensors/camera/enums/Setting.h"
#include "cedar/devices/sensors/camera/enums/Decoding.h"

// SYSTEM INCLUDES
#include <boost/lexical_cast.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::Properties::Properties
(
  cedar::aux::Configurable *pOwner,
  cv::VideoCapture capture,
  QReadWriteLock* lock
)
:
cedar::aux::Configurable(),
mVideoCapture(capture),
mpVideoCaptureLock(lock)
{
  // create new node for properties in the configuration
  cedar::aux::ConfigurablePtr properties(new cedar::aux::Configurable());
  pOwner->addConfigurableChild("properties", properties);

  mpPropertiesList = cedar::dev::sensors::camera::Properties::CamPropertiesMapPtr
                     (
                       new cedar::dev::sensors::camera::Properties::CamPropertiesMap
                     );

  // Add all properties to the properties-list
  int num_properties = cedar::dev::sensors::camera::Property::type().list().size();
  for (int i=0; i<num_properties; i++)
  {
    cedar::dev::sensors::camera::Property::Id prop_id
      = cedar::dev::sensors::camera::Property::type().list().at(i).id();

    // get name of property and create it with default value
    std::string prop_name = cedar::dev::sensors::camera::Property::type().list().at(i).prettyString();

    //create property with default parameter
    cedar::dev::sensors::camera::CamPropertyPtr p_prop(new cedar::dev::sensors::camera::CamProperty(prop_id,prop_name));

    //add to properties-list
    mpPropertiesList->insert(std::pair<unsigned int, CamPropertyPtr>(prop_id,p_prop));
    properties->addConfigurableChild(prop_name, p_prop);

    QObject::connect(
                      p_prop.get(),
                      SIGNAL(propertyValueChanged(cedar::dev::sensors::camera::Property::Id,double)),
                      this,
                      SLOT(propertyValueChanged(cedar::dev::sensors::camera::Property::Id,double))
                    );

    QObject::connect(
                      p_prop.get(),
                      SIGNAL(propertyModeChanged(
                                                  cedar::dev::sensors::camera::Property::Id,
                                                  cedar::dev::sensors::camera::PropertyMode::Id
                                                )),
                      this,
                      SLOT(propertyModeChanged(
                                                cedar::dev::sensors::camera::Property::Id,
                                                cedar::dev::sensors::camera::PropertyMode::Id
                                              ))
                    );
  }
}

cedar::dev::sensors::camera::Properties::~Properties()
{
}

//----------------------------------------------------------------------------------------------------------------------
// slots
//----------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::Properties::propertyValueChanged
(
  cedar::dev::sensors::camera::Property::Id propertyId,
  double newValue
)
{
  // std::cout << "[propertyValueChanged] prop :" << propertyId << " to " << newValue << std::endl;

  // get the used CamProperty
  cedar::dev::sensors::camera::CamPropertyPtr p_prop = (*mpPropertiesList)[propertyId];

  // and the mode
  cedar::dev::sensors::camera::PropertyMode::Id prop_mode_id = p_prop->getMode();

  //change the property value on the camera only if the mode of this property is set to manual
  if (prop_mode_id == cedar::dev::sensors::camera::PropertyMode::MANUAL)
  {
    setPropertyToCamera(propertyId,newValue);
  }

  // in modes BACKEND_DEFAULT and AUTO it is enough to update the values for displaying
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::Properties::propertyModeChanged
(
  cedar::dev::sensors::camera::Property::Id propertyId,
  cedar::dev::sensors::camera::PropertyMode::Id newMode
)
{

  //std::cout << "[propertyModeChanged] prop :" << propertyId << " to " << newMode << std::endl;

  // get the used CamProperty
  cedar::dev::sensors::camera::CamPropertyPtr p_prop = (*mpPropertiesList)[propertyId];

  // get values from the camera depends on mode
  if (newMode == cedar::dev::sensors::camera::PropertyMode::MANUAL)
  {
    double value = getPropertyFromCamera(propertyId);
    p_prop->setValue(value);
  }

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::camera::Properties::setVideoCaptureObject(cv::VideoCapture capture)
{
  mVideoCapture = capture;
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::Properties::setDefaultValue
(
  cedar::dev::sensors::camera::Property::Id propertyId,
  double value
)
{
  (*mpPropertiesList)[propertyId]->setDefaultValue(value);
}
//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::camera::Properties::setProperty
(
  cedar::dev::sensors::camera::Property::Id propertyId,
  double value
)
{
  // check if new value is different from old
  if ((*mpPropertiesList)[propertyId]->getValue() == value)
  {
    return true;
  }

  // set value
  (*mpPropertiesList)[propertyId]->setValue(value);

  // check if value is set to the right value

  if ((*mpPropertiesList)[propertyId]->getValue() == value)
  {
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::camera::Properties::setPropertyMode
(
  cedar::dev::sensors::camera::Property::Id propertyId,
  cedar::dev::sensors::camera::PropertyMode::Id mode
)
{
  cedar::dev::sensors::camera::CamPropertyPtr p_prop = (*mpPropertiesList)[propertyId];
  p_prop->setMode(mode);
  return (p_prop->getMode() == mode);
}

//--------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::PropertyMode::Id cedar::dev::sensors::camera::Properties::getPropertyMode
(
  cedar::dev::sensors::camera::Property::Id propertyId
)
{
  // get the used CamProperty
  cedar::dev::sensors::camera::CamPropertyPtr p_prop = (*mpPropertiesList)[propertyId];

  return p_prop->getMode();
}


//--------------------------------------------------------------------------------------------------------------------
double cedar::dev::sensors::camera::Properties::getProperty
(
  cedar::dev::sensors::camera::Property::Id propertyId
)
{
  cedar::dev::sensors::camera::CamPropertyPtr p_prop = (*mpPropertiesList)[propertyId];

  // only return this value, if set to manual or if backend-default value
  if (!(p_prop->isSupported()))
  {
    return CAMERA_PROPERTY_NOT_SUPPORTED;
  }
  if (p_prop->getMode() == cedar::dev::sensors::camera::PropertyMode::AUTO)
  {
    return CAMERA_PROPERTY_MODE_AUTO;
  }

  if (p_prop->getMode() == cedar::dev::sensors::camera::PropertyMode::BACKEND_DEFAULT)
  {
    return CAMERA_PROPERTY_MODE_DEFAULT;
  }

  return p_prop->getValue();
}


//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::camera::Properties::setPropertyToCamera(unsigned int propertyId, double value)
{
  std::string prop_name = cedar::dev::sensors::camera::Property::type().get(propertyId).prettyString();
/*  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             "set property " + prop_name
                                             + " to " + boost::lexical_cast<std::string>(value),
                                             "cedar::dev::sensors::camera::Properties::setPropertyToCamera()"
                                           );*/

  bool result = false;
  mpVideoCaptureLock->lockForWrite();
  if (mVideoCapture.isOpened())
  {
    //set only real values or CAMERA_PROPERTY_MODE_AUTO
    if (    (value != CAMERA_PROPERTY_NOT_SUPPORTED)
         || (value != CAMERA_PROPERTY_MODE_DEFAULT)
       )
    {
      result = mVideoCapture.set(propertyId, value);
    }
  }
  mpVideoCaptureLock->unlock();

  // check if set
  if (result)
  {
    double set_value = this->getPropertyFromCamera(propertyId);
    if ( set_value != value)
    {
      cedar::aux::LogSingleton::getInstance()->warning
                                             (
                                               "property " + prop_name
                                               + " couldn't set to " + boost::lexical_cast<std::string>(value)
                                               + ". New value: " + boost::lexical_cast<std::string>(set_value),
                                               "cedar::dev::sensors::camera::Properties::setPropertyToCamera()"
                                             );
      return false;
    }
  }

  return true;
}


//--------------------------------------------------------------------------------------------------------------------
double cedar::dev::sensors::camera::Properties::getPropertyFromCamera(unsigned int propertyId)
{
  double result = -1;
  mpVideoCaptureLock->lockForWrite();
  if (mVideoCapture.isOpened())
  {
    result = mVideoCapture.get(propertyId);
  }
  mpVideoCaptureLock->unlock();
  return result;
}

void cedar::dev::sensors::camera::Properties::disableProperty(cedar::dev::sensors::camera::Property::Id propertyId)
{
  (*mpPropertiesList)[propertyId]->disable();
}
void cedar::dev::sensors::camera::Properties::enableProperty(cedar::dev::sensors::camera::Property::Id propertyId)
{
  (*mpPropertiesList)[propertyId]->enable();
}

//--------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::PropertyMode::Id cedar::dev::sensors::camera::Properties::getMode
(
  cedar::dev::sensors::camera::Property::Id propertyId
)
{
  return (*mpPropertiesList)[propertyId]->getMode();
}


//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::Properties::setMode
(
  cedar::dev::sensors::camera::Property::Id propertyId,
  cedar::dev::sensors::camera::PropertyMode::Id modeId
)
{
  (*mpPropertiesList)[propertyId]->setMode(modeId);
}

//--------------------------------------------------------------------------------------------------------------------
double cedar::dev::sensors::camera::Properties::getMinValue(Property::Id propertyId)
{
  return (*mpPropertiesList)[propertyId]->getMinValue();
}

//--------------------------------------------------------------------------------------------------------------------
double cedar::dev::sensors::camera::Properties::getMaxValue(Property::Id propertyId)
{
  return (*mpPropertiesList)[propertyId]->getMaxValue();
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::camera::Properties::isSupported(Property::Id propertyId)
{
  return (*mpPropertiesList)[propertyId]->isSupported();
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::camera::Properties::isAutoCapable(Property::Id propertyId)
{
  return (*mpPropertiesList)[propertyId]->isAutoCapable();
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::camera::Properties::isManualCapable(Property::Id propertyId)
{
  return (*mpPropertiesList)[propertyId]->isManualCapable();
}

//--------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::CamPropertyPtr
  cedar::dev::sensors::camera::Properties::getPropertyClass(Property::Id propertyId)
{
  return (*mpPropertiesList)[propertyId];
}
