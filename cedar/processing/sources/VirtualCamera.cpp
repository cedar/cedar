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
#include <QPoint>
#include <QtGlobal>
#include "boost/lexical_cast.hpp"

#ifdef CEDAR_USE_QGLVIEWER

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
                                                      (1. / 2.) * cedar::aux::math::pi, 0.01)),
        mProvidePointToWorldTrafos(new cedar::aux::UIntParameter(this, "provide pixel to world transformation input/output pairs", 0, 0, 1000 )), 
        timerID(0)
{
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


  QObject::connect(this , SIGNAL(destroyed()), mpViewer.get(), SLOT(deleteLater()));
  QObject::connect( mpViewer.get() , SIGNAL(updated()), this , SLOT(updatedView()));



  this->registerFunction("show/hide viewport", boost::bind(&cedar::aux::gui::Viewer::toggleVisible, mpViewer));

  this->declareOutput("Image Matrix (RGB)", mpOutput);
  mLock = mpViewer.get()->registerGrabber();
  this->cameraPositionChanged();
  this->cameraOrientationChanged();

  // optional: quick n dirty: calculate world coordinates from pixel
  mProvidePointToWorldTrafos->markAdvanced(true);
  QObject::connect(mProvidePointToWorldTrafos.get(), SIGNAL(valueChanged()), this, SLOT(showPointToWorldChanged()));
}

cedar::proc::sources::VirtualCamera::~VirtualCamera() {

  //TODO this destructor seemingly never gets called ?
  //     investigate how these cameras are created and how they should be deleted
  ///     Fixed:
  ///     object parameters persist throughout different sessions
  ///     and even after logging out entirely

//  std::cout<<"Virtual Camera Destroy!"<<std::endl;

  mpViewer.get()->deregisterGrabber(mLock);
  mpViewer.get()->close();

}

//!@note: update the output during simulation
void cedar::proc::sources::VirtualCamera::compute(const cedar::proc::Arguments &) {

  mLock->lockForRead();

  auto image = mpViewer.get()->grabImage();

  if (!image.empty())
  {
    image.resize(mOutputSizes->at(0), mOutputSizes->at(1));
    auto clonedImage = image.clone();
    this->mpOutput->setData(clonedImage);
  }
  mLock->unlock();

//  computePixelsToWorld();
}

//!@note: this enables the output plot to function even while the simulation is not running
///       the relevant signal is only emitted when the camera is moved so this code should not be called
///       at all during simulation (unless the camera moves during simulation)
void cedar::proc::sources::VirtualCamera::updatedView() {

    mLock->lockForRead();

    auto image = mpViewer.get()->grabImage();

    if (!image.empty())
    {
        image.resize(mOutputSizes->at(0), mOutputSizes->at(1));
        auto clonedImage = image.clone();
        this->mpOutput->setData(clonedImage);
    }
    mLock->unlock();

}

void cedar::proc::sources::VirtualCamera::resolutionChanged() {
  cv::Mat new_output_mat = cv::Mat(mOutputSizes->at(0), mOutputSizes->at(1), CV_8UC3);
//  std::cout<<"VC ResolutionChanged"<<std::endl;
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

  qglviewer::Vec pos = mpViewer.get()->camera()->position();
  mCameraPosition->setValue(0, pos.x, true);
  mCameraPosition->setValue(1, pos.y, true);
  mCameraPosition->setValue(2, pos.z, true);

}

void cedar::proc::sources::VirtualCamera::cameraOrientationChangedFromViewport() {


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

}

void cedar::proc::sources::VirtualCamera::inputConnectionChanged(const std::string& inputName)
{
  if (mProvidePointToWorldTrafos->getValue() < 1)
    return;

  // Assign the input to the member. This saves us from casting in every computation step.
  auto casted_input = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));

  auto inputIt = mInputTrafos.find( inputName );
  if ( inputIt != mInputTrafos.end() )
  {
    inputIt->second= casted_input;
  }

  auto outputIt = mOutputTrafos.find( inputName );
  if( outputIt != mOutputTrafos.end() )
  {
    auto outputPtr= outputIt->second;


    if (!casted_input)
    {
      outputPtr->setData( cv::Mat() );
    }
    else if (outputPtr->isEmpty())
    {
      outputPtr->setData( cv::Mat::zeros(3, 1, CV_32F) );
    }
  }
  else
  {
    // ignore
//std::cout << "ouput name not found" << std::endl;
  }
}

void cedar::proc::sources::VirtualCamera::computePixelsToWorld()
{
  if (mProvidePointToWorldTrafos->getValue() < 1)
    return;

  // this needs to be done from the GUI Thread, alone!
  CEDAR_DEBUG_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

  //mpViewer.get()->camera()->computeModelViewMatrix();
  //mpViewer.get()->camera()->computeProjectionMatrix();

  for (const auto &iter : mOutputTrafos )
  {
    auto key = iter.first;
    auto outputPtr = iter.second;

    std::string key_in= key + " in";

    if(auto inputPtr = mInputTrafos.at( key_in ))
    {
      cv::Mat mat = inputPtr->getData();
      if (!mat.empty())
      {
        const QPoint point = QPoint( mat.at<float>(0,0),
                                     mat.at<float>(1,0) );
//std::cout << "   in: " << mat.at<float>(0,0)
//          << ", " << mat.at<float>(1,0) << std::endl;
        bool found = false;
        qglviewer::Vec pos = mpViewer.get()->camera()->pointUnderPixel( 
                               point, found );

        outputPtr->getData().at<float>(0,0) = pos.x;
        outputPtr->getData().at<float>(1,0) = pos.y;
//std::cout << "     found? " << found << std::endl;        
//std::cout << "     out: " << pos.x << ", " << pos.y << std::endl;        

#if 0
        qreal near = mpViewer.get()->camera()->zNear();
        qreal far= mpViewer.get()->camera()->zFar();
        float myz=  far / ( far-near ) * ( 1.0 - near / 1.3 ); 
        qglviewer::Vec pixelpoint= qglviewer::Vec( mat.at<float>(0,0),
                                                   mat.at<float>(1,0),
                                                   myz );

        qglviewer::Vec campos = mpViewer.get()->camera()->unprojectedCoordinatesOf( pixelpoint );

        qglviewer::Vec worldpos = mpViewer.get()->camera()->worldCoordinatesOf( 
                                campos );
std::cout << "      pixelpoint: " << pixelpoint.x << ", " << pixelpoint.y << ", " << pixelpoint.z << std::endl;
std::cout << "      campos:     " << campos.x << ", " << campos.y << ", " << campos.z << std::endl;
std::cout << "      worldpos:   " << worldpos.x << ", " << worldpos.y << ", " << worldpos.z << std::endl;
#endif
      }
    }
  }
}

void cedar::proc::sources::VirtualCamera::reset()
{
  computePixelsToWorld();
}

void cedar::proc::sources::VirtualCamera::timerEvent(QTimerEvent*)
{
  // to be able to call this from the GUI thread:
  computePixelsToWorld();
}

void cedar::proc::sources::VirtualCamera::showPointToWorldChanged()
{
  if (timerID)
    killTimer(timerID);

  // clear all
  for (const auto &iter: mOutputTrafos )
  {
    auto key= iter.first;

    this->removeOutputSlot( key );
  }
  for (const auto &iter: mInputTrafos )
  {
    auto key= iter.first;
    this->removeInputSlot( key );
  }

  mOutputTrafos.clear();
  mInputTrafos.clear();
  // end clear all:

  unsigned int num_trafos= mProvidePointToWorldTrafos->getValue();
  if (num_trafos > 0)
  {
    for (unsigned int i=0; i < num_trafos; i++)
    {
      std::string name= "trafo" + boost::lexical_cast<std::string>(i+1);
      std::string name_in= name + " in";
      std::string name_out= name; // see computePixelsToWorld()

      cedar::aux::MatDataPtr dataOutPtr = cedar::aux::MatDataPtr(new cedar::aux::MatData( cv::Mat::zeros(3,1,CV_32F) ));
      this->declareOutput(name_out, dataOutPtr);

      mOutputTrafos.insert(std::pair<std::string,cedar::aux::MatDataPtr>(name_out,dataOutPtr)); 

      cedar::aux::MatDataPtr dataInPtr = cedar::aux::MatDataPtr(new cedar::aux::MatData( cv::Mat::zeros(3,1,CV_32F) ));
      this->declareInput( name_in, false ); // optional
      mInputTrafos.insert(std::pair<std::string,cedar::aux::MatDataPtr>(name_in,dataInPtr)); 
    }

    this->startTimer(500); // this is good enough. dont want to bog down the
                           // GUI thread
  }
  else
  {
    // do nothing
  }
}

#endif  // CEDAR_USE_QGLVIEWER
