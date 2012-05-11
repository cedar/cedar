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
#include "cedar/processing/sources/namespace.h"
#include "cedar/processing/Step.h"
#include "cedar/devices/sensors/visual/VideoGrabber.h"
#include "cedar/auxiliaries/ImageData.h"
#include "cedar/auxiliaries/FileParameter.h"
//#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/units/TimeUnit.h"


// SYSTEM INCLUDES


//!@brief A video file  source for the processingIde
class cedar::proc::sources::Video : public cedar::proc::Step
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

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

public slots:

  //!@brief a slot that is triggered if a new filename should be set
  void setFileName();

  //!@brief Set looping on or off
  void setLoop();

  //!@brief Sets a new configuration filename
  void setConfigurationFileName();

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
  void onStart();
  void reset();

  //create a new grabber instance
  void createGrabber();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief the actually grabbed frame
  cedar::aux::ImageDataPtr mImage;

  //!@brief the grabber
  cedar::dev::sensors::visual::VideoGrabberPtr mGrabber;

  //!@brief the time in ms between two frames. Depends on the the framerate of the video
  //!@todo: change to cedar::unit::Time
  cedar::unit::Milliseconds mFrameDuration;

  //!@brief the time elapsed since the last frame is displayed
  //!@todo: change to cedar::unit::Time
  cedar::unit::Milliseconds mTimeElapsed;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet


private:
  //!@brief the filename to grab from
  cedar::aux::FileParameterPtr _mFileName;

  //!@brief Looping through the video-file
  cedar::aux::BoolParameterPtr _mLoop;

  //!@brief The configuration filename
  cedar::aux::FileParameterPtr _mConfigurationFileName;


}; // class cedar::proc::sources::Video

#endif // CEDAR_PROC_SOURCES_VIDEO_H


