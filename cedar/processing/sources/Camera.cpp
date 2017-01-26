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

    File:        Camera.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.ruhr-uni-bochum.d
    Date:        2012 04 20

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/sources/Camera.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/Arguments.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
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
      new ElementDeclarationTemplate<cedar::proc::sources::Camera>
      (
        "Sources",
        "cedar.processing.sources.Camera"
      )
    );
    declaration->setIconPath(":/steps/camera_grabber.svg");
    declaration->setDescription
    (
      "A source that reads images from a camera.\nNote:\nAn exception will be thrown when changing "
      "settings while a looped trigger is active. In this case reset the grabber."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::sources::Camera::Camera()
:
cedar::proc::sources::GrabberBase()
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  // on creation, create also the cameragrabber.
  cedar::dev::sensors::camera::GrabberPtr grabber;
  grabber = cedar::dev::sensors::camera::GrabberPtr
            (
               new cedar::dev::sensors::camera::Grabber()
            );

  // no exception here, so we could use it
  this->mpGrabber = grabber;

  this->addConfigurableChild("CameraGrabber", this->getCameraGrabber());
  this->declareOutput("camera", mImage);

  // applyParameter as an action
  this->registerFunction("apply parameter", boost::bind(&cedar::proc::sources::Camera::applyParameter, this), false);

  // update picture as an action
  this->registerFunction("update frame", boost::bind(&cedar::proc::sources::Camera::updateFrame, this), false);

  // listen to changed framesize in order to annotate a new image
  QObject::connect(grabber.get(),SIGNAL(frameSizeChanged()),this, SLOT(changedFrameSize()));
}

cedar::proc::sources::Camera::~Camera()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//----------------------------------------------------------------------------------------------------------------------
// slots
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sources::Camera::changedFrameSize()
{
  this->lock(cedar::aux::LOCK_TYPE_READ);
  this->compute(cedar::proc::Arguments());
  this->unlock();
  this->emitOutputPropertiesChangedSignal("camera");
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sources::Camera::onStart()
{
  if (!this->getCameraGrabber()->isCreated())
  {
    this->applyParameter();
  }

  if (this->getCameraGrabber()->isCreated())
  {
    this->getCameraGrabber()->setIsGrabbing(true);
  }
}


void cedar::proc::sources::Camera::onStop()
{
  this->getCameraGrabber()->setIsGrabbing(false);
}


void cedar::proc::sources::Camera::applyParameter()
{
  if (this->getCameraGrabber()->isCreated() && this->getCameraGrabber()->isRunningNolocking())
  {
    std::string msg = this->getCameraGrabber()->getName() + ": Already grabbing! Please stop grabbing and try again!";
    cedar::aux::LogSingleton::getInstance()->warning(msg,"void cedar::proc::sources::Camera::applyParameter()");
    return;
  }

  if (this->getCameraGrabber()->applyParameter())
  {
    this->updateFrame();
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->error
                                             (
                                               this->getCameraGrabber()->getName() + ": ERROR on applying parameter",
                                               "void cedar::proc::sources::Camera::applyParameter()"
                                             );
  }
  //this->callReset();
}


void cedar::proc::sources::Camera::updateFrame()
{
  if (this->getCameraGrabber()->isCreated())
  {
    for (int i = 0; i < 5; ++i)
    {
      cedar::aux::sleep(cedar::unit::Time(50.0 * cedar::unit::milli * cedar::unit::seconds));
      this->onTrigger();
      this->annotateImage();
    }
  }
}


void cedar::proc::sources::Camera::compute(const cedar::proc::Arguments&)
{
  if (this->getCameraGrabber()->isCreated())
  {
    this->getCameraGrabber()->grab();
    this->mImage->setData(this->getCameraGrabber()->getImage().clone());
  }
}

