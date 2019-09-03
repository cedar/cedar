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

    Description: Source file for the class cedar::proc::sources::VirtualCamera.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/sources/VirtualCamera.h"

// CEDAR INCLUDES
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/gl/GlobalScene.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/auxiliaries/math/constants.h"
#include "boost/shared_ptr.hpp"

// SYSTEM INCLUDES

namespace {
  bool declare() {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration(new ElementDeclarationTemplate<cedar::proc::sources::VirtualCamera>("Sources",
                                                                                                          "cedar.processing.sources.VirtualCamera"));
    declaration->setIconPath(":/steps/virtual_camera.svg");
    declaration->declare();
    declaration->setDescription
            (
                    "Outputs a matrix with a camera view (picture) of the virtual scene. "
                    "NOTE: The separately opened viewer window can be closed, and "
                    "reopened via the context menu."
            );

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::sources::VirtualCamera::VirtualCamera()
        :
        cedar::proc::Step(true),
        mpOutput(new cedar::aux::MatData(cv::Mat(256, 256, CV_8UC3))),
        mOutputSizes(new cedar::aux::UIntVectorParameter(this, "output sizes", 2, 256 , 0, 1024, 10)),
        mCameraPosition(new cedar::aux::DoubleVectorParameter(this, "camera position", 3, 0.0, -10., 10. , 0.1)),
        mHorizontalOrientation(
                new cedar::aux::DoubleVectorParameter(this, "camera horizontal", 1, 0., -(1.) * cedar::aux::math::pi,
                                                      (1.) * cedar::aux::math::pi, 0.01)),
        mVerticalOrientation(
                new cedar::aux::DoubleVectorParameter(this, "camera vertical", 1, 0., -(1. / 2.) * cedar::aux::math::pi,
                                                      (1. / 2.) * cedar::aux::math::pi, 0.01)) {
  mCameraPosition->setValue(2, 1.0);  // set z axis to 1

  cedar::aux::gl::ScenePtr scene = cedar::aux::gl::GlobalSceneSingleton::getInstance();
  mpViewer = boost::shared_ptr<cedar::aux::gui::Viewer>(new cedar::aux::gui::Viewer(scene, false));

  mpViewer.get()->startTimer(25);
  mpViewer.get()->setSceneRadius(scene->getSceneLimit());
  mpViewer.get()->setFixedSize(mOutputSizes->at(0), mOutputSizes->at(1));
  mpViewer.get()->setWindowFlags(Qt::WindowStaysOnTopHint);
  mpViewer.get()->show();
  //mpViewer->hide();
  QObject::connect(mOutputSizes.get(), SIGNAL(valueChanged()), this, SLOT(resolutionChanged()));
  QObject::connect(mCameraPosition.get(), SIGNAL(valueChanged()), this, SLOT(cameraPositionChanged()));
  QObject::connect(mHorizontalOrientation.get(), SIGNAL(valueChanged()), this, SLOT(cameraOrientationChanged()));
  QObject::connect(mVerticalOrientation.get(), SIGNAL(valueChanged()), this, SLOT(cameraOrientationChanged()));
  QObject::connect(mpViewer.get(), SIGNAL(cameraMoved()), this, SLOT(cameraPositionChangedFromViewport()));
  QObject::connect(mpViewer.get(), SIGNAL(cameraMoved()), this, SLOT(cameraOrientationChangedFromViewport()));

  QObject::connect(mpViewer.get(), SIGNAL(updated()), this, SLOT(updateOutput()));

  QObject::connect(this , SIGNAL(destroyed()), mpViewer.get(), SLOT(deleteLater()));



  this->registerFunction("show/hide viewport", boost::bind(&cedar::aux::gui::Viewer::toggleVisible, mpViewer));

  this->declareOutput("Image Matrix (RGB)", mpOutput);
  mLock = mpViewer.get()->registerGrabber();
  this->cameraPositionChanged();
  this->cameraOrientationChanged();

}

cedar::proc::sources::VirtualCamera::~VirtualCamera() {

  //TODO this destructor seemingly never gets called ?
  //     investigate how these cameras are created and how they should be deleted
  ///     Fixed:
  ///     object parameters persist throughout different sessions
  ///     and even after logging out entirely

  mpViewer.get()->deregisterGrabber(mLock);
  mpViewer.get()->close();
}

//!@note: since this code is never called it has been duplicated in the Signal below as a temporary fix
void cedar::proc::sources::VirtualCamera::compute(const cedar::proc::Arguments &) {
  mLock->lockForRead();

  auto image = mpViewer.get()->grabImage();

  if (!image.empty()) {
    image.resize(mOutputSizes->at(0), mOutputSizes->at(1));
    this->mpOutput->setData(image);
  }
  mLock->unlock();
}

void cedar::proc::sources::VirtualCamera::updateOutput() {
  mLock->lockForRead();

  auto image = mpViewer.get()->grabImage();

  if (!image.empty()) {
    image.resize(mOutputSizes->at(0), mOutputSizes->at(1));
    this->mpOutput->setData(image);

  }
  mLock->unlock();
}

void cedar::proc::sources::VirtualCamera::resolutionChanged() {
  cv::Mat new_output_mat = cv::Mat(mOutputSizes->at(0), mOutputSizes->at(1), CV_8UC3);
  mpOutput->setData(new_output_mat);

  mpViewer.get()->setFixedSize(mOutputSizes->at(1), mOutputSizes->at(0));

}

void cedar::proc::sources::VirtualCamera::cameraPositionChanged() {

  mpViewer.get()->changeCameraPosition(mCameraPosition->at(0), mCameraPosition->at(1), mCameraPosition->at(2));

}

void cedar::proc::sources::VirtualCamera::cameraOrientationChanged() {

  mpViewer.get()->changeCameraOrientation(mHorizontalOrientation->at(0), mVerticalOrientation->at(0));

}

void cedar::proc::sources::VirtualCamera::cameraPositionChangedFromViewport() {
#ifdef CEDAR_USE_QGLVIEWER
  qglviewer::Vec pos = mpViewer.get()->camera()->position();
  mCameraPosition->setValue(0, pos.x, true);
  mCameraPosition->setValue(1, pos.y, true);
  mCameraPosition->setValue(2, pos.z, true);
#endif
}

void cedar::proc::sources::VirtualCamera::cameraOrientationChangedFromViewport() {
#ifdef CEDAR_USE_QGLVIEWER

  qglviewer::Vec dir = mpViewer.get()->camera()->viewDirection();
  if (dir.z < -1.)
    dir.z = -1.;
  if (dir.z > 1.)
    dir.z = 1.;
  double beta = std::asin(dir.z);
  double alpha = std::atan2(dir.y, dir.x);
  mVerticalOrientation->setValue(0, beta, true);
  if(dir.z != 1. && dir.z != -1.)
    mHorizontalOrientation->setValue(0, alpha, true);
#endif
}

