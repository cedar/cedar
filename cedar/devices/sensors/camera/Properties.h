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

  // typedef cedar::aux::ObjectMapParameterTemplate<CamProperty> CamPropertiesMap;
  typedef std::map<unsigned int, CamPropertyPtr> CamPropertiesMap;
  CEDAR_GENERATE_POINTER_TYPES(CamPropertiesMap);

  Q_OBJECT

protected slots:

//!@brief A slot is used, to set the changed property in the camera
//  void propertyChanged();

  //!@brief This signal is emitted, when the value of the property has changed.
  void propertyValueChanged(cedar::dev::sensors::camera::Property::Id propertyId, double newValue);

  //!@brief This signal is emitted, when the mode of the property has changed.
  void propertyModeChanged
  (
    cedar::dev::sensors::camera::Property::Id propertyId,
    cedar::dev::sensors::camera::PropertyMode::Id newMode
  );


signals:

//!@brief This signal is emitted, when a setting has changed.
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

  ///! set the value to the cv::VideoCapture object with respect to capabilities and settings
  bool setProperty(cedar::dev::sensors::camera::Property::Id propertyId, double value);

  ///! get the value from the cv::VideoCapture object with respect to capabilities and settings
  /// @return If the value is negative, then the mode of the property is either BACKEND_DEFAULT or AUTO
  double getProperty(cedar::dev::sensors::camera::Property::Id propertyId);


//  cedar::dev::sensors::camera::PropertiesMap& getPropertiesList();
  // void setPropertiesList(cedar::dev::sensors::camera::PropertyMap propertiesList);

  /// Get the mode of operation of the used parameter
  cedar::dev::sensors::camera::PropertyMode::Id getMode
  (
    cedar::dev::sensors::camera::Property::Id propertyId
  );

  /// Set the mode of operation
  void setMode
  (
    cedar::dev::sensors::camera::Property::Id propertyId,
    cedar::dev::sensors::camera::PropertyMode::Id modeId
  );

  void disableProperty(cedar::dev::sensors::camera::Property::Id propertyId);
  void enableProperty(cedar::dev::sensors::camera::Property::Id propertyId);

  void setDefaultValue(cedar::dev::sensors::camera::Property::Id propertyId, double value);

  /*! \brief Get the minimum possible value that can be set of the given property
   *  \param propId The id of the property
   */
  double getMinValue(cedar::dev::sensors::camera::Property::Id propertyId);

  /*! \brief Get the maximum possible value that can be set of the given property
   *  \param propId The id of the property
   */
  double getMaxValue(cedar::dev::sensors::camera::Property::Id propertyId);

  /*! \brief This method tells you, if the given property is supported by the used camera
   *  \param propId The id of the  property
   */
  bool isSupported(cedar::dev::sensors::camera::Property::Id propertyId);

  /*! \brief This method tells you, if the given property can be set to auto-mode
   *  \param propId The id of the  property
   */
  bool isAutoCapable(cedar::dev::sensors::camera::Property::Id propertyId);

  /*! \brief This method tells you, if the given property can be set manually
   *  \param propId The id of the  property
   */
  bool isManualCapable(cedar::dev::sensors::camera::Property::Id propertyId);

  /*! \brief This method tells you, if the given property can be set to an absolute value
   *  \param propId The id of the  property
   */
  // bool isAbsoluteCapable(Property::Id propId);

    /*! \brief This method tells you, if the given property supports the OnePushAuto mode
   *
   *     OnePushAuto is a special mode. <br>
   *     It is used as follows: Set a value to a property and then to OnePushAuto mode.
   *     The camera now will try to hold this value automatically.
   *  \param propId The id of the  property
   */
  // bool isOnePushCapable(Property::Id propId);

  /*! \brief This method tells you, if the given property could be turn off and on
   *  \param propId The id of the  property
   */
  // bool isOnOffCapable(Property::Id propId);


  /*! \brief Set the mode of the wanted property
   *  \param propId The id of the property
   *  \param modeId the new mode
   *  \return True if new value is properly set
   */
  bool setPropertyMode
  (
    cedar::dev::sensors::camera::Property::Id propertyId,
    cedar::dev::sensors::camera::PropertyMode::Id modeId
  );

  /*! \brief get the mode of the wanted property
   *  \param propId The id of the property
   *  \return A value from the PropertyMode enum class
   */
  cedar::dev::sensors::camera::PropertyMode::Id getPropertyMode
  (
    cedar::dev::sensors::camera::Property::Id propertyId
  );


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

  cedar::dev::sensors::camera::CamPropertyPtr getPropertyClass(Property::Id propertyId);


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  /*! @brief Set a property direct in the cv::VideoCapture class
   *
   *  @param propertyId The OpenCV constants for cv::VideoCapture.set() method
   *  @param value The new value
   *  @return Boolean value, that indicates if the value is properly set
   */
  bool setPropertyToCamera(unsigned int propertyId, double value);


  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // return CamProperty
  //@exception std::out_of_range if the searched value doesn't exist
  // cedar::dev::sensors::visual::CamPropertyPtr getPropertyPtr(Property::Id propId);


  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  ///!The list of properties
  CamPropertiesMapPtr mpPropertiesList;

  /// Camera interface
  cv::VideoCapture mVideoCapture;

  /// The lock for the concurrent access to the cv::VideoCapture
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

