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
#include "cedar/processing/typecheck/SameSizedCollection.h"
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
        "Algebra",
        "cedar.processing.steps.Stack"
      )
    );
    declaration->setIconPath(":/steps/stack.svg");
    declaration->setDescription
    (
      "Stacks an arbitrary set of 2D input matrices. All matrices must have the same size."
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
// outputs
mOutput(new cedar::aux::MatData(cv::Mat::zeros(3, 1, CV_32F)))
{
  // declare all data
  auto input_slot = this->declareInputCollection("input matrices");

  cedar::proc::typecheck::And input_check;
  cedar::proc::typecheck::SameSizedCollection same_check(false,false);
  cedar::proc::typecheck::Matrix matrix_check;
  matrix_check.addAcceptedDimensionality(2);
  matrix_check.addAcceptedType(CV_32F);
  input_check.addCheck(same_check);
  input_check.addCheck(matrix_check);
  input_slot->setCheck(input_check);

  this->declareOutput("stack", this->mOutput);

  this->mInputs = this->getInputSlot("input matrices");
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::proc::steps::Stack::compute(const cedar::proc::Arguments&)
{
  cv::Mat& output = this->mOutput->getData();
  int sizes[3];
  sizes[0] = 0;
  sizes[1] = 0;
  sizes[2] = static_cast<int>(this->mInputs->getDataCount());
  for(int row = 0; row < output.size[0]; ++row)
  {
     sizes[0] = row;
     for(int col = 0; col < output.size[1]; ++col)
     {
       sizes[1] = col;
	for(size_t i = 0; i < this->mInputs->getDataCount(); ++i)
     	{
       	  sizes[2] = i;
auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->mInputs->getData(i));

	  output.at<float>(sizes) = mat_data->getData().at<float>(row,col);
     	}	
     }
  }
}

void cedar::proc::steps::Stack::inputConnectionChanged(const std::string& /*inputName*/)
{
  if (this->mInputs->getDataCount() > 0)
  {
    if (!this->allInputsValid())
    {
      return;
    }
    QWriteLocker r_l(&this->mOutput->getLock());
    cv::Mat old_output = this->mOutput->getData().clone();
    r_l.unlock();
    if (this->mInputs->getDataCount() > 0)
    {
      auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->mInputs->getData(0));
      if (mat_data)
      {
	int sizes[3];
	sizes[0] = mat_data->getData().rows;
	sizes[1] = mat_data->getData().cols;
	sizes[2] = static_cast<int>(this->mInputs->getDataCount());

        QWriteLocker l(&this->mOutput->getLock());
	this->mOutput->setData(cv::Mat(3,sizes,CV_32F));        
        l.unlock();
      }
    }
    this->callComputeWithoutTriggering();
    cedar::proc::Step::ReadLocker locker(this);
    const cv::Mat& output = this->mOutput->getData();
    bool changed = old_output.type() != output.type() || old_output.size != output.size;
    locker.unlock();
    if (changed)
    {
      this->emitOutputPropertiesChangedSignal("stack");
    }
  }
}
