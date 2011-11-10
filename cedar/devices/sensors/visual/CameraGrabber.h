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

// PROJECT INCLUDES

// SYSTEM INCLUDES



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

  /*! \brief  Constructor for a single-file grabber
   *  \param configFileName Filename for the configuration
   *  \param Camera    Filename to grab from. Look at OpenCV cv::VideoCapture documentation for details
   */
  CameraGrabber(
                 const std::string& configFileName,
                 unsigned int Camera
               );


  /*! \brief Constructor for a stereo-file grabber
   *  \param configFileName Filename for the configuration
   *  \param Camera0 Device to grab from for channel 0. Look at OpenCV cv::VideoCapture documentation for details
   *  \param Camera1 Device to grab from for channel 1. Look at OpenCV cv::VideoCapture documentation for details
   */
  CameraGrabber(
                 const std::string& configFileName,
                 unsigned int Camera0,
                 unsigned int Camera1
               );

  /*! \brief Destructor */
  ~CameraGrabber();


  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! \brief This passes the arguments directly to the corresponding capture device
   *   \remarks With this Method, it is possible to get Information on any channel.
   *   \param channel This is the index of the source you want to get the parameter value.<br>
   *   \param propId This is any supported property-Id<br>
   *     If property-id is not supported or unknown, return value will be 0.
   *   \remarks see OpenCV documentation for VideoCapture::get() for details on supported parameters
   */
  double getCameraParam(
                         unsigned int channel,
                         int propId
                       );



  /*! \brief Get fps of the given camera
   *  \remarks
   *    Default channel is 0
   */
  double getCameraParamFps (unsigned int channel=0) ;

  /*! \brief Get fourcc of the given camera
   *  \remarks
   *    Default channel is 0
   */
  double getCameraParamEncoding (unsigned int channel=0);

  double getCameraParamBrightness (unsigned int channel=0);
  double getCameraParamContrast (unsigned int channel=0);
  double getCameraParamSaturation (unsigned int channel=0);
  double getCameraParamHue (unsigned int channel=0);
  double getCameraParamGain (unsigned int channel=0);
  double getCameraParamExposure (unsigned int channel=0);


  bool setCameraParam (unsigned int channel,int propId, double value);
  
  bool setCameraParamFps (unsigned int channel, double value);
  bool setCameraParamEncoding (unsigned int channel, double value);
  bool setCameraParamBrightness (unsigned int channel, double value);
  bool setCameraParamContrast (unsigned int channel, double value);
  bool setCameraParamSaturation (unsigned int channel, double value);
  bool setCameraParamHue (unsigned int channel, double value);
  bool setCameraParamGain (unsigned int channel, double value);
  bool setCameraParamExposure (unsigned int channel, double value);

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
  std::vector<unsigned int> mCameraId;


  /*! \brief This vector contains the needed captures.
   *    One for every camera.
   *  \see
   *    mCameraId
   */
  std::vector<cv::VideoCapture> mCaptureVector;
private:
  // none yet

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


