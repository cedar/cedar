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

    File:        Convolution.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 11 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/Convolution.h"
#include "cedar/auxiliaries/convolution/BorderType.h"
#include "cedar/auxiliaries/convolution/Mode.h"
#include "cedar/auxiliaries/convolution/OpenCV.h"
#include "cedar/auxiliaries/kernel/Kernel.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::conv::Convolution::Convolution()
:
mCombinedKernel(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
_mBorderType
(
  new cedar::aux::EnumParameter
  (
    this,
    "borderType",
    cedar::aux::conv::BorderType::typePtr(),
    cedar::aux::conv::BorderType::Zero
  )
),
_mMode
(
  new cedar::aux::EnumParameter
  (
    this,
    "mode",
    cedar::aux::conv::Mode::typePtr(),
    cedar::aux::conv::Mode::Same
  )
),
_mEngine
(
  new cedar::aux::conv::EngineParameter(this, "engine", cedar::aux::conv::EnginePtr(new cedar::aux::conv::OpenCV()))
)
{
  this->selectedEngineChanged();

  QObject::connect(this->_mEngine.get(), SIGNAL(valueChanged()), this, SLOT(selectedEngineChanged()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::conv::Convolution::slotKernelAdded(size_t index)
{
  cedar::aux::kernel::ConstKernelPtr kernel = this->getKernelList().getKernel(index);
  QObject::connect(kernel.get(), SIGNAL(kernelUpdated()), this, SLOT(updateCombinedKernel()));
  this->updateCombinedKernel();
}

void cedar::aux::conv::Convolution::slotKernelChanged(size_t)
{
  this->updateCombinedKernel();
}

void cedar::aux::conv::Convolution::slotKernelRemoved(size_t)
{
  this->updateCombinedKernel();
  //!@todo disconnect kernelUpdated slot!
}

void cedar::aux::conv::Convolution::updateCombinedKernel()
{
  //!@todo make that this works for more than one/two dimensional kernels
  cv::Mat new_combined_kernel = cv::Mat::zeros(1, 1, CV_32F);

  for (size_t i = 0; i < this->getKernelList().size(); ++i)
  {
    cedar::aux::kernel::ConstKernelPtr kernel = this->getKernelList().getKernel(i);
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

  this->mCombinedKernel->lockForWrite();
  this->mCombinedKernel->setData(new_combined_kernel);
  this->mCombinedKernel->unlock();
}

void cedar::aux::conv::Convolution::selectedEngineChanged()
{
  mKernelAddedConnection.disconnect();
  mKernelChangedConnection.disconnect();
  mKernelRemovedConnection.disconnect();

  // connect to the new kernel list.
  mKernelAddedConnection = this->getKernelList().connectToKernelAddedSignal
                           (
                             boost::bind(&cedar::aux::conv::Convolution::slotKernelAdded, this, _1)
                           );

  mKernelChangedConnection = this->getKernelList().connectToKernelChangedSignal
                             (
                               boost::bind(&cedar::aux::conv::Convolution::slotKernelChanged, this, _1)
                             );

  mKernelRemovedConnection = this->getKernelList().connectToKernelRemovedSignal
                             (
                               boost::bind(&cedar::aux::conv::Convolution::slotKernelRemoved, this, _1)
                             );

  this->updateCombinedKernel();
}
