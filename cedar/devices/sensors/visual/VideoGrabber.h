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
    along with cedar. If not, see <http:// www.gnu.org/licenses/>.

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

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/Grabber.h"
#include "cedar/devices/sensors/visual/VideoChannel.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/IntParameter.h"
#include "cedar/auxiliaries/FileParameter.h"

// SYSTEM INCLUDES
#include <string>


/*! @brief This grabber grabs images from video-files
 *
 *    This grabber will grab from all video-files known by OpenCV and/or ffmpeg
 *    Please look at their documentation for supported types (i.e. mpg, avi, ogg,...)
 */
class cedar::dev::sensors::visual::VideoGrabber
:
public cedar::dev::sensors::visual::Grabber
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // slots and signals
  //--------------------------------------------------------------------------------------------------------------------

  protected slots:

  //! @brief A slot that is triggered if a new filename is set
  void fileNameChanged();

  //! @brief A slot that is triggered if the speedfactor is set
  void speedFactorChanged();

  signals:

  //! @brief This signal is emitted, when a new videofile is successfully opened.
  void doVideoChanged();

  //! @brief This signal is emitted, when the speedFactor is changed.
  void doSpeedFactorChanged();

  private:
  /*! @brief Boost slot method. Invoked if a channel is added as an ObjectListParameter as an object
   */
  void channelAdded(int index);

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------


  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief  Constructor for a single-file grabber
   *  @param videoFileName  Filename to grab from
   *  @param looped Flag, if the grabbing should restart at the end of the video file. Default is true
   *  @param speedFactor Grab at an other framerate as the one from the video file. Default is one
   */
  VideoGrabber
  (
    const std::string& videoFileName = "",
    bool looped = true,
    double speedFactor = 1.0
  );

  /*! @brief Constructor for a stereo-file grabber
   *  @param videoFileName0 Filename to grab from for channel 0
   *  @param videoFileName1 Filename to grab from for channel 1
   *  @param looped Flag, if the grabbing should restart at the end of the video file. Default is true
   *  @param speedFactor Grab at an other framerate as the one from the video file. Default is one
   */
  VideoGrabber
  (
    const std::string& videoFileName0,
    const std::string& videoFileName1,
    bool looped = true,
    double speedFactor = 1.0
  );

  //!@brief Destructor
  ~VideoGrabber();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
 /*! @brief Turn looping on or off (i.e., restart on end) */
  void setLooped(bool loop = true);

  //! @brief Check if looping is on or off
  bool getLooped() const;

  /*! @brief Set the factor for grabbing speed
   *
   *   The speed stored in the AVI-File will be multiplied with this factor.<br>
   *    Effective FPS should be SpeedFactor*AVI-Speed<br>
   *  @remarks
   *    The grabber thread have to be restarted to take setLooped effect.<br>
   *    This is done internally (by calling setFPS), but keep that in mind.
   *  @see
   *    setFPS
   */
  void setSpeedFactor(double speedFactor);

  /*! @brief Get the factor for grabbing speed
   *
   *    The speed stored in the AVI-File will be multiplied with this factor
   *    so effective FPS should be _mSpeedFactor*AVI-Speed
   */
  double getSpeedFactor() const;

  /*! @brief Set postion in the AVI-Files relative
   *
   *    Range is from 0..1<br>
   *    For absolute positioning use setPositionAbsolute()
   *  @param newPositionRel New position in the range is from 0..1
   */
  void setPositionRelative(double newPositionRel);

  /*! @brief Return the actual position relative to the file size.
   *  @return Range is from 0..1
   */
  double getPositionRelative();

  /*! @brief Set the position in the avi-files to an absolute position
   *
   *      For relative positioning use setPostionRel()
   *  @param newPositionAbs New position in the range is from 0..FrameCount
   */
  void setPositionAbsolute(unsigned int newPositionAbs);

  /*! @brief Returns the position in the file, i.e. the framenumber.
   *  @return Range is from 0..FrameCount
   */
  unsigned int getPositionAbsolute();

  /*! @brief Get the count of frames in the AVI
   *
   *    In the case of a stereo grabber and different length,
   *    the shortest avi-file determine the length
   */
  unsigned int getFrameCount() const;

  /*! @brief This passes the arguments directly to the corresponding capture
   *
   *    With this Method, it is possible to get Information on any channel.
   *  @param channel This is the index of the source you want parameter value.< br >
   *  @param propId This is any supported property - Id<br>
   *    If property-id is not supported or unknown, return value will be 0
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *
   *  @remarks Look at the OpenCV documentation for VideoCapture::get() for details
   */
  double getSourceProperty(unsigned int channel, int propId);

  /*! @brief Get fps for the given channel. This value will be read from the appropriate video-file.
   *
   *    Default channel is 0
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   */
  double getSourceFramerate(unsigned int channel = 0);

  /*! @brief Get the codec for the given channel. This value will be read from the appropriate video-file.
   *
   *    Default channel is 0. <br>
   *    This value is the FOURCC-code from the video-file.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   */
  double getSourceEncoding(unsigned int channel = 0);

  /*! @brief Get the used file to grab from
   *  @param channel The channel which filename should be read
   *  @throw IndexOutOfRangeException If channel isn't fit
   */
  const std::string getSourceFile(unsigned int channel = 0) const;


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  // inherited from Grabber
  void onCreateGrabber();
  void onCloseGrabber();
  std::string onGetSourceInfo(unsigned int channel);
  void onCleanUp();

  /*! @brief Grab on all available files
   *
   *     The shortest file determine the end.<br>
   *     In case of looping through the files, the shortest file define the restart moment
   *     The image matrix in getImage() could be empty also if the return value is true.
   *     This happens if looped is false and all frames grabbed from the file, i.e. it is over.
   *
   *  @param channel The channel, which should be grabbed
   */
  void onGrab(unsigned int channel);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*! @brief This function does internal variable initialization in  constructor
   */
  void init(double speedFactor);


  /*! @brief Cast the storage vector from base channel struct "GrabberChannelPtr" to
   *  derived class VideoChannelPtr
   */
  inline VideoChannelPtr getVideoChannel(unsigned int channel)
  {
    return boost::static_pointer_cast<VideoChannel>
           (
             cedar::dev::sensors::visual::Grabber::_mChannels->at(channel)
           );
  }

  /*! @brief Cast the storage vector from base channel struct "GrabberChannelPtr" to
   *  derived class VideoChannellPtr
   */
  inline ConstVideoChannelPtr getVideoChannel(unsigned int channel) const
  {
    return boost::static_pointer_cast<const VideoChannel>
           (
             cedar::dev::sensors::visual::Grabber::_mChannels->at(channel)
           );
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  
  /*! @brief Count of frames of the shortest file
   *
   *    Used for scrolling in the avi-file
   *  @see
   *    setPositionRelative, getPositionRelative, setPositionAbsolute, setPositionRelative
   */
  unsigned int mFramesCount;

private:
  // none yet


  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! @brief Indicates if looping is on
  cedar::aux::BoolParameterPtr _mLooped;

  /*! @brief Factor for grabbing speed
   *  @remarks the speed stored in the AVI-File will be multiplied with this factor
   *    so effective FPS should be _mSpeedFactor*AVI-Speed
   */
  cedar::aux::DoubleParameterPtr _mSpeedFactor;

}; // class cedar::dev::sensors::visual::VideoGrabber

#endif // CEDAR_DEV_SENSORS_VISUAL_VIDEO_GRABBER_H

