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

// PROJECT INCLUDES

// SYSTEM INCLUDES


/* Sony firewire camera capabilities:

OP   - one push capable
RC   - readout capable
O/OC - on/off capable
AC   - auto capable
MC   - manual capable
ABS  - absolute capable

 *
  Brightness:
    RC  MC  (active is: MAN)
    min: 0 max 255
    current value is: 200
  Exposure:
    RC  O/OC  MC  (active is: MAN)
    Feature: OFF  min: 1 max 255
    current value is: 1
  Sharpness:
    RC  MC  (active is: MAN)
    min: 0 max 255
    current value is: 0
  White Balance:
    RC  MC  (active is: AUTO)  AC  (active is: AUTO)  OP  (active is: AUTO)
    min: 1 max 255
    B/U value: 146 R/V value: 156
  Hue:
    RC  MC  (active is: MAN)
    min: 0 max 255
    current value is: 121
  Saturation:
    RC  MC  (active is: MAN)
    min: 0 max 255
    current value is: 123
  Gamma:
    RC  MC  (active is: MAN)
    min: 128 max 130
    current value is: 128
  Shutter:
    RC  MC  (active is: AUTO)  AC  (active is: AUTO)
    min: 1952 max 2575
    current value is: 2048
  Gain:
    RC  MC  (active is: AUTO)  AC  (active is: AUTO)
    min: 0 max 18
    current value is: 0
  Iris:
    RC  MC  (active is: AUTO)  AC  (active is: AUTO)
    min: 0 max 4095
    current value is: 2048
  Focus:
    RC  MC  (active is: MAN)
    min: 0 max 447
    current value is: 261
  Temperature:
    NOT AVAILABLE
  Trigger:
    RC  O/OC
    Feature: OFF
    AvailableTriggerModes: 384
    AvailableTriggerSources: none
    Polarity Change Capable: False
    Current Polarity: POS
    current mode: 384
  Trigger Delay:
    NOT AVAILABLE
  White Shading:
    NOT AVAILABLE
  Frame Rate:
    NOT AVAILABLE
  Zoom:
    RC  MC  (active is: MAN)
    min: 40 max 1432
    current value is: 40
  Pan:
    NOT AVAILABLE
  Tilt:
    NOT AVAILABLE
  Optical Filter:
    RC  MC  (active is: MAN)
    min: 0 max 1
    current value is: 0
  Capture Size:
    NOT AVAILABLE
  Capture Quality:
    NOT AVAILABLE
*/


/*! \class cedar::dev::sensors::visual::CameraGrabber
 *  \brief This grabber grabs images from firewire cameras
 *  \remarks This functionality is implemented by using OpenCV's
 *           cv::VideoCapture class. See their documentation for details about
 *           supported cameras.
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
                 bool isGuid = false,
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
                 bool isGuid = false,
                 bool finishInitialization = true
               );


  /*! \brief Constructor for a camera grabber. The complete configuration will be read from configuration file
   *  \param configFileName Filename for the configuration
   *  \param numCameras How many cameras you want to use
   *  \param finishInitialization Flag, if the initialization should be finished. Have a look at the CameraGrabber()
   *          Constructor for details
   *  \remarks
   *        If the system can't find the wanted camera, initialization will fail
   */
  /*CameraGrabber(
                 const std::string& configFileName,
                 unsigned int numCameras,
                 bool finishInitialization = true
               );
  */

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
                         CameraSetting::Id setting,
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
                           CameraSetting::Id setting
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
                      CameraVideoMode::Id mode
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
                     CameraFrameRate::Id fps
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
                          CameraIsoSpeed::Id isoSpeed
                        );

  /*! \brief Gets the actual ISO-Speed of the IEEE1394/firewire bus.
   *  \param channel This is the index of the source you want to set the parameter value.
   */
  CameraIsoSpeed::Id getCameraIsoSpeed(
                                        unsigned int channel
                                      );

   /*! \brief Set the framesize of the camera.
    *  \param channel This is the index of the source you want to set the parameter value.
    *  \param size The new value
    *
    * \remarks
    *   This can only be done, if the first frame wasn't already grabbed
    * \par
    *   If the framesize isn't supported by the actual video mode, a similar mode
    *   will be used. The return value will also be true in this case.
    *
    *   The framesize depends also on the video mode. So, you can adjust the size either
    *   by setting the frame-size or by the video-mode
    * \see getCameraFrameSize
    */

  bool setCameraFrameSize(
                           unsigned int channel,
                           cv::Size size
                         );

  /*! \brief Gets the actual framesize.
   *  \param channel This is the index of the source you want to set the parameter value.
   */
cv::Size getCameraFrameSize(
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

  /*! \brief Grab on all available files
   *  \remarks
   *      The shortest file determine the end
   *      In case of looping through the files, the shortest file define the restart moment
   */
  bool onGrab();

  bool onDeclareParameters();
  std::string onGetSourceInfo(
                               unsigned int channel
                             ) const;

  void onCleanUp();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)


protected:

  /*! \brief The Id's of the cameras
   *
   */
  std::vector<unsigned int> mCameraIds;

  /*! \brief This vector contains the needed captures.
   *    One for every camera.
   *  \see
   *    mCameraIds
   */
  //std::vector<cv::VideoCapture> mVideoCaptures;
  std::vector<cv::VideoCapture> mVideoCaptures;
private:

  bool mFinishInitialization;
  bool mCreateGrabberByGuid;
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


