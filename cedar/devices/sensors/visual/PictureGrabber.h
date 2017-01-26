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

    File:        PictureGrabber.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 01 08

    Description: Header for the @em cedar::dev::sensors::visual::PictureGrabber class.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSORS_VISUAL_PICTURE_GRABBER_H
#define CEDAR_DEV_SENSORS_VISUAL_PICTURE_GRABBER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/Grabber.h"
#include "cedar/devices/sensors/visual/PictureChannel.h"
#include "cedar/auxiliaries/FileParameter.h"

// SYSTEM INCLUDES
#include <string>


/*! @class cedar::dev::sensors::visual::PictureGrabber
 *  @brief This grabber grabs images from a picture-file
 *  @remarks This functionality is implemented by using the OpenCV class
 *    cv::ImRead. See their documentation for details about
 *    supported image types.
 */
class cedar::dev::sensors::visual::PictureGrabber
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


  signals:

  //! @brief This signal is emitted, when a new picture is available with the getImage() method.
  void pictureChanged();

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
   *  @param pictureFileName  Filename to grab from
   */
  PictureGrabber
  (
    const std::string& pictureFileName = ""
  );

  /*! @brief Constructor for a stereo-file grabber
   *  @param pictureFileName0  Filename to grab from for channel 0
   *  @param pictureFileName1  Filename to grab from for channel 1
   */
  PictureGrabber
  (
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
   *    Supported image-types depend on operating system and installed libs. <br>
   *    For details look at the OpenCV-documentation (Section "imread").
   *  @param channel which should be changed.
   *  @param fileName of the new picture.
   *  @throw IndexOutOfRangeException If channel isn't fit
   *  @throw InitializationException If the grabber couldn't grab from the file
   */
  void setSourceFile(unsigned int channel, const std::string& fileName);

  /*! @brief Set a new picture to grab from
   *    This is for a single channel grabber or for channel 0 on a stereo grabber
   *  @param fileName of the new picture.
   *  @throw IndexOutOfRangeException If channel isn't fit
   *  @throw InitializationException If the grabber couldn't grab from the file
   *  @see setSourceFile(unsigned int, const std::string&)
   */
  void setSourceFile(const std::string& fileName);

  /*! @brief Get the used file to grab from
   *  @param channel The channel which filename should be read
   *  @throw  IndexOutOfRangeException If channel isn't fit
   */
  const std::string getSourceFile(unsigned int channel = 0) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  // inherited from Grabber
  void onGrab(unsigned int channel);
  void onCreateGrabber();
  void onCloseGrabber();
  std::string onGetSourceInfo(unsigned int channel);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*! @brief This function does internal variable initialization in  constructor
   */
  void init();

  //! @brief Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class PictureChannelPtr
  inline PictureChannelPtr getPictureChannel(unsigned int channel)
  {
    return boost::static_pointer_cast<PictureChannel>
           (
             cedar::dev::sensors::visual::Grabber::_mChannels->at(channel)
           );
  }

  //! @brief Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class PictureChannelPtr
  inline ConstPictureChannelPtr getPictureChannel(unsigned int channel) const
  {
    return boost::static_pointer_cast<const PictureChannel>
           (
             cedar::dev::sensors::visual::Grabber::_mChannels->at(channel)
           );
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

}; // class cedar::dev::sensors::visual::PictureGrabber

#endif // CEDAR_DEV_SENSORS_VISUAL_PICTURE_GRABBER_H
