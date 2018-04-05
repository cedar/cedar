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

    File:        Kernel.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 07 07

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/kernel/Kernel.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::kernel::Kernel::Kernel(unsigned int dimensionality)
:
cedar::aux::Configurable(),
mKernel(new cedar::aux::MatData()),
_mDimensionality(new cedar::aux::UIntParameter(this, "dimensionality", dimensionality, 1, 1000))
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  std::vector<int> anchor_defaults;
  anchor_defaults.resize(dimensionality, 0);
  _mAnchor = cedar::aux::IntVectorParameterPtr
             (
               new cedar::aux::IntVectorParameter
               (
                 this,
                 "anchor",
                 anchor_defaults,
                 std::numeric_limits<int>::min(),
                 std::numeric_limits<int>::max()
               )
             );
  this->_mAnchor->markAdvanced();
  this->_mDimensionality->markAdvanced();
  mpReadWriteLockOutput = new QReadWriteLock();
  _mDimensionality->setValue(dimensionality);
  _mDimensionality->setConstant(true);

  // we need a direct connection here so the update always happens at predictable times, i.e., every time the
  // dimensionality changes; otherwise, there might be short moments where dimensionality and the rest of the kernel
  // don't match
  QObject::connect(this->_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(dimensionalityChanged()), Qt::DirectConnection);
}

cedar::aux::kernel::Kernel::~Kernel()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);

  if (mpReadWriteLockOutput)
  {
    delete mpReadWriteLockOutput;
    mpReadWriteLockOutput = 0;
  }
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

unsigned int cedar::aux::kernel::Kernel::getSize(size_t dimension) const
{
  CEDAR_ASSERT(dimension < this->getDimensionality());

  // make sure that casting to unsigned doesn't have bad sideeffects
  CEDAR_DEBUG_ASSERT(this->mKernel->getData().size[dimension] >= 0);

  return static_cast<unsigned int>(this->mKernel->getData().size[dimension]);
}

void cedar::aux::kernel::Kernel::dimensionalityChanged()
{
  this->_mAnchor->resize(this->getDimensionality(), 0);
}

void cedar::aux::kernel::Kernel::hideDimensionality(bool hide)
{
  _mDimensionality->setHidden(hide);
}

QReadWriteLock* cedar::aux::kernel::Kernel::getReadWriteLock() const
{
  return mpReadWriteLockOutput;
}

const cv::Mat& cedar::aux::kernel::Kernel::getKernel() const
{
  return mKernel->getData();
}

const cedar::aux::DataPtr cedar::aux::kernel::Kernel::getKernelRaw() const
{
  return this->mKernel;
}

unsigned int cedar::aux::kernel::Kernel::getDimensionality() const
{
  return _mDimensionality->getValue();
}

void cedar::aux::kernel::Kernel::setDimensionality(unsigned int dimensionality)
{
  this->_mDimensionality->setValue(dimensionality);
}

void cedar::aux::kernel::Kernel::updateKernel()
{
  this->calculate();
  emit kernelUpdated();
}
