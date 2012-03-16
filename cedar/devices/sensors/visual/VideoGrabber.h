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

    File:        VideoGrabber.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Header for the @em cedar::dev::sensors::visual::VideoGrabber class.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSORS_VISUAL_VIDEO_GRABBER_H
#define CEDAR_DEV_SENSORS_VISUAL_VIDEO_GRABBER_H

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/GrabberInterface.h"

// SYSTEM INCLUDES


/*! \class cedar::dev::sensors::visual::VideoGrabber
 *  \brief This grabber grabs images from video-files
 *  \remarks This grabber will grab from all video-files known by OpenCV and/or ffmpeg
 *		Please look at their documentation for supported types (i.e. mpg, avi, ogg,...)
 */
class cedar::dev::sensors::visual::VideoGrabber
:
public cedar::dev::sensors::visual::GrabberInterface
{

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  /*! \struct VideoChannel
   *  \brief Additional data of a camera channel
   */
  struct VideoChannel
  :
  cedar::dev::sensors::visual::GrabberInterface::GrabberChannel
  {
    cv::VideoCapture videoCapture;              ///! Camera interface
    std::string sourceFileName;                 ///! Filename of video to grab from
  };

  typedef boost::shared_ptr<VideoChannel> VideoChannelPtr;




  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*!	\brief  Constructor for a single-file grabber
   *	\param configFileName	Filename for the configuration
   *  \param aviFileName		Filename to grab from
   */
  VideoGrabber(
                const std::string& configFileName,
                const std::string& aviFileName
              );

  /*!	\brief Constructor for a stereo-file grabber
   *	\param configFileName	Filename for the configuration
   *  \param aviFileName0	Filename to grab from for channel 0
   *  \param aviFileName1	Filename to grab from for channel 1
   */
  VideoGrabber(
                const std::string& configFileName,
                const std::string& aviFileName0,
                const std::string& aviFileName1
              );

  //!@brief Destructor
  ~VideoGrabber();


  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
 /*! \brief Turn looping on or off (i.e., restart on end) */
  void setLoop(
                bool loop = true
              );

  /*! \brief set the factor for grabbing speed
   *  \remarks the speed stored in the AVI-File will be multiplied with this factor.<br>
   *		Effective FPS should be SpeedFactor*AVI-Speed<br>
   *  \remarks
   *		The grabberthread have to be restarted to take effect.<br>
   *		This is done internally (by calling setFPS), but keep that in mind.
   *  \see
   *		setFPS
   */
  void setSpeedFactor(
                       double speedFactor
                     );

  /*! \brief Get the factor for grabbing speed
   *  \remarks the speed stored in the AVI-File will be multiplied with this factor
   *		so effective FPS should be _mSpeedFactor*AVI-Speed
   */
  double getSpeedFactor() const;

  /*! \brief Set postion in the AVI-Files relative
   *  \param newPositionRel New position in the range is from 0..1
   *  \remarks
   *		Range is from 0..1<br>
   *		For absolute positioning use setPositionAbs()
   */
  //todo Relative
  void setPositionRel(
                       double newPositionRel
                     );

  /*! \brief Return the actual position relative to the file size.
   *  \return Range is from 0..1
   */
  double getPositionRel();

  /*! \brief Set the position in the avi-files absolute
   *  \param newPositionAbs New position in the range is from 0..FrameCount
   *  \remarks
   *      For relative positioning use setPostionRel()
   */
  //todo Absolute
  void setPositionAbs(
                       unsigned int newPositionAbs
                     );

  /*! \brief Return the position in the file, i.e. the framenumber.
   *  \return Range is from 0..FrameCount
   */
  unsigned int getPositionAbs();

  /*! \brief Get the count of frames in the AVI
   *  \remarks
   *		In the case of a stereo grabber and different length
   *		the shortest avi-file determine the length
   */
  unsigned int getFrameCount() const;

  /*! \brief This passes the arguments directly to the corresponding capture
   *  \remarks With this Method, it is possible to get Information on any channel.
   *  \param channel This is the index of the source you want parameter value.< br >
   *  \param propId This is any supported property - Id<br>
   *	  If property-id is not supported or unknown, return value will be 0.
   *  \remarks Look at the OpenCV documentation for VideoCapture::get() for details
   */
  double getSourceProperty(
                            unsigned int channel,
                            int propId
                          );

  /*! \brief Get fps for the given channel. This value will be read from the appropriate video-file.
   *  \remarks
   *    Default channel is 0
   */
  double getSourceFps(
                       unsigned int channel = 0
                     );

  /*! \brief Get the codec for the given channel. This value will be read from the appropriate video-file.
   *  \remarks
   *    Default channel is 0. <br>
   *    This value is the FOURCC-code from the video-file.
   */
  double getSourceEncoding(
                            unsigned int channel = 0
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
  const std::string& onGetSourceInfo(unsigned int channel) const;
  void onCleanUp();
  void onAddChannel();

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
  
  /*! \brief Indicates if looping is on
   */
  bool _mLoop;

  /*! \brief Factor for grabbing speed
   *  \remarks the speed stored in the AVI-File will be multiplied with this factor
   *    so effective FPS should be _mSpeedFactor*AVI-Speed
   */
  double _mSpeedFactor;

  /*! \brief Count of frames of the shortest file
   *  \remarks
   *		Used for scrolling in the avi-file
   *  \see
   *		setPositionRel, getPositionRel, setPositionAbs, setPositionRel
   */
  unsigned int mFramesCount;




private:
  /*! Cast the storage vector from base channel struct "GrabberChannelPtr" to
   *  derived class VideoChannelPtr
   */
  inline VideoChannelPtr getChannel(unsigned int channel)
  {
    //!@todo: change to asserted_cast
    //return cedar::aux::asserted_cast<VideoChannelPtr>(mChannels.at(channel))
    return boost::static_pointer_cast<VideoChannel>
           (
             cedar::dev::sensors::visual::GrabberInterface::mChannels.at(channel)
           );
  }

  //!@todo: after merging change to ConstCameraChannelPtr
  /*! Cast the storage vector from base channel struct "GrabberChannelPtr" to
   *  derived class VideoChannellPtr
   */
  inline boost::shared_ptr<const VideoChannel> getChannel(unsigned int channel) const
  {
    //!@todo: change to asserted_cast
    //return cedar::aux::asserted_cast<VideoChannelPtr>(mChannels.at(channel))
    return boost::static_pointer_cast<const VideoChannel>
           (
             cedar::dev::sensors::visual::GrabberInterface::mChannels.at(channel)
           );
  }



  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::sensors::visual::VideoGrabber

#endif // CEDAR_DEV_SENSORS_VISUAL_VIDEO_GRABBER_H

