/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/devices/sensors/camera/exceptions.h"
#include "cedar/devices/sensors/camera/enums/IsoSpeed.h"
#include "cedar/devices/sensors/camera/enums/Property.h"
#include "cedar/devices/sensors/camera/enums/VideoMode.h"
#include "cedar/devices/sensors/camera/enums/FrameRate.h"
#include "cedar/devices/sensors/camera/enums/Setting.h"
#include "cedar/devices/sensors/camera/enums/Decoding.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/lexical_cast.hpp>
#endif
#include <utility>
#include <string>

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
    mPropertiesList.insert(std::pair<unsigned int, CamPropertyPtr>(prop_id,p_prop));
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

void cedar::dev::sensors::camera::Properties::blockSignals(bool block)
{
  int num_properties = cedar::dev::sensors::camera::Property::type().list().size();
  for (int i=0; i<num_properties; i++)
  {
    cedar::dev::sensors::camera::Property::Id prop_id = cedar::dev::sensors::camera::Property::type().list().at(i).id();
    mPropertiesList[prop_id]->blockSignals(block);
  }
}


void cedar::dev::sensors::camera::Properties::propertyValueChanged
(
  cedar::dev::sensors::camera::Property::Id propertyId,
  double newValue
)
{
  // get the used CamProperty
  cedar::dev::sensors::camera::CamPropertyPtr p_prop = mPropertiesList[propertyId];

  // and the mode
  cedar::dev::sensors::camera::PropertyMode::Id prop_mode_id = p_prop->getMode();

  //change the property value on the camera only if the mode of this property is set to manual
  if (prop_mode_id == cedar::dev::sensors::camera::PropertyMode::MANUAL)
  {
    try
    {
      setPropertyToCamera(propertyId,newValue);
    }
    catch(cedar::dev::sensors::camera::PropertyNotSetException& e)
    {
      cedar::aux::LogSingleton::getInstance()->warning
                                               (
                                                 e.getMessage(),
                                                 "cedar::dev::sensors::camera::Properties::propertyValueChanged()"
                                                );
    }
  }
  // in modes BACKEND_DEFAULT and AUTO it is enough to update the values for displaying
}


void cedar::dev::sensors::camera::Properties::propertyModeChanged
(
  cedar::dev::sensors::camera::Property::Id propertyId,
  cedar::dev::sensors::camera::PropertyMode::Id //newMode
)
{
  // newMode is automatically set in the backend, because of the use as enum-parameter

  // get the used CamProperty
  cedar::dev::sensors::camera::CamPropertyPtr p_prop = mPropertiesList[propertyId];

  // get actual values from the camera
  double value = getPropertyFromCamera(propertyId);
  p_prop->setValue(value);

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::camera::Properties::setVideoCaptureObject(cv::VideoCapture capture)
{
  mVideoCapture = capture;
}


void cedar::dev::sensors::camera::Properties::setDefaultValue
(
  cedar::dev::sensors::camera::Property::Id propertyId,
  double value
)
{
  mPropertiesList[propertyId]->setDefaultValue(value);
}


void cedar::dev::sensors::camera::Properties::setProperty
(
  cedar::dev::sensors::camera::Property::Id propertyId,
  double value
)
{
  // check if new value is different from old
  if (mPropertiesList[propertyId]->getValue() == value)
  {
    return;
  }

  // set value
  mPropertiesList[propertyId]->setValue(value);

  // check if value is set to the right value
  if (mPropertiesList[propertyId]->getValue() != value)
  {
    std::string prop_name = Property::type().get(propertyId).prettyString();
    std::string msg =  "Could not set Property " + prop_name + " to value " + cedar::aux::toString(value);
    CEDAR_THROW(cedar::dev::sensors::camera::PropertyNotSetException,msg);
  }
}


void cedar::dev::sensors::camera::Properties::setPropertyMode
(
  cedar::dev::sensors::camera::Property::Id propertyId,
  cedar::dev::sensors::camera::PropertyMode::Id mode
)
{
  cedar::dev::sensors::camera::CamPropertyPtr p_prop = mPropertiesList[propertyId];
  p_prop->setMode(mode);
  if (p_prop->getMode() != mode)
  {
    std::string prop_name = Property::type().get(propertyId).prettyString();
    std::string prop_mode = PropertyMode::type().get(mode).prettyString();
    std::string msg =  "Could not set Property " + prop_name + " to mode " + prop_mode;
    CEDAR_THROW(cedar::dev::sensors::camera::PropertyNotSetException,msg);
  }
}


cedar::dev::sensors::camera::PropertyMode::Id cedar::dev::sensors::camera::Properties::getPropertyMode
(
  cedar::dev::sensors::camera::Property::Id propertyId
)
{
  return (mPropertiesList[propertyId])->getMode();
}


double cedar::dev::sensors::camera::Properties::getProperty
(
  cedar::dev::sensors::camera::Property::Id propertyId
)
{
  //!@ todo: throw exceptions instead of returning constants for special values
  cedar::dev::sensors::camera::CamPropertyPtr p_prop = mPropertiesList[propertyId];

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


void cedar::dev::sensors::camera::Properties::setPropertyToCamera(unsigned int propertyId, double value)
{
  bool result = false;
  mpVideoCaptureLock->lockForWrite();
  if (mVideoCapture.isOpened())
  {
    //set only real values or CAMERA_PROPERTY_MODE_AUTO
    if ((value != CAMERA_PROPERTY_NOT_SUPPORTED) || (value != CAMERA_PROPERTY_MODE_DEFAULT))
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
      std::string prop_name = cedar::dev::sensors::camera::Property::type().get(propertyId).prettyString();
      std::string msg = "Property " + prop_name + " couldn't set to " + cedar::aux::toString(value)
                           + ". New value: " + cedar::aux::toString(set_value);
      CEDAR_THROW(cedar::dev::sensors::camera::PropertyNotSetException,msg)
    }
  }
}


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
  mPropertiesList[propertyId]->disable();
}


void cedar::dev::sensors::camera::Properties::enableProperty(cedar::dev::sensors::camera::Property::Id propertyId)
{
  mPropertiesList[propertyId]->enable();
}


cedar::dev::sensors::camera::PropertyMode::Id cedar::dev::sensors::camera::Properties::getMode
(
  cedar::dev::sensors::camera::Property::Id propertyId
)
{
  return mPropertiesList[propertyId]->getMode();
}


void cedar::dev::sensors::camera::Properties::setMode
(
  cedar::dev::sensors::camera::Property::Id propertyId,
  cedar::dev::sensors::camera::PropertyMode::Id modeId
)
{
  mPropertiesList[propertyId]->setMode(modeId);
}


double cedar::dev::sensors::camera::Properties::getMinValue(Property::Id propertyId)
{
  return mPropertiesList[propertyId]->getMinValue();
}


double cedar::dev::sensors::camera::Properties::getMaxValue(Property::Id propertyId)
{
  return mPropertiesList[propertyId]->getMaxValue();
}


bool cedar::dev::sensors::camera::Properties::isSupported(Property::Id propertyId)
{
  return mPropertiesList[propertyId]->isSupported();
}


bool cedar::dev::sensors::camera::Properties::isAutoCapable(Property::Id propertyId)
{
  return mPropertiesList[propertyId]->isAutoCapable();
}


bool cedar::dev::sensors::camera::Properties::isManualCapable(Property::Id propertyId)
{
  return mPropertiesList[propertyId]->isManualCapable();
}


cedar::dev::sensors::camera::CamPropertyPtr
  cedar::dev::sensors::camera::Properties::getPropertyObject(Property::Id propertyId)
{
  return mPropertiesList[propertyId];
}
