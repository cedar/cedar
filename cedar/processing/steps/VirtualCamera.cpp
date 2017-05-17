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

    File:        VirtualCamera.cpp

    Maintainer:  Nico Kuerschner
    Email:       Nico.Kuerschner@ini.rub.de
    Date:        2017 05 12

    Description: Source file for the class cedar::proc::steps::VirtualCamera.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/VirtualCamera.h"

// CEDAR INCLUDES
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/gl/GlobalScene.h"
#include "cedar/auxiliaries/gl/Scene.h"

// SYSTEM INCLUDES

namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration(new ElementDeclarationTemplate<cedar::proc::steps::VirtualCamera>("Sources", "cedar.processing.steps.VirtualCamera"));
    declaration->setIconPath(":/steps/virtual_camera.svg");
    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::VirtualCamera::VirtualCamera()
  :
  Step(false),
  mpOutput(new cedar::aux::MatData(cv::Mat(256, 256, CV_8UC3))),
  mOutputSizes(new cedar::aux::UIntVectorParameter(this, "output sizes", 2, 256))
{
  cedar::aux::gl::ScenePtr scene = cedar::aux::gl::GlobalSceneSingleton::getInstance();
  mpViewer = new cedar::aux::gui::Viewer(scene);
  mpViewer->startTimer(25);
  mpViewer->setSceneRadius(scene->getSceneLimit());
  mpViewer->resize(mOutputSizes->at(0), mOutputSizes->at(1));
  mpViewer->show();

  //mpViewer->setVisible(false);

  declareOutput("Image Matrix (RGB)", mpOutput);
  mLock = mpViewer->registerGrabber();
}

cedar::proc::steps::VirtualCamera::~VirtualCamera()
{
  mpViewer->deregisterGrabber(mLock);
}

void cedar::proc::steps::VirtualCamera::compute(const cedar::proc::Arguments &)
{
  cv::Mat& output = this->mpOutput->getData();
  output = mpViewer->grabImage().clone();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
