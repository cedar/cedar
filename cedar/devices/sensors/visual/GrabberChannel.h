/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        GrabberChannel.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 09 28

    Description: Class GrabberChannel

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_GRABBER_CHANNEL_H
#define CEDAR_DEV_SENSORS_VISUAL_GRABBER_CHANNEL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/namespace.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/FileParameter.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>



//! @brief Class to store all channel related stuff inside
class cedar::dev::sensors::visual::GrabberChannel
:
virtual public cedar::aux::Configurable
{
  //!@brief friend class of Grabber for direct access to the members
  friend class cedar::dev::sensors::visual::Grabber;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  GrabberChannel()
  :
  cedar::aux::Configurable(),
  mChannelInfo(""),
  _mSnapshotName(new cedar::aux::FileParameter(this,"snapshot file",cedar::aux::FileParameter::WRITE,"./snapshot.jpg")),
  _mRecordName(new cedar::aux::FileParameter(this, "record file",cedar::aux::FileParameter::WRITE,"./record.avi"))
  {
    //init classes
  };

  //!@brief Destructor
  virtual ~GrabberChannel()
  {
  };

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

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
  //! the picture frame
  cv::Mat mImageMat;

  //! for recordings
  cv::VideoWriter mVideoWriter;

  //! The channel information
  std::string mChannelInfo;

private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! Filename for snapshot
  cedar::aux::FileParameterPtr _mSnapshotName;

  //! Filename for recording
  cedar::aux::FileParameterPtr _mRecordName;

private:
  // none yet


}; // class cedar::dev::sensors::visual::GrabberChannel

#endif // CEDAR_DEV_SENSORS_VISUAL_GRABBER_CHANNEL_H


