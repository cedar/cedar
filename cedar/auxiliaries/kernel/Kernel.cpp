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

// LOCAL INCLUDES
#include "auxiliaries/kernel/Kernel.h"
#include "auxiliaries/NumericParameter.h"
#include "processing/Data.h"
#include "processing/DataT.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::kernel::Kernel::Kernel()
:
mKernel(new cedar::proc::DataT<cv::Mat>()),
_mDimensionality(new cedar::aux::UIntParameter("dimensionality", 1, 1000)),
_mKernelMatrixFile(new cedar::aux::StringParameter("kernelMatrixFile", "dummy_matrix_file.yml"))
{
  mpReadWriteLockOutput = new QReadWriteLock();
  _mDimensionality->setConstant(true);
  this->registerParameter(_mDimensionality);
  this->_mName->setHidden(true);
}

cedar::aux::kernel::Kernel::Kernel(unsigned int dimensionality, const std::string& kernelFile)
:
cedar::aux::Configurable(),
mKernel(new cedar::proc::DataT<cv::Mat>()),
_mDimensionality(new cedar::aux::UIntParameter("dimensionality", 1, 1000)),
_mKernelMatrixFile(new cedar::aux::StringParameter("kernelMatrixFile", kernelFile))
{
  mpReadWriteLockOutput = new QReadWriteLock();
  _mDimensionality->set(dimensionality);
  _mDimensionality->setConstant(true);
  this->registerParameter(_mDimensionality);
  this->_mName->setHidden(true);
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

QReadWriteLock* cedar::aux::kernel::Kernel::getReadWriteLock()
{
  return mpReadWriteLockOutput;
}

const cv::Mat& cedar::aux::kernel::Kernel::getKernel() const
{
  return mKernel->getData<cv::Mat>();
}

const cedar::proc::DataPtr cedar::aux::kernel::Kernel::getKernelRaw() const
{
  return this->mKernel;
}

void cedar::aux::kernel::Kernel::loadKernelFromFile()
{
  mpReadWriteLockOutput->lockForWrite();
  cv::FileStorage fs(_mKernelMatrixFile->get(), cv::FileStorage::READ);
  fs["kernel"] >> mKernel->getData<cv::Mat>();
  mpReadWriteLockOutput->unlock();
}

void cedar::aux::kernel::Kernel::saveKernelToFile() const
{
  mpReadWriteLockOutput->lockForRead();
  cv::FileStorage fs(_mKernelMatrixFile->get(), cv::FileStorage::WRITE);
  fs << "kernel" << mKernel->getData<cv::Mat>();
  mpReadWriteLockOutput->unlock();
}

unsigned int cedar::aux::kernel::Kernel::getDimensionality() const
{
  return _mDimensionality->get();
}

void cedar::aux::kernel::Kernel::updateKernel()
{
  this->calculate();
}
