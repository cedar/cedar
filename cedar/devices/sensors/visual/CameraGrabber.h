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
   *  \param grabberName    The name for this grabber
   *  \param configFileName Filename for the configuration
   *  \param aviFileName    Filename to grab from
   */
  CameraGrabber(
                const std::string& configFileName,
                unsigned int Camera0
               );


  /*! \brief Constructor for a stereo-file grabber
   *  \param grabberName    The name for this grabber
   *  \param configFileName Filename for the configuration
   *  \param aviFileName0   Filename to grab from for channel 0
   *  \param aviFileName1   Filename to grab from for channel 1
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

  /*! \brief This passes the arguments directly to the corresponding capture
   *   \remarks With this Method, it is possible to get Information on any channel.
   *   \param channel This is the index of the source you want parameter value.<br>
   *   \param propId This is any supported property-Id<br>
   *     If property-id is not supported or unknown, return value will be 0.
   *   \remarks see OpenCV documentation for VideoCapture::get() for details
   */
  double getCameraParam(
                        unsigned int channel,
                        int          propId
                       );



  /*! \brief Get fps for the given camera
   *  \remarks
   *    Default channel is 0
   */
  //double getSourceFps (unsigned int channel=0) ;

  /*! \brief Get fourcc for the given camera
   *  \remarks
   *    Default channel is 0
   */
  //double getSourceEncoding (unsigned int channel=0);

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

  virtual bool onDestroy();

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


