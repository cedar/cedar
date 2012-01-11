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

// LOCAL INCLUDES
#include "GrabberInterface.h"
#include "CameraIsoSpeed.h"
#include "CameraProperty.h"
#include "CameraVideoMode.h"
#include "CameraFrameRate.h"
#include "CameraSetting.h"
#include "CameraCapabilities.h"
#include "CameraConfiguration.h"
#include "CameraConfigFileStorage.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//--------------------------------------------------------------------------------------------------------------------
//mappings from OpenCv constants
//return value, if a property is not supported from the actual device
#define CAMERA_PROPERTY_NOT_SUPPORTED -1

//sets a feature to auto-mode. The propery have to be auto_capable.
//Assigning a value to a feature, sets its mode to manual
#define CAMERA_PROPERTY_MODE_AUTO CV_CAP_PROP_DC1394_MODE_AUTO // =-2

//if a feature is on/off capable, this value turns it off
#define CAMERA_PROPERTY_MODE_OFF CV_CAP_PROP_DC1394_OFF //=-4

//this sets a property to its one_push_auto mode. The feature have to be one_push_auto capable.
//one_push_auto: set the property to the wanted value and then to one_push_auto.
//the camera will try to automatically hold this value
#define CAMERA_PROPERTY_MODE_ONE_PUSH_AUTO CV_CAP_PROP_DC1394_MODE_ONE_PUSH_AUTO //-1
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
  /*! \brief The Id's of the cameras
   *
   */
typedef struct CameraIdStruct
{
  unsigned int busId;
  unsigned int guid;
} CameraId;
//--------------------------------------------------------------------------------------------------------------------


/*! \class cedar::dev::sensors::visual::CameraGrabber
 *  \brief This grabber grabs images from a camera
 *  \remarks This functionality is implemented by using the OpenCV class cv::VideoCapture
 */
class cedar::dev::sensors::visual::CameraGrabber
:
public GrabberInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! \brief  Constructor for a camera grabber with one connected camera
   *  \param configFileName Filename for the configuration
   *  \param camera Device to grab from. Look at OpenCV cv::VideoCapture documentation for details
   *  \param isGuid This flag should be set, if the value in Parameter camera is the guid of the camera
   *  \param finishInitialization Flag, if the initialization should be finished. In this case it isn't possible to
   *         change the camera resolution or the camera framerate if you use a firewire camera. <br>
   *         The settings will be restored from the configfile. If there is no configfile,
   *         the best settings (i.e. highest framerate, largest picture size) will be used.
   *         If the finishInitialization flag isn't set, you can change this settings before you grab the first Frame.<br>
   *         But be aware, that getImage() will return empty matrices unless the first frame have been grabbed.
   *         This is done by starting the grabbing thread via the start() method or manually grab the first picture
   *         with grab().
   *  \remarks
   *    OpenCV selects the capture framework at the basis of the camera parameter. Use a multiple of 100 to select
   *    the interface or use the CV_CAP_xxx constants in /usr/local/include/opencv2/highgui/highgui_c.h for a
   *    base unit (like CV_CAP_FIREWIRE, CV_CAP_ANY)
   */
  CameraGrabber(
                 const std::string& configFileName,
                 unsigned int camera,
                 bool isGuid,
                 bool finishInitialization = true
               );


  /*! \brief Constructor for a stereo camera grabber
   *  \param configFileName Filename for the configuration
   *  \param camera0 Device to grab from for channel 0. Look at OpenCV cv::VideoCapture documentation for details
   *  \param camera1 Device to grab from for channel 1. Look at OpenCV cv::VideoCapture documentation for details
   *  \param isGuid This flag should be set, if the values in camera0 or camera1 are the guids of the cameras
   *  \param finishInitialization Flag, if the initialization should be finished. Have a look at the CameraGrabber()
   *          Constructor for details
   *  \see CameraGrabber() for details about the used framework
   */
  CameraGrabber(
                 const std::string& configFileName,
                 unsigned int camera0,
                 unsigned int camera1,
                 bool isGuid,
                 bool finishInitialization = true
               );


  /*! \brief Constructor for a camera grabber. The complete configuration will be read from configuration file
   *  \param configFileName Filename for the configuration
   *  \param numCameras How many cameras you want to use
   *  \param finishInitialization Flag, if the initialization should be finished. Have a look at the CameraGrabber()
   *          Constructor for details
   *  \remarks
   *        If the system can't find the wanted camera, initialization will fail.
   *  \par
   *        The camera always will be initialized (i.e. the first frame is already grabbed on initialization)
   */
  CameraGrabber(
                 const std::string& configFileName,
                 unsigned int numCameras
               );


  /*! \brief Destructor */
  ~CameraGrabber();


  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*!  \brief With this method, it is possible to get Information on any channel.
   *   \remarks This method passes the arguments directly to the corresponding capture device
   *   \param channel This is the index of the source you want to get the parameter value.
   *   \param propId This is any supported property-Id<br>
   *     If property-id is not supported or unknown, return value will be -1.
   *   \remarks Have a look at the OpenCV documentation for VideoCapture::get() for details on supported parameters
   *           or look at /usr/local/include/opencv2/highgui/highgui_c.h
   */
  double getCameraProperty(
                            unsigned int channel,
                            CameraProperty::Id propId
                          );

  /*  \brief Get informations on camera 0
   *  \see getCameraProperty(unsigned int, CameraParam_t)
   *  \param propId This is any supported property-Id<br>
   *     If property-id is not supported or unknown, return value will be -1.
   */
  double getCameraProperty(
                            CameraProperty::Id propId
                          );


  /*!  \brief With this method, it is possible to set Information on any channel.
   *   \remarks This method passes the arguments directly to the corresponding capture device
   *   \param channel This is the index of the source you want to set the parameter value.
   *   \param propId This is any supported property-Id<br>
   *     If property-id is not supported or unknown, return value will be false.
   *   \param value This is the new value.
   *   \remarks Have a look at the OpenCV documentation for VideoCapture::get() for details on supported parameters
   *           or look at /usr/local/include/opencv2/highgui/highgui_c.h
   */
  bool setCameraProperty(
                          unsigned int channel,
                          CameraProperty::Id propId,
                          double value
                        );
  
  /*  \brief Set informations on camera 0
   *  \see setCameraProperty(unsigned int, CameraParam_t)
   *   \param propId This is any supported property-Id<br>
   *     If property-id is not supported or unknown, return value will be false.
   *   \param value This is the new value.
   */
  bool setCameraProperty(
                          CameraProperty::Id propId,
                          double value
                        );

  /*! \brief Set values on the camera which have to be adjusted before the first image will be grabbed
   *  \param channel This is the index of the source you want to set the parameter value.
   *  \param setting The id of the setting you want to change
   *  \param value The new value
   *  \remarks
   *      This method can be used to directly set such a feature (Mode, Fps, IsoSpeed, FrameSize). <br>
   *      It is advised to use the enum-style getCamera... and setCamera... methods.
   *  \see  setCameraMode, setCameraFps, setCameraIsoSpeed, setCameraFrameSize
   */
  bool setCameraSetting(
                         unsigned int channel,
                         CameraSetting::Id settingId,
                         double value
                       );

  /*! \brief Get values of the camera which have to be adjusted before the first image will be grabbed
   *  \param channel This is the index of the source you want to set the parameter value.
   *  \param setting The id of the setting you want to change
   *
   *  \remarks
   *      This method can be used to directly get such a feature (Mode, Fps, IsoSpeed, FrameSize) as double <br>
   *      It is advised to use the enum-style getCamera... and setCamera... methods.
   *  \see  setCameraMode, setCameraFps, setCameraIsoSpeed, setCameraFrameSize
   */
  double getCameraSetting(
                           unsigned int channel,
                           CameraSetting::Id settingId
                         );

  /*! \brief Set the video mode of the camera.
   *  \param channel This is the index of the source you want to set the parameter value.
   *  \param mode The new value
   *
   * \remarks
   *   This can only be done, if the first frame wasn't already grabbed
   * \see getCameraMode
   */
  bool setCameraMode(
                      unsigned int channel,
                      CameraVideoMode::Id modeId
                    );

  /*! \brief Gets the actual mode.
   *  \param channel This is the index of the source you want to set the parameter value.
   */
  CameraVideoMode::Id getCameraMode(
                                     unsigned int channel
                                   );

  /*! \brief Set the framerate of the camera.
   *  \param channel This is the index of the source you want to set the parameter value.
   *  \param fps The new value
   *
   * \remarks
   *   This can only be done, if the first frame wasn't already grabbed
   * \par
   *   If the framerate isn't supported by the actual video mode, a similar mode
   *   will be used. The return value will also be true in this case.
   * \see setCameraFps
   */
  bool setCameraFps(
                     unsigned int channel,
                     CameraFrameRate::Id fpsId
                   );

  /*! \brief Gets the actual fps of the camera.
   *  \param channel This is the index of the source you want to set the parameter value.
   */
  CameraFrameRate::Id getCameraFps(
                                    unsigned int channel
                                  );

  /*! \brief Set the ISO-speed of the IEEE1394/firewire bus.
   *  \param channel This is the index of the source you want to set the parameter value.
   *  \param isoSpeed The new value
   *
   * \remarks
   *   This can only be done, if the first frame wasn't already grabbed
   * \par
   *   If the wanted ISO-speed isn't supported by the camera, a similar mode
   *   will be used. The return value will also be true in this case.
   * \see setCameraIsoSpeed
   */
  bool setCameraIsoSpeed(
                          unsigned int channel,
                          CameraIsoSpeed::Id isoSpeedId
                        );

  /*! \brief Gets the actual ISO-Speed of the IEEE1394/firewire bus.
   *  \param channel This is the index of the source you want to set the parameter value.
   */
  CameraIsoSpeed::Id getCameraIsoSpeed(
                                        unsigned int channel
                                      );

  /*! \brief Gets the GUID of the camera
   *  \param channel This is the index of the source channel
   */
  unsigned int getCameraGuid(
                              unsigned int channel
                            );


  /*! \brief Gets the actual framesize.
   *  \param channel This is the index of the source channel.
   */
  cv::Size getCameraFrameSize(
                               unsigned int channel
                             );

  /*! \brief Write out all properties
   *  \remarks
   *      The properties are read directly from cam
   *  \param channel This is the index of the source you want to print the properties.
   */
  void printAllProperties(
                           unsigned int channel
                         );

  /*! \brief Write out all settings
   *  \remarks
   *      The settings are read directly from cam
   *  \param channel This is the index of the source you want to print the settings.
   */
  void printAllSettings(
                         unsigned int channel
                       );


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  
  //------------------------------------------------------------------------
  //From GrabberInterface
  //------------------------------------------------------------------------

  bool onInit();

  bool onGrab();

  bool onDeclareParameters();
  std::string onGetSourceInfo(
                               unsigned int channel
                             ) const;

  ///! \brief Sync all Parameters from cameras with the local buffer
  bool onWriteConfiguration();

  void onCleanUp();


  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:



  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------

public:
  // none yet (hopefully never!)


protected:

  /*! \brief The Id's of the cameras
   *  \remarks
   *      this contains the device number on the bus and the guid
   */
  std::vector<CameraId> mCamIds;

  /*! \brief This vector contains the needed captures.
   *    One for every camera.
   *  \see
   *    mCameraIds
   */
  std::vector<cv::VideoCapture> mVideoCaptures;
  

  //the manager of all settings and properties. One for each camera
  //std::vector<CameraConfiguration> mCamConfigurations;
  std::vector<CameraConfigurationPtr> mCamConfigurations;


private:

  bool mFinishInitialization;
  bool mCreateGrabberByGuid;
  //bool mCreateFromConfigFile; // get guid/busId from configfile
  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::sensors::visual::CameraGrabber

#endif // CEDAR_DEV_SENSORS_VISUAL_CAMERA_GRABBER_H


