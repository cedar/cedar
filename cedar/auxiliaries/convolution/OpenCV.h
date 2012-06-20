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
#include "cedar/auxiliaries/convolution/Engine.h"

// SYSTEM INCLUDES


/*!@brief A convolution engine based on OpenCV's filter engine.
 *
 * @todo describe more.
 *
 * @todo Intelligent strategies should be useable here, e.g., when multiple kernels in a row are TYPE_FULL, they can be
 *       summed together etc.
 *
 * @todo Write a unit test for this class
 *
 * @todo This class should also be able to work with more matrices of more than two dimensions.
 *
 * @todo Right now, this computes correlation (it should flip the kernel)
 *
 */
class cedar::aux::conv::OpenCV : public cedar::aux::conv::Engine
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
  cv::Mat convolve
  (
    const cv::Mat& matrix,
    cedar::aux::conv::BorderType::Id borderType,
    cedar::aux::conv::Mode::Id mode
  ) const;

  cv::Mat convolve
  (
    const cv::Mat& matrix,
    const cv::Mat& kernel,
    cedar::aux::conv::BorderType::Id borderType,
    cedar::aux::conv::Mode::Id mode,
    const std::vector<int>& anchor
  ) const;

  cv::Mat convolve
  (
    const cv::Mat& matrix,
    cedar::aux::kernel::ConstKernelPtr kernel,
    cedar::aux::conv::BorderType::Id borderType,
    cedar::aux::conv::Mode::Id mode
  ) const;

  cv::Mat convolve
  (
    const cv::Mat& matrix,
    const cedar::aux::conv::KernelList& kernel,
    cedar::aux::conv::BorderType::Id borderType,
    cedar::aux::conv::Mode::Id mode
  ) const;

  cv::Mat convolve
  (
    const cv::Mat& matrix,
    cedar::aux::kernel::ConstSeparablePtr kernel,
    cedar::aux::conv::BorderType::Id borderType,
    cedar::aux::conv::Mode::Id mode
  ) const;

  bool checkCapability
  (
    size_t matrixDim,
    size_t kernelDim,
    cedar::aux::conv::BorderType::Id borderType,
    cedar::aux::conv::Mode::Id mode
  ) const;

  bool checkBorderTypeCapability
  (
    cedar::aux::conv::BorderType::Id borderType
  ) const;

  bool checkModeCapability
  (
    cedar::aux::conv::Mode::Id mode
  ) const;

  //!@brief method for setting the kernel list
  void setKernelList(cedar::aux::conv::KernelListPtr kernelList);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void kernelRemoved(size_t index);

  void updateKernelType(size_t index);

  int translateBorderType(cedar::aux::conv::BorderType::Id borderType) const;

  void translateAnchor
  (
    cv::Point& anchor,
    const std::vector<int>& anchor_vector,
    const cv::Mat::MSize& msize
  ) const;

  void translateAnchor
  (
    cv::Point& anchor,
    cedar::aux::kernel::ConstKernelPtr kernel
  ) const;

  cv::Mat createFullMatrix(
                          const cv::Mat& matrix,
                          int kernelRows,
                          int kernelCols,
                          cedar::aux::conv::BorderType::Id borderType
                          ) const;

  cv::Mat createFullMatrix(
                          const cv::Mat& matrix,
                          const cv::Mat& kernel,
                          cedar::aux::conv::BorderType::Id borderType
                          ) const;
  cv::Mat createFullMatrix(
                          const cv::Mat& matrix,
                          const cedar::aux::kernel::ConstKernelPtr kernel,
                          cedar::aux::conv::BorderType::Id borderType
                          ) const;

  cv::Mat createFullMatrix(
                          const cv::Mat& matrix,
                          const cedar::aux::conv::KernelList& kernelList,
                          cedar::aux::conv::BorderType::Id borderType
                          ) const;

  cv::Mat createFullMatrix(
                          const cv::Mat& matrix,
                          const cedar::aux::kernel::ConstSeparablePtr kernel,
                          cedar::aux::conv::BorderType::Id borderType
                          ) const;

  cv::Mat createFullMatrix(
                          const cv::Mat& matrix,
                          cedar::aux::conv::BorderType::Id borderType
                          ) const;

  cv::Mat cutOutResult(
                      const cv::Mat& result,
                      unsigned int matrixRows,
                      unsigned int matrixCols,
                      unsigned int kernelRows,
                      unsigned int kernelCols
                      ) const;

  cv::Mat cutOutResult(const cv::Mat& result, const cv::Mat& matrix, const cv::Mat& kernel) const;

  cv::Mat cutOutResult(const cv::Mat& result, const cv::Mat& matrix, const cedar::aux::kernel::ConstKernelPtr kernel) const;

  cv::Mat cutOutResult(
                      const cv::Mat& result,
                      unsigned int kernelRows,
                      unsigned int kernelCols
                      ) const;

  cv::Mat cutOutResult(const cv::Mat& result, const cv::Mat& kernel) const;

  cv::Mat cutOutResult(const cv::Mat& result, const cedar::aux::kernel::ConstKernelPtr kernel) const;

  cv::Mat cvConvolve
  (
    const cv::Mat& matrix,
    const cv::Mat& kernel,
    int cvBorderType,
    const cv::Point& anchor
  ) const;

  cv::Mat cvConvolve
  (
    const cv::Mat& matrix,
    const cedar::aux::kernel::ConstSeparablePtr kernel,
    int cvBorderType,
    const cv::Point& anchor
  ) const;

  cv::Mat cvConvolve
  (
    const cv::Mat& matrix,
    const cedar::aux::kernel::ConstKernelPtr kernel,
    int cvBorderType,
    const cv::Point& anchor
  ) const;

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

