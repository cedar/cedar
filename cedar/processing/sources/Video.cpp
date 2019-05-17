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

    File:        Video.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.ruhr-uni-bochum.d
    Date:        2012 04 20

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/sources/Video.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/StepTime.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#include <sstream>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration
    (
      new ElementDeclarationTemplate<cedar::proc::sources::Video>
      (
        "Sources",
        "cedar.processing.sources.Video"
      )
    );
    declaration->setIconPath(":/steps/video_grabber.svg");
    declaration->setDescription
    (
      "Reads a video file and outputs the frames. Note: Supported formats depend on your version of OpenCV."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::sources::Video::Video()
:
cedar::proc::sources::GrabberBase(),
mFrameDuration(0.0 * cedar::unit::seconds),
mTimeElapsed(0.0 * cedar::unit::seconds)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  // on creation, create also the videograbber. It doesn't matter if the file is valid.
  cedar::dev::sensors::visual::VideoGrabberPtr grabber;
  grabber = cedar::dev::sensors::visual::VideoGrabberPtr
            (
               new cedar::dev::sensors::visual::VideoGrabber()
            );

  //no exception here, so we could use it
  this->mpGrabber = grabber;

  this->addConfigurableChild("VideoGrabber", this->getVideoGrabber());
  this->declareOutput("Video", mImage);

  QObject::connect(this->getVideoGrabber().get(), SIGNAL(doVideoChanged()), this, SLOT(updateVideo()));
  QObject::connect(this->getVideoGrabber().get(), SIGNAL(doSpeedFactorChanged()), this, SLOT(updateSpeedFactor()));

  const std::string file_name = this->getVideoGrabber()->getSourceFile();
  if ( ! (file_name == "." || file_name == "") )
  {
    this->reset();
  }

}

cedar::proc::sources::Video::~Video()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::Video::reset()
{
  if (this->getVideoGrabber()->applyParameter())
  {
    // assuming that a reset does not change the output size, do not emit a (blocking) signal
    updateVideo(false);
  }
  else
  {
    const std::string name = mpGrabber->getName();
    std::stringstream error_message;
    error_message << "Couldn't load video file: "  << this->getVideoGrabber()->getSourceFile(0) << std::endl;
    cedar::aux::LogSingleton::getInstance()->error
                                             (
                                               name + ": " + error_message.str(),
                                               "cedar::dev::sensors::visual::Video::Video()"
                                             );
  }
}


void cedar::proc::sources::Video::compute(const cedar::proc::Arguments &arguments)
{
  if (this->getVideoGrabber()->isCreated())
  {
    // grab a new frame only if FrameDuration is larger than fps of the video
    try
    {
      const cedar::proc::StepTime& step_time = dynamic_cast<const cedar::proc::StepTime&>(arguments);
      const cedar::unit::Time& t = step_time.getStepTime();
      this->mTimeElapsed += t;
    }
    catch (const std::bad_cast& e)
    {
      CEDAR_THROW(cedar::proc::InvalidArgumentsException, "Bad arguments passed to dynamics. Expected StepTime.");
    }

    //!@todo: scroll forward, if fps is much larger than steptime
    if (this->mTimeElapsed > mFrameDuration)
    {
      this->getVideoGrabber()->grab();
      this->mImage->setData(this->getVideoGrabber()->getImage());
      this->mTimeElapsed -= mFrameDuration;
    }
  }
}


void cedar::proc::sources::Video::updateVideo(bool emitOutputPropertyChanged)
{
  this->mImage->setData(this->getVideoGrabber()->getImage());
  //!@todo fix getFps() to include frequency as unit
  mFrameDuration = 1.0 / this->getVideoGrabber()->getFramerate() * cedar::unit::seconds;
  mTimeElapsed = 0.0 * cedar::unit::seconds;
  mRecording->setValue(this->getVideoGrabber()->isRecording());
  this->annotateImage();
  if (emitOutputPropertyChanged)
  {
    this->emitOutputPropertiesChangedSignal("Video");
  }
}

void cedar::proc::sources::Video::updateSpeedFactor()
{
  //!@todo fix getFps() to include frequency as unit
  mFrameDuration = 1.0/this->getVideoGrabber()->getFramerate() * cedar::unit::seconds;
}

