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

    File:        TemplateMatching.cpp

    Maintainer:  Raul Grieben
    Email:       raul.grieben@ini.ruhr-uni-bochum.de
    Date:        2020 01 14

    Description: Source file for the class cedar::proc::steps::TemplateMatching.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/TemplateMatching.h"

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
      new ElementDeclarationTemplate<cedar::proc::steps::TemplateMatching>
      (
        "Image Processing"
      )
    );
    declaration->setIconPath(":/steps/template_matching.svg");
    declaration->setDescription
    (
      "Searches for matches between an image patch and an input image."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::TemplateMatching::TemplateMatching()
:
// inputs
mImage(this, "image"),
mPatch(this, "patch"),
//output
mOutput(new cedar::aux::MatData(cv::Mat::zeros(10, 10, CV_32F)))
{
  // declare outputs
  auto output_slot = this->declareOutput("output", this->mOutput);

  // check the input for the proper type/dimensionality
  cedar::proc::typecheck::Matrix check;
  check.addAcceptedDimensionality(2);
  check.addAcceptedType(CV_32F);
  this->mImage.getSlot()->setCheck(check);
  this->mPatch.getSlot()->setCheck(check);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::TemplateMatching::compute(const cedar::proc::Arguments&)
{
  const cv::Mat& image = this->mImage.getData();
  const cv::Mat& patch = this->mPatch.getData();

  int result_cols =  image.cols - patch.cols + 1;
  int result_rows = image.rows - patch.rows + 1;

  if(result_cols > 0 && result_rows > 0)
  {
      cv::Mat result = cv::Mat::zeros(result_rows, result_cols, CV_32F);
      cv::matchTemplate( image, patch, result, CV_TM_CCORR_NORMED );
      this->mOutput->setData(result.clone());
  }
}
