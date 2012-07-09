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

    File:        GaussInput.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 19

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/sources/GaussInput.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/math/functions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <iostream>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr input_decl
    (
      new ElementDeclarationTemplate<cedar::proc::sources::GaussInput>
      (
        "Sources",
        "cedar.processing.sources.GaussInput"
      )
    );
    input_decl->setIconPath(":/steps/gauss_input.svg");
    input_decl->setDescription("Generates a matrix that contains a sampled Gauss function.");
    cedar::proc::DeclarationRegistrySingleton::getInstance()->declareClass(input_decl);

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::sources::GaussInput::GaussInput()
:
cedar::proc::Step(),
mOutput(new cedar::aux::MatData(cv::Mat())),
_mAmplitude(new cedar::aux::DoubleParameter(this, "amplitude", 1.0)),
_mDimensionality(new cedar::aux::UIntParameter(this, "dimensionality", 2, 1, 1000)),
_mSigmas(new cedar::aux::DoubleVectorParameter(this, "sigma", 2, 3.0, 0.01, 1000.0)),
_mCenters(new cedar::aux::DoubleVectorParameter(this, "centers", 2, 3.0, -10000.0, 10000.0)),
_mSizes(new cedar::aux::UIntVectorParameter(this, "sizes", 2, 10, 1, 1000.0)),
_mIsCyclic(new cedar::aux::BoolParameter(this, "cyclic", false))
{
  this->declareOutput("Gauss input");
  this->setOutput("Gauss input", mOutput);
  QObject::connect(_mAmplitude.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mSigmas.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mCenters.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mSizes.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(updateDimensionality()));
  QObject::connect(_mIsCyclic.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  this->updateMatrix();
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sources::GaussInput::compute(const cedar::proc::Arguments&)
{
  std::vector<cv::Mat> kernel_parts;
  const unsigned int& dimensionality = _mDimensionality->getValue();
  const std::vector<double>& sigmas = _mSigmas->getValue();
  const std::vector<unsigned int>& sizes_uint = _mSizes->getValue();
  kernel_parts.resize(dimensionality);
  for (size_t dim = 0; dim < dimensionality; ++dim)
  {
    kernel_parts.at(dim) = cv::Mat(static_cast<int>(sizes_uint.at(dim)), 1, CV_32F);
    CEDAR_DEBUG_ASSERT(sigmas.at(dim) > 0.0);

    //!@todo may want to move this outside of (for dim) block, since all dims are cyclic
    if (_mIsCyclic->getValue()) // is this a cyclic kernel? (only check once)
    {
      for (int row = 0; row < kernel_parts.at(dim).rows; ++row)
      {
        int index = row - _mCenters->at(dim);
        int current_size = static_cast<int>(sizes_uint.at(dim));
        /* if the kernel is shifted away from the center of the matrix, make sure to take as many values from a
         * cyclic kernel as the kernel center is shifted away from the matrix center
         */
        int shift = _mCenters->at(dim) - current_size / 2;
        if (shift > 0 && row - shift < 0) // kernel center is to the right of matrix center and index is in cyclic range
        {
          index += current_size;
        }
        if (shift < 0 && row - shift >= current_size) // kernel center is to the left of matrix center, cyclic index
        {
          index -= current_size;
        }
        kernel_parts.at(dim).at<float>(row, 0)
              = cedar::aux::math::gauss(index, sigmas.at(dim));
      }
    }
    else // nothing special to do here
    {
      for (int row = 0; row < kernel_parts.at(dim).rows; ++row)
      {
        kernel_parts.at(dim).at<float>(row, 0)
              = cedar::aux::math::gauss(static_cast<int>(row) - _mCenters->at(dim), sigmas.at(dim));
      }
    }
  }
  kernel_parts.at(0) *= _mAmplitude->getValue();
  // assemble the input
  std::vector<int> sizes(static_cast<size_t>(dimensionality));
  for (unsigned int i = 0; i < dimensionality; i++)
  {
    sizes[i] = sizes_uint.at(i);
  }
  if (dimensionality == 1)
  {
    mOutput->getData() = cv::Mat(static_cast<int>(sizes.at(0)), 1, CV_32F);
  }
  else
  {
    mOutput->getData() = cv::Mat(static_cast<int>(dimensionality), &sizes.at(0), CV_32F);
  }
  cv::Mat& kernel = mOutput->getData();
  // now fill up the big kernel matrix
  std::vector<int> position(static_cast<size_t>(dimensionality));
  unsigned int max_index = 1;
  double max_index_d = 1.0;
  for (unsigned int dim = 0; dim < dimensionality; dim++)
  {
    position[dim] = 0;
    max_index_d *= sizes[dim];
    if (max_index_d > std::numeric_limits<unsigned int>::max()/100.0)
    {
      CEDAR_THROW(cedar::aux::RangeException, "cannot handle inputs of this size");
    }
  }
  max_index = static_cast<unsigned int>(max_index_d);
  for (unsigned int i = 0; i < max_index; i++)
  {
    float value = 1.0;
    for (unsigned int dim = 0; dim < dimensionality; dim++)
    {
      value *= kernel_parts.at(dim).at<float>(position.at(dim), 0);
    }
    if (dimensionality == 1)
    {
      kernel.at<float>(position[0], 0) = value;
    }
    else
    {
      kernel.at<float>(&position.at(0)) = value;
    }
    // increment index
    position[0]++;
    for (unsigned int dim = 0; dim < dimensionality; dim++)
    {
      if (position[dim] >= static_cast<int>(sizes[dim]))
      {
        position[dim] = 0;
        if (dim+1 < dimensionality)
        {
          position[dim+1]++;
        }
      }
    }
  }
}

void cedar::proc::sources::GaussInput::updateMatrix()
{
  this->onTrigger();
}

void cedar::proc::sources::GaussInput::updateDimensionality()
{
  int new_dimensionality = static_cast<int>(_mDimensionality->getValue());
  _mSigmas->resize(new_dimensionality, _mSigmas->getDefaultValue());
  _mSigmas->setDefaultSize(new_dimensionality);
  _mCenters->resize(new_dimensionality, _mCenters->getDefaultValue());
  _mCenters->setDefaultSize(new_dimensionality);
  _mSizes->resize(new_dimensionality, _mSizes->getDefaultValue());
  _mSizes->setDefaultSize(new_dimensionality);
  this->onTrigger();
}
