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

    File:        Stack.cpp

    Maintainer:  
    Email:       
    Date:        

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/Stack.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/typecheck/SameSize.h"
#include "cedar/processing/typecheck/SameTypeCollection.h"
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/processing/typecheck/And.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/casts.h"


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
      new ElementDeclarationTemplate<cedar::proc::steps::Stack>
      (
        "Arrays",
        "cedar.processing.steps.Stack"
      )
    );
    declaration->setIconPath(":/steps/stack.svg");
    declaration->setDescription
    (
      "Stacks an arbitrary set of matrices (tensors of second order). All matrices must have the same size."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::Stack::Stack()
:
_mOutputDimension (new cedar::aux::UIntParameter(this, "dimensionality", 1,1,255))
{
  QObject::connect(_mOutputDimension.get(), SIGNAL(valueChanged()), this, SLOT(dimensionChanged()));
  auto slot_0 = this->declareInput("0",false);
  cedar::proc::typecheck::And input_check;
  cedar::proc::typecheck::SameSize same_check;
  cedar::proc::typecheck::Matrix matrix_check;
  matrix_check.addAcceptedDimensionality(2);
  matrix_check.addAcceptedType(CV_32F);
  matrix_check.acceptsEmptyMatrix(true);
  same_check.addSlot(slot_0);
  input_check.addCheck(same_check);
  input_check.addCheck(matrix_check);
  slot_0->setCheck(input_check);
  this->mInputs.resize(1);
  int sizes[3];
  sizes[0] = 1;
  sizes[1] = 1;
  sizes[2] = 1;
  mOutput = cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat(3, sizes, CV_32F, 0.)));
  this->declareOutput("stack", this->mOutput);
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Stack::dimensionChanged()
{
  unsigned long oldsize = this->mInputs.size();
  unsigned long newsize = static_cast<unsigned long>(_mOutputDimension->getValue());
  this->mInputs.resize(newsize);
  int sizes[3];
  sizes[0] = this->rows;
  sizes[1] = this->cols;
  sizes[2] = static_cast<int>(this->mInputs.size());
  QWriteLocker l(&this->mOutput->getLock());
  this->mOutput->setData(cv::Mat(3, sizes, CV_32F, 0.));
  l.unlock();
  if(oldsize > newsize)
  {
    for(int i = static_cast<int>(newsize); i < static_cast<int>(oldsize); ++i)
    {
      this->removeInputSlot(QString::number(i).toStdString());
    }
  }
  else
  {
    for(int i = static_cast<int>(oldsize); i < static_cast<int>(newsize); ++i)
    {
      this->declareInput(QString::number(i).toStdString(),false);
    }
  }
  cedar::proc::typecheck::And input_check;
  cedar::proc::typecheck::SameSize same_check;
  cedar::proc::typecheck::Matrix matrix_check;
  matrix_check.addAcceptedDimensionality(2);
  matrix_check.addAcceptedType(CV_32F);
  matrix_check.acceptsEmptyMatrix(true);
  for(int i = 0; i < static_cast<int>(newsize); ++i)
  {
    auto slot = this->getInputSlot(QString::number(i).toStdString());
    same_check.addSlot(slot);
  }
  input_check.addCheck(same_check);
  input_check.addCheck(matrix_check);
  for(int i = 0; i < static_cast<int>(newsize); ++i)
  {
    auto slot = this->getInputSlot(QString::number(i).toStdString());
    slot->setCheck(input_check);
    this->redetermineInputValidity(QString::number(i).toStdString());
  }
  if (!this->allInputsValid())
  {
    return;
  }
  this->callComputeWithoutTriggering();
  this->emitOutputPropertiesChangedSignal("stack");
}

void cedar::proc::steps::Stack::compute(const cedar::proc::Arguments&)
{
  if (!this->allInputsValid())
  {
    return;
  }

  cv::Mat& output = this->mOutput->getData();
  int sizes[3];
  sizes[0] = 0;
  sizes[1] = 0;
  sizes[2] = static_cast<int>(this->mInputs.size());
  for(int row = 0; row < output.size[0]; ++row)
  {
    sizes[0] = row;
    for(int col = 0; col < output.size[1]; ++col)
    {
      sizes[1] = col;
      for(int i = 0; i < static_cast<int>(this->mInputs.size()); ++i)
      {
        sizes[2] = i;
        if(auto data = this->getInput(QString::number(i).toStdString()))
        {
          if(auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
          {
            const cv::Mat& input = mat_data->getData();
            output.at<float>(sizes) = input.at<float>(row,col);
          }
        }
      }
    }
  }
}

void cedar::proc::steps::Stack::inputConnectionChanged(const std::string& inputName)
{
  if (auto data = this->getInput(inputName))
  {
    if(auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
    {
      unsigned int i = static_cast<unsigned int>(QString::fromStdString(inputName).toInt());
      //mInputs.at(i) = mat_data;
      this->rows = mat_data->getData().rows;
      this->cols = mat_data->getData().cols;
      if (mOutput->getData().size[0] != mat_data->getData().rows || mOutput->getData().size[1] != mat_data->getData().cols)
      {
        int sizes[3];
        sizes[0] = mat_data->getData().rows;
        sizes[1] = mat_data->getData().cols;
        sizes[2] = static_cast<int>(this->mInputs.size());
        QWriteLocker l(&this->mOutput->getLock());
        this->mOutput->setData(cv::Mat(3, sizes, CV_32F, 0.));
        l.unlock();
      }
      for (unsigned int j = 0; j < this->mInputs.size(); ++j)
      {
        if (j != i)
        {
          this->redetermineInputValidity(QString::number(j).toStdString());
        }
      }
      if (!this->allInputsValid())
      {
        return;
      }
      this->callComputeWithoutTriggering();
      this->emitOutputPropertiesChangedSignal("stack");

    }
  }
}
