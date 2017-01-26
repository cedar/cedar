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
    along with cedar. If not, see <http:// www.gnu.org/licenses/>.

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        Grabber.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Header for the @em cedar::dev::sensors::camera::Grabber class.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSORS_CAMERA_GRABBER_H
#define CEDAR_DEV_SENSORS_CAMERA_GRABBER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/IntParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/devices/sensors/visual/Grabber.h"
#include "cedar/devices/sensors/camera/namespace.h"
#include "cedar/devices/sensors/camera/exceptions.h"
#include "cedar/devices/sensors/camera/enums/IsoSpeed.h"
#include "cedar/devices/sensors/camera/enums/Property.h"
#include "cedar/devices/sensors/camera/enums/VideoMode.h"
#include "cedar/devices/sensors/camera/enums/FrameRate.h"
#include "cedar/devices/sensors/camera/enums/Decoding.h"
#include "cedar/devices/sensors/camera/Properties.h"
#include "cedar/devices/sensors/camera/Channel.h"
#include "cedar/devices/sensors/camera/backends/BackendType.h"
#include "cedar/devices/sensors/camera/backends/Backend.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>

/*! @class cedar::dev::sensors::camera::Grabber
 *  @brief This grabber grabs images from a camera. The functionality is implemented by
 *   using the OpenCV class cv::VideoCapture, but with different hardware backends
 */
class cedar::dev::sensors::camera::Grabber
:
public cedar::dev::sensors::visual::Grabber
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

public:

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // slots and signals
  //--------------------------------------------------------------------------------------------------------------------

  protected slots:

  /*! @brief A slot that must be triggered if a camera-settings has changed. In that case,
   * a new camera-device will be created. It is internally connected with the CameraChannel->settingsChanged signal.
   */
  void settingChanged();

  /*! @brief A slot that must be triggered if the backend has changed. It is internally connected with the
   *      CameraChannel->Backend Enum-Parameter valueChanged() signal.
   */
  void backendChanged();

  signals:

  /*! @brief This signal is raised, if the grabber will be recreated
   *
   *   This signal is used from the gui Camera step to annotate a new imagesize
   */
  void frameSizeChanged();


  private:
  /*! @brief Boost slot method. Invoked if a channel is added as an ObjectListParameter as an object
   */
  void channelAdded(int);  // int index

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief  Constructor for a single camera grabber
   *  @param cameraId  The identification of the camera. In case of a firewirecamera this could be the GUID of the
   *                   wanted cam. In all other backends, this is the bus id
   *  @param isGuid This flag is only available, when CEDAR is built with libdc/firewire support. If this flag is set
   *                true, then the value in cameraId is treated as the unique GUID of the camera you want.
   *  @param backendType Use this backend for the camera
   *  @param decodeFilter Use this decode filter
   *
   *  @remarks
   *    OpenCV selects the capture framework at the basis of the cameraId parameter. Use a multiple of 100 to select
   *    the interface or use the CV_CAP_xxx constants in /usr/local/include/opencv2/highgui/highgui_c.h for a
   *    base unit (like CV_CAP_FIREWIRE, CV_CAP_ANY)
   */
  Grabber
  (
    unsigned int cameraId = 0,
    bool isGuid = false,
    cedar::dev::sensors::camera::BackendType::Id backendType
      = cedar::dev::sensors::camera::BackendType::AUTO,
    cedar::dev::sensors::camera::Decoding::Id decodeFilter
      = cedar::dev::sensors::camera::Decoding::NONE
  );

  /*! @brief  Constructor for a stereo camera grabber
   *  @param cameraId0  The identification of the camera on channel 0. In case of a firewire camera
   *                  this could be the GUID of the wanted cam. In all other backends, this is the bus id.
   *  @param cameraId1  The identification of the camera on channel 1. In case of a firewire camera
   *                  this could be the GUID of the wanted cam. In all other backends, this is the bus id.
   *  @param isGuid This flag is only available, when CEDAR is built with libdc/firewire support. If this flag is set
   *                true, then the value in cameraId is treated as the unique GUID of the camera you want.
   *  @param backendType Use this backend for the camera
   *  @param decodeFilter Use this decode filter
   */
  Grabber
  (
    unsigned int cameraId0,
    unsigned int cameraId1,
    bool isGuid = false,
    cedar::dev::sensors::camera::BackendType::Id backendType
       = cedar::dev::sensors::camera::BackendType::AUTO,
    cedar::dev::sensors::camera::Decoding::Id decodeFilter
       = cedar::dev::sensors::camera::Decoding::NONE
  );

  /*! @brief Destructor */
  ~Grabber();


  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! @brief With this method, it is possible to get Information on any channel.
   *
   *  This method passes the arguments directly to the corresponding capture device
   *  @param channel This is the index of the source you want to get the parameter value.
   *  @param propId This is any supported property-Id<br>
   *    If property-id is not supported or unknown, return value will be -1.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @return Returns either a double value or one of the following constants:
   *         - CAMERA_PROPERTY_NOT_SUPPORTED
   *         - CAMERA_PROPERTY_MODE_AUTO
   *         - CAMERA_PROPERTY_MODE_DEFAULT
   *  @see Property
   */
  double getProperty(unsigned int channel, Property::Id propId);

  /*! @brief Get the real value of a Property which is set to auto.
   *
   *   On all other properties it is the same as getProperty()
   *  @param channel This is the index of the source you want to get the parameter value.
   *  @param propId This is any known property-Id from class Property
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @return Returns either a double value or one of the following constants:
   *          - CAMERA_PROPERTY_NOT_SUPPORTED
   *          - CAMERA_PROPERTY_MODE_AUTO
   *          - CAMERA_PROPERTY_MODE_DEFAULT
   */
  double getPropertyValue(unsigned int channel, Property::Id propId);

  /*! @brief Get informations on camera on channel 0
   *  @see getProperty
   *  @param propId This is any known property-Id from class Property
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @return Returns either a double value or one of the following constants:
   *          - CAMERA_PROPERTY_NOT_SUPPORTED
   *          - CAMERA_PROPERTY_MODE_AUTO
   *          - CAMERA_PROPERTY_MODE_DEFAULT
   *  @see Property
   */
  double getProperty(Property::Id propId);


  /*! @brief Get the mode of a property
   *  @param channel This is the index of the source you want to set the parameter value.
   *  @param propId This is any known property-Id from class Property
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @return Returns a value of the PropertyMode enum class
   */
  cedar::dev::sensors::camera::PropertyMode::Id getPropertyMode
  (
    unsigned int channel,
    cedar::dev::sensors::camera::Property::Id propId
  ) const;


  /*! @brief Set the mode of a property
   *  @param channel This is the index of the source you want to set the parameter value.
   *  @param propId This is any known property-Id from class Property
   *  @param modeId This is the new mode
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @throw cedar::dev::sensors::camera::PropertyNotSetException Thrown, if property mode could not set
   */
  void setPropertyMode
  (
    unsigned int channel,
    cedar::dev::sensors::camera::Property::Id propId,
    cedar::dev::sensors::camera::PropertyMode::Id modeId
  );

  /*!@brief With this method, it is possible to set a property
   * @param channel This is the index of the source you want to set the parameter value.
   * @param propId This is any known property-Id<br>
   *   If property-id is not supported or unknown, return value will be false.
   * @param value This is the new value.
   * @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   * @throw cedar::dev::sensors::camera::PropertyNotSetException Thrown, if property mode could not set
   * @see Property
   */
  void setProperty(unsigned int channel, Property::Id propId, double value);
  
  /*!@brief Set informations on camera 0
   * @param propId This is any knoqn property-Id<br>
   *   If property-id is not supported or unknown, return value will be false.
   * @param value This is the new value.
   * @throw cedar::dev::sensors::camera::PropertyNotSetException Thrown, if property mode could not set
   */
  void setProperty(Property::Id propId, double value);

  /*!@brief Set the video mode of the camera.
   *
   *  This can only be done, if the first frame wasn't already grabbed
   * @param channel This is the index of the source you want to set the parameter value.
   * @param modeId The new value from class VideoMode
   * @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   * @see getCameraMode
   */
  void setCameraVideoMode(unsigned int channel, cedar::dev::sensors::camera::VideoMode::Id modeId);

  /*! @brief Gets the actual mode.
   *  @param channel This is the index of the source you want to set the parameter value.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   */
  cedar::dev::sensors::camera::VideoMode::Id getCameraVideoMode(unsigned int channel) const;

  /*! @brief Set the framerate of the camera.
   *
   *   This can only be done, if the first frame wasn't already grabbed
   *  @param channel This is the index of the source you want to set the parameter value.
   *  @param fpsId The new value
   * @par
   *   If the framerate isn't supported by the actual video mode, a similar mode
   *   will be used. The return value will also be true in this case.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   * @see setCameraFps
   */
  void setCameraFramerate(unsigned int channel, FrameRate::Id fpsId);

  /*! @brief Gets the actual fps of the camera.
   *  @param channel This is the index of the source you want to set the parameter value.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   */
  FrameRate::Id getCameraFramerate(unsigned int channel) const;

#ifdef CEDAR_USE_LIB_DC1394

  /*! @brief Set the ISO-speed of the IEEE1394/firewire bus.
   *
   *   This can only be done, if the first frame wasn't already grabbed
   *  @param channel This is the index of the source you want to set the parameter value.
   *  @param isoSpeedId The new value
   * @par
   *   If the wanted ISO-speed isn't supported by the camera, a similar mode
   *   will be used. The return value will also be true in this case.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   * @see setCameraIsoSpeed
   */
  void setCameraIsoSpeed(unsigned int channel, IsoSpeed::Id isoSpeedId);

  /*! @brief Gets the actual ISO-Speed of the IEEE1394/firewire bus.
   *  @param channel This is the index of the source you want to set the parameter value.
   */
  IsoSpeed::Id getCameraIsoSpeed(unsigned int channel) const;

#endif

  /*! @brief Gets the ID of the camera
   *
   *  @remarks On firewire cameras this value can also be the guid of the camera.
   *    If isGuid() is true, then this value is the GUID otherwise it is the number of the camera on the bus,
   *    i.e the bus-ID
   *  @see isGuid
   *  @param channel This is the index of the source channel
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   */
  unsigned int getCameraId( unsigned int channel) const;

  /*! @brief Flag if the camera is instantiated by its Guid or by the Bus-ID
   *  @param channel This is the index of the source channel.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @see getCamereaId
   *  @remarks Only firewire cameras support the GUID field.
   */
  bool isGuid(unsigned int channel) const;

  /*! @brief Gets the actual framesize.
   *  @param channel This is the index of the source channel.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   */
  cv::Size getCameraFrameSize(unsigned int channel);

  /*! @brief Write out all properties
   *
   *      The properties are read directly from cam
   *  @param channel This is the index of the source you want to print the properties.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   */
  std::vector<std::string> getAllProperties(unsigned int channel);

  /*! @brief Write out all settings
   *
   *      The settings are read directly from cam
   *  @param channel This is the index of the source you want to print the settings.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   */
  std::vector<std::string> getAllSettings(unsigned int channel);

  /*! @brief Set the decoding filter for the grabbed frames
   *
   *  @param channel The channel you want to change
   *  @param filterId The decode-filter to use as cedar::dev::sensors::camera::Decoding::Id
   */
  void setDecodeFilter(unsigned int channel, cedar::dev::sensors::camera::Decoding::Id filterId);

  /*! @brief Set the decoding filter for the grabbed frames
   *
   *   This method changes the decode-filter on the first channel.
   *   For a multi-channel grabber, have a look at
   *  @see setDecodeFilter(unsigned int, cedar::dev::sensors::camera::Decoding::Id)
   *
   *  @param filterId The decode-filter to use as cedar::dev::sensors::camera::Decoding::Id
   */
  void setDecodeFilter(cedar::dev::sensors::camera::Decoding::Id filterId);

  /*! @brief Get the used decoding filter
   *
   *  @param channel The channel you want the decode-filter
   *  @return The Filter as cedar::dev::sensors::camera::Decoding::Id
   */
  cedar::dev::sensors::camera::Decoding::Id getDecodeFilter(unsigned int channel = 0) const;


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  

  /*! @brief Get a property directly form the cv::VideoCapture
   *
   *    Use this method only for properties which are not (yet) supported by cedar Property()
   *    or Setting() class. But be aware, that there is no check if the wanted property is supported
   *    by the used backend
   *
   *  @param channel The channel you want to change the property
   *  @param propertyId The OpenCV constants for cv::VideoCapture.set() method
   *  @return Value, that indicates the exit-state of cv::VideoCapture.set()
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   */
  double getPropertyFromCamera(unsigned int channel, unsigned int propertyId);

  // inherited from Grabber
  void onGrab(unsigned int channel);
  void onCleanUp();
  void onCreateGrabber();
  void onCloseGrabber();
  std::string onGetSourceInfo(unsigned int channel);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /// This function does internal variable initialization for the constructors
  void init();

  /*! @brief This function connects the used signals
   */
  void connectSignals();

  /// @brief Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class ChannelPtr
  inline cedar::dev::sensors::camera::ChannelPtr getCameraChannel(unsigned int channel)
  {
    return boost::static_pointer_cast<cedar::dev::sensors::camera::Channel>
           (
             cedar::dev::sensors::camera::Grabber::_mChannels->at(channel)
           );
  }

  /// @brief Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class ChannelPtr
  inline cedar::dev::sensors::camera::ConstChannelPtr getCameraChannel(unsigned int channel) const
  {
    return boost::static_pointer_cast<const cedar::dev::sensors::camera::Channel>
           (
             cedar::dev::sensors::camera::Grabber::_mChannels->at(channel)
           );
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet


}; // class cedar::dev::sensors::camera::Grabber

#endif // CEDAR_DEV_SENSORS_CAMERA_GRABBER_H


