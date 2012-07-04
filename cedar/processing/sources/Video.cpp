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

// SYSTEM INCLUDES

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
    cedar::proc::DeclarationRegistrySingleton::getInstance()->declareClass(declaration);

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
mFrameDuration(0.0),
mTimeElapsed(0.0),
_mFileName(new cedar::aux::FileParameter(this, "videofile",cedar::aux::FileParameter::READ,"./video.avi")),
_mLoop(new cedar::aux::BoolParameter(this, "loop", true))
{
  // default config-filename
  GrabberBase::_mConfigurationFileName->setValue("./videograbber.cfg");

  this->declareOutput("video", mImage);
  QObject::connect(_mFileName.get(), SIGNAL(valueChanged()), this, SLOT(setFileName()));
  QObject::connect(_mLoop.get(), SIGNAL(valueChanged()), this, SLOT(setLoop()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::Video::onStart()
{
  cedar::aux::LogSingleton::getInstance()->debugMessage("start grabber","cedar::proc::sources::Video::onStart()");
  std::string filename = this->_mFileName->getPath();

  // check if videofile is there
  if (filename == "")
  {
    std::string message = this->mGrabber->getName() + ": There is no file to grab from! Please set one!";
    cedar::aux::LogSingleton::getInstance()->warning(message, "cedar::proc::sources::Video::onStart()");
  }

  // if there is no grabber instance, create one
  else if (!mGrabber)
  {
    this->createGrabber();
  }

  // if grabber already grabbing, do nothing
}


//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::Video::reset()
{
  // Rewind the video
  if (mGrabber)
  {
    this->getGrabber()->setPositionAbsolute(0);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::Video::compute(const cedar::proc::Arguments &arguments)
{
  if (mGrabber)
  {
    // grab a new frame only if FrameDuration is larger than fps of the video
    try
    {
      const cedar::proc::StepTime& step_time = dynamic_cast<const cedar::proc::StepTime&>(arguments);
      const cedar::unit::Time& t = step_time.getStepTime();
      cedar::unit::Milliseconds step_time_ms(t);
      mTimeElapsed += step_time_ms;
    }
    catch (const std::bad_cast& e)
    {
      CEDAR_THROW(cedar::proc::InvalidArgumentsException, "Bad arguments passed to dynamics. Expected StepTime.");
    }

    if (mTimeElapsed > mFrameDuration)
    {
      this->mGrabber->grab();

      // get new frame
      cv::Mat frame = this->mGrabber->getImage();

      // set only if it isn't empty (could be, if videofile is over)
      if (!frame.empty())
      {
        this->mImage->setData(frame.clone());
      }
      mTimeElapsed = 0.0;
      //!@todo This should only happen once when the grabber is created/first frame is read.
      this->annotateImage();
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::Video::setFileName()
{
  std::string message = "VideoGrabber: Set new filename to \"" + this->_mFileName->getPath()+"\"";

  // if already grabbing, a new grabber have to be created
  // because there is no possibility to change the filename
  if (mGrabber)
  {
    this->createGrabber();
    message = this->mGrabber->getName() + ": " + message;
  }
  cedar::aux::LogSingleton::getInstance()->message(message,"cedar::proc::sources::Video::setFileName()");
}


//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::Video::setLoop()
{
  if (mGrabber)
  {
    this->getGrabber()->setLooped(this->_mLoop->getValue());
  }
}


//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::Video::onCreateGrabber()
{
  std::string filename = this->_mFileName->getPath();

  cedar::dev::sensors::visual::VideoGrabberPtr grabber;
  grabber = cedar::dev::sensors::visual::VideoGrabberPtr
            (
               new cedar::dev::sensors::visual::VideoGrabber(this->_mConfigurationFileName->getPath(),filename)
            );
  const std::string message2= "New grabber created";
  cedar::aux::LogSingleton::getInstance()->debugMessage(message2,"cedar::proc::sources::Video::createGrabber()");

  // the new grabber created without exception, so we can use it
  GrabberBase::mGrabber = grabber;

  // calcualte the frameduration in ms
  double fps = this->getGrabber()->getSourceFps(0);
  mFrameDuration = static_cast<cedar::unit::Milliseconds>(1000.0 / fps);

  // set the loop
  this->getGrabber()->setLooped(this->_mLoop->getValue());

}
