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
//    input_decl->setIconPath(":/steps/gauss_input.svg");
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
cedar::proc::Step(false, true),
mImage(new cedar::aux::ImageData(cv::Mat::zeros(1, 1, CV_32F)))
{
  this->declareOutput("Video", mImage);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sources::Video::onStart()
{
  if (!mGrabber)
  {
    mGrabber = cedar::dev::sensors::visual::VideoGrabberPtr
               (
                  new cedar::dev::sensors::visual::VideoGrabber
                      (
                        "Videograbber.cfg",
                        "/opt/matlab/R2011a/toolbox/images/imdemos/rhinos.avi"
                      )
               );
  }
}

void cedar::proc::sources::Video::compute(const cedar::proc::Arguments&)
{
  this->mGrabber->grab();
  this->mImage->setData(this->mGrabber->getImage());
}