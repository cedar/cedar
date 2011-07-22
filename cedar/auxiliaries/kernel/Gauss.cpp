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

    File:        Gauss.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 07 07

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "auxiliaries/kernel/Gauss.h"
#include "auxiliaries/NumericParameter.h"
#include "processing/DataT.h"
#include "auxiliaries/math/functions.h"
#include "auxiliaries/exceptions.h"
#include "auxiliaries/NumericVectorParameter.h"
#include "auxiliaries/macros.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>
#include <limits.h>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::kernel::Gauss::Gauss()
:
cedar::aux::kernel::Separable(),
_mAmplitude(new cedar::aux::DoubleParameter("amplitude", -10000.0, 10000.0)),
_mSigmas(new cedar::aux::DoubleVectorParameter("sigmas", 2, 3.0, 0.0, 10000)),
_mShifts(new cedar::aux::DoubleVectorParameter("shifts", 2, 3.0, 0.0, 10000)),
_mLimit(new cedar::aux::DoubleParameter("limit", 0.01, 1000.0))
{
  this->registerParameter(_mAmplitude);
  this->registerParameter(_mLimit);
  this->registerParameter(_mSigmas);
  this->registerParameter(_mShifts);
  this->onInit();
}

cedar::aux::kernel::Gauss::Gauss(
                                  double amplitude,
                                  std::vector<double> sigmas,
                                  std::vector<double> shifts,
                                  double limit,
                                  unsigned int dimensionality,
                                  const std::string& kernelFile
                                )
:
cedar::aux::kernel::Separable(dimensionality, kernelFile),
_mAmplitude(new cedar::aux::DoubleParameter("amplitude", amplitude, -10000.0, 10000.0)),
_mSigmas(new cedar::aux::DoubleVectorParameter("sigmas", sigmas, 0.0, 10000)),
_mShifts(new cedar::aux::DoubleVectorParameter("shifts", shifts, 0.0, 10000)),
_mLimit(new cedar::aux::DoubleParameter("limit", limit, 0.01, 1000.0))
{
  this->registerParameter(_mAmplitude);
  this->registerParameter(_mLimit);
  this->registerParameter(_mSigmas);
  this->registerParameter(_mShifts);
  this->mCenters.resize(dimensionality);
  this->mKernelParts.resize(dimensionality);
  this->mSizes.resize(dimensionality);
  this->onInit();
}

cedar::aux::kernel::Gauss::~Gauss()
{
#ifdef DEBUG
  std::cout << "> freeing data (Gauss)" << std::endl;
#endif
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::kernel::Gauss::onInit()
{
  calculate();
  QObject::connect(_mAmplitude.get(), SIGNAL(parameterChanged()), this, SLOT(updateKernel()));
  QObject::connect(_mLimit.get(), SIGNAL(parameterChanged()), this, SLOT(updateKernel()));
  QObject::connect(_mSigmas.get(), SIGNAL(parameterChanged()), this, SLOT(updateKernel()));
  QObject::connect(_mShifts.get(), SIGNAL(parameterChanged()), this, SLOT(updateKernel()));
  QObject::connect(_mDimensionality.get(), SIGNAL(parameterChanged()), this, SLOT(updateDimensionality()));
}

void cedar::aux::kernel::Gauss::calculate()
{
  mpReadWriteLockOutput->lockForWrite();
  const unsigned int& dimensionality = _mDimensionality->get();
  const double& amplitude = _mAmplitude->get();
  // sanity check
  CEDAR_DEBUG_ASSERT(dimensionality == _mSigmas->get().size());
  CEDAR_DEBUG_ASSERT(dimensionality == _mShifts->get().size());
  try
  {
    mKernelParts.resize(dimensionality);
    mCenters.resize(dimensionality);
    mSizes.resize(dimensionality);
    // calculate the kernel parts for every dimension
    for (unsigned int dim = 0; dim < dimensionality; dim++)
    {
      // estimate width
      if (_mSigmas->get().at(dim) != 0)
      {
        mSizes.at(dim) = getWidth(dim);
      }
      else
      {
        mSizes.at(dim) = 1;
      }
      mCenters.at(dim) = floor(mSizes.at(dim) / 2) + _mShifts->get().at(dim);
      mKernelParts.at(dim) = cv::Mat::zeros(mSizes.at(dim), 1, CV_32FC1);

      // calculate kernel part
      if (_mSigmas->get().at(dim) != 0)
      {
        for (unsigned int j = 0; j < mSizes.at(dim); j++)
        {
          //!\todo move Gauss function and filling up of matrix to some tool function
          mKernelParts.at(dim).at<float>(j, 0)
              = cedar::aux::math::gauss(static_cast<int>(j) - mCenters.at(dim), _mSigmas->get().at(dim));
        }
      }
      else // discrete case
      {
        mKernelParts.at(dim).at<float>(0, 0) = 1;
      }
      // normalize
      mKernelParts.at(dim) = mKernelParts.at(dim) * (1. / cv::sum(mKernelParts.at(dim)).val[0]);
      if (dim == 0)
      {
        mKernelParts.at(dim) = amplitude * mKernelParts.at(dim);
      }
    }
    // assemble the kernel
    int sizes[dimensionality];
    for (unsigned int i = 0; i < mSizes.size(); i++)
    {
      sizes[i] = mSizes.at(i);
    }
    if (dimensionality == 1)
    {
      mKernel->getData<cv::Mat>() = cv::Mat(sizes[0], 1, CV_32F);
    }
    else
    {
      mKernel->getData<cv::Mat>() = cv::Mat(static_cast<int>(dimensionality), sizes, CV_32F);
    }
    cv::Mat& kernel = mKernel->getData<cv::Mat>();
    // now fill up the big kernel matrix
    int position[dimensionality];
    unsigned int max_index = 1;
    double max_index_d = 1.0;
    for (unsigned int dim = 0; dim < dimensionality; dim++)
    {
      position[dim] = 0;
      max_index_d *= mSizes.at(dim);
      if (max_index_d > std::numeric_limits<unsigned int>::max()/100.0)
      {
        CEDAR_THROW(cedar::aux::RangeException, "cannot handle kernels of this size");
      }
    }
    max_index = static_cast<unsigned int>(max_index_d);
    for (unsigned int i = 0; i < max_index; i++)
    {
      float value = 1.0;
      for (unsigned int dim = 0; dim < dimensionality; dim++)
      {
        value *= mKernelParts.at(dim).at<float>(position[dim], 0);
      }
      if (dimensionality == 1)
      {
        kernel.at<float>(position[0], 1) = value;
      }
      else
      {
        kernel.at<float>(position) = value;
      }
      // increment index
      position[0]++;
      for (unsigned int dim = 0; dim < dimensionality; dim++)
      {
        if (position[dim] >= static_cast<int>(mSizes.at(dim)))
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
  catch(std::out_of_range& error)
  {
    std::cerr << "> Error (kernel::Gauss) :" << error.what() << " in calculate().\n"
        << "  Check your configuration files." << std::endl;
    // set matrices to semi-meaningful sizes to prevent further crashes
    // todo
  }
  mpReadWriteLockOutput->unlock();
}

void cedar::aux::kernel::Gauss::setSigma(unsigned int dimension, double sigma)
{
  try
  {
    _mSigmas->get().at(dimension) = sigma;
  }
  catch(std::exception& e)
  {
    std::cout << "Error in " << this->_mName->get()
              << " (cedar::aux::kernel::Gauss::setSigma): vector out of bounds " << std::endl;
  }
}

double cedar::aux::kernel::Gauss::getSigma(unsigned int dimension) const
{
  return _mSigmas->get().at(dimension);
}

void cedar::aux::kernel::Gauss::setShift(unsigned int dimension, double shift)
{
  try
  {
    _mShifts->get().at(dimension) = shift;
  }
  catch(std::exception& e)
  {
    std::cout << "Error in " << this->_mName->get()
              << " (cedar::aux::kernel::Gauss::setShift): vector out of bounds " << std::endl;
  }
}

double cedar::aux::kernel::Gauss::getShift(unsigned int dimension) const
{
  return _mShifts->get().at(dimension);
}

void cedar::aux::kernel::Gauss::setAmplitude(double amplitude)
{
  _mAmplitude->set(amplitude);
}

double cedar::aux::kernel::Gauss::getAmplitude() const
{
  return _mAmplitude->get();
}

unsigned int cedar::aux::kernel::Gauss::getWidth(unsigned int dim) const
{
  unsigned int tmp;
  /* size of kernel is determined by limit * sigma
   */
  tmp = static_cast<unsigned int>(ceil(_mLimit->get() * _mSigmas->get().at(dim)));
  // check if kernel size is even and if so, make it odd
  if (tmp % 2 == 0)
  {
    tmp++;
  }
  return tmp;
}

void cedar::aux::kernel::Gauss::updateDimensionality()
{
  int new_dimensionality = static_cast<int>(_mDimensionality->get());
  _mSigmas->get().resize(new_dimensionality, _mSigmas->getDefaultValue());
  _mShifts->get().resize(new_dimensionality, _mShifts->getDefaultValue());
  this->updateKernel();
}
