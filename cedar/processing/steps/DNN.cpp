/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        DNN.cpp

    Maintainer:  Raul Grieben
    Email:       Raul.Grieben@ini.ruhr-uni-bochum.de
    Date:        2021 11 30

    Description: Source file for the class cedar::proc::steps::DNN.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/DNN.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/processing/steps/Resize.h"
#include "cedar/auxiliaries/convolution/BorderType.h"
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
  new ElementDeclarationTemplate<cedar::proc::steps::DNN>
  (
    "Image Processing",
    "cedar.processing.steps.DNN"
  )
);

//declaration->setIconPath(":/steps/image_dnn.svg");
declaration->setDescription
(
  "DNN inference"
);

declaration->declare();

return true;
}

bool declared = declare();
}


//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::proc::steps::DNN::dnnType>
cedar::proc::steps::DNN::dnnType::mType("dnnType::");

#ifndef CEDAR_COMPILER_MSVC
const cedar::proc::steps::DNN::dnnType::Id cedar::proc::steps::DNN::dnnType::TL;
const cedar::proc::steps::DNN::dnnType::Id cedar::proc::steps::DNN::dnnType::B5;
const cedar::proc::steps::DNN::dnnType::Id cedar::proc::steps::DNN::dnnType::B4;
const cedar::proc::steps::DNN::dnnType::Id cedar::proc::steps::DNN::dnnType::B3;
const cedar::proc::steps::DNN::dnnType::Id cedar::proc::steps::DNN::dnnType::B2;
const cedar::proc::steps::DNN::dnnType::Id cedar::proc::steps::DNN::dnnType::B1;
#endif // CEDAR_COMPILER_MSVC

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::DNN::DNN()
:
cedar::proc::Step(true),
// inputs
mImage(this, "image"),
// outputs
mOutput(new cedar::aux::MatData(cv::Mat())),
image_back(cv::Mat::zeros(cv::Size(1, 1), CV_32FC3)),
_mDnnType
(
    new cedar::aux::EnumParameter
    (
        this,
        "dnn type",
        cedar::proc::steps::DNN::dnnType::typePtr(),
        dnnType::TL
    )
)
{

  cedar::aux::Path filename("resource://dnn/model_topless.onnx");
  model = cv::dnn::readNetFromONNX(filename.absolute());

  // declare outputs
  auto output_slot = this->declareOutput("DNN Inference", this->mOutput);

  // check the input for the proper type/dimensionality
  cedar::proc::typecheck::Matrix check;
  check.addAcceptedDimensionality(2);
  check.addAcceptedType(CV_32FC3);
  this->mImage.getSlot()->setCheck(check);

  // connect signals
  QObject::connect(this->_mDnnType.get(), SIGNAL(valueChanged()), this, SLOT(updateDnnType()));
  QObject::connect(this->_mDnnType.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::DNN::recompute()
{
    this->onTrigger();
}


void cedar::proc::steps::DNN::updateDnnType()
{
    image_back = cv::Mat::zeros(cv::Size(1, 1), CV_32FC3);

    switch (this->_mDnnType->getValue())
    {
    case dnnType::TL:
    {
        cedar::aux::Path filename("resource://dnn/model_topless.onnx");
        model = cv::dnn::readNetFromONNX(filename.absolute());
    }
        break;
    case dnnType::B5:
    {
        cedar::aux::Path filename("resource://dnn/model_early_b5.onnx");
        model = cv::dnn::readNetFromONNX(filename.absolute());
    }
    break;
    case dnnType::B4:
    {
        cedar::aux::Path filename("resource://dnn/model_early_b4.onnx");
        model = cv::dnn::readNetFromONNX(filename.absolute());
    }
    break;
    case dnnType::B3:
    {
        cedar::aux::Path filename("resource://dnn/model_early_b3.onnx");
        model = cv::dnn::readNetFromONNX(filename.absolute());
    }
    break;
    case dnnType::B2:
    {
        cedar::aux::Path filename("resource://dnn/model_early_b2.onnx");
        model = cv::dnn::readNetFromONNX(filename.absolute());
    }
    break;
    case dnnType::B1:
    {
        cedar::aux::Path filename("resource://dnn/model_early_b1.onnx");
        model = cv::dnn::readNetFromONNX(filename.absolute());
    }
    break;
    

    default:
        // unhandled type, do nothing.
        return;
    }

}


void cedar::proc::steps::DNN::compute(const cedar::proc::Arguments&)
{
  const cv::Mat& image = this->mImage.getData();

    if(sum(image) != sum(image_back))
    {
      cv::Mat blob = cv::dnn::blobFromImage(image, 1.0, cv::Size(224, 224), cv::Scalar(123.68, 116.779, 103.939), false, false);
      model.setInput(blob);
      cv::Mat output = model.forward();
      if(4 == output.dims)
      {
        int sizes[] = {output.size[2], output.size[3], output.size[1]};
        cv::Mat out = cv::Mat(3, sizes, CV_32FC1, cv::Scalar(0));

        for (int k = 0; k < output.size[1]; k++)
        {
            for (int i = 0; i < output.size[2]; i++)
            {
              for (int j = 0; j < output.size[3]; j++)
              {
                int p[] = {0, k, i, j};
                int p2[] = {i, j, k};
                out.at<float>(&p2[0]) = output.at<float>(&p[0]);
              }
            }
        }

        this->mOutput->setData(out.clone());
        emitOutputPropertiesChangedSignal("DNN Inference");
      }
      image_back = image.clone();
    }

}
