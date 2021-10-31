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

    File:        DiagonalReadout.cpp

    Maintainer:  
    Email:       
    Date:        

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/DiagonalReadout.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/Arguments.h"
#include "cedar/auxiliaries/DataTemplate.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/processing/typecheck/SameSize.h"
#include "cedar/processing/typecheck/And.h"
#include "cedar/processing/typecheck/Matrix.h"

// SYSTEM INCLUDES
#include <boost/make_shared.hpp>


//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr multiply_decl
    (
      new ElementDeclarationTemplate<cedar::proc::steps::DiagonalReadout>
      (
        "DFT",
        "cedar.processing.DiagonalReadout"
      )
    );
    //multiply_decl->setIconPath(":/steps/a_minus_b.svg");
    multiply_decl->setDescription
    (
      "THIS STEP IS STILL IN EARLY DEVELOPMENT.\n"
      "For input vectors a_i and b_j, read-out the sum along the diagonals "
      "of the matrix A_ij and apply a function g() to every element: "
      "A_ij= g(a_i + b_j). Where g() transfer function. "
      "Currently this is the Semi-Linear Sigmoid with a bias "
      "of -1.\n"
      "TODO: generalize to 2nd and 3rd order tensors.\n"
      "TODO: make (the inner) transfer function selectable. Also no "
      "transfer function should be an option.\n"
      "TODO: make an icon.\n"
    );

    multiply_decl->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::DiagonalReadout::DiagonalReadout()
:
mOutput(new cedar::aux::MatData()),
mInput(new cedar::aux::MatData()),
mInput2(new cedar::aux::MatData())
{
  auto matrix_slot = this->declareInput("input1");
  cedar::proc::typecheck::And input_check;
  cedar::proc::typecheck::Matrix matrix_check;
  matrix_check.addAcceptedType(CV_32F);

  auto matrix2_slot = this->declareInput("input2");
  cedar::proc::typecheck::And input2_check;
  cedar::proc::typecheck::Matrix matrix2_check;
  matrix2_check.addAcceptedType(CV_32F);

  cedar::proc::typecheck::SameSize same_check;
  same_check.addSlot(matrix_slot);
  same_check.addSlot(matrix2_slot);
  input_check.addCheck(matrix_check);
  input2_check.addCheck(matrix2_check);
  input_check.addCheck(same_check);
  input2_check.addCheck(same_check);

  matrix_slot->setCheck(input_check);
  matrix2_slot->setCheck(input2_check);

  this->declareOutput("result", mOutput);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::DiagonalReadout::inputConnectionChanged(const std::string& inputName)
{
  auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->getInput(inputName));
  int siz= 0;

  if (!mat_data || mat_data->isEmpty())
    return;

  if (inputName == "input1")
  {
    this->mInput = mat_data;
    siz= mat_data->getData().rows;
  }
  else if (inputName == "input2")
  {
    this->mInput2 = mat_data;
    siz= mat_data->getData().rows; // yes, rows
  }
  else
  {
    CEDAR_ASSERT(false && "This should not happen.");
  }

  int out_siz= 2 * siz - 1;
  if (!this->mOutput || this->mOutput->isEmpty()
      || mOutput->getData().rows != out_siz)
  {
    if (siz <= 0)
    {
      this->mOutput->setData( cv::Mat() );
      this->emitOutputPropertiesChangedSignal("result");
      return; // dont recompute
    }
    else
    {
      this->mOutput->setData( cv::Mat::zeros(out_siz, 1, CV_32F) );
    }
  }

  // tell successive steps that the output changed
  recompute();
  this->emitOutputPropertiesChangedSignal("result");
}

void cedar::proc::steps::DiagonalReadout::compute(const cedar::proc::Arguments&)
{
  recompute();
}

void cedar::proc::steps::DiagonalReadout::recompute()
{
  if (!mInput
      || !mInput2)
  {
    return;
  }

  const cv::Mat &matrix = mInput->getData();
  const cv::Mat &matrix2 = mInput2->getData();

  if (matrix.empty()
      || matrix2.empty())
  {
    return;
  }

  int siz1= matrix.rows; // cols is 1
  //int siz2= matrix2.rows; // cols is 1
  int both_siz = siz1; // siz1 == siz2 guaranteed
  int out_siz = both_siz * 2 - 1;

  if ( out_siz != mOutput->getData().rows )
  {
    return;
  }

  this->mOutput->setData( cv::Mat::zeros(out_siz, 1, CV_32F) );

  // first half, incl mid-point (i.e. longest diagonal)
  for (int index_out= 0; index_out < both_siz; index_out++ )
  {
     for (int i=0; i < index_out; i++)
     {
        float val1= matrix.at<float>( index_out - i , 0 );
        float val2= matrix2.at<float>( i, 0 );

        // TODO: make this inner transfer function selectable 
        mOutput->getData().at<float>( index_out, 0 )+= std::max( 0.0, val1 + val2 - 1.0); 
    }
  }

  // second half (excluding the long diagonal)
  for (int index_out = both_siz; index_out < out_siz; index_out++ ) // 50..99
  {
     for (int i=0; i < out_siz - index_out; i++) // 0..99-index_out
     {
        float val1= matrix.at<float>( both_siz - i - 1, 0 );
        float val2= matrix2.at<float>( index_out - both_siz + i, 0 );

        // TODO: make this inner transfer function selectable 
        mOutput->getData().at<float>( index_out, 0 )+= std::max( 0.0, val1 + val2 - 1.0); // TODO
    }
  }

}

