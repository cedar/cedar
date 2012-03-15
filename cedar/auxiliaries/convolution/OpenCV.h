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

    File:        OpenCV.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 14

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_CONVOLUTION_OPENCV_H
#define CEDAR_AUX_CONVOLUTION_OPENCV_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/namespace.h"
#include "cedar/auxiliaries/convolution/Convolution.h"

// SYSTEM INCLUDES


/*!@brief A convolution engine based on OpenCV's filter engine.
 *
 * @todo describe more.
 *
 * @todo Intelligent strategies might be used here, e.g., when multiple kernels in a row are TYPE_FULL, they can be
 *       summed together etc.
 *
 * @todo This should not inherit convolution, but be part of it
 *
 * @todo Write a unit test for this class
 *
 * @todo Make this class thread-safe
 */
class cedar::aux::conv::OpenCV : public cedar::aux::conv::Convolution
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  enum KernelType
  {
    KERNEL_TYPE_UNKNOWN,
    KERNEL_TYPE_FULL,
    KERNEL_TYPE_SEPARABLE
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  OpenCV();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  cv::Mat convolve(const cv::Mat& matrix) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void updateKernelType(size_t index);

  inline int getTranslatedBorderType() const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  std::vector<KernelType> mKernelTypes;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::conv::OpenCV

#endif // CEDAR_AUX_CONVOLUTION_OPENCV_H

