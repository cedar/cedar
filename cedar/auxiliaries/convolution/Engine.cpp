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

    File:        ConvolutionEngine.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 15

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/Engine.h"
#include "cedar/auxiliaries/kernel/Separable.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cv::Mat cedar::aux::conv::Engine::convolveSeparable
        (
          const cv::Mat& matrix,
          cedar::aux::kernel::ConstSeparablePtr kernel,
          cedar::aux::conv::BorderType::Id borderType,
          cedar::aux::conv::BorderType::Id mode,
          bool alternateEvenCenter
        )
        const
{
  // default implementation: call the normal convolve method
  return this->convolve(matrix, cedar::aux::kernel::ConstKernelPtr(kernel), borderType, mode, alternateEvenCenter);
}

void cedar::aux::conv::Engine::setKernelList(cedar::aux::conv::KernelListPtr kernelList)
{
  CEDAR_DEBUG_ASSERT(kernelList.get() != nullptr);
  this->mKernelList = kernelList;
  emit kernelListChanged();
}
