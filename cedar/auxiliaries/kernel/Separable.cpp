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

    File:        Separable.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 07 07

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/kernel/Separable.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/MatrixIterator.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/DataTemplate.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::kernel::Separable::Separable(unsigned int dimensionality)
:
cedar::aux::kernel::Kernel(dimensionality)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  this->dimensionalityChanged();
  // we need a direct connection here so that changes to the dimensionality are applied immediately; otherwise, there
  // might be short moments where the kernel is in an undefined state
  QObject::connect(this->_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(dimensionalityChanged()), Qt::DirectConnection);
}

cedar::aux::kernel::Separable::~Separable()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::kernel::Separable::calculate()
{
  this->calculateParts();

  this->updateKernelMatrix();
}

unsigned int cedar::aux::kernel::Separable::getSize(size_t dimension) const
{
  // make sure that casting to unsigned doesn't have bad side effects
  CEDAR_DEBUG_ASSERT(this->getKernelPart(dimension).size[0] >= 0);
  return static_cast<unsigned int>(this->getKernelPart(dimension).size[0]);
}

void cedar::aux::kernel::Separable::dimensionalityChanged()
{
  this->mKernelParts.resize(std::max(static_cast<unsigned int>(1), this->getDimensionality()));
}

const cv::Mat& cedar::aux::kernel::Separable::getKernelPart(unsigned int dimension) const
{
  CEDAR_DEBUG_ASSERT(dimension < this->mKernelParts.size());
  return this->mKernelParts.at(dimension);
}

void cedar::aux::kernel::Separable::setKernelPart(unsigned int dimension, const cv::Mat& mat)
{
  CEDAR_DEBUG_ASSERT(dimension < this->mKernelParts.size());
  this->mKernelParts.at(dimension) = mat;
}

void cedar::aux::kernel::Separable::updateKernelMatrix()
{
  if (this->getDimensionality() == 0)
  {
    this->mKernel->lockForWrite();
    const cv::Mat& kernel = this->mKernelParts.at(0);
    CEDAR_ASSERT(cedar::aux::math::getDimensionalityOf(kernel) == 0);
    this->mKernel->setData(kernel);
    this->mKernel->unlock();
  }
  else if (this->getDimensionality() <= 2)
  {
    this->mpReadWriteLockOutput->lockForRead();

    cv::Mat combined = this->mKernelParts.at(0).clone();

    CEDAR_DEBUG_ASSERT(this->mKernelParts.size() == this->getDimensionality());

    for (size_t i = 1; i < this->mKernelParts.size(); ++i)
    {
      combined = combined * this->mKernelParts.at(i).t();
    }

    this->mpReadWriteLockOutput->unlock();

    this->mKernel->lockForWrite();
    this->mKernel->setData(combined);
    this->mKernel->unlock();
  }
  else
  {
    CEDAR_ASSERT(this->getDimensionality() > 2);
    std::vector<int> sizes;
    for (unsigned int dim = 0; dim < this->getDimensionality(); ++dim)
    {
      sizes.push_back(static_cast<int>(this->getSize(dim)));
    }
    cv::Mat kernel(this->getDimensionality(), &sizes.front(), CV_32F);
    cedar::aux::MatrixIterator iter(kernel);
    do
    {
      float value = 1.0;
      const std::vector<int>& position = iter.getCurrentIndexVector();
      for (unsigned int dim = 0; dim < this->getDimensionality(); ++dim)
      {
        value *= cedar::aux::math::getMatrixEntry<float>(this->getKernelPart(dim), position.at(dim));
      }
      cedar::aux::math::assignMatrixEntry(kernel, position, value);
    }
    while (iter.increment());

    this->mKernel->lockForWrite();
    this->mKernel->setData(kernel);
    this->mKernel->unlock();
  }
}
