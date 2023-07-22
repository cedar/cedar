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

    File:        ArrayFire.cpp

    Maintainer:
    Email:
    Date:

    Description:

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"

#ifdef CEDAR_USE_ARRAYFIRE

// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/ArrayFire.h"
#include "cedar/auxiliaries/convolution/BorderType.h"
#include "cedar/auxiliaries/convolution/Mode.h"
#include "cedar/auxiliaries/convolution/EngineManager.h"
#include "cedar/auxiliaries/kernel/Separable.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/casts.h"


// SYSTEM INCLUDES
//----------------------------------------------------------------------------------------------------------------------
// register type with the factory
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered = cedar::aux::conv::EngineManagerSingleton::getInstance()->registerType<cedar::aux::conv::ArrayFirePtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::conv::ArrayFire::ArrayFire()
{
}

cedar::aux::conv::ArrayFire::~ArrayFire()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::aux::conv::ArrayFire::checkCapability
    (
        size_t matrixDim,
        size_t kernelDim,
        cedar::aux::conv::BorderType::Id borderType,
        cedar::aux::conv::Mode::Id mode
    )
const
{
  if (matrixDim > 3 || kernelDim > 3){
    return false;
  }
  if (!this->checkBorderTypeCapability(borderType) || !this->checkModeCapability(mode)){
    return false;
  }
  return true;
}

bool cedar::aux::conv::ArrayFire::checkCapability
    (
        cv::Mat matrix,
        cv::Mat kernel,
        cedar::aux::conv::BorderType::Id borderType,
        cedar::aux::conv::Mode::Id mode
    ) const
{
  if (!this->checkCapability(cedar::aux::math::getDimensionalityOf(matrix), cedar::aux::math::getDimensionalityOf(kernel), borderType, mode))
  {
    return false;
  }
  return true;
}

bool cedar::aux::conv::ArrayFire::checkBorderTypeCapability
    (
        cedar::aux::conv::BorderType::Id borderType
    ) const
{
  return borderType == cedar::aux::conv::BorderType::Zero;
}

bool cedar::aux::conv::ArrayFire::checkModeCapability
    (
        cedar::aux::conv::Mode::Id mode
    ) const
{
  return mode == cedar::aux::conv::Mode::Same ;//|| mode == cedar::aux::conv::Mode::Full todo
}

cv::Mat cedar::aux::conv::ArrayFire::convolve
    (
        const cv::Mat& matrix,
        const cv::Mat& kernel,
        cedar::aux::conv::BorderType::Id borderType,
        cedar::aux::conv::Mode::Id mode,
        const std::vector<int>& anchorVector,
        bool alternateEvenCenter
    ) const
{
  CEDAR_ASSERT(mode == cedar::aux::conv::Mode::Same|| mode == cedar::aux::conv::Mode::Full);
  return this->convolveInternal(matrix, kernel, borderType, mode, alternateEvenCenter);
}

cv::Mat cedar::aux::conv::ArrayFire::convolve
    (
        const cv::Mat& matrix,
        const cedar::aux::kernel::ConstKernelPtr kernel,
        cedar::aux::conv::BorderType::Id borderType,
        cedar::aux::conv::Mode::Id mode,
        bool alternateEvenCenter
    ) const
{
  CEDAR_ASSERT(mode == cedar::aux::conv::Mode::Same|| mode == cedar::aux::conv::Mode::Full);
  return this->convolveInternal(matrix, kernel->getKernel(), borderType, mode, alternateEvenCenter);
}

cv::Mat cedar::aux::conv::ArrayFire::convolve
    (
        const cv::Mat& matrix,
        cedar::aux::conv::ConstKernelListPtr kernelList,
        cedar::aux::conv::BorderType::Id borderType,
        cedar::aux::conv::Mode::Id mode,
        bool alternateEvenCenter
    ) const
{
  CEDAR_ASSERT(mode == cedar::aux::conv::Mode::Same|| mode == cedar::aux::conv::Mode::Full);
  if (kernelList->size() > 0){
    return this->convolveInternal(matrix, kernelList->getCombinedKernel(), borderType, mode, alternateEvenCenter);
  }else{
    return cv::Mat(matrix.dims, matrix.size, matrix.type(), cv::Scalar(0.0));
  }
}

cv::Mat cedar::aux::conv::ArrayFire::convolve
    (
        const cv::Mat& matrix,
        cedar::aux::conv::BorderType::Id borderType,
        cedar::aux::conv::Mode::Id mode,
        bool alternateEvenCenter
    ) const
{
  CEDAR_ASSERT(mode == cedar::aux::conv::Mode::Same|| mode == cedar::aux::conv::Mode::Full);
  if (this->getKernelList()->size() > 0){
    return this->convolveInternal(matrix, this->getKernelList()->getCombinedKernel(), borderType, mode, alternateEvenCenter);
  }else{
    return cv::Mat(matrix.dims, matrix.size, matrix.type(), cv::Scalar(0.0));
  }
}

cv::Mat cedar::aux::conv::ArrayFire::convolveInternal
    (
        const cv::Mat& matrixIn,
        const cv::Mat& kernelIn,
        cedar::aux::conv::BorderType::Id  borderType,
        cedar::aux::conv::Mode::Id mode,
        bool alternateEvenCenter
    ) const
{
  cv::Mat matrix, kernel;
  if (alternateEvenCenter)
  {
    // to alternate the kernel center, we flip all inputs and then flip the result back
    std::vector<bool> flipped;
    flipped.assign(cedar::aux::math::getDimensionalityOf(matrixIn), true);
    // preallocate the flipped matrices (cedar::aux::math::flip expects this)
    matrix = 0.0 * matrixIn.clone();
    kernel = 0.0 * kernelIn.clone();
    // flip the matrices
    cedar::aux::math::flip(matrixIn, matrix, flipped);
    cedar::aux::math::flip(kernelIn, kernel, flipped);
  }
  else
  {
    matrix = matrixIn;
    kernel = kernelIn;
  }
  if (cedar::aux::math::getDimensionalityOf(kernel) == 0)
  {
    return matrix * cedar::aux::math::getMatrixEntry<double>(kernel, 0, 0);
  }
  else if (cedar::aux::math::getDimensionalityOf(matrix) == 0)
  {
    return cv::Mat(1, 1, kernel.type(), cv::sum(kernel * cedar::aux::math::getMatrixEntry<double>(matrix, 0, 0)));
  }
  auto type = matrix.type();
  if (type != CV_32F) {
    CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot convolve matrices of this type.");//todo add more
  }
  cv::Mat returned = cv::Mat(matrix.dims, matrix.size, type, cv::Scalar(0.0));
  if(cedar::aux::math::getDimensionalityOf(kernel) == cedar::aux::math::getDimensionalityOf(matrix) && matrix.isContinuous() && kernel.isContinuous())
  {
    af::array af_result;
    auto af_mode =  (mode == cedar::aux::conv::Mode::Full) ? AF_CONV_EXPAND : AF_CONV_DEFAULT;//todo resize
    switch (cedar::aux::math::getDimensionalityOf(kernel)) {
      case 1:{
        cv::Mat matrixTF = matrix.t();
        cv::Mat kernelTF = kernel.t();
        af::array af_matrix(matrix.size[0], matrix.ptr<float>(0));
        af::array af_kernel(kernel.size[0], kernel.ptr<float>(0));
        af_result = af::convolve(af_matrix, af_kernel,af_mode);
        auto data = returned.ptr<float>(0);
        af_result.T().host((void*)data);
      }break;
      case 2:{
        cv::Mat matrixTF = matrix.t();
        cv::Mat kernelTF = kernel.t();
        af::array af_matrix(matrix.size[0], matrix.size[1], matrixTF.ptr<float>(0));
        af::array af_kernel(kernel.size[0], kernel.size[1], kernelTF.ptr<float>(0));
        af_result = af::convolve(af_matrix, af_kernel,af_mode);
        auto data = returned.ptr<float>(0);
        af_result.T().host((void*)data);
      }break;
      case 3:{
        std::vector<float> vec_matrix, vec_kernel;
        for (int dim = 0; dim < matrix.size[2]; dim++){
          for (int col = 0; col < matrix.size[1]; col++){
            for (int row = 0; row < matrix.size[0]; row++){
              vec_matrix.push_back(matrix.at<float>(row,col,dim));
            }
          }
        }
        for (int dim = 0; dim < kernel.size[2]; dim++){
          for (int col = 0; col < kernel.size[1]; col++){
            for (int row = 0; row < kernel.size[0]; row++){
              vec_kernel.push_back(kernel.at<float>(row,col,dim));
            }
          }
        }
        af::array af_matrix(matrix.size[0], matrix.size[1], matrix.size[2],vec_matrix.data());
        af::array af_kernel(kernel.size[0], kernel.size[1], kernel.size[2],vec_kernel.data());
        af_result = af::convolve(af_matrix, af_kernel,af_mode);
        std::vector<float> vec_result(af_result.elements());
        af_result.host(&vec_result[0]);
        int col = 0,row = 0,dim = 0;
        for(float i : vec_result){
          returned.at<float>(row,col, dim) = i;
          row++;
          if(row == matrix.size[0]){
            row = 0;
            col++;
          }
          if(col == matrix.size[1]){
            col = 0;
            row = 0;
            dim++;
          }
        }
      }break;
    }
  }
  if (alternateEvenCenter)
  {
    // to alternate the kernel center, we flip all inputs and then flip the result back
    std::vector<bool> flipped;
    flipped.assign(cedar::aux::math::getDimensionalityOf(matrixIn), true);
    cv::Mat tmp_returned = returned.clone() * 0.0;
    cedar::aux::math::flip(returned, tmp_returned, flipped);
    returned = tmp_returned;
  }
  return returned;
}
#endif