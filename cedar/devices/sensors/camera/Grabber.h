/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/devices/sensors/camera/namespace.h"
#include "cedar/devices/sensors/visual/Grabber.h"
#include "cedar/devices/sensors/camera/enums/IsoSpeed.h"
#include "cedar/devices/sensors/camera/enums/Property.h"
#include "cedar/devices/sensors/camera/enums/VideoMode.h"
#include "cedar/devices/sensors/camera/enums/FrameRate.h"
#include "cedar/devices/sensors/camera/enums/Decoding.h"
#include "cedar/devices/sensors/camera/Properties.h"
#include "cedar/devices/sensors/camera/Channel.h"
#include "cedar/auxiliaries/IntParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"


// backends
#include "cedar/devices/sensors/camera/backends/BackendType.h"
#include "cedar/devices/sensors/camera/backends/Device.h"
//#include "cedar/devices/sensors/camera/backends/DeviceCvVideoCapture.h"
//
//#ifdef CEDAR_USE_VIDEO_FOR_LINUX
//#include "cedar/devices/sensors/camera/backends/DeviceVfl.h"
//#endif // CEDAR_USE_VIDEO_FOR_LINUX
//
//#ifdef CEDAR_USE_LIB_DC1394
//#include "cedar/devices/sensors/camera/backends/DeviceDc1394.h"
//#endif // CEDAR_USE_LIB_DC1394

// SYSTEM INCLUDES

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

  //!@brief A slot that must be triggered if a camera-settings has changed. In that case,
  // a new camera-device will be created
  void cameraChanged();


  // signals:

  //!@brief This signal is emitted, when a new picture is available with the getImage() method.
 // void pictureChanged();

  private:
  /*! @brief Boost slot method. Invoked if a channel is added as an ObjectListParameter as an object
   */
  void channelAdded(int);  // int index

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief Constructor for a camera grabber with one connected camera
   *  @param configFileName Filename for the configuration
   *  @param camera Device to grab from. Look at OpenCV cv::VideoCapture documentation for details
   *  @param isGuid This flag have to be set, if the value in parameter camera is the guid of the camera. In this
   *         case, the camera with this guid will be searched on the bus.
   *  @param finishInitialization Flag, if the initialization should be finished. In this case it isn't possible to
   *         change the camera resolution or the camera framerate if you use a firewire camera. <br>
   *         The settings will be restored from the configfile. If there is no configfile,
   *         the best settings (i.e. highest framerate, largest picture size) will be used.
   *         If the finishInitialization flag isn't set, you can change this settings before you grab the first Frame.<br>
   *         But be aware, that getImage() will return empty matrices unless the first frame have been grabbed.
   *         This is done by starting the grabbing thread via the startGrabber() method or manually grab the first picture
   *         with grab().
   *  @remarks
   *    OpenCV selects the capture framework at the basis of the camera parameter. Use a multiple of 100 to select
   *    the interface or use the CV_CAP_xxx constants in /usr/local/include/opencv2/highgui/highgui_c.h for a
   *    base unit (like CV_CAP_FIREWIRE, CV_CAP_ANY)
   */



  /*! @brief  Constructor for a single camera grabber
   *  @param backendType  The type of the backend you want to use.
   *  @param cameraId  The identification of the camera. In case of a firewirecamera this could be the GUID of the
   *                   wanted cam. In all other backends, this is the bus id
   *  @param isGuid This flag is only available, when CEDAR is built with libdc/firewire support. If this flag is set
   *                true, then the value in cameraId is treated as the unique GUID of the camera you want.
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
   *  @param backendType  The type of the backend you want to use.
   *  @param cameraId0  The identification of the camera on channel 0. In case of a firewire camera
   *                  this could be the GUID of the wanted cam. In all other backends, this is the bus id.
   *  @param cameraId1  The identification of the camera on channel 1. In case of a firewire camera
   *                  this could be the GUID of the wanted cam. In all other backends, this is the bus id.
   *  @param isGuid This flag is only available, when CEDAR is built with libdc/firewire support. If this flag is set
   *                true, then the value in cameraId is treated as the unique GUID of the camera you want.
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


  /*! @brief Constructor for a camera grabber. The complete configuration will be read from configuration file.<br>
   *   If the system can't find the wanted camera, initialization will fail.
   *  @par
   *        The camera always will be initialized (i.e. the first frame is already grabbed on initialization)
   *  @param configFileName Filename for the configuration
   *  @param numCameras How many cameras you want to use
   *  @remarks
   *        If there is no configuration file with the given name, a new one will be created. But be aware,
   *        the default camera capabilities filename (build with guid or busId) will be used. It is possible,
   *        that this file isn't the right one for your camera.
   */
  // Grabber(unsigned int numCameras);


  /*! @brief Destructor */
  ~Grabber();


  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!  @brief With this method, it is possible to get Information on any channel.
   *
   *   This method passes the arguments directly to the corresponding capture device
   *   @param channel This is the index of the source you want to get the parameter value.
   *   @param propId This is any supported property-Id<br>
   *     If property-id is not supported or unknown, return value will be -1.
   *   @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *   @return Returns either a double value or one of the following constants:
   *          - CAMERA_PROPERTY_NOT_SUPPORTED
   *          - CAMERA_PROPERTY_MODE_AUTO
   *          - CAMERA_PROPERTY_MODE_DEFAULT
   *   @see Property
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
  );


  /*! @brief Set the mode of a property
   *  @param channel This is the index of the source you want to set the parameter value.
   *  @param propId This is any known property-Id from class Property
   *  @param modeId This is the new mode
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @return Returns True if the mode is set successfully
   */
  bool setPropertyMode
  (
    unsigned int channel,
    cedar::dev::sensors::camera::Property::Id propId,
    cedar::dev::sensors::camera::PropertyMode::Id modeId
  );

  /*!  @brief With this method, it is possible to set a property
   *   @param channel This is the index of the source you want to set the parameter value.
   *   @param propId This is any known property-Id<br>
   *     If property-id is not supported or unknown, return value will be false.
   *   @param value This is the new value.
   *   @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *   @see Property
   */
  bool setProperty(unsigned int channel, Property::Id propId, double value);
  
  /*!  @brief Set informations on camera 0
   *   @param propId This is any knoqn property-Id<br>
   *     If property-id is not supported or unknown, return value will be false.
   *   @param value This is the new value.
   */
  bool setProperty(Property::Id propId, double value);

  /*! @brief Set the video mode of the camera.
   *
   *   This can only be done, if the first frame wasn't already grabbed
   *  @param channel This is the index of the source you want to set the parameter value.
   *  @param modeId The new value from class VideoMode
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   * @see getCameraMode
   */
  void setCameraVideoMode(unsigned int channel, cedar::dev::sensors::camera::VideoMode::Id modeId);

  /*! @brief Gets the actual mode.
   *  @param channel This is the index of the source you want to set the parameter value.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   */
  cedar::dev::sensors::camera::VideoMode::Id getCameraVideoMode(unsigned int channel);

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
  void setCameraFps(unsigned int channel, FrameRate::Id fpsId);

  /*! @brief Gets the actual fps of the camera.
   *  @param channel This is the index of the source you want to set the parameter value.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   */
  FrameRate::Id getCameraFps(unsigned int channel);

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
  IsoSpeed::Id getCameraIsoSpeed(unsigned int channel);

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
  unsigned int getCameraId( unsigned int channel);

  /*! @brief Flag if the camera is instantiated by its Guid or by the Bus-ID
   *  @param channel This is the index of the source channel.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @see getCamereaId
   *  @remarks Only firewire cameras support the GUID field.
   */
  bool isGuid(unsigned int channel);

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


  void setDecodeFilter(unsigned int channel, cedar::dev::sensors::camera::Decoding::Id filterId);

  void setDecodeFilter(cedar::dev::sensors::camera::Decoding::Id filterId);


  cedar::dev::sensors::camera::Decoding::Id getDecodeFilter(unsigned int channel = 0);


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
   *  @param propertyId The OpenCV constants for cv::VideoCapture.set() method
   *  @return Value, that indicates the exit-state of cv::VideoCapture.set()
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   */
  double getPropertyFromCamera(unsigned int channel, unsigned int propertyId);

  //------------------------------------------------------------------------
  // From Grabber
  //------------------------------------------------------------------------
  bool onGrab();
  void onCleanUp();
  bool onCreateGrabber();
  void onCloseGrabber();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /// This function does internal variable initialization for the constructors
  void init();

  /*! @brief This function connects the used signals
   */
  void connectSignals();

  /// @brief updates the channel informations
  void setChannelInfo(unsigned int channel);

  /// @brief Sets the channel-id which depends on the isGuid-flag (only used in constructor)
  // void setChannelId(unsigned int channel, unsigned int id, bool isGuid);

  /*! This string identifies, that the default-filename (containing grabber-guid) should be used
   * If the entry in the configuration file is different, then that file will be used
   */
/*  inline std::string useAutogeneratedFilenameString()
  {
    return "USE_AUTOGENERATED_FILENAME_WITH_GUID";
  }

  /// @brief Default filename for the config-file of the camera capabilities
  inline std::string getCapabilitiesFilename(unsigned int guid)
  {
    return "camera_"+boost::lexical_cast<std::string>(guid)+".capabilities";
  }
*/
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

  /// Set if Initialization should be finished in constructor
  // bool mFinishInitialization;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet


}; // class cedar::dev::sensors::camera::Grabber

#endif // CEDAR_DEV_SENSORS_CAMERA_GRABBER_H


