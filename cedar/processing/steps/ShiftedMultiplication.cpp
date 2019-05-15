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

    File:        ShiftedMultiplication.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2016 01 08

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
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
#include <cedar/auxiliaries/math/constants.h>
#include <cedar/processing/steps/ShiftedMultiplication.h>
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"

// SYSTEM INCLUDES
#include <cmath>


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
    new ElementDeclarationTemplate<cedar::proc::steps::ShiftedMultiplication>
    (
      "Arrays",
      "cedar.processing.steps.ShiftedMultiplication"
    )
  );

  declaration->setIconPath("");
  declaration->setDescription
  (
    "A step that multiplies two arrays by shifting them by a fixed distance against each other and rotating them around each other by increments. Yields a three-dimensional output. TODO: a formula would sure be nice, here"
  );

  declaration->declare();

  return true;
}

bool declared = declare();
}


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ShiftedMultiplication::ShiftedMultiplication()
:
mOutput(new cedar::aux::MatData(cv::Mat::zeros(10, 10, CV_32F))),
_mDistance(new cedar::aux::UIntParameter(this, "shift distance", 10, cedar::aux::UIntParameter::LimitType::positive(1000))),
_mOrientationSize(new cedar::aux::UIntParameter(this, "orientation size", 10, cedar::aux::UIntParameter::LimitType::positive(1000)))
{
  // input
  this->declareInput("toward signal", true);
  this->declareInput("away signal", true);

  // output
  this->declareOutput("output", mOutput);

  QObject::connect(_mDistance.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(_mOrientationSize.get(), SIGNAL(valueChanged()), this, SLOT(reconfigure()));

  this->recompute();
}

cedar::proc::steps::ShiftedMultiplication::~ShiftedMultiplication()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::ShiftedMultiplication::recompute()
{
  unsigned int distance_shift = _mDistance->getValue();
  unsigned int orientation_size = _mOrientationSize->getValue();

  auto toward_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(getInput("toward signal"));
  auto away_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(getInput("away signal"));
  auto output = mOutput->getData(); 

  if (toward_data && away_data)
  {
    auto size_x = toward_data->getData().cols;
    auto size_y = toward_data->getData().rows;

    // go through all orientations
    for (unsigned int i = 0; i < orientation_size; ++i)
    {
      // angle in radians that a single rotation covers
      double rotation_angle = 2.0 * cedar::aux::math::pi / orientation_size;

      // pixel shifts for the away image
      int shift_x = static_cast<int>(round(cos(i * rotation_angle) * distance_shift));
      int shift_y = static_cast<int>(round(sin(i * rotation_angle) * distance_shift));

      for (int x = 0; x < size_x; ++x)
      {
        for (int y = 0; y < size_y; ++y)
        {
          // index for 
          int x0 = x + shift_x;
          int y0 = y + shift_y;

          float toward_activation = 0.0;

          if (x0 < size_x && x0 >= 0 && y0 < size_y && y0 >= 0)
          {
            toward_activation = toward_data->getData().at<float>(y0, x0);
          }

          output.at<float>(y, x, i) = toward_activation * away_data->getData().at<float>(y, x);
        }
      }
    }

    // this triggers all connected steps.
    this->onTrigger();
  }
}

void cedar::proc::steps::ShiftedMultiplication::inputConnectionChanged(const std::string&)
{
  reconfigure();
}

void cedar::proc::steps::ShiftedMultiplication::reconfigure()
{
  unsigned int orientation_size = _mOrientationSize->getValue();

  auto toward_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(getInput("toward signal"));
  auto away_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(getInput("away signal"));

  if (toward_data && away_data)
  {
    auto size_x = toward_data->getData().cols;
    auto size_y = toward_data->getData().rows;

    CEDAR_ASSERT(size_x == away_data->getData().cols);
    CEDAR_ASSERT(size_y == away_data->getData().rows);

    std::vector<int> sizes;
    sizes.push_back(size_y);
    sizes.push_back(size_x);
    sizes.push_back(orientation_size);

    mOutput->getData() = cv::Mat(3, &sizes.at(0), CV_32F, cv::Scalar(0));
  }
}

void cedar::proc::steps::ShiftedMultiplication::compute(const cedar::proc::Arguments&)
{
  this->recompute();
}
