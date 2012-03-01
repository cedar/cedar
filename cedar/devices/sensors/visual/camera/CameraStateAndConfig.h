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

    File:        CameraStateAndConfig.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 12 01

    Description: Header of the @em cedar::devices::visual::CameraStateAndConfig class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_CAMERA_STATE_AND_CONFIG_H
#define CEDAR_DEV_SENSORS_VISUAL_CAMERA_STATE_AND_CONFIG_H

// LOCAL INCLUDES
#include "cedar/devices/sensors/visual/CameraGrabber.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*! \class cedar::dev::sensors::visual::CameraStateAndConfig
 *  \brief This class manage the properties and capabilities of a camera.
 *
 *  \remarks
 *    With the methods of this class, the CameraGraber class can evaluate the available properties
 *    and their possible values.
 *
 *  \par
 *    The main purpose of this class is to manage all the capabilities and properties of one used camera.
 *    There will be two classes created: <br>
 *    1. CameraCapabilities() <br>
 *        This class have a separate configuration file for the capabilities of the used camera. All supported
 *        properties and the range of their values will be managed by this class.<br>
 *    2. CameraConfig()  <br>
 *        This class main purpose is to encapsulate the ConfigurationInterface() for the actual set properties
 *        of the used camera. The configuration normally is written to the same file that your cameragrabber use.
 *
 */
class cedar::dev::sensors::visual::CameraStateAndConfig
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! \brief The standard constructor.
   *  \param VideoCapture The cv::VideoCapture object, which this configuration is assigned to
   *         This will be used, to read and set the values
   *  \param videoCaptureLock The lock, for the concurrent access to the cv::VideoCapture object
   *         through grabbing and/or change settings
   *  \param configurationFileName The filename for the configuration file used to store camera properties in
   *         This could be the same file, which the cameragrabber uses for configuration storage
   *  \param capabilitiesFileName The filename of the capabilities. This file have to be adjusted for the used camera
   */
  CameraStateAndConfig(
                       cv::VideoCapture videoCapture,
                       QReadWriteLockPtr videoCaptureLock,
                       unsigned int channel,
                       const std::string configurationFileName,
                       const std::string capabilitiesFileName
                     );

  //!@brief Destructor
  ~CameraStateAndConfig();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! \brief Set a property in the cv::VideoCapture class
   *   \remarks This method checks if given value is supported
   */
  bool setProperty(CameraProperty::Id propId, double value);

  /*! \brief Set a property in the cv::VideoCapture class
   *   \remarks This method checks if given value is supported
   *   \return return value is either the value of the property or a one of the following constants
   *    CAMERA_PROPERTY_NOT_SUPPORTED,
   *    CAMERA_PROPERTY_MODE_AUTO,
   *    CAMERA_PROPERTY_MODE_OFF,
   *    CAMERA_PROPERTY_MODE_ONE_PUSH_AUTO
   */
  double getProperty(CameraProperty::Id propId);

  /*! \brief Get the real value of a Property which is set to auto.
   *  \remarks On all other properties it is the same as getProperty()
   */
  double getPropertyValue(CameraProperty::Id propId);


  ///! Set a Parameter in the cv::VideoCapture class
  bool setSetting(CameraSetting::Id settingId, double value);

  ///! Get a Parameter in the cv::VideoCapture class
  double getSetting(CameraSetting::Id settingId);

  /*! \brief Get the minimum possible value that can be set of the given property
   *  \param propId The id of the property
   */
  int getMinValue(CameraProperty::Id propId);

  /*! \brief Get the maximum possible value that can be set of the given property
   *  \param propId The id of the property
   */
  int getMaxValue(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property is supported by the used camera
   *  \param propId The id of the  property
   */
  bool isSupported(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property is readable by the used camera
   *  \param propId The id of the  property
   */
  bool isReadable(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property supports the OnePushAuto mode
   *  \remarks OnePushAuto is a special mode.
   *     It is used as follows: Set a value to a property and then to OnePushAuto mode.
   *     The camera now will try to hold this value automatically.
   *  \param propId The id of the  property
   */
  bool isOnePushCapable(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property could be turn off and on
   *  \param propId The id of the  property
   */
  bool isOnOffCapable(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property can be set to auto-mode
   *  \param propId The id of the  property
   */
  bool isAutoCapable(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property can be set manually
   *  \param propId The id of the  property
   */
  bool isManualCapable(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property can be set to an absolute value
   *  \param propId The id of the  property
   */
  bool isAbsoluteCapable(CameraProperty::Id propId);
  
  /*! \brief This method is called from the CameraGrabber when the configuration
   *   should be saved
   */
  bool saveConfiguration();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*! Set a property in the cv::VideoCapture class
   * implements the cv::VideoCapture.set() method with respect to concurrent access
   */ 
  bool setCamProperty(unsigned int propId, double value);

  /*! Get a property form the cv::VideoCapture
   * implements the cv::VideoCapture.get() method with respect to concurrent access
   */
  double getCamProperty(unsigned int propId);

  bool setAllParametersToCam();
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:

  /// @cond SKIPPED_DOCUMENTATION

  /*! \brief This is the struct, for the values of the camera settings
   *  \remarks This is the local storage for the camera settings
   *    and is only needed for ConfigurationInterface class
   */
  CameraSettings mCamSettings;

  /*! \brief This is the map, where all properties and their actual values stored in
   *  \remarks This is used to map those settings like CAMERA_PROPERTY_MODE_AUTO
   */
  CameraPropertyValues mCamPropertyValues;

  ///! The CameraCapability class manage the capabilities of the camera
  CameraCapabilitiesPtr mpCamCapabilities;

  ///! The CameraConfig class manage the properties and settings of the camera, i.e. the actual state
  CameraConfigPtr mpCamConfig;

  ///! The filename of the configuration file for camera settings
  std::string mConfigurationFileName;

  ///! The filename of the capabilities file for camera capabilities
  std::string mCapabilitiesFileName;

  ///! The channel number for this configuration. This is only used to display this information on the console
  unsigned int mChannel;

  ///! A short string, where the channel number is stored in.
  // Used as a prefix to store the properties in the configuration file
  std::string mChannelPrefix;


  ///! The already created cv::VideoCapture object from the CameraGrabber class
  cv::VideoCapture mVideoCapture;

  ///! The lock for concurrent access to the VideoCapture
  QReadWriteLockPtr mpVideoCaptureLock;

  ///! Internal flag for initialization period
  // Used to suppress warning messages in setProperty on startup
  bool mInitialization;

  /// @endcond

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:

}; // class cedar::dev::sensors::visual::CameraStateAndConfig

#endif // CEDAR_DEV_SENSORS_VISUAL_CAMERA_STATE_AND_CONFIG_H

