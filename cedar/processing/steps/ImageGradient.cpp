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

    File:        ImageGradient.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2016 01 25

    Description: Source file for the class cedar::proc::steps::ImageGradient.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/ImageGradient.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/processing/ElementDeclaration.h"

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
      new ElementDeclarationTemplate<cedar::proc::steps::ImageGradient>
      (
        "Image Processing"
      )
    );
    declaration->setIconPath(":/steps/image_gradient.svg");
    declaration->setDescription
    (
      "Calculates gradient of an image via a Sobel filter."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ImageGradient::ImageGradient()
:
// inputs
mImage(this, "image"),
// buffers
mDX(new cedar::aux::MatData(cv::Mat())),
mDY(new cedar::aux::MatData(cv::Mat())),
// outputs
mOrientations(new cedar::aux::MatData(cv::Mat())),
mMagnitudes(new cedar::aux::MatData(cv::Mat()))
{
  // declare buffers
  this->declareBuffer("gradient x", this->mDX);
  this->declareBuffer("gradient y", this->mDY);

  // declare outputs
  auto orientations_slot = this->declareOutput("orientations", this->mOrientations);
  auto magnitudes_slot = this->declareOutput("magnitudes", this->mMagnitudes);

  // copy the input properties to the output and buffers
  auto copy_input_to_slot = [&](cedar::proc::DataSlotPtr slot)
  {
    cedar::proc::CopyMatrixPropertiesPtr orientation_copier(new cedar::proc::CopyMatrixProperties(slot));
    orientation_copier->setTypeOverride(CV_32F);
    this->mImage.addOutputRelation(orientation_copier);
  };

  copy_input_to_slot(orientations_slot);
  copy_input_to_slot(magnitudes_slot);

  // check the input for the proper type/dimensionality
  cedar::proc::typecheck::Matrix check;
  check.addAcceptedDimensionality(2);
  check.addAcceptedType(CV_8UC1);
  this->mImage.getSlot()->setCheck(check);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::ImageGradient::compute(const cedar::proc::Arguments&)
{
  const cv::Mat& image = this->mImage.getData();
  cv::Mat& dx = this->mDX->getData();
  cv::Mat& dy = this->mDY->getData();
  cv::Mat& magnitude = this->mMagnitudes->getData();
  cv::Mat& orientation = this->mOrientations->getData();

  //!@todo Make output type a parameter
  //!@todo Make kernel type selectable (ksize = -1 == Scharr, otherwise, ksize = kernel size)
  int ksize = 3;
  cv::Sobel(image, dx, CV_32F, 1, 0, ksize);
  cv::Sobel(image, dy, CV_32F, 0, 1, ksize);

  cv::magnitude(dx, dy, magnitude);
  cv::phase(dx, dy, orientation);
}
