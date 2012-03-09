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

// CEDAR INCLUDES
#include "cedar/auxiliaries/kernel/Gauss.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/math/functions.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <iostream>
#include <limits.h>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared
    = cedar::aux::kernel::FactoryManagerSingleton::getInstance()->registerType<cedar::aux::kernel::GaussPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::kernel::Gauss::Gauss()
:
cedar::aux::kernel::Separable(),
_mAmplitude(new cedar::aux::DoubleParameter(this, "amplitude", 1.0, -10000.0, 10000.0)),
_mSigmas(new cedar::aux::DoubleVectorParameter(this, "sigmas", 2, 3.0, 0.0, 10000)),
_mShifts(new cedar::aux::DoubleVectorParameter(this, "shifts", 2, 3.0, 0.0, 10000)),
_mLimit(new cedar::aux::DoubleParameter(this, "limit", 0.01, 1000.0))
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  _mDimensionality->setValue(_mSigmas->size());

  this->onInit();
}

cedar::aux::kernel::Gauss::Gauss(
                                  double amplitude,
                                  std::vector<double> sigmas,
                                  std::vector<double> shifts,
                                  double limit,
                                  unsigned int dimensionality
                                )
:
cedar::aux::kernel::Separable(dimensionality),
_mAmplitude(new cedar::aux::DoubleParameter(this, "amplitude", amplitude, -10000.0, 10000.0)),
_mSigmas(new cedar::aux::DoubleVectorParameter(this, "sigmas", sigmas, 0.0, 10000)),
_mShifts(new cedar::aux::DoubleVectorParameter(this, "shifts", shifts, 0.0, 10000)),
_mLimit(new cedar::aux::DoubleParameter(this, "limit", limit, 0.01, 1000.0))
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  this->mCenters.resize(dimensionality);
  this->mSizes.resize(dimensionality);
  this->onInit();
}

cedar::aux::kernel::Gauss::~Gauss()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::kernel::Gauss::onInit()
{
  calculate();
  QObject::connect(_mAmplitude.get(), SIGNAL(valueChanged()), this, SLOT(updateKernel()));
  QObject::connect(_mLimit.get(), SIGNAL(valueChanged()), this, SLOT(updateKernel()));
  QObject::connect(_mSigmas.get(), SIGNAL(valueChanged()), this, SLOT(updateKernel()));
  QObject::connect(_mShifts.get(), SIGNAL(valueChanged()), this, SLOT(updateKernel()));
  QObject::connect(_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(updateDimensionality()));
}

void cedar::aux::kernel::Gauss::calculate()
{
  mpReadWriteLockOutput->lockForWrite();
  const unsigned int& dimensionality = _mDimensionality->getValue();
  const double& amplitude = _mAmplitude->getValue();
  // sanity check
  CEDAR_DEBUG_ASSERT(dimensionality == _mSigmas->getValue().size());
  CEDAR_DEBUG_ASSERT(dimensionality == _mShifts->getValue().size());
  try
  {
    this->mKernelParts.resize(dimensionality);
    mCenters.resize(dimensionality);
    mSizes.resize(dimensionality);
    // calculate the kernel parts for every dimension
    for (unsigned int dim = 0; dim < dimensionality; dim++)
    {
      // estimate width
      if (_mSigmas->at(dim) != 0)
      {
        mSizes.at(dim) = getWidth(dim);
      }
      else
      {
        mSizes.at(dim) = 1;
      }
      mCenters.at(dim) = static_cast<int>(mSizes.at(dim) / 2) + _mShifts->at(dim);
      this->setKernelPart(dim, cv::Mat::zeros(mSizes.at(dim), 1, CV_32FC1));

      // calculate kernel part
      if (_mSigmas->at(dim) != 0)
      {
        for (unsigned int j = 0; j < mSizes.at(dim); j++)
        {
          //!\todo move filling up of matrix to some tool function
          this->mKernelParts.at(dim).at<float>(j, 0)
              = cedar::aux::math::gauss(static_cast<int>(j) - mCenters.at(dim), _mSigmas->at(dim));
        }
      }
      else // discrete case
      {
        this->mKernelParts.at(dim).at<float>(0, 0) = 1;
      }
      // normalize
      this->setKernelPart(dim, this->getKernelPart(dim) * (1. / cv::sum(this->getKernelPart(dim)).val[0]));
      if (dim == 0)
      {
        this->setKernelPart(dim, amplitude * this->getKernelPart(dim));
      }
    }
    // assemble the kernel
    std::vector<int> sizes(static_cast<size_t>(dimensionality));
    for (size_t i = 0; i < mSizes.size(); i++)
    {
      sizes[i] = mSizes.at(i);
    }
    if (dimensionality == 1)
    {
      mKernel->getData() = cv::Mat(sizes[0], 1, CV_32F);
    }
    else
    {
      mKernel->getData() = cv::Mat(static_cast<int>(dimensionality), &sizes.at(0), CV_32F);
    }
    cv::Mat& kernel = mKernel->getData();
    // now fill up the big kernel matrix
    std::vector<int> position(static_cast<size_t>(dimensionality));
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
        value *= this->getKernelPart(dim).at<float>(position[dim], 0);
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
    CEDAR_THROW(
                 cedar::aux::IndexOutOfRangeException,
                 "kernel::Gauss has encountered inconsistent vector sizes, check your configuration file"
               );
  }
  mpReadWriteLockOutput->unlock();
}

void cedar::aux::kernel::Gauss::setSigma(unsigned int dimension, double sigma)
{
  try
  {
    _mSigmas->set(dimension, sigma);
  }
  //!@todo Catch only the out-of-bounds exception here
  catch(std::exception& e)
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException, "Error in cedar::aux::kernel::Gauss::setSigma: vector out of bounds");
  }
}

double cedar::aux::kernel::Gauss::getSigma(unsigned int dimension) const
{
  return _mSigmas->at(dimension);
}

void cedar::aux::kernel::Gauss::setShift(unsigned int dimension, double shift)
{
  try
  {
    _mShifts->set(dimension, shift);
  }
  //!@todo Catch only the out-of-bounds exception here
  catch(std::exception& e)
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException, "Error in cedar::aux::kernel::Gauss::setShift: vector out of bounds");
  }
}

double cedar::aux::kernel::Gauss::getShift(unsigned int dimension) const
{
  return _mShifts->at(dimension);
}

void cedar::aux::kernel::Gauss::setAmplitude(double amplitude)
{
  _mAmplitude->setValue(amplitude);
}

double cedar::aux::kernel::Gauss::getAmplitude() const
{
  return _mAmplitude->getValue();
}

unsigned int cedar::aux::kernel::Gauss::getWidth(unsigned int dim) const
{
  unsigned int tmp;
  /* size of kernel is determined by limit * sigma
   */
  tmp = static_cast<unsigned int>(ceil(_mLimit->getValue() * _mSigmas->at(dim)));
  // check if kernel size is even and if so, make it odd
  if (tmp % 2 == 0)
  {
    tmp++;
  }
  return tmp;
}

void cedar::aux::kernel::Gauss::updateDimensionality()
{
  mpReadWriteLockOutput->lockForWrite();
  unsigned int new_dimensionality = _mDimensionality->getValue();
  _mSigmas->resize(new_dimensionality, _mSigmas->getDefaultValue());
  _mShifts->resize(new_dimensionality, _mShifts->getDefaultValue());
  mpReadWriteLockOutput->unlock();
  this->updateKernel();
}
