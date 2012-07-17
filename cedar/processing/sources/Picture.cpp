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
#include "cedar/auxiliaries/annotation/ColorSpace.h"


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
    declaration->setDescription
    (
      "Reads an image from a file. What filetypes are supported depends on what your opencv "
      "version supports."
    );
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
cedar::proc::sources::GrabberBase()
{
  cedar::dev::sensors::visual::PictureGrabberPtr grabber;
  grabber = cedar::dev::sensors::visual::PictureGrabberPtr
            (
               new cedar::dev::sensors::visual::PictureGrabber()
            );

  //no exception here, so we could use it
  //GrabberBase::mGrabber = grabber;
  this->mpGrabber = grabber;

  this->addConfigurableChild("PictureGrabber", this->getPictureGrabber());
  this->declareOutput("Picture", mImage);

  QObject::connect(this->getPictureGrabber().get(), SIGNAL(pictureChanged()), this, SLOT(updatePicture()));

  if (this->getPictureGrabber()->applyParameter())
  {
    this->mImage->setData(this->getPictureGrabber()->getImage());
  }
  else
  {
    const std::string name = mpGrabber->getName();
    std::stringstream init_message;
    init_message << "Couldn't load image: "  << this->getPictureGrabber()->getSourceFile(0) << std::endl;
    cedar::aux::LogSingleton::getInstance()->error
                                             (
                                               name + ": " + init_message.str(),
                                               "cedar::dev::sensors::visual::Picture::Picture()"
                                             );
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
void cedar::proc::sources::Picture::onStart()
{
//  if (this->mGrabber->applyParameter())
//  {
//    // this->mGrabber->grab();
//    this->mImage->setData(this->mGrabber->getImage());
//  }
}

//----------------------------------------------------------------------------------------------------
void cedar::proc::sources::Picture::updatePicture()
{
  cedar::aux::LogSingleton::getInstance()->debugMessage
                                           (
                                             this->getPictureGrabber()->getName() + ": Picture change detected.",
                                             "cedar::dev::sensors::visual::Picture::updatePicture()"
                                           );
  onTrigger();
  // update the annotation
  this->annotateImage();
}

//----------------------------------------------------------------------------------------------------
void cedar::proc::sources::Picture::compute(const cedar::proc::Arguments&)
{
  if (getPictureGrabber()->isCreated())
  {
    this->getPictureGrabber()->grab();
    this->mImage->setData(this->getPictureGrabber()->getImage());
  }
}



