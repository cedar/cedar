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

    File:        CameraSettings.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Header for the cedar::dev::sensors::camera::Settings class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_CAMERA_CHANNEL_H
#define CEDAR_DEV_SENSORS_CAMERA_CHANNEL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/namespace.h"
#include "cedar/devices/sensors/camera/Grabber.h"

#include "cedar/devices/sensors/camera/backends/BackendType.h"
#include "cedar/devices/sensors/camera/enums/Decoding.h"
#include "cedar/devices/sensors/camera/enums/Setting.h"
#include "cedar/devices/sensors/camera/enums/VideoMode.h"
#include "cedar/devices/sensors/camera/enums/FrameRate.h"
#include "cedar/devices/sensors/camera/enums/IsoSpeed.h"

#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/Configurable.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>
#include <boost/enable_shared_from_this.hpp>


/*! @struct Channel
 *  @brief Additional data of a camera channel
 */
class cedar::dev::sensors::camera::Channel
:
public QObject,
public cedar::dev::sensors::visual::GrabberChannel
//public boost::enable_shared_from_this<cedar::dev::sensors::camera::Channel>
{

  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------

protected slots:

//!@brief A slot that is triggered if a setting has changed
  void settingChanged();


signals:

//!@brief This signal is emitted, when a setting has changed.
// The CameraGrabber class have to create a new grabber with the new settings.
  void settingsChanged();


  //!@brief friend class of GLGrabber for direct access to the members
  friend class cedar::dev::sensors::camera::Grabber;

  friend class cedar::dev::sensors::camera::Device;
  friend class cedar::dev::sensors::camera::DeviceCvVideoCapture;
#ifdef CEDAR_USE_LIB_DC1394
  friend class cedar::dev::sensors::camera::DeviceDc1394;
#endif // CEDAR_USE_LIB_DC1394




  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /// The standard constructor.
  Channel
  (
   unsigned int cameraId = 0,
   bool byGuid = false,
   cedar::dev::sensors::camera::BackendType::Id backendType = cedar::dev::sensors::camera::BackendType::AUTO,
   cedar::dev::sensors::camera::Decoding::Id decodeFilter = cedar::dev::sensors::camera::Decoding::NONE
 );
  /// Destructor
  ~Channel();


  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  // bool setSetting(cedar::dev::sensors::camera::Setting::Id settingId, double value);
  // double getSetting(cedar::dev::sensors::camera::Setting::Id settingId);

  unsigned int getCameraId();
  bool getByGuid();

  cedar::dev::sensors::camera::VideoMode::Id getVideoMode();
  cedar::dev::sensors::camera::FrameRate::Id getFPS();

#ifdef CEDAR_USE_LIB_DC1394
  cedar::dev::sensors::camera::IsoSpeed::Id getIsoSpeed();
  void setIsoSpeed( cedar::dev::sensors::camera::IsoSpeed::Id isoSpeed);
#endif // CEDAR_USE_LIB_DC1394

  void setCameraId(unsigned int CameraId, bool isGuid = false);

  void setVideoMode(cedar::dev::sensors::camera::VideoMode::Id videoMode);
  void setFPS(cedar::dev::sensors::camera::FrameRate::Id fps);

  /// Change visible attributes to various parameters of the settings-part
  void setBackendType(cedar::dev::sensors::camera::BackendType::Id backendType);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:


  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void hideFwVideoModes();
  void createBackend();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /// Camera interface
  cv::VideoCapture mVideoCapture;

  /*!@brief The lock for the concurrent access to the cv::VideoCapture.
   *
   *   Used in the CameraGrabber itself.
   *   Do not mix up with the capture lock from the CameraGrabber, which will be used to lock the grabbed images
   *   not the VideoCapture class
   */
  QReadWriteLock* mpVideoCaptureLock;

  /*! The istance of the used backend
   *
   *  Only used until all parameters applied and the grabbing cv::VideoCapture object is created
   */
  cedar::dev::sensors::camera::DevicePtr mpBackend;


private:
  cedar::dev::sensors::camera::BackendType::Id mBackendType;
  //unsigned int mCameraId;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:

  /// The backend to use
  cedar::aux::EnumParameterPtr _mpBackendType;

  /// Set the the camera bayer pattern filter (if any is needed)
  cedar::aux::EnumParameterPtr _mpDecodeFilter;


  /// Class for all settings
//  cedar::dev::sensors::camera::SettingsPtr mpSettings;

  /// Class for all properties
  cedar::dev::sensors::camera::PropertiesPtr mpProperties;


  /// create with guid or with bus id
  cedar::aux::BoolParameterPtr _mpByGuid;

  /// Camera-ID. Either the Bus-ID or the GUID (depends on _mpByGuid)
  cedar::aux::UIntParameterPtr _mpCameraId;

  /// framesize as mode
  cedar::aux::EnumParameterPtr _mpGrabMode;

  /// framerate of grabbing
  cedar::aux::EnumParameterPtr _mpFPS;


#ifdef CEDAR_USE_LIB_DC1394
  /// the iso-speed of the firewire bus
  cedar::aux::EnumParameterPtr _mpIsoSpeed;
#endif



private:
  // none yet

};


#endif // CEDAR_DEV_SENSORS_CAMERA_CHANNEL_H

