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

    File:        Camera.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.ruhr-uni-bochum.d
    Date:        2012 04 20

    Description:

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"   // MAKE FIREWIRE OPTIONAL
#ifdef CEDAR_USE_LIB_DC1394

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
    declaration->setIconPath(":/steps/video_grabber.svg");
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
cedar::proc::Step(false, true),
mImage(new cedar::aux::ImageData(cv::Mat::zeros(1, 1, CV_32F))),
mDeBayer(new cedar::aux::BoolParameter(this, "deBayer", true)),
mBusId(new cedar::aux::UIntParameter(this, "busId", 0, 0, 999)),
_mConfigurationFileName(new cedar::aux::FileParameter(this, "config",cedar::aux::FileParameter::READ,""))
{

  //default file values
  _mConfigurationFileName->setValue("./cameragrabber.cfg");
  mGrabber.reset();


  this->declareOutput("camera", mImage);
  QObject::connect(_mConfigurationFileName.get(), SIGNAL(valueChanged()), this, SLOT(setConfigurationFileName()));
  QObject::connect(mDeBayer.get(), SIGNAL(valueChanged()), this, SLOT(setDeBayer()));
  QObject::connect(mBusId.get(), SIGNAL(valueChanged()), this, SLOT(setBusId()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::Camera::onStart()
{
  if (!mGrabber)
  {
    createGrabber();
  }
}


//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::Camera::setDeBayer()
{

}

//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::Camera::setBusId()
{
  //recreate grabber if needed
  if (mGrabber)
  {
    createGrabber();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::Camera::setConfigurationFileName()
{

}

//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::Camera::compute(const cedar::proc::Arguments&)
{
  if (mGrabber)
  {
    this->mGrabber->grab();

    cv::Mat frame;

    //check if conversion from bayer-pattern to cv::Mat BGR format is needed
    if(this->mDeBayer->getValue())
    {
      cv::cvtColor(this->mGrabber->getImage(),frame,CV_BayerGR2BGR);
    }
    else
    {
      frame = this->mGrabber->getImage();
    }
    this->mImage->setData(frame.clone());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::Camera::createGrabber()
{

  // destroy the old grabber (if any), in order to save the configuration
  if (mGrabber)
  {
    const std::string message1 = "Old grabber deleted";
    cedar::aux::LogSingleton::getInstance()->debugMessage(message1,"cedar::proc::sources::Camera::createGrabber()");
  }
  mGrabber.reset();

  const std::string conf_file = this->_mConfigurationFileName->getPath();
  const unsigned int bus_id = this->mBusId->getValue();

  // create a new one
  cedar::dev::sensors::visual::CameraGrabberPtr grabber;
  grabber = cedar::dev::sensors::visual::CameraGrabberPtr
            (
               new cedar::dev::sensors::visual::CameraGrabber(conf_file,bus_id,false,true)
            );
  const std::string message2= "New grabber created";
  cedar::aux::LogSingleton::getInstance()->debugMessage(message2,"cedar::proc::sources::Video::createGrabber()");

  // the new grabber created without exception, so we can use it
  mGrabber = grabber;

  //!@todo read debayer value from config-file
}

#endif // CEDAR_USE_LIB_DC1394
