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

    File:        Properties.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Header for the cedar::dev::sensors::camera::Properties class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_CAMERA_PROPERTIES_H
#define CEDAR_DEV_SENSORS_CAMERA_PROPERTIES_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/namespace.h"
#include "cedar/devices/sensors/camera/enums/Setting.h"
#include "cedar/devices/sensors/camera/enums/PropertyMode.h"
#include "cedar/devices/sensors/camera/enums/Property.h"
#include "cedar/devices/sensors/camera/CamProperty.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/ObjectMapParameterTemplate.h"


// SYSTEM INCLUDES
#include <QObject>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>
#include <map>


/*!@brief This class manages all properties and their capabilities of one camerachannel
 *
 * A property is a specific property from the cv::VideoCapture class
 */
class cedar::dev::sensors::camera::Properties
:
public QObject,
public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------

  /*! Store all properties in this map.
   *
   *   It maps from the OpenCV Property-ID (from cv::VideoCapture.get()/set() methods)
   *   to the appropriate object that handles the property
   */
  typedef std::map<unsigned int, CamPropertyPtr> CamPropertiesMap;

  //--------------------------------------------------------------------------------------------------------------------
  // signals and slots
  //--------------------------------------------------------------------------------------------------------------------

  Q_OBJECT

protected slots:

  //!@brief This signal is emitted, when the value of the property has changed.
  void propertyValueChanged(cedar::dev::sensors::camera::Property::Id propertyId, double newValue);

  //!@brief This signal is emitted, when the mode of the property has changed.
  void propertyModeChanged
  (
    cedar::dev::sensors::camera::Property::Id propertyId,
    cedar::dev::sensors::camera::PropertyMode::Id newMode
  );


signals:

  //! @brief This signal is emitted, when a setting has changed.
  void propertiesChanged();

public:

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Properties(cedar::aux::Configurable *pOwner, cv::VideoCapture capture, QReadWriteLock* lock);

  //!@brief Destructor
  ~Properties();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief Set the value to the cv::VideoCapture object with respect to capabilities and settings
   *
   *  @param propertyId The Id of the property as cedar::dev::sensors::camera::Property::Id
   *  @param value The value to set the property
   *  @throw cedar::dev::sensors::camera::PropertyNotSetException Thrown, if property could not set
   */
  void setProperty(cedar::dev::sensors::camera::Property::Id propertyId, double value);

  /*! @brief Get the value from the cv::VideoCapture object with respect to capabilities and settings
   *
   * @param propertyId The Id of the property as cedar::dev::sensors::camera::Property::Id
   * @return The value of the property
   */
  double getProperty(cedar::dev::sensors::camera::Property::Id propertyId);

  //! @brief Get the mode of operation of the used parameter
  cedar::dev::sensors::camera::PropertyMode::Id getMode
  (
    cedar::dev::sensors::camera::Property::Id propertyId
  );

  /*! @brief Set the mode of operation of the property
   *
   *  @param propertyId The Id of the property as cedar::dev::sensors::camera::Property::Id
   *  @param modeId The new mode (auto, manual, backendDefault) as cedar::dev::sensors::camera::PropertyMode::Id
   */
  void setMode
  (
    cedar::dev::sensors::camera::Property::Id propertyId,
    cedar::dev::sensors::camera::PropertyMode::Id modeId
  );

  /*! @brief Disable the Property with the given ID
   *
   *  @param propertyId The Id of the property as cedar::dev::sensors::camera::Property::Id
   */
  void disableProperty(cedar::dev::sensors::camera::Property::Id propertyId);

  /*! @brief Enable the Property with the given ID
   *
   *  @param propertyId The Id of the property as cedar::dev::sensors::camera::Property::Id
   */
  void enableProperty(cedar::dev::sensors::camera::Property::Id propertyId);


  /*! @brief Set the default value of the property
   *
   *   Normally, this value comes from the backend, but it is possible to change it
   *  @param propertyId The Id of the property as cedar::dev::sensors::camera::Property::Id
   *  @param value The new value, which should be used as default value
   */
  void setDefaultValue(cedar::dev::sensors::camera::Property::Id propertyId, double value);

  /*! @brief Get the minimum possible value that can be set of the given property
   *  @param propertyId The id of the property
   */
  double getMinValue(cedar::dev::sensors::camera::Property::Id propertyId);

  /*! @brief Get the maximum possible value that can be set of the given property
   *  @param propertyId The id of the property
   */
  double getMaxValue(cedar::dev::sensors::camera::Property::Id propertyId);

  /*! @brief This method tells you, if the given property is supported by the used camera
   *  @param propertyId The id of the  property
   */
  bool isSupported(cedar::dev::sensors::camera::Property::Id propertyId);

  /*! @brief This method tells you, if the given property can be set to auto-mode
   *  @param propertyId The id of the  property
   */
  bool isAutoCapable(cedar::dev::sensors::camera::Property::Id propertyId);

  /*! @brief This method tells you, if the given property can be set manually
   *  @param propertyId The id of the  property
   */
  bool isManualCapable(cedar::dev::sensors::camera::Property::Id propertyId);

  /*! @brief Set the mode of the wanted property
   *  @param propertyId The id of the property
   *  @param modeId the new mode
   *  @throw cedar::dev::sensors::camera::PropertyNotSetException Thrown, if property mode could not set
   */
  void setPropertyMode
  (
    cedar::dev::sensors::camera::Property::Id propertyId,
    cedar::dev::sensors::camera::PropertyMode::Id modeId
  );

  /*! @brief get the mode of the wanted property
   *  @param propertyId The id of the property
   *  @return A value from the PropertyMode enum class
   */
  cedar::dev::sensors::camera::PropertyMode::Id getPropertyMode
  (
    cedar::dev::sensors::camera::Property::Id propertyId
  );


  /*! @brief Set the internally used VideoCapture object to a new one
   *
   *   It is necessary to change the object, because OpenCv uses shared-pointer like structures for the VideoCapture.
   *   At the creation of a new object in the backend, it doesn't change at the properties
   *
   *  @param capture The new capture object
   */
  void setVideoCaptureObject(cv::VideoCapture capture);


  /*! @brief Get a property directly form the cv::VideoCapture
   *
   *    Use this method only for properties which are not (yet) supported by cedar Property()
   *    or CameraSetting() class. But be aware, that there is no check if the wanted property is supported
   *    by the used backend
   *
   *  @param propertyId The OpenCV constants for cv::VideoCapture.set() method
   *  @return Value, that indicates the exit-state of cv::VideoCapture.set()
   */
  double getPropertyFromCamera(unsigned int propertyId);

  /*! @brief Get the object of the Property class
   *
   *  @param propertyId The wanted property
   *  @return A shared pointer to the CamProperty object of the given property
   */
  cedar::dev::sensors::camera::CamPropertyPtr getPropertyObject(Property::Id propertyId);


  /*! @brief Enable or disable the eventhandling of the properties on setValue() methods
   *
   *  @param block True if no signal-processing should be done, otherwise false
   *
   *  @remarks This method invokes the QObject::blockSignals() method for every used property
   */
  void blockSignals(bool block);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  /*! @brief Set a property direct in the cv::VideoCapture class
   *
   *  @param propertyId The OpenCV constants for cv::VideoCapture.set() method
   *  @param value The new value
   *  @throw cedar::dev::sensors::camera::PropertyNotSetException Thrown, if the value is not properly set
   */
  void setPropertyToCamera(unsigned int propertyId, double value);


  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! The list of properties
  CamPropertiesMap mPropertiesList;

  //! Camera interface
  cv::VideoCapture mVideoCapture;

  //! The lock for the concurrent access to the cv::VideoCapture
  QReadWriteLock* mpVideoCaptureLock;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::sensors::camera::Properties

#endif // CEDAR_DEV_SENSORS_CAMERA_PROPERTIES_H

