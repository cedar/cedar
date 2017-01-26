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

    File:        KernelList.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 13

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/convolution/KernelList.h"
#include "cedar/auxiliaries/kernel/Kernel.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <QReadLocker>
#include <QWriteLocker>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::conv::KernelList::KernelList()
:
mCombinedKernel(new cedar::aux::MatData())
{

}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::conv::KernelList::append(cedar::aux::kernel::KernelPtr kernel)
{
  this->mKernels.push_back(kernel);
  this->connect(kernel.get(), SIGNAL(kernelUpdated()), SLOT(calculateCombinedKernel()));
  calculateCombinedKernel();
  this->mKernelAddedSignal(this->mKernels.size() - 1);
}

void cedar::aux::conv::KernelList::remove(size_t index)
{
  CEDAR_ASSERT(index < this->size());
  this->mKernels.at(index)->disconnect(this, SLOT(calculateCombinedKernel()));
  this->mKernels.erase(this->mKernels.begin() + index);
  calculateCombinedKernel();
  this->mKernelRemovedSignal(index);
}

cv::Mat cedar::aux::conv::KernelList::getCombinedKernel() const
{
  QReadLocker read_lock(&this->mCombinedKernel->getLock());
  cv::Mat combined = this->mCombinedKernel->getData().clone();
  return combined;
}

void cedar::aux::conv::KernelList::calculateCombinedKernel()
{
  QWriteLocker write_lock(&this->mCombinedKernel->getLock());
  // sanity check
  if (this->size() > 1)
  {
    unsigned int dim = this->getKernel(0)->getDimensionality();
    for (unsigned int i = 1; i < this->size(); ++i)
    {
      if (dim != this->getKernel(i)->getDimensionality())
      {
        // inconsistency, happens while kernels are being updated, ignore
        return;
      }
    }
  }

  cv::Mat new_combined_kernel;
  if (this->size() > 0 && this->getKernel(0)->getDimensionality() < 3)
  {
    new_combined_kernel = cv::Mat::zeros(1, 1, CV_32F);

    for (size_t i = 0; i < this->size(); ++i)
    {
      cedar::aux::kernel::ConstKernelPtr kernel = this->getKernel(i);
      kernel->lockForRead();
      cv::Mat kernel_mat = kernel->getKernel().clone();
      kernel->unlock();

      if (kernel_mat.empty())
      {
        cedar::aux::LogSingleton::getInstance()->warning("Empty kernel encountered. Skipping entry in kernel list.", CEDAR_CURRENT_FUNCTION_NAME);
        continue;
      }

      if (kernel_mat.rows > new_combined_kernel.rows || kernel_mat.cols > new_combined_kernel.cols)
      {
        int dw = std::max(0, (kernel_mat.cols - new_combined_kernel.cols + 1)/2);
        int dh = std::max(0, (kernel_mat.rows - new_combined_kernel.rows + 1)/2);
        // in-place copyMakeBorder is apparently not supported in opencv < 2.4
#if CV_MAJOR_VERSION < 2 || (CV_MAJOR_VERSION <= 2 && CV_MINOR_VERSION < 4)
        cv::Mat tmp;
        cv::copyMakeBorder(new_combined_kernel, tmp, dh, dh, dw, dw, cv::BORDER_CONSTANT, cv::Scalar(0));
        new_combined_kernel = tmp;
#else // OpenCV version >= 2.4
        cv::copyMakeBorder(new_combined_kernel, new_combined_kernel, dh, dh, dw, dw, cv::BORDER_CONSTANT, cv::Scalar(0));
#endif // OpenCV version
      }
      int row_lower = (new_combined_kernel.rows - kernel_mat.rows)/2;
      int row_upper = row_lower + kernel_mat.rows;
      int col_lower = (new_combined_kernel.cols - kernel_mat.cols)/2;
      int col_upper = col_lower + kernel_mat.cols;
      cv::Range row_range(row_lower, row_upper);
      cv::Range col_range(col_lower, col_upper);

      new_combined_kernel(row_range, col_range) += kernel_mat;
    }
  }
  else if (this->size() > 0 && this->getKernel(0)->getDimensionality() > 2)
  {
    std::vector<int> sizes;
    sizes.resize(this->getKernel(0)->getDimensionality());
    for (size_t dim = 0; dim < this->getKernel(0)->getDimensionality(); ++dim)
    {
      sizes.at(dim) = 0;
    }
    for (size_t i = 0; i < this->size(); ++i)
    {
      for (size_t dim = 0; dim < this->getKernel(i)->getDimensionality(); ++dim)
      {
        if (static_cast<unsigned int>(sizes.at(dim)) < this->getKernel(i)->getSize(dim))
        {
          sizes.at(dim) = this->getKernel(i)->getSize(dim);
        }
      }
    }
    new_combined_kernel = cv::Mat(this->getKernel(0)->getDimensionality(), &sizes.front(), CV_32F);
    new_combined_kernel = 0.0;
    for (size_t i = 0; i < this->size(); ++i)
    {
      // determine ROI
      std::vector<cv::Range> ranges;
      for (size_t dim = 0; dim < this->getKernel(i)->getDimensionality(); ++dim)
      {
        int diff = new_combined_kernel.size[dim] - this->getKernel(i)->getSize(dim);
        ranges.push_back(cv::Range(0 + diff / 2, new_combined_kernel.size[dim] - (diff + 1) / 2));
      }
      new_combined_kernel(&ranges.front()) += this->getKernel(i)->getKernel();
    }
  }
  mCombinedKernel->setData(new_combined_kernel);
  write_lock.unlock();
  emit combinedKernelUpdated();
}

bool cedar::aux::conv::KernelList::checkForSameKernelSize() const
{
  if (this->size() == 0)  // nothing to to
  {
    return false;
  }

  // get first kernel in list
  cedar::aux::kernel::ConstKernelPtr first_kernel = this->getKernel(0);

  // compare dimensionality and sizes of first kernel with each other kernel
  for (size_t i = 1; i < this->size(); ++i)
  {
    cedar::aux::kernel::ConstKernelPtr kernel = this->getKernel(i);

    // check dimensionality
    if (kernel->getDimensionality() != first_kernel->getDimensionality())
    {
      return false;
    }
    // if dimensionality is equal check sizes of each dimension
    for (size_t d = 0; d < first_kernel->getDimensionality(); ++d)
    {
      if (first_kernel->getSize(d) != kernel->getSize(d))
      {
        return false;
      }
    }
  }
  return true;
}
