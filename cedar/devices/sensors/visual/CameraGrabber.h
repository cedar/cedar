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

    File:        CameraGrabber.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Header for the @em cedar::dev::sensors::visual::CameraGrabber class.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSORS_VISUAL_CAMERA_GRABBER_H
#define CEDAR_DEV_SENSORS_VISUAL_CAMERA_GRABBER_H

#include "cedar/configuration.h"   // MAKE FIREWIRE OPTIONAL
#ifdef CEDAR_USE_LIB_DC1394

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/Grabber.h"
#include "cedar/devices/sensors/visual/camera/CameraIsoSpeed.h"
#include "cedar/devices/sensors/visual/camera/CameraProperty.h"
#include "cedar/devices/sensors/visual/camera/CameraVideoMode.h"
#include "cedar/devices/sensors/visual/camera/CameraFrameRate.h"
#include "cedar/devices/sensors/visual/camera/CameraSetting.h"
#include "cedar/devices/sensors/visual/camera/CameraCapabilities.h"
#include "cedar/devices/sensors/visual/camera/CameraStateAndConfig.h"
#include "cedar/devices/sensors/visual/camera/CameraConfig.h"

// SYSTEM INCLUDES


/*! @class cedar::dev::sensors::visual::CameraGrabber
 *  @brief This grabber grabs images from a camera. This functionality is implemented by
 *   using the OpenCV class cv::VideoCapture
 */
class cedar::dev::sensors::visual::CameraGrabber
:
public cedar::dev::sensors::visual::Grabber
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //!@cond SKIPPED_DOCUMENTATION

  /*! @struct CameraId
   *  @brief Store the unique Id of a camera
   */
  struct CameraId
  {
    unsigned int busId;  /// The bus id
    unsigned int guid;   /// The unique id of the device
  };


  /*! @struct CameraChannel
   *  @brief Additional data of a camera channel
   */
  struct CameraChannel
  :
  cedar::dev::sensors::visual::Grabber::GrabberChannel
  {
    /// Unique channel id
    CameraId mCamId;

    /// Camera interface
    cv::VideoCapture mVideoCapture;

    /// The lock for the concurrent access to the cv::VideoCapture
    QReadWriteLock* pmVideoCaptureLock;

    /// The manager of settings and properties
    CameraStateAndConfigPtr mCamStateAndConfig;

    /// Filename for the capabilities
    std::string mCamCapabilitiesFileName;
  };

  CEDAR_GENERATE_POINTER_TYPES(CameraChannel);


  //!@endcond

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

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
  CameraGrabber(const std::string& configFileName, unsigned int camera, bool isGuid, bool finishInitialization = true);


  /*! @brief Constructor for a stereo camera grabber
   *  @param configFileName Filename for the configuration
   *  @param camera0 Device to grab from for channel 0. Look at OpenCV cv::VideoCapture documentation for details
   *  @param camera1 Device to grab from for channel 1. Look at OpenCV cv::VideoCapture documentation for details
   *  @param isGuid This flag have to be set, if the values in camera0 or camera1 are the guids of the cameras
   *  @param finishInitialization Flag, if the initialization should be finished. Have a look at the CameraGrabber()
   *          Constructor for details
   *  @see cedar::dev::sensors::visual::CameraGrabber::CameraGrabber(const std::string&, unsigned int, bool, bool)
   *       for details about the used framework
   */
  CameraGrabber
  (
    const std::string& configFileName,
    unsigned int camera0,
    unsigned int camera1,
    bool isGuid,
    bool finishInitialization = true
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
  CameraGrabber(const std::string& configFileName, unsigned int numCameras);


  /*! @brief Destructor */
  ~CameraGrabber();


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
   *          - CAMERA_PROPERTY_MODE_OFF
   *   @see CameraProperty
   */
  double getCameraProperty( unsigned int channel, CameraProperty::Id propId);

  /*! @brief Get the real value of a Property which is set to auto.
   *
   *   On all other properties it is the same as getCameraProperty()
   *  @param channel This is the index of the source you want to get the parameter value.
   *  @param propId This is any known property-Id from class CameraProperty
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @return Returns either a double value or one of the following constants:
   *          - CAMERA_PROPERTY_NOT_SUPPORTED
   *          - CAMERA_PROPERTY_MODE_AUTO
   *          - CAMERA_PROPERTY_MODE_OFF
   */
  double getCameraPropertyValue(unsigned int channel, CameraProperty::Id propId);

  /*! @brief Get informations on camera on channel 0
   *  @see getCameraProperty(unsigned int, CameraParam_t)
   *  @param propId This is any known property-Id from class CameraProperty
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @return Returns either a double value or one of the following constants:
   *          - CAMERA_PROPERTY_NOT_SUPPORTED
   *          - CAMERA_PROPERTY_MODE_AUTO
   *          - CAMERA_PROPERTY_MODE_OFF
   *  @see CameraProperty
   */
  double getCameraProperty(CameraProperty::Id propId);


  /*!  @brief With this method, it is possible to set Information on any channel.
   *    This method passes the arguments directly to the corresponding capture device
   *   @param channel This is the index of the source you want to set the parameter value.
   *   @param propId This is any known property-Id<br>
   *     If property-id is not supported or unknown, return value will be false.
   *   @param value This is the new value.
   *   @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *   @see CameraProperty
   */
  bool setCameraProperty(unsigned int channel, CameraProperty::Id propId, double value);
  
  /*!  @brief Set informations on camera 0
   *   @param propId This is any knoqn property-Id<br>
   *     If property-id is not supported or unknown, return value will be false.
   *   @param value This is the new value.
   */
  bool setCameraProperty(CameraProperty::Id propId, double value);

  /*! @brief Set values on the camera which have to be adjusted before the first image will be grabbed.
   *      This method can be used to directly set Mode, Fps, IsoSpeed and FrameSize. <br>
   *  @param channel This is the index of the source you want to set the parameter value.
   *  @param settingId The id of the setting you want to change (from class CameraSetting)
   *  @param value The new value
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @see  setCameraMode, setCameraFps, setCameraIsoSpeed, setCameraFrameSize, CameraSetting
   */
  bool setCameraSetting(unsigned int channel, CameraSetting::Id settingId, double value);

  /*! @brief Get values of the camera which have to be adjusted before the first image will be grabbed
   *
   *      This method can be used to directly set Mode, Fps, IsoSpeed and FrameSize
   *      before initialization will be finished.
   *  @param channel This is the index of the source you want to set the parameter value.
   *  @param settingId The id of the setting you want to change (from class CameraSetting)
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @see  setCameraMode, setCameraFps, setCameraIsoSpeed, CameraSetting
   */
  double getCameraSetting(unsigned int channel, CameraSetting::Id settingId);

  /*! @brief Set the video mode of the camera.
   *
   *   This can only be done, if the first frame wasn't already grabbed
   *  @param channel This is the index of the source you want to set the parameter value.
   *  @param modeId The new value from class CameraVideoMode
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   * @see getCameraMode
   */
  bool setCameraMode(unsigned int channel, CameraVideoMode::Id modeId);

  /*! @brief Gets the actual mode.
   *  @param channel This is the index of the source you want to set the parameter value.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   */
  CameraVideoMode::Id getCameraMode(unsigned int channel);

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
  bool setCameraFps(unsigned int channel, CameraFrameRate::Id fpsId);

  /*! @brief Gets the actual fps of the camera.
   *  @param channel This is the index of the source you want to set the parameter value.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   */
  CameraFrameRate::Id getCameraFps(unsigned int channel);

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
  bool setCameraIsoSpeed(unsigned int channel, CameraIsoSpeed::Id isoSpeedId);

  /*! @brief Gets the actual ISO-Speed of the IEEE1394/firewire bus.
   *  @param channel This is the index of the source you want to set the parameter value.
   */
  CameraIsoSpeed::Id getCameraIsoSpeed(unsigned int channel);

#endif

  /*! @brief Gets the GUID of the camera
   *  @param channel This is the index of the source channel
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   */
  unsigned int getCameraGuid( unsigned int channel);


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

  /*! @brief Set a property direct in the cv::VideoCapture class
   *
   *    Use this method only for properties which are not (yet) supported by cedar CameraProperty()
   *    or CameraSetting() class. But be aware, that there is no check if the wanted property is supported
   *    by the used backend
   *
   *   @remarks
   *      Use this only for above mentioned reasons, because there is no value-checking and the
   *      internal values which are cached from the CameraGrabber class isn't updated!
   *
   *  @param channel This is the index of the source you want to set the parameter value.
   *  @param propId The OpenCV constants for cv::VideoCapture.set() method
   *  @param value The new value
   *  @return Boolean value, that indicates the exit-state of cv::VideoCapture.set()
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @see  setCameraMode, setCameraFps, setCameraIsoSpeed, CameraSetting, setCameraProperty
   *
   *
   */
  bool setRawProperty(unsigned int channel, unsigned int propId, double value);

  /*! @brief Get a property directly form the cv::VideoCapture
   *
   *    Use this method only for properties which are not (yet) supported by cedar CameraProperty()
   *    or CameraSetting() class. But be aware, that there is no check if the wanted property is supported
   *    by the used backend
   *
   *  @param channel This is the index of the source you want to set the parameter value.
   *  @param propId The OpenCV constants for cv::VideoCapture.set() method
   *  @return Value, that indicates the exit-state of cv::VideoCapture.set()
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @see  getCameraMode, getCameraFps, getCameraIsoSpeed, CameraSetting, getCameraProperty
   */
  double getRawProperty(unsigned int channel, unsigned int propId);
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  
  //------------------------------------------------------------------------
  //From Grabber
  //------------------------------------------------------------------------

  bool onInit();

  bool onGrab();

  bool onDeclareParameters();
  void onUpdateSourceInfo(unsigned int channel);

  ///! @brief Sync all Parameters from cameras with the local buffer
  bool onWriteConfiguration();

  ///! @brief Do the local clean up
  void onCleanUp();

  ///! Create and initialize the channel-structure for ony channel (only used in constructor)
  void onAddChannel();


  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  /// @brief Sets the channel-id which depends on the isGuid-flag (only used in constructor)
  void setChannelId(unsigned int channel, unsigned int id, bool isGuid);

  /*! This string identifies, that the default-filename (containing grabber-guid) should be used
   * If the entry in the configuration file is different, then that file will be used
   */
  inline std::string useAutogeneratedFilenameString()
  {
    return "USE_AUTOGENERATED_FILENAME_WITH_GUID";
  }

  /// @brief Default filename for the config-file of the camera capabilities
  inline std::string getCapabilitiesFilename(unsigned int guid)
  {
    return "camera_"+boost::lexical_cast<std::string>(guid)+".capabilities";
  }

  /// @brief Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class CameraChannelPtr
  inline CameraChannelPtr getChannel(unsigned int channel)
  {
    return boost::static_pointer_cast<CameraChannel>
           (
             cedar::dev::sensors::visual::Grabber::mChannels.at(channel)
           );
  }

  /// @brief Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class CameraChannelPtr
  inline ConstCameraChannelPtr getChannel(unsigned int channel) const
  {
    return boost::static_pointer_cast<const CameraChannel>
           (
             cedar::dev::sensors::visual::Grabber::mChannels.at(channel)
           );
  }

  //!@brief The default name for the grabber
  virtual inline std::string defaultGrabberName() const
  {
    return "CameraGrabber";
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:

  /// Set if Initialization should be finished in constructor
  bool mFinishInitialization;

  /// Set if the CameraGrabber should search the bus for a camera with the give guid
  bool mCreateGrabberByGuid;


  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::sensors::visual::CameraGrabber

#endif // CEDAR_USE_LIB_DC1394
#endif // CEDAR_DEV_SENSORS_VISUAL_CAMERA_GRABBER_H


