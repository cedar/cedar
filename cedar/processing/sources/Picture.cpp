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
#include "cedar/processing/Arguments.h"
#include "cedar/auxiliaries/annotation/ColorSpace.h"
#include "cedar/auxiliaries/math/tools.h"

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
      new ElementDeclarationTemplate<cedar::proc::sources::Picture>
      (
        "Sources",
        "cedar.processing.sources.Picture"
      )
    );
    declaration->setIconPath(":/steps/picture_grabber.svg");
    declaration->setDescription
    (
      "Reads an image from a file and outputs it as matrix. Note: Exactly what file types are supported depends on your OpenCV "
      "version."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::sources::Picture::Picture()
:
cedar::proc::sources::GrabberBase(false)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  cedar::dev::sensors::visual::PictureGrabberPtr grabber;
  grabber = cedar::dev::sensors::visual::PictureGrabberPtr
            (
               new cedar::dev::sensors::visual::PictureGrabber()
            );

  //no exception here, so we could use it
  this->mpGrabber = grabber;

  this->addConfigurableChild("PictureGrabber", this->getPictureGrabber());
  this->declareOutput("Picture", mImage);

  QObject::connect(this->getPictureGrabber().get(), SIGNAL(pictureChanged()), this, SLOT(updatePicture()));

  const std::string file_name = this->getPictureGrabber()->getSourceFile();
  if ( ! (file_name == "." || file_name == "") )
  {
    this->reset();
  }
}

cedar::proc::sources::Picture::~Picture()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::MatDataPtr cedar::proc::sources::Picture::getImage()
{
  return this->mImage;
}

cedar::aux::ConstMatDataPtr cedar::proc::sources::Picture::getImage() const
{
  return this->mImage;
}

void cedar::proc::sources::Picture::setSourceFileName(const std::string& imagePath)
{
  this->getPictureGrabber()->setSourceFile(imagePath);
}

void cedar::proc::sources::Picture::reset()
{
  // we have to disconnect the slot here to avoid a deadlock
  QObject::disconnect(this->getPictureGrabber().get(), SIGNAL(pictureChanged()), this, SLOT(updatePicture()));
  if (this->getPictureGrabber()->applyParameter())
  {
    this->mImage->setData(this->getPictureGrabber()->getImage());
  }
  else
  {
    const std::string name = mpGrabber->getName();
    std::stringstream error_message;
    error_message << "Couldn't load image: "  << this->getPictureGrabber()->getSourceFile(0) << std::endl;
    cedar::aux::LogSingleton::getInstance()->error
                                             (
                                               name + ": " + error_message.str(),
                                               "cedar::dev::sensors::visual::Picture::Picture()"
                                             );
  }
  // reconnect the slot
  QObject::connect(this->getPictureGrabber().get(), SIGNAL(pictureChanged()), this, SLOT(updatePicture()));
}


void cedar::proc::sources::Picture::updatePicture()
{
  cedar::proc::Step::ReadLocker locker(this);
  cv::Mat old_image = this->mImage->getData();
  // fill output with new image
  this->compute(cedar::proc::Arguments());
  cv::Mat new_image = this->mImage->getData();
  if (!cedar::aux::math::matrixSizesEqual(old_image, new_image) || old_image.type() != new_image.type())
  {
    this->annotateImage();
    locker.unlock();
    this->emitOutputPropertiesChangedSignal("Picture");
  }
  else
  {
    locker.unlock();
  }
  onTrigger();
}

void cedar::proc::sources::Picture::compute(const cedar::proc::Arguments&)
{
  if (getPictureGrabber()->isCreated())
  {
    this->getPictureGrabber()->grab();
    this->mImage->setData(this->getPictureGrabber()->getImage());
  }
}



