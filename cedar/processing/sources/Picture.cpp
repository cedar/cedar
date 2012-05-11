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

    File:        Picture.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.ruhr-uni-bochum.d
    Date:        2012 04 20

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/sources/Picture.h"
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
      new ElementDeclarationTemplate<cedar::proc::sources::Picture>
      (
        "Sources",
        "cedar.processing.sources.Picture"
      )
    );
    declaration->setIconPath(":/steps/picture_grabber.svg");
    cedar::proc::DeclarationRegistrySingleton::getInstance()->declareClass(declaration);

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::sources::Picture::Picture()
:
cedar::proc::Step(false, true),
mImage(new cedar::aux::ImageData(cv::Mat::zeros(1, 1, CV_32F))),
_mFileName(new cedar::aux::FileParameter(this, "filename", cedar::aux::FileParameter::READ,"")),
_mConfigurationFileName(new cedar::aux::FileParameter(this, "config",cedar::aux::FileParameter::READ,""))
{
  //default-filenames
  _mFileName->setValue("./picture.png");
  _mConfigurationFileName->setValue("./picturegrabber.cfg");

  this->declareOutput("Picture", mImage);
  QObject::connect(_mFileName.get(), SIGNAL(valueChanged()), this, SLOT(setFileName()));
  QObject::connect(_mConfigurationFileName.get(), SIGNAL(valueChanged()), this, SLOT(setConfigurationFileName()));
  std::cout << " - finished" << std::endl;

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
void cedar::proc::sources::Picture::onStart()
{
}

//----------------------------------------------------------------------------------------------------
void cedar::proc::sources::Picture::compute(const cedar::proc::Arguments&)
{
  if (!mGrabber)
  {
    //no: create a new one
    this->createGrabber();
  }

  //if creation was successfully or the grabber was already there
  if (mGrabber)
  {
    // and set the filename
    this->mGrabber->grab();
    this->mImage->setData(this->mGrabber->getImage());
  }
}

//----------------------------------------------------------------------------------------------------
void cedar::proc::sources::Picture::setFileName()
{
  //update grabber if already there
  if (mGrabber)
  {
    std::string filename = this->_mFileName->getValue().path().toStdString();
    this->mGrabber->setSourceFile(filename);

    std::string message = this->mGrabber->getName()+ ": Set new filename: " + filename;
    cedar::aux::LogSingleton::getInstance()->message(message,"cedar::proc::sources::Picture::setFileName()");
  }

  //if there isn't a grabber instance created, the filename is stored internally,
  //onTrigger() will then create the new grabber and get the images
  this->onTrigger();
}


//----------------------------------------------------------------------------------------------------
void cedar::proc::sources::Picture::setConfigurationFileName()
{

}

//----------------------------------------------------------------------------------------------------
void cedar::proc::sources::Picture::createGrabber()
{
  mGrabber.reset();

  //create grabber in temp. variable in order to verify the correct creation
  cedar::dev::sensors::visual::PictureGrabberPtr grabber;
  grabber = cedar::dev::sensors::visual::PictureGrabberPtr
            (
               new cedar::dev::sensors::visual::PictureGrabber
                   (
                     this->_mConfigurationFileName->getPath(),
                     this->_mFileName->getPath()
                   )
            );

  //no exception here, so we could use it
  mGrabber = grabber;
}
