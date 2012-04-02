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

    File:        KernelList.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 13

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/KernelList.h"
#include "cedar/auxiliaries/kernel/Kernel.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::conv::KernelList::append(cedar::aux::kernel::KernelPtr kernel)
{
  this->mKernels.push_back(kernel);

  this->mKernelAddedSignal(this->mKernels.size() - 1);
}

void cedar::aux::conv::KernelList::remove(size_t index)
{
  CEDAR_ASSERT(index < this->size());

  this->mKernels.erase(this->mKernels.begin() + index);

  this->mKernelRemovedSignal(index);
}

cv::Mat cedar::aux::conv::KernelList::getCombinedKernel() const
{
  //!@todo make that this works for more than one/two dimensional kernels
  cv::Mat new_combined_kernel = cv::Mat::zeros(1, 1, CV_32F);

  for (size_t i = 0; i < this->size(); ++i)
  {
    cedar::aux::kernel::ConstKernelPtr kernel = this->getKernel(i);
    kernel->lockForRead();
    cv::Mat kernel_mat = kernel->getKernel();
    kernel->unlock();
    if (kernel_mat.rows > new_combined_kernel.rows || kernel_mat.cols > new_combined_kernel.cols)
    {
      int dw = std::max(0, (kernel_mat.cols - new_combined_kernel.cols + 1)/2);
      int dh = std::max(0, (kernel_mat.rows - new_combined_kernel.rows + 1)/2);
      cv::copyMakeBorder(new_combined_kernel, new_combined_kernel, dh, dh, dw, dw, cv::BORDER_CONSTANT, cv::Scalar(0));
    }
    int row_lower = (new_combined_kernel.rows - kernel_mat.rows)/2;
    int row_upper = row_lower + kernel_mat.rows;
    int col_lower = (new_combined_kernel.cols - kernel_mat.cols)/2;
    int col_upper = col_lower + kernel_mat.cols;
    cv::Range row_range(row_lower, row_upper);
    cv::Range col_range(col_lower, col_upper);

    new_combined_kernel(row_range, col_range) += kernel_mat;
  }
  return new_combined_kernel;
}
