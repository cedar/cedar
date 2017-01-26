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

    File:        Channel.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Header for the cedar::dev::sensors::camera::Channel class

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
#ifndef Q_MOC_RUN
  #include <boost/enable_shared_from_this.hpp>
#endif


/*! @brief Additional data of a camera channel */
class cedar::dev::sensors::camera::Channel
:
public QObject,
public cedar::dev::sensors::visual::GrabberChannel
{

  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------

protected slots:

  /*! @brief A slot that is triggered from the used parameters if a setting has changed
   *
   *  If this slot is invoked, the changeSetting() signal is emitted.
   *  This slot is internally invoked if one of the fields Framerate, IsoSpeed or the CameraId is changed
   */
  void deviceChanged();

  //! @brief Slot invoked, if the grabmode is changed
  void grabModeChanged();

signals:

  /*!@brief This signal is emitted, when a different camera should be used.
   *
   * The CameraGrabber have to react on this signal, to recreate the backend with the new settings
   */
  void changeCamera();


  /*!@brief This signal is emitted, when a setting has changed.
   *
   * The CameraGrabber have to react on this signal. If the grabber is already working,
   * then a new grabbing-object with the new settings will be created. Otherwise this signal
   * could be ignored.
   */
  void changeSetting();


  // friend classes of this channel for direct access to the members
  friend class cedar::dev::sensors::camera::Grabber;
  friend class cedar::dev::sensors::camera::Backend;
  friend class cedar::dev::sensors::camera::BackendCvVideoCapture;
#ifdef CEDAR_USE_LIB_DC1394
  friend class cedar::dev::sensors::camera::BackendDc1394;
#endif // CEDAR_USE_LIB_DC1394


  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! The standard constructor.
  Channel
  (
   unsigned int cameraId = 0,
   bool byGuid = false,
   cedar::dev::sensors::camera::BackendType::Id backendType = cedar::dev::sensors::camera::BackendType::AUTO,
   cedar::dev::sensors::camera::Decoding::Id decodeFilter = cedar::dev::sensors::camera::Decoding::NONE
 );
  //! Destructor
  ~Channel();


  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief Get the ID of the used Camera
   *
   *  @return The id as unsigned int
   */
  unsigned int getCameraId() const;

  //! @brief Flag to indicate, if the grabber was created by using the GUID of the camera
  bool getByGuid() const;

  /*! @brief Get the actual used video mode of the camera
   *
   *  @return The used video mode as enum parameter
   */
  cedar::dev::sensors::camera::VideoMode::Id getVideoMode() const;

  /*! @brief Get the actual used framerate of the camera
   *
   *  @return The used framerate as enum parameter
   */
  cedar::dev::sensors::camera::FrameRate::Id getFramerate() const;

#ifdef CEDAR_USE_LIB_DC1394
  /*! @brief Get the actual used ISO-speed of the Firewire-bus
   *  @return The used ISO-speed as enum parameter
   *  @remarks This method is only available, if cedar was built with firewire support
   */
  cedar::dev::sensors::camera::IsoSpeed::Id getIsoSpeed() const;

  /*! @brief Set the actual used ISO-speed of the Firewire-bus
   *  @remarks This method is only available, if cedar was built with firewire support
   */
  void setIsoSpeed( cedar::dev::sensors::camera::IsoSpeed::Id isoSpeed);
#endif // CEDAR_USE_LIB_DC1394

  /*! @brief Set the camera id.
   * This will close the actual used camera, and the one with the given ID will be used
   *
   * @param CameraId The ID of the camera you want to use.
   *    It depends on isGuid, if this value is used as unique Identifier or as the number on the bus.
   * @param isGuid Flag that indicates, if CameraId is treated as bus-ID or as GUID
   *
   * @remarks
   *    The GUID is only supported from the firewire (DC1394) backend
   */
  void setCameraId(unsigned int CameraId, bool isGuid = false);

  /*! @brief Set a new framemode.
   *
   *   The videomode determins the size of the grabbed images and the colorspace
   *
   *  @param videoMode The wanted video mode as enum parameter id
   */
  void setVideoMode(cedar::dev::sensors::camera::VideoMode::Id videoMode);

  /*! @brief Set a new grabbing framerate to the camera
   *
   *   It depends on the camera, if this function is supported.
   *   Firewire cameras support different framerates out ot the box.
   *
   * @param fps The wanted framerate as enum parameter
   */
  void setFramerate(cedar::dev::sensors::camera::FrameRate::Id fps);

  /*! @brief Set a new backendtype to the used channel.
   *
   * This will also change some visible attributes to various parameters in the gui
   *
   * @param backendType The wanted backend as enum parameter
   */
  void setBackendType(cedar::dev::sensors::camera::BackendType::Id backendType);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:


  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //! @brief Method to all Videomodes related to DC1394 in the VideoMode enum parameter to disabled
  void hideFwVideoModes();

  /*! @brief Method to create the wanted backend which depends on the Backend enum parameter
   *  @throw cedar::dev::sensors::camera::CreateBackendException Thrown on an error, for example if the
   *         camera could not be found
   */
  void createBackend();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! @brief Camera interface
  cv::VideoCapture mVideoCapture;

  /*! @brief The lock for the concurrent access to the cv::VideoCapture.
   *
   *   Used in the CameraGrabber itself.
   *   Do not mix up with the capture lock from the CameraGrabber, which will be used to lock the grabbed images
   *   not the VideoCapture class
   */
  QReadWriteLock* mpVideoCaptureLock;

  /*! @brief The instance of the used backend
   *
   *  Only used until all parameters applied and the grabbing cv::VideoCapture object is created
   */
  cedar::dev::sensors::camera::BackendPtr mpBackend;


private:

  //! @brief The actual used backend for the camera
  cedar::dev::sensors::camera::BackendType::Id mBackendType;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //! @brief The backend to use
  cedar::aux::EnumParameterPtr _mpBackendType;

  //! @brief Set the the camera bayer pattern filter (if any is needed)
  cedar::aux::EnumParameterPtr _mpDecodeFilter;

  //! @brief Class for all properties
  cedar::dev::sensors::camera::PropertiesPtr mpProperties;


  //! @brief Create with guid or with bus id
  cedar::aux::BoolParameterPtr _mpByGuid;

  //! @brief Camera-ID. Either the Bus-ID or the GUID (depends on _mpByGuid)
  cedar::aux::UIntParameterPtr _mpCameraId;

  //! @brief Framesize as mode
  cedar::aux::EnumParameterPtr _mpGrabMode;

  //! @brief Framerate of grabbing
  cedar::aux::EnumParameterPtr _mpFPS;


#ifdef CEDAR_USE_LIB_DC1394
  //! @brief The iso-speed of the firewire bus
  cedar::aux::EnumParameterPtr _mpIsoSpeed;
#endif

private:
  // none yet

};

#endif // CEDAR_DEV_SENSORS_CAMERA_CHANNEL_H

