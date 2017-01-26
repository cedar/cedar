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

    File:        VideoChannel.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 09 28

    Description: Class VideoChannel contains additional data of an Video-grabbing channel

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_VIDEO_CHANNEL_H
#define CEDAR_DEV_SENSORS_VISUAL_VIDEO_CHANNEL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/namespace.h"
#include "cedar/devices/sensors/visual/GrabberChannel.h"
#include "cedar/auxiliaries/FileParameter.h"

// SYSTEM INCLUDES


//!@brief VideoChannel contains additional data of a video grabbing channel
class cedar::dev::sensors::visual::VideoChannel
:
public cedar::dev::sensors::visual::GrabberChannel
{
  //!@brief friend class of VideoGrabber for direct access to the members
  friend class cedar::dev::sensors::visual::VideoGrabber;
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  VideoChannel(const std::string& fileName = "")
  :
  cedar::dev::sensors::visual::GrabberChannel(),
  _mSourceFileName(new cedar::aux::FileParameter(this, "filename", cedar::aux::FileParameter::READ, fileName))
  {
  };

  //!@brief Destructor
  virtual ~VideoChannel()
  {
  };

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

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
  //! @brief Read the video file with this object
  cv::VideoCapture mVideoCapture;

  //! @brief The filename of the video file to grab from
  cedar::aux::FileParameterPtr _mSourceFileName;

private:
  // none yet

}; // class cedar::dev::sensors::visual::VideoChannel

#endif // CEDAR_DEV_SENSORS_VISUAL_VIDEO_CHANNEL_H

