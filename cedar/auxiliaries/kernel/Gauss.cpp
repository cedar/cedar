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
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/stringFunctions.h"
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
cedar::aux::kernel::Gauss::Gauss
(
  unsigned int dimensionality,
  double amplitude,
  double sigmas,
  double shifts,
  double limit
)
:
cedar::aux::kernel::Separable(),
_mAmplitude(new cedar::aux::DoubleParameter(this, "amplitude", amplitude, -10000.0, 10000.0)),
_mSigmas(new cedar::aux::DoubleVectorParameter(this, "sigmas", dimensionality, sigmas, 0.01, 10000)),
_mNormalize(new cedar::aux::BoolParameter(this, "normalize", true)),
_mShifts(new cedar::aux::DoubleVectorParameter(this, "shifts", dimensionality, shifts, -10000.0, 10000)),
_mLimit(new cedar::aux::DoubleParameter(this, "limit", limit, 0.01, 1000.0))
{
  this->_mLimit->markAdvanced();
  this->_mShifts->markAdvanced();
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  this->setDimensionality(dimensionality);
  
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
_mSigmas(new cedar::aux::DoubleVectorParameter(this, "sigmas", sigmas, 0.01, 10000)),
_mNormalize(new cedar::aux::BoolParameter(this, "normalize", true)),
_mShifts(new cedar::aux::DoubleVectorParameter(this, "shifts", shifts, -10000.0, 10000)),
_mLimit(new cedar::aux::DoubleParameter(this, "limit", limit, 0.01, 1000.0))
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  CEDAR_ASSERT(sigmas.size() == dimensionality);
  CEDAR_ASSERT(shifts.size() == dimensionality);

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
  updateDimensionality();
  // we need direct connections here so the update always happens at predictable times, i.e., every time the
  // values change; otherwise, there might be short moments where dimensionality and the rest of the kernel don't match
  QObject::connect(_mAmplitude.get(), SIGNAL(valueChanged()), this, SLOT(updateKernel()), Qt::DirectConnection);
  QObject::connect(_mLimit.get(), SIGNAL(valueChanged()), this, SLOT(updateKernel()), Qt::DirectConnection);
  QObject::connect(_mSigmas.get(), SIGNAL(valueChanged()), this, SLOT(updateKernel()), Qt::DirectConnection);
  QObject::connect(_mShifts.get(), SIGNAL(valueChanged()), this, SLOT(updateKernel()), Qt::DirectConnection);
  QObject::connect(_mNormalize.get(), SIGNAL(valueChanged()), this, SLOT(updateKernel()), Qt::DirectConnection);
  QObject::connect(_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(updateDimensionality()), Qt::DirectConnection);
}

void cedar::aux::kernel::Gauss::calculateParts()
{
  QWriteLocker lock(mpReadWriteLockOutput);
  unsigned int dimensionality = this->getDimensionality();
  QReadLocker amp_lock(this->_mAmplitude->getLock());
  double amplitude = _mAmplitude->getValue();
  amp_lock.unlock();

  // sanity check

  // assert the correct size of all parameters & lists
  CEDAR_DEBUG_ASSERT((dimensionality == 0 && _mSigmas->size() == 1) || _mSigmas->size() == dimensionality);
  CEDAR_DEBUG_ASSERT((dimensionality == 0 && _mShifts->size() == 1) || _mShifts->size() == dimensionality);
  CEDAR_DEBUG_ASSERT((dimensionality == 0 && mSizes.size() == 1)   || mSizes.size() == dimensionality);
  CEDAR_DEBUG_ASSERT((dimensionality == 0 && mCenters.size() == 1) || mCenters.size() == dimensionality);

  // calculate the kernel parts for every dimension
  if (dimensionality > 0)
  {
    for (unsigned int dim = 0; dim < dimensionality; dim++)
    {
      QReadLocker sigma_lock(_mSigmas->getLock());
      double sigma = _mSigmas->at(dim);
      sigma_lock.unlock();

      QReadLocker shift_lock(_mShifts->getLock());
      double shift = _mShifts->at(dim);
      shift_lock.unlock();

      // estimate width
      if (sigma != 0)
      {
        this->mSizes.at(dim) = this->estimateWidth(dim);
      }
      else
      {
        this->mSizes.at(dim) = 1;
      }
      this->mCenters.at(dim) = static_cast<int>(mSizes.at(dim) / 2) + shift;
      cv::Mat kernel_part = cv::Mat::zeros(mSizes.at(dim), 1, CV_32F);

      // calculate kernel part
      if (sigma != 0)
      {
        for (unsigned int j = 0; j < mSizes.at(dim); j++)
        {
          kernel_part.at<float>(j, 0)
              = cedar::aux::math::gauss(static_cast<int>(j) - mCenters.at(dim), sigma);
        }
      }
      else // discrete case
      {
        kernel_part.at<float>(0, 0) = 1;
      }

      // normalize
      if (this->_mNormalize->getValue())
      {
        kernel_part /= cv::sum(kernel_part).val[0];
      }

      this->setKernelPart(dim, kernel_part);
    }

    this->setKernelPart(0, amplitude * this->getKernelPart(0));
  }
  else
  {
    this->setKernelPart(0, amplitude * cv::Mat::ones(1, 1, CV_32F));
  }
}

void cedar::aux::kernel::Gauss::setSigma(unsigned int dimension, double sigma)
{
  if (dimension < _mSigmas->size())
  {
    _mSigmas->setValue(dimension, sigma);
  }
  else
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
  if (dimension < this->_mShifts->size())
  {
    _mShifts->setValue(dimension, shift);
  }
  else
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

unsigned int cedar::aux::kernel::Gauss::estimateWidth(unsigned int dim) const
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
  QWriteLocker locker(mpReadWriteLockOutput);
  unsigned int new_dimensionality = this->getDimensionality();
  unsigned int new_size = new_dimensionality;
  if (new_dimensionality == 0)
  {
    new_size = 1;
  }
  _mSigmas->resize(new_size);
  _mSigmas->setDefaultSize(new_size);
  _mShifts->resize(new_size);
  _mShifts->setDefaultSize(new_size);
  this->mCenters.resize(new_size);
  this->mSizes.resize(new_size);
  locker.unlock();

  this->updateKernel();
}
