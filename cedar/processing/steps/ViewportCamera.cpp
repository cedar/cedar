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

    File:        ViewportCamera.cpp

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2023 01 24

    Description: Source file for the class cedar::proc::steps::ViewportCamera.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/ViewportCamera.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/typecheck/Matrix.h"

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

    ElementDeclarationPtr resize_decl
      (
        new ElementDeclarationTemplate<cedar::proc::steps::ViewportCamera>
          (
            "Image Processing",
            "cedar.processing.ViewpointCamera"
          )
      );
    resize_decl->setIconPath(":/steps/viewport_camera.svg");
    resize_decl->setDescription
      (
        "Crops an image to a viewport given as an activation input."
      );

    resize_decl->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ViewportCamera::ViewportCamera()
:
  mOutput(new cedar::aux::MatData(cv::Mat())),
  mViewportSize(new cedar::aux::UIntVectorParameter(this, "viewport size", 2, 10,
                                                      1, 500000))
{

  // declare all data
  auto input_slot = this->declareInput("input");
  auto viewport_center_slot = this->declareInput("viewport center", false);

  cedar::proc::typecheck::Matrix input_check;
  input_check.addAcceptedDimensionality(2);
  input_slot->setCheck(input_check);
  viewport_center_slot->setCheck(input_check);

  this->declareOutput("output", mOutput);

  // connect the parameter's change signal
  QObject::connect(mViewportSize.get(), SIGNAL(valueChanged()), this, SLOT(viewportSizeChanged()));
}

cedar::proc::steps::ViewportCamera::~ViewportCamera()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::ViewportCamera::compute(const cedar::proc::Arguments&)
{
  if(!this->mInput)
  {
    return;
  }
  const cv::Mat& input = this->mInput->getData();
  cv::Mat& output = this->mOutput->getData();

  int inputWidth = input.size[1];
  int inputHeight = input.size[0];
  this->updateViewportSizeRange();
  int viewportWidth = this->mViewportSize->at(1);
  int viewportHeight = this->mViewportSize->at(0);
  if(viewportWidth > inputWidth || viewportHeight > inputHeight)
  {
    cedar::aux::LogSingleton::getInstance()->error
      (
        "Viewport windows is set to be larger than the input image",
        "cedar::proc::steps::ViewportCamera::compute()"
      );
    return;
  }
  // Find peak in activation input and use it as the center point for the viewport
  double centerX, centerY;
  this->getCenterPoint(centerX, centerY);
  int left = std::round(centerX * (inputWidth - viewportWidth));
  int top = std::round(centerY * (inputHeight - viewportHeight));
  CEDAR_ASSERT(left >= 0 && left <= inputWidth - viewportWidth)
  CEDAR_ASSERT(top >= 0 && top <= inputHeight - viewportHeight)

  // Crop the image
  cv::Rect viewportRect(left, top, viewportWidth, viewportHeight);
  output = input(viewportRect);
}


void cedar::proc::steps::ViewportCamera::updateViewportSizeRange()
{
  if(this->mInput)
  {
    const cv::Mat& input = this->mInput->getData();
    this->mViewportSize->setMaximum(std::max(input.size[0], input.size[1]));
  }
}

void cedar::proc::steps::ViewportCamera::getCenterPoint(double &centerX, double &centerY)
{
  if(this->mViewportCenterInput)
  {
    const cv::Mat& viewportCenterInput = this->mViewportCenterInput->getData();

    // Find the max peak in the input
    double min, max;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(viewportCenterInput, &min, &max, &minLoc, &maxLoc);
    centerX = (maxLoc.x * 1.0) / viewportCenterInput.size[0];
    centerY = (maxLoc.y * 1.0) / viewportCenterInput.size[1];
    CEDAR_ASSERT(centerX >= 0 && centerX < 1)
    CEDAR_ASSERT(centerY >= 0 && centerY < 1)
  }
  else
  {
    centerX = 0.5;
    centerY = 0.5;
  }
}

void cedar::proc::steps::ViewportCamera::viewportSizeChanged()
{
  this->lock(cedar::aux::LOCK_TYPE_READ);
  this->compute(cedar::proc::Arguments());
  this->unlock();
  this->emitOutputPropertiesChangedSignal("output");
  this->onTrigger();
}

void cedar::proc::steps::ViewportCamera::inputConnectionChanged(const std::string& inputName)
{
  if(inputName == "input")
  {
    this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
    this->updateViewportSizeRange();
  }
  if(inputName == "viewport center")
  {
    this->mViewportCenterInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }
}
