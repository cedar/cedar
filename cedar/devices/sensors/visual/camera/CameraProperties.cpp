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
#include <boost/lexical_cast.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraProperties::CameraProperties
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

    // get name of property and create it with default value
    std::string prop_name = cedar::dev::sensors::visual::CameraProperty::type().list().at(i).prettyString();

    //create property with default parameter
    cedar::dev::sensors::visual::CamPropertyPtr p_prop(new cedar::dev::sensors::visual::CamProperty(prop_id,prop_name));

    //add to properties-list
    mpPropertiesList->insert(std::pair<unsigned int, CamPropertyPtr>(prop_id,p_prop));
    properties->addConfigurableChild(prop_name, p_prop);

    QObject::connect(
                      p_prop.get(),
                      SIGNAL(propertyValueChanged(cedar::dev::sensors::visual::CameraProperty::Id,double)),
                      this,
                      SLOT(propertyValueChanged(cedar::dev::sensors::visual::CameraProperty::Id,double))
                    );

    QObject::connect(
                      p_prop.get(),
                      SIGNAL(propertyModeChanged(
                                                  cedar::dev::sensors::visual::CameraProperty::Id,
                                                  cedar::dev::sensors::visual::CameraPropertyMode::Id
                                                )),
                      this,
                      SLOT(propertyModeChanged(
                                                cedar::dev::sensors::visual::CameraProperty::Id,
                                                cedar::dev::sensors::visual::CameraPropertyMode::Id
                                              ))
                    );
  }
}

cedar::dev::sensors::visual::CameraProperties::~CameraProperties()
{
}

//----------------------------------------------------------------------------------------------------------------------
// slots
//----------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraProperties::propertyValueChanged
(
  cedar::dev::sensors::visual::CameraProperty::Id propertyId,
  double newValue
)
{
   std::cout << "[propertyValueChanged] prop :" << propertyId << " to " << newValue << std::endl;

  // get the used CamProperty
  cedar::dev::sensors::visual::CamPropertyPtr p_prop = (*mpPropertiesList)[propertyId];

  // and the mode
  cedar::dev::sensors::visual::CameraPropertyMode::Id prop_mode_id = p_prop->getMode();

  //change the property value on the camera only if this property is mode manual
  if (prop_mode_id == cedar::dev::sensors::visual::CameraPropertyMode::MANUAL)
  {
    setPropertyToCamera(propertyId,newValue);
  }

  // in modes BACKEND_DEFAULT and AUTO it is enough to update the values for displaying
}

//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraProperties::propertyModeChanged
(
  cedar::dev::sensors::visual::CameraProperty::Id propertyId,
  cedar::dev::sensors::visual::CameraPropertyMode::Id newMode
)
{

  std::cout << "[propertyModeChanged] prop :" << propertyId << " to " << newMode << std::endl;

  // get the used CamProperty
  cedar::dev::sensors::visual::CamPropertyPtr p_prop = (*mpPropertiesList)[propertyId];

  // get values from the camera depends on mode
  if (newMode == cedar::dev::sensors::visual::CameraPropertyMode::MANUAL)
  {
    double value = getPropertyFromCamera(propertyId);
    p_prop->setValue(value);
  }

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::visual::CameraProperties::setVideoCaptureObject(cv::VideoCapture capture)
{
  mVideoCapture = capture;
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraProperties::setProperty
(
  cedar::dev::sensors::visual::CameraProperty::Id propertyId,
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
bool cedar::dev::sensors::visual::CameraProperties::setPropertyMode
(
  cedar::dev::sensors::visual::CameraProperty::Id propertyId,
  cedar::dev::sensors::visual::CameraPropertyMode::Id mode
)
{
  cedar::dev::sensors::visual::CamPropertyPtr p_prop = (*mpPropertiesList)[propertyId];
  p_prop->setMode(mode);
  return (p_prop->getMode() == mode);
}

//--------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraPropertyMode::Id cedar::dev::sensors::visual::CameraProperties::getPropertyMode
(
  cedar::dev::sensors::visual::CameraProperty::Id propertyId
)
{
  // get the used CamProperty
  cedar::dev::sensors::visual::CamPropertyPtr p_prop = (*mpPropertiesList)[propertyId];

  return p_prop->getMode();
}


//--------------------------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraProperties::getProperty
(
  cedar::dev::sensors::visual::CameraProperty::Id propertyId
)
{
  cedar::dev::sensors::visual::CamPropertyPtr p_prop = (*mpPropertiesList)[propertyId];

  // only return this value, if set to manual or if backend-default value
  if (!(p_prop->isSupported()))
  {
    return CAMERA_PROPERTY_NOT_SUPPORTED;
  }
  if (p_prop->getMode() == cedar::dev::sensors::visual::CameraPropertyMode::AUTO)
  {
    return CAMERA_PROPERTY_MODE_AUTO;
  }

  if (p_prop->getMode() == cedar::dev::sensors::visual::CameraPropertyMode::BACKEND_DEFAULT)
  {
    return CAMERA_PROPERTY_MODE_DEFAULT;
  }

  return p_prop->getValue();
}


//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraProperties::setPropertyToCamera(unsigned int propertyId, double value)
{
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
    if (this->getPropertyFromCamera(propertyId) == value)
    {
      return true;
    }
  }

  std::string prop_name = cedar::dev::sensors::visual::CameraProperty::type().get(propertyId).prettyString();
  cedar::aux::LogSingleton::getInstance()->warning
                                           (
                                             "property " + prop_name
                                             + " couldn't set to " + boost::lexical_cast<std::string>(value),
                                             "cedar::dev::sensors::visual::CameraProperties::setPropertyToCamera()"
                                           );
  return false;
}


//--------------------------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraProperties::getPropertyFromCamera(unsigned int propertyId)
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

//--------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraPropertyMode::Id cedar::dev::sensors::visual::CameraProperties::getMode
(
  cedar::dev::sensors::visual::CameraProperty::Id propertyId
)
{
  return (*mpPropertiesList)[propertyId]->getMode();
}


//--------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraProperties::setMode
(
  cedar::dev::sensors::visual::CameraProperty::Id propertyId,
  cedar::dev::sensors::visual::CameraPropertyMode::Id modeId
)
{
  (*mpPropertiesList)[propertyId]->setMode(modeId);
}

//--------------------------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraProperties::getMinValue(CameraProperty::Id propertyId)
{
  return (*mpPropertiesList)[propertyId]->getMinValue();
}

//--------------------------------------------------------------------------------------------------------------------
double cedar::dev::sensors::visual::CameraProperties::getMaxValue(CameraProperty::Id propertyId)
{
  return (*mpPropertiesList)[propertyId]->getMaxValue();
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraProperties::isSupported(CameraProperty::Id propertyId)
{
  return (*mpPropertiesList)[propertyId]->isSupported();
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraProperties::isReadable(CameraProperty::Id propertyId)
{
  return (*mpPropertiesList)[propertyId]->isReadable();
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraProperties::isAutoCapable(CameraProperty::Id propertyId)
{
  return (*mpPropertiesList)[propertyId]->isAutoCapable();
}

//--------------------------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::CameraProperties::isManualCapable(CameraProperty::Id propertyId)
{
  return (*mpPropertiesList)[propertyId]->isManualCapable();
}
