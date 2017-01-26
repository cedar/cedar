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

    File:        Video.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.ruhr-uni-bochum.d
    Date:        2012 04 20

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_SOURCES_VIDEO_H
#define CEDAR_PROC_SOURCES_VIDEO_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/sources/GrabberBase.h"
#include "cedar/processing/Step.h"
#include "cedar/devices/sensors/visual/VideoGrabber.h"
#include "cedar/auxiliaries/FileParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/units/Time.h"

// FORWARD DECLARATIONS
#include "cedar/processing/sources/Video.fwd.h"

// SYSTEM INCLUDES


//!@brief A video file source for the processing framework.
class cedar::proc::sources::Video
:
public cedar::proc::sources::GrabberBase
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Video();

  //!@brief Destructor
  ~Video();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

public slots:

  //!@brief This slot should be invoked, when the video in the VideoGrabber has changed.
  void updateVideo(bool emitOutputPropertyChanged = true);

  //!@brief This slot should be invoked, when the speed factor in the VideoGrabber has changed.
  void updateSpeedFactor();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void compute(const cedar::proc::Arguments&);
  void reset();

  //!@brief Cast the base GrabberBasePtr to derived class VideoGrabberPtr
  inline cedar::dev::sensors::visual::VideoGrabberPtr getVideoGrabber()
  {
    return boost::static_pointer_cast<cedar::dev::sensors::visual::VideoGrabber>
           (
             this->cedar::proc::sources::GrabberBase::mpGrabber
           );
  }

  //!@brief Cast the base GrabberBasePtr to derived class VideoGrabberPtr
  inline cedar::dev::sensors::visual::ConstVideoGrabberPtr getVideoGrabber() const
  {
    return boost::static_pointer_cast<const cedar::dev::sensors::visual::VideoGrabber>
           (
            cedar::proc::sources::GrabberBase::mpGrabber
           );
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief the time between two frames. Depends on the frame rate of the video
  cedar::unit::Time mFrameDuration;

  //!@brief the time elapsed since the last frame is displayed
  cedar::unit::Time mTimeElapsed;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet
}; // class cedar::proc::sources::Video
#endif // CEDAR_PROC_SOURCES_VIDEO_H

