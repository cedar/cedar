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

// SYSTEM INCLUDES
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::kernel::Kernel::Kernel()
:
mKernel(new cedar::aux::MatData()),
_mDimensionality(new cedar::aux::UIntParameter(this, "dimensionality", 1, 1000)),
_mKernelMatrixFile(new cedar::aux::StringParameter(this, "kernelMatrixFile", "dummy_matrix_file.yml"))
{
  mpReadWriteLockOutput = new QReadWriteLock();
  _mDimensionality->setConstant(true);
}

cedar::aux::kernel::Kernel::Kernel(unsigned int dimensionality, const std::string& kernelFile)
:
cedar::aux::Configurable(),
mKernel(new cedar::aux::DataTemplate<cv::Mat>()),
_mDimensionality(new cedar::aux::UIntParameter(this, "dimensionality", 1, 1000)),
_mKernelMatrixFile(new cedar::aux::StringParameter(this, "kernelMatrixFile", kernelFile))
{
  mpReadWriteLockOutput = new QReadWriteLock();
  _mDimensionality->setValue(dimensionality);
  _mDimensionality->setConstant(true);
}

cedar::aux::kernel::Kernel::~Kernel()
{
#ifdef DEBUG
  std::cout << "> freeing data (Kernel)" << std::endl;
#endif
  if (mpReadWriteLockOutput)
  {
    delete mpReadWriteLockOutput;
    mpReadWriteLockOutput = 0;
  }
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cv::Mat cedar::aux::kernel::Kernel::convolveWith(const cv::Mat& mat) const
{
  return cedar::aux::math::convolve(mat, this->mKernel->getData());
}

void cedar::aux::kernel::Kernel::hideDimensionality(bool hide)
{
  _mDimensionality->setHidden(hide);
}

QReadWriteLock* cedar::aux::kernel::Kernel::getReadWriteLock()
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

void cedar::aux::kernel::Kernel::loadKernelFromFile()
{
  mpReadWriteLockOutput->lockForWrite();
  cv::FileStorage fs(_mKernelMatrixFile->getValue(), cv::FileStorage::READ);
  fs["kernel"] >> mKernel->getData();
  mpReadWriteLockOutput->unlock();
}

void cedar::aux::kernel::Kernel::saveKernelToFile() const
{
  mpReadWriteLockOutput->lockForRead();
  cv::FileStorage fs(_mKernelMatrixFile->getValue(), cv::FileStorage::WRITE);
  fs << "kernel" << mKernel->getData();
  mpReadWriteLockOutput->unlock();
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
