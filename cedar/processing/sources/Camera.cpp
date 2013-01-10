/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
      new ElementDeclarationTemplate<cedar::proc::sources::Camera>
      (
        "Sources",
        "cedar.processing.sources.Camera"
      )
    );
    declaration->setIconPath(":/steps/camera_grabber.svg");
    declaration->setDescription
    (
      "A source that reads images from a camera."
    );
    cedar::proc::DeclarationRegistrySingleton::getInstance()->declareClass(declaration);

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

  //no exception here, so we could use it
  this->mpGrabber = grabber;

  this->addConfigurableChild("CameraGrabber", this->getCameraGrabber());
  this->declareOutput("camera", mImage);

  //this->reset();

  //QObject::connect(mDeBayer.get(), SIGNAL(valueChanged()), this, SLOT(setDeBayer()));
  //QObject::connect(mBusId.get(), SIGNAL(valueChanged()), this, SLOT(setBusId()));

  // applyParameter as an action
  this->registerFunction("apply parameter", boost::bind(&cedar::proc::sources::Camera::applyParameter, this));

  // update picture as an action
  this->registerFunction("update frame", boost::bind(&cedar::proc::sources::Camera::updateFrame, this));

}

cedar::proc::sources::Camera::~Camera()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::Camera::onStart()
{
  if (!this->getCameraGrabber()->isCreated())
  {
  }
}

//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::Camera::applyParameter()
{
  if (this->getCameraGrabber()->applyParameter())
  {
    for (int i = 0; i < 5; ++i)
    {
      usleep(5000);
      this->onTrigger();
      this->annotateImage();
    }
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
                                             (
                                               this->getCameraGrabber()->getName() + ": ERROR on applying parameter",
                                               "void cedar::proc::sources::Camera::applyParameter()"
                                             );
  }
}


//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::Camera::updateFrame()
{
  if (this->getCameraGrabber()->isCreated())
  {
    for (int i = 0; i < 10; ++i)
    {
      usleep(50000);
      this->onTrigger();
      this->annotateImage();
    }
  }
}


//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::Camera::compute(const cedar::proc::Arguments&)
{
  // get the time-diff between two steps
  /*
  try
  {
    const cedar::proc::StepTime& step_time = dynamic_cast<const cedar::proc::StepTime&>(arguments);
    this->eulerStep(step_time.getStepTime());
  }
  catch (const std::bad_cast& e)
  {
    CEDAR_THROW(cedar::proc::InvalidArgumentsException, "Bad arguments passed to dynamics. Expected StepTime.");
  }
 */
  if (this->getCameraGrabber()->isCreated())
  {
    this->getCameraGrabber()->grab();
    //!@todo Don't constantly reannotate here!
    //this->annotateImage();
    this->mImage->setData(this->getCameraGrabber()->getImage().clone());
  }
}

