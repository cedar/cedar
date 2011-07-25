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

// LOCAL INCLUDES
#include "processing/source/GaussInput.h"
#include "dynamics/Activation.h"
#include "auxiliaries/DataT.h"
#include "auxiliaries/NumericParameter.h"
#include "auxiliaries/math/functions.h"
#include "processing/Arguments.h"
#include "auxiliaries/macros.h"
#include "auxiliaries/math/tools.h"
#include "auxiliaries/NumericVectorParameter.h"
#include "auxiliaries/exceptions.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>
#include <vector>
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::source::GaussInput::GaussInput()
:
cedar::proc::Step(),
mOutput(new cedar::dyn::MatActivation(cv::Mat())),
_mAmplitude(new cedar::aux::DoubleParameter("amplitude", 1.0, -10000.0, 10000.0)),
_mDimensionality(new cedar::aux::UIntParameter("dimensionality", 1, 1000)),
_mSigmas(new cedar::aux::DoubleVectorParameter("sigma", 2, 3.0, 0.01, 1000.0)),
_mCenters(new cedar::aux::DoubleVectorParameter("centers", 2, 3.0, -10000.0, 10000.0)),
_mSizes(new cedar::aux::UIntVectorParameter("sizes", 2, 10, 1, 1000.0))
{
  this->registerParameter(_mAmplitude);
  this->registerParameter(_mDimensionality);
  _mDimensionality->set(2);
//  _mDimensionality->setConstant(true);
  this->registerParameter(_mSigmas);
  _mSigmas->makeDefault();
  this->registerParameter(_mCenters);
  _mCenters->makeDefault();
  this->registerParameter(_mSizes);
  _mSizes->makeDefault();
//  _mSizes->setConstant(true);
  this->declareOutput("Gauss input");
  this->setOutput("Gauss input", mOutput);
  QObject::connect(_mAmplitude.get(), SIGNAL(parameterChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mSigmas.get(), SIGNAL(parameterChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mCenters.get(), SIGNAL(parameterChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mDimensionality.get(), SIGNAL(parameterChanged()), this, SLOT(updateDimensionality()));
  this->updateMatrix();
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::source::GaussInput::compute(const cedar::proc::Arguments&)
{
  std::vector<cv::Mat> kernel_parts;
  const unsigned int& dimensionality = _mDimensionality->get();
  const std::vector<double>& sigmas = _mSigmas->get();
  const std::vector<unsigned int>& sizes_uint = _mSizes->get();
  kernel_parts.resize(dimensionality);
  for (unsigned int dim = 0; dim < dimensionality; ++dim)
  {
    kernel_parts.at(dim) = cv::Mat(10, 1, CV_32F);
    CEDAR_DEBUG_ASSERT(sigmas.at(dim) > 0.0);
    for (int row = 0; row < kernel_parts.at(dim).rows; ++row)
    {
      kernel_parts.at(dim).at<float>(row, 0)
          = cedar::aux::math::gauss(static_cast<int>(row) - _mCenters->get().at(dim), sigmas.at(dim));
    }
  }
  kernel_parts.at(0) *= _mAmplitude->get();
  // assemble the input
  std::vector<int> sizes(static_cast<size_t>(dimensionality));
  for (unsigned int i = 0; i < dimensionality; i++)
  {
    sizes[i] = sizes_uint.at(i);
  }
  if (dimensionality == 1)
  {
    mOutput->getData() = cv::Mat(sizes.at(0), 1, CV_32F);
  }
  else
  {
    mOutput->getData() = cv::Mat(static_cast<int>(dimensionality), &sizes.at(0), CV_32F);
  }
  mOutput->lockForWrite();
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
      kernel.at<float>(position[0], 1) = value;
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
  mOutput->unlock();
}

void cedar::proc::source::GaussInput::updateMatrix()
{
  this->compute(cedar::proc::Arguments());
}

void cedar::proc::source::GaussInput::updateDimensionality()
{
  int old_dimensionality = mOutput->getData().dims;
  if (mOutput->getData().cols == 1) // check for special case of 1d matrix
  {
    old_dimensionality = 1;
  }
  int new_dimensionality = static_cast<int>(_mDimensionality->get());
  _mSigmas->get().resize(new_dimensionality);
  _mCenters->get().resize(new_dimensionality);
  _mSizes->get().resize(new_dimensionality);
  if (new_dimensionality < old_dimensionality) // reduced dimensionality
  {
    // nothing to do here at the moment
  }
  else if (new_dimensionality > old_dimensionality) // higher dimensionality
  {
    for (int new_dim = old_dimensionality; new_dim < new_dimensionality; ++new_dim)
    {
      _mSigmas->get().at(new_dim) = _mSigmas->getDefaultValue();
      _mCenters->get().at(new_dim) = _mCenters->getDefaultValue();
      _mSizes->get().at(new_dim) = _mSizes->getDefaultValue();
    }
  }
  this->updateMatrix();
}
