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

    File:        ViewportSlice.cpp

    Maintainer:  Lars Janssen, Raul Grieben
    Email:       lars.janssen@ini.rub.de, raul.grieben@ini.rub.de
    Date:        2023 02 26

    Description: Source file for the class cedar::dyn::steps::ViewportSlice.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/dynamics/steps/ViewportSlice.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

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
        new ElementDeclarationTemplate<cedar::dyn::steps::ViewportSlice>
          (
            "Image Processing",
            "cedar.dynamics.ViewportSlice"
          )
      );
    resize_decl->setIconPath(":/steps/viewport_camera.svg");
    resize_decl->setDescription
      (
        "Crops an array to a viewport given as an activation input."
      );

    resize_decl->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dyn::steps::ViewportSlice::ViewportSlice()
:
  mOutput(new cedar::aux::MatData(cv::Mat(120, 120, CV_32F, cv::Scalar(0.0)))),
  mOutputCOS(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
  mOutputKernel(new cedar::aux::MatData(cv::Mat::zeros(120, 180, CV_32F))),
  _mElapsedTime(0.0),
  _lastX(0),
  _lastY(0),
  _startSC(false),
  _endSC(false),
  mViewportSize(new cedar::aux::UIntVectorParameter(this, "viewport size", 2, 10,
                                                      1, 500000)),
  _msimplified(new cedar::aux::BoolParameter(this, "simplified", false))
{

  // declare all data
  auto input_slot = this->declareInput("input");
  auto viewport_center_slot = this->declareInput("viewport center", false);

  cedar::proc::typecheck::Matrix input_check;
  input_check.addAcceptedDimensionality(2);
  input_slot->setCheck(input_check);
  viewport_center_slot->setCheck(input_check);

  this->declareOutput("output", mOutput);
  this->declareOutput("kernel", mOutputKernel);
  this->declareOutput("CoS", mOutputCOS);
}

cedar::dyn::steps::ViewportSlice::~ViewportSlice()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dyn::steps::ViewportSlice::eulerStep(const cedar::unit::Time& time)
{
  if(!this->mInput)
  {
    return;
  }
  const cv::Mat& input = this->mInput->getData();
  cv::Mat& output = this->mOutput->getData();
  cv::Mat& kernel = this->mOutputKernel->getData();
  cv::Mat& cos = this->mOutputCOS->getData();
  int inputWidth = input.size[1];
  int inputHeight = input.size[0];
  int viewportWidth = this->mViewportSize->at(1);
  int viewportHeight = this->mViewportSize->at(0);

  if(viewportWidth > inputWidth || viewportHeight > inputHeight)
  {
    cedar::aux::LogSingleton::getInstance()->error
      (
        "Viewport windows is set to be larger than the input image",
        "cedar::dyn::steps::ViewportSlice::eulerStep()"
      );
    return;
  }
  // Find peak in activation input and use it as the center point for the viewport
  double centerX, centerY, max;
  this->getCenterPoint(centerX, centerY, max);

  if(this->mViewportCenterInput)
  {
      const cv::Mat& viewportCenterInput = this->mViewportCenterInput->getData();

      if(kernel.size[0] != viewportCenterInput.size[0] || kernel.size[1] != viewportCenterInput.size[1])
      {
          _startSC = false;
          _endSC = false;
          _lastX = 0.5;
          _lastY = 0.5;
          _mElapsedTime = 0.0;
          kernel = cv::Mat(viewportCenterInput.size[0],viewportCenterInput.size[1], CV_32F, cv::Scalar(0.0));
          kernel.at<float>(std::round(_lastY * kernel.size[0]), std::round(_lastX * kernel.size[1])) = 1.0;
          this->emitOutputPropertiesChangedSignal("kernel");
      }

      if(this->_msimplified->getValue())
      {
          if(max < 0.5)
          {
              output = cv::Mat(viewportHeight, viewportWidth, CV_32F, cv::Scalar(0.0));
          }
          else
          {
              kernel = cv::Mat(viewportCenterInput.size[0],viewportCenterInput.size[1], CV_32F, cv::Scalar(0.0));
              kernel.at<float>(std::round(centerY * kernel.size[0]), std::round(centerX * kernel.size[1])) = 1.0;

              int hvw = std::round(viewportWidth/2);
              int hvh = std::round(viewportHeight/2);
              cv::Mat input_padded;
              cv::copyMakeBorder( input, input_padded, hvh, hvh, hvw, hvw, cv::BORDER_CONSTANT,  cv::Scalar(0, 0, 0) );
              int left = std::round(centerX * inputWidth);
              int top = std::round(centerY * inputHeight);
              // Crop the image
              cv::Rect viewportRect(left, top, viewportWidth, viewportHeight);
              output = input_padded(viewportRect);
          }
      }
      else
      {
          if( !_startSC && !_endSC && (std::abs(_lastX - centerX) > 0.05 || std::abs(_lastY - centerY) > 0.05) && max > 0.5 )
          {
              _lastX = centerX;
              _lastY = centerY;
              _startSC = true;
              _endSC = false;
              _mElapsedTime = 0.0;
              kernel = cv::Mat(viewportCenterInput.size[0],viewportCenterInput.size[1], CV_32F, cv::Scalar(0.0));
              kernel.at<float>(std::round(centerY * kernel.size[0]), std::round(centerX * kernel.size[1])) = 1.0;
          }

          if(_startSC)
          {
              mOutputCOS->getData().setTo(0);
              output = cv::Mat(viewportHeight, viewportWidth, CV_32F, cv::Scalar(0.0));
              if(_mElapsedTime > 20)
              {
                  _startSC = false;
                  _endSC = true;
                  mOutputCOS->getData().setTo(1);
                  _mElapsedTime = 0.0;
              }
              else
              {
                  _mElapsedTime +=  time / cedar::unit::Time(1*cedar::unit::milli * cedar::unit::seconds);
              }
          }
          else
          {
              if(_endSC)
              {
                  if(_mElapsedTime > 20)
                  {
                      _startSC = false;
                      _endSC = false;
                      mOutputCOS->getData().setTo(0);
                      _mElapsedTime = 0.0;
                  }
                  else
                  {
                      _mElapsedTime +=  time / cedar::unit::Time(1*cedar::unit::milli * cedar::unit::seconds);
                  }
              }

              int hvw = std::round(viewportWidth/2);
              int hvh = std::round(viewportHeight/2);
              cv::Mat input_padded;
              cv::copyMakeBorder( input, input_padded, hvh, hvh, hvw, hvw, cv::BORDER_CONSTANT,  cv::Scalar(0, 0, 0) );
              int left = std::round(_lastX * inputWidth);
              int top = std::round(_lastY * inputHeight);
              // Crop the image
              cv::Rect viewportRect(left, top, viewportWidth, viewportHeight);
              output = input_padded(viewportRect);
          }
      }
  }
  else
  {
      _lastX = 0.5;
      _lastY = 0.5;
      _startSC = false;
      _endSC = false;
      _mElapsedTime = 0.0;
      output = cv::Mat(viewportHeight, viewportWidth, CV_32F, cv::Scalar(0.0));
      kernel = cv::Mat(120,180, CV_32F, cv::Scalar(0.0));
      mOutputCOS->getData().setTo(0);
  }
}

void cedar::dyn::steps::ViewportSlice::getCenterPoint(double &centerX, double &centerY, double &max)
{
  if(this->mViewportCenterInput)
  {
    const cv::Mat& viewportCenterInput = this->mViewportCenterInput->getData();

    // Find the max peak in the input
    double min;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(viewportCenterInput, &min, &max, &minLoc, &maxLoc);
    centerX = (maxLoc.x * 1.0) / viewportCenterInput.size[1];
    centerY = (maxLoc.y * 1.0) / viewportCenterInput.size[0];
    CEDAR_ASSERT(centerX >= 0 && centerX < 1)
    CEDAR_ASSERT(centerY >= 0 && centerY < 1)
  }
  else
  {
      centerX = 0.5;
      centerY = 0.5;
      max = 0.0;
  }
}

void cedar::dyn::steps::ViewportSlice::reset()
{
    _lastX = 0.5;
    _lastY = 0.5;
    _startSC = false;
    _endSC = false;
    _mElapsedTime = 0.0;
    mOutput->getData().setTo(0);
    mOutputCOS->getData().setTo(0);
    cv::Mat& kernel = this->mOutputKernel->getData();
    kernel = cv::Mat(120,180, CV_32F, cv::Scalar(0.0));
}

void cedar::dyn::steps::ViewportSlice::inputConnectionChanged(const std::string& inputName)
{
  if(inputName == "input")
  {
    this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }
  if(inputName == "viewport center")
  {
    this->mViewportCenterInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }
}
