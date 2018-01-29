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

    File:        Nullspace.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       
    Date:        

    Description: Source file for the class cedar::proc::steps::Nullspace.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/steps/Nullspace.h"
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
      new ElementDeclarationTemplate<cedar::proc::steps::Nullspace>
      (
        "Robotics",
        "cedar.processing.Nullspace"
      )
    );
    declaration->setIconPath(":/steps/nullspace.svg");
    declaration->setDescription
    (
      "Calculates the (kinematic) Nullspace matrix from a Jacobian. "
      "Optionally also applies a vector to that nullspace matrix and "
      "calculates its orthogonal vector."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::Nullspace::Nullspace()
:
mInputJacobian(this, "jacobian"),
mInputVector(this, "vector", false),
mNullspace(new cedar::aux::MatData(cv::Mat())),
mProjectedVector(new cedar::aux::MatData(cv::Mat())),
mOrthogonalVector(new cedar::aux::MatData(cv::Mat()))
{
  auto output = this->declareOutput("nullspace", mNullspace);
  auto output2 = this->declareOutput("projected vector", mProjectedVector);
  auto output3 = this->declareOutput("orthogonal vector", mOrthogonalVector);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Nullspace::compute(const cedar::proc::Arguments&)
{
  recompute();
}

void cedar::proc::steps::Nullspace::recompute()
{
  auto input = getInput("jacobian");
  if (!input)
    return;

  auto input_mat = input->getData<cv::Mat>();

  cv::Mat ident_mat = cv::Mat::eye( input_mat.cols, input_mat.cols, CV_32F );
  cv::Mat invert_mat;

  cv::invert( input_mat, invert_mat, cv::DECOMP_SVD);


  cv::Mat tmp_mat =  invert_mat * input_mat;

  cv::Mat retNullspace= ident_mat - tmp_mat;

  this->mNullspace->setData(retNullspace);

  // the rest is is optional:
  auto input2 = getInput("vector");

  cv::Mat inputvector= cv::Mat();

  if (input2)
  {
    inputvector= input2->getData<cv::Mat>();
  }

  if (!input2
      || inputvector.empty())
  {
    if (mProjectedVector
        && !(mProjectedVector->getData().empty()) )
    {
      mProjectedVector->setData(cv::Mat());
    }

    if (mOrthogonalVector
        && !(mOrthogonalVector->getData().empty()) )
    {
      mOrthogonalVector->setData(cv::Mat());
    }

    return;
  }

  cv::Mat nullspacevector = retNullspace * inputvector;
  this->mProjectedVector->setData( nullspacevector );
  this->mOrthogonalVector->setData( inputvector - nullspacevector );
}

void cedar::proc::steps::Nullspace::inputConnectionChanged(const std::string&)
{
  recompute();
}

