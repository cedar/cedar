/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        LinearSpatialTemplate.cpp

    Maintainer:  
    Email:       
    Date:        

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include <cedar/processing/sources/LinearSpatialTemplate.h>

// SYSTEM INCLUDES
#include <cedar/auxiliaries/assert.h>
#include <cedar/auxiliaries/exceptions.h>
#include <cedar/auxiliaries/DataTemplate.h>
#include <cedar/auxiliaries/MatData.h>
#include <cedar/auxiliaries/BoolParameter.h>
#include <cedar/auxiliaries/UIntParameter.h>
#include <cedar/auxiliaries/DoubleParameter.h>
#include <cedar/auxiliaries/ObjectParameterTemplate.h>
#include <cedar/auxiliaries/math/TransferFunction.h>
#include <cedar/auxiliaries/math/transferFunctions/AbsSigmoid.h>
#include <cedar/auxiliaries/math/tools.h>
#include <cedar/processing/ElementDeclaration.h>
#include <cmath>
#include <iostream>

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
      new ElementDeclarationTemplate<cedar::proc::sources::LinearSpatialTemplate>
      (
        "Sources",
        "cedar.processing.sources.LinearSpatialTemplate"
      )
    );
    declaration->setIconPath(":/steps/linear_spatial_template.svg");
    declaration->setDescription("Generates a matrix with a linear gradient. Note: Defaults to diagonal orientation in the 2D case; use the Projection step from a 1D gradient to generate a linear gradient along one dimension, only.");

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::sources::LinearSpatialTemplate::LinearSpatialTemplate()
:
mTemplate(new cedar::aux::MatData(cv::Mat::zeros(10, 10, CV_32F))),
_mSizeX(new cedar::aux::UIntParameter(this, "size x", 50, cedar::aux::UIntParameter::LimitType::positive(1000))), 
_mSizeY(new cedar::aux::UIntParameter(this, "size y", 50, cedar::aux::UIntParameter::LimitType::positive(1000))),
_mStart(new cedar::aux::DoubleParameter(this, "lowest", 0.0)),
_mEnd(new cedar::aux::DoubleParameter(this, "upmost", 1.0))
{
  // output
  this->declareOutput("spatial pattern", mTemplate);

  QObject::connect(_mSizeX.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(_mSizeY.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(_mStart.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(_mEnd.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));


  this->recompute();
}

cedar::proc::sources::LinearSpatialTemplate::~LinearSpatialTemplate()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sources::LinearSpatialTemplate::recompute()
{
  unsigned int size_x = _mSizeX->getValue();
  unsigned int size_y = _mSizeY->getValue();
  unsigned int size_sum = size_x + size_y;

  double start = _mStart->getValue();
  double end = _mEnd->getValue();

  mTemplate->getData() = cv::Mat(size_x, size_y, CV_32F);

  // go through all positions of the pattern
  for (unsigned int i = 0; i < size_x; i++)
  {
    for (unsigned int j = 0; j < size_y; j++)
    {
      mTemplate->getData().at<float>(i, j)
        = static_cast<float>( start
                              + ( end-start ) * (i+j)/(size_sum-2) );
    }
  }

  // this triggers all connected steps.
  this->onTrigger();
}

void cedar::proc::sources::LinearSpatialTemplate::compute(const cedar::proc::Arguments&)
{
}
