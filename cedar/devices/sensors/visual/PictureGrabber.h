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

    File:        PictureGrabber.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 01 08

    Description: Header for the @em cedar::dev::sensors::visual::PictureGrabber class.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSORS_VISUAL_PICTURE_GRABBER_H
#define CEDAR_DEV_SENSORS_VISUAL_PICTURE_GRABBER_H

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/Grabber.h"

// SYSTEM INCLUDES


/*! @class cedar::dev::sensors::visual::PictureGrabber
 *  @brief This grabber grabs images from a picture-file
 *  @remarks This functionality is implemented by using the OpenCV class
 *		cv::ImRead. See their documentation for details about
 *		supported image types.
 */
class cedar::dev::sensors::visual::PictureGrabber
:
public cedar::dev::sensors::visual::Grabber
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //!@cond SKIPPED_DOCUMENTATION

  /*! @struct PictureChannel
   *  @brief Additional data of a picture grabbing channel
   */
  struct PictureChannel
  :
  cedar::dev::sensors::visual::Grabber::GrabberChannel
  {
    //! @brief The filenames
    std::string mSourceFileName;
  };

  CEDAR_GENERATE_POINTER_TYPES(PictureChannel);

  //!@endcond

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief  Constructor for a single-file grabber
   *  @param configFileName	Filename for the configuration
   *  @param pictureFileName	Filename to grab from
   */
  PictureGrabber(const std::string& configFileName,const std::string& pictureFileName);

  /*! @brief Constructor for a stereo-file grabber
   *  @param configFileName		Filename for the configuration
   *  @param pictureFileName0	Filename to grab from for channel 0
   *  @param pictureFileName1	Filename to grab from for channel 1
   */
  PictureGrabber
  (
    const std::string& configFileName,
    const std::string& pictureFileName0,
    const std::string& pictureFileName1
  );

  /*! @brief Destructor */
  ~PictureGrabber();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief Set a new picture to grab from
   *  @remarks
   *	Supported image-types depend on operating system and installed libs. <br>
   *	For details look at the OpenCV-documentation (Section "imread").
   *  @param channel which should be changed.
   *  @param fileName of the new picture.
   *  @throws IndexOutOfRangeException If channel is't fit
   *  @throws InitializationException If the grabber couldn't grab from the file
   */
  void setSourceFile(unsigned int channel, const std::string& fileName);

  /*! @brief Set a new picture to grab from
   *   This is for a single channel grabber or for channel 0 on a stereor grabber
   *  @param fileName of the new picture.
   *  @throws IndexOutOfRangeException If channel is't fit
   *  @throws InitializationException If the grabber couldn't grab from the file
   *  @see setSourceFile(unsigned int, const std::string&)
   */

  void setSourceFile(const std::string& fileName);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  // inherited from Grabber
  bool onInit();
  bool onGrab();
  bool onDeclareParameters();
  void onUpdateSourceInfo(unsigned int channel);
  void onAddChannel();

  // inherited from NamedConfiguration
  void readConfiguration(const cedar::aux::ConfigurationNode& node);


  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /// @brief Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class PictureChannelPtr
  inline PictureChannelPtr getChannel(unsigned int channel)
  {
    return boost::static_pointer_cast<PictureChannel>
           (
             cedar::dev::sensors::visual::Grabber::mChannels.at(channel)
           );
  }

  /// @brief Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class PictureChannelPtr
  inline ConstPictureChannelPtr getChannel(unsigned int channel) const
  {
    return boost::static_pointer_cast<const PictureChannel>
           (
             cedar::dev::sensors::visual::Grabber::mChannels.at(channel)
           );
  }

  //!@brief The default name for the grabber
  virtual inline std::string defaultGrabberName() const
  {
    return "PictureGrabber";
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; //class cedar::dev::sensors::visual::PictureGrabber

#endif //CEDAR_DEV_SENSORS_VISUAL_PICTURE_GRABBER_H
