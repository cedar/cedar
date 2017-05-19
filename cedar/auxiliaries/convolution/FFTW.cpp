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

    File:        FFTW.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 11 28

    Description:

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"

#ifdef CEDAR_USE_FFTW

#define CEDAR_INTERNAL
#include "cedar/internals.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/FFTW.h"
#include "cedar/auxiliaries/convolution/FFTWPlanningStrategy.h"
#include "cedar/auxiliaries/convolution/EngineManager.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/kernel/Kernel.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/Settings.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/Path.h"

// SYSTEM INCLUDES
#ifdef CEDAR_USE_FFTW_THREADED
#include <omp.h>
#endif // CEDAR_USE_FFTW_THREADED

QReadWriteLock cedar::aux::conv::FFTW::mPlanLock;
bool cedar::aux::conv::FFTW::mMultiThreadActivated = false;
std::set<std::string> cedar::aux::conv::FFTW::mLoadedWisdoms;
std::map<std::string, fftw_plan> cedar::aux::conv::FFTW::mForwardPlans;
std::map<std::string, fftw_plan> cedar::aux::conv::FFTW::mBackwardPlans;

//----------------------------------------------------------------------------------------------------------------------
// register type with the factory
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered
    = cedar::aux::conv::EngineManagerSingleton::getInstance()->registerType<cedar::aux::conv::FFTWPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::conv::FFTW::FFTW()
:
mAllocatedSize(0),
mMatrixBuffer(nullptr),
mKernelBuffer(nullptr),
mResultBuffer(nullptr),
mRetransformKernel(true)
{
 this->connect(this, SIGNAL(kernelListChanged()), SLOT(kernelListChanged()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
cv::Mat cedar::aux::conv::FFTW::convolve
(
  const cv::Mat& matrix,
  cedar::aux::conv::BorderType::Id borderType,
  cedar::aux::conv::Mode::Id /* mode */,
  bool alternateEvenCenter
) const
{
  if (this->getKernelList()->size() > 0)
  {
    return this->convolveInternal(matrix, this->getKernelList()->getCombinedKernel(), borderType, alternateEvenCenter);
  }
  else
  {
    return cv::Mat(matrix.dims, matrix.size, matrix.type(), cv::Scalar(0.0));
  }
}

cv::Mat cedar::aux::conv::FFTW::convolve
(
  const cv::Mat& matrix,
  const cv::Mat& kernel,
  cedar::aux::conv::BorderType::Id borderType,
  cedar::aux::conv::Mode::Id /* mode */,
  const std::vector<int>& /* anchor */,
  bool alternateEvenCenter
) const
{
  this->kernelChanged();
  return this->convolveInternal(matrix, kernel, borderType, alternateEvenCenter);
}

cv::Mat cedar::aux::conv::FFTW::convolve
(
  const cv::Mat& matrix,
  cedar::aux::kernel::ConstKernelPtr kernel,
  cedar::aux::conv::BorderType::Id borderType,
  cedar::aux::conv::Mode::Id /* mode */,
  bool alternateEvenCenter
) const
{
  this->kernelChanged();
  return this->convolveInternal(matrix, kernel->getKernel(), borderType, alternateEvenCenter);
}

cv::Mat cedar::aux::conv::FFTW::convolve
(
  const cv::Mat& matrix,
  cedar::aux::conv::ConstKernelListPtr kernelList,
  cedar::aux::conv::BorderType::Id borderType,
  cedar::aux::conv::Mode::Id /* mode */,
  bool alternateEvenCenter
) const
{
  if (kernelList->size() > 0)
  {
    this->kernelChanged();
    return this->convolveInternal(matrix, kernelList->getCombinedKernel(), borderType, alternateEvenCenter);
  }
  else
  {
    return cv::Mat(matrix.dims, matrix.size, matrix.type(), cv::Scalar(0.0));
  }
}

cv::Mat cedar::aux::conv::FFTW::convolveInternal
        (
          const cv::Mat& matrixIn,
          const cv::Mat& kernelIn,
          cedar::aux::conv::BorderType::Id /* borderType */,
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
  //!@todo Why the - 1?
  for (unsigned int dim = 0 ; dim < cedar::aux::math::getDimensionalityOf(matrix) - 1; ++dim)
  {
    if (matrix.size[dim] < kernel.size[dim])
    {
      CEDAR_THROW
      (
        cedar::aux::RangeException,
        "Kernel size is too big for FFTW convolution, (dimension " + cedar::aux::toString(dim) + ": matrix is "
        + cedar::aux::toString(matrix.size[dim]) + ", kernel is " + cedar::aux::toString(kernel.size[dim])
        + " elements wide). Please decrease kernel size or switch to different convolution engine."
      );
    }
  }

  cv::Mat matrix_64;
  cv::Mat kernel_64;
  if (matrix.type() != CV_64F)
  {
    matrix.convertTo(matrix_64, CV_64F);
  }
  else
  {
    CEDAR_ASSERT(matrix.type() == CV_64F);
    matrix_64 = matrix;
  }

  if (kernel.type() != CV_64F)
  {
    kernel.convertTo(kernel_64, CV_64F);
  }
  else
  {
    CEDAR_ASSERT(kernel.type() == CV_64F);
    kernel_64 = kernel;
  }

  cv::Mat output = matrix_64.clone();
  output = 0.0;
  cv::Mat padded_kernel = this->padKernel(matrix_64, kernel_64);

  unsigned int transformed_elements = 1;
  double number_of_elements = 1.0;
  for (unsigned int dim = 0 ; dim < cedar::aux::math::getDimensionalityOf(matrix_64) - 1; ++dim)
  {
    transformed_elements *= matrix_64.size[dim];
    number_of_elements *= matrix_64.size[dim];
  }
  transformed_elements *= matrix_64.size[cedar::aux::math::getDimensionalityOf(matrix_64) -1] / 2 + 1;
  number_of_elements *= matrix_64.size[cedar::aux::math::getDimensionalityOf(matrix_64) -1];

  if (transformed_elements != mAllocatedSize)
  {
    if (mMatrixBuffer)
    {
      fftw_free(mMatrixBuffer);
    }

    if (mResultBuffer)
    {
      fftw_free(mResultBuffer);
    }

    if (mKernelBuffer)
    {
      fftw_free(mKernelBuffer);
    }

    mMatrixBuffer = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * transformed_elements);
    mResultBuffer = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * transformed_elements);
    mKernelBuffer = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * transformed_elements);
    mAllocatedSize = transformed_elements;
    QWriteLocker write_lock(&this->mKernelTransformLock);
    this->mRetransformKernel = true;
  }

//  fftw_execute(matrix_plan_forward);
  std::vector<unsigned int> mat_sizes(cedar::aux::math::getDimensionalityOf(matrix_64));
  for (unsigned int dim = 0; dim < mat_sizes.size(); ++dim)
  {
   mat_sizes.at(dim) = static_cast<unsigned int>(matrix.size[dim]);
  }

  fftw_execute_dft_r2c
  (
    cedar::aux::conv::FFTW::getForwardPlan(cedar::aux::math::getDimensionalityOf(matrix_64), mat_sizes),
    const_cast<double*>(matrix_64.ptr<double>()),
    mMatrixBuffer
  );

  QWriteLocker write_lock(&this->mKernelTransformLock);
  if (this->mRetransformKernel)
  {
    fftw_execute_dft_r2c
    (
      cedar::aux::conv::FFTW::getForwardPlan(cedar::aux::math::getDimensionalityOf(padded_kernel), mat_sizes),
      const_cast<double*>(padded_kernel.ptr<double>()),
      mKernelBuffer
    );
    this->mRetransformKernel = false;
  }
  write_lock.unlock();

  // go trough all data points
  for (unsigned int xyz = 0; xyz < transformed_elements; ++xyz)
  {
    // complex multiplication (lateral)
    mResultBuffer[xyz][0] = (mKernelBuffer[xyz][0] * mMatrixBuffer[xyz][0] - mKernelBuffer[xyz][1] * mMatrixBuffer[xyz][1]) / number_of_elements;
    mResultBuffer[xyz][1] = (mKernelBuffer[xyz][1] * mMatrixBuffer[xyz][0] + mKernelBuffer[xyz][0] * mMatrixBuffer[xyz][1]) / number_of_elements;
  }

  // transform interaction back to time domain (ifft)
  fftw_execute_dft_c2r
  (
    cedar::aux::conv::FFTW::getBackwardPlan(cedar::aux::math::getDimensionalityOf(output), mat_sizes),
    mResultBuffer,
    const_cast<double*>(output.ptr<double>())
  );

  cv::Mat returned;
  if (matrix.type() != CV_64F)
  {
    output.convertTo(returned, matrix.type());
  }
  else
  {
    returned = output;
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

cv::Mat cedar::aux::conv::FFTW::padKernel(const cv::Mat& matrix, const cv::Mat& kernel) const
{
  /* prepare the kernel for Fourier transform (pad to matrix size and flip); example for 2D:
   * 010    211
   * 121 => 100 anchor moves to 0,0
   * 010    001
   */

  CEDAR_DEBUG_ASSERT(matrix.dims == kernel.dims);

  cv::Mat output = matrix.clone();
  output = 0.0;
  std::vector<cv::Range> region;
  std::vector<std::vector<cv::Range> > regions;
  regions.resize(2);
  std::vector<std::vector<cv::Range> > kernel_regions;
  kernel_regions.resize(2);
  for (size_t dim = 0; dim < cedar::aux::math::getDimensionalityOf(matrix); ++dim)
  {
    int kernel_center = kernel.size[dim]/2;
    regions.at(0).push_back(cv::Range(0, kernel_center + (kernel.size[dim] % 2))); // lower limit
    if (dim >= cedar::aux::math::getDimensionalityOf(kernel)) // fix for strange kernels (kind of a ridge padding)
    {
      regions.at(1).push_back(cv::Range(matrix.size[dim] - 1, matrix.size[dim])); // upper limit
      kernel_regions.at(0).push_back(cv::Range(0, 1)); // lower limit
    }
    else
    {
      regions.at(1).push_back(cv::Range(matrix.size[dim] - kernel_center, matrix.size[dim])); // upper limit
      kernel_regions.at(0).push_back(cv::Range(0, kernel_center)); // lower limit
    }

    kernel_regions.at(1).push_back(cv::Range(kernel_center, kernel.size[dim])); // upper limit
  }

  unsigned int dim_0_fix = 0;
  if (cedar::aux::math::getDimensionalityOf(matrix) == 1)
  {
    dim_0_fix = 1;
  }

  std::vector<cv::Range> output_index(cedar::aux::math::getDimensionalityOf(matrix) + dim_0_fix);
  std::vector<cv::Range> kernel_index(cedar::aux::math::getDimensionalityOf(matrix) + dim_0_fix);

  for (size_t part = 0; part < static_cast<unsigned int>((1 << cedar::aux::math::getDimensionalityOf(matrix))); ++part)
  {
    for (size_t dim = 0; dim < cedar::aux::math::getDimensionalityOf(matrix); ++dim)
    {
      if (part & (1 << dim))
      {
        output_index[dim] = regions.at(0).at(dim);
        kernel_index[dim] = kernel_regions.at(1).at(dim);
      }
      else
      {
        output_index[dim] = regions.at(1).at(dim);
        kernel_index[dim] = kernel_regions.at(0).at(dim);
      }
    }

    //!@todo Should't this check if rows or cols is 1 and decide where to put this based on that?
    if (cedar::aux::math::getDimensionalityOf(matrix) == 1)
    {
      output_index[1] = cv::Range(0,1);
      kernel_index[1] = cv::Range(0,1);
    }

    // if there are empty ranges (where start == end), skip this part
    bool skip = false;
    for (auto range : output_index)
    {
      if (range.start == range.end)
      {
        skip = true;
        break;
      }
    }

    if (skip)
    {
      continue;
    }

    // if 1.0 is missing, the temporary cv::Mat view onto output is replaced by kernel, instead of setting the values
    // at the specified region to the values of kernel (1.0 * kernel returns a cv::MatExpr, not cv::Mat...)

    output(&(output_index.front())) = 1.0 * kernel(&(kernel_index.front()));
  }
  return output;
}

bool cedar::aux::conv::FFTW::checkCapability
     (
       size_t matrixDim,
       size_t kernelDim,
       cedar::aux::conv::BorderType::Id borderType,
       cedar::aux::conv::Mode::Id mode
     )
     const
{
  if (matrixDim != kernelDim)
  {
    return false;
  }

  if (!this->checkBorderTypeCapability(borderType) || !this->checkModeCapability(mode))
  {
    return false;
  }

  return true;
}

bool cedar::aux::conv::FFTW::checkCapability
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

  CEDAR_DEBUG_ASSERT(matrix.dims == kernel.dims);

  for (int d = 0; d < matrix.dims; ++d)
  {
    if (matrix.size[d] < kernel.size[d])
    {
      return false;
    }
  }

  return true;
}

bool cedar::aux::conv::FFTW::checkBorderTypeCapability
     (
       cedar::aux::conv::BorderType::Id borderType
     ) const
{
  switch (borderType)
  {
    case cedar::aux::conv::BorderType::Cyclic:
      return true;

    default:
      return false;
  }
}

bool cedar::aux::conv::FFTW::checkModeCapability
     (
       cedar::aux::conv::Mode::Id mode
     ) const
{
  // currently, only the full same is supported
  return mode == cedar::aux::conv::Mode::Same;
}

void cedar::aux::conv::FFTW::loadWisdom(const std::string& uniqueIdentifier)
{
  if (cedar::aux::conv::FFTW::mLoadedWisdoms.find(uniqueIdentifier) == cedar::aux::conv::FFTW::mLoadedWisdoms.end())
  {
    std::string path = cedar::aux::getUserApplicationDataDirectory()
                         + "/.cedar/fftw/fftw."
                         + CEDAR_BUILT_ON_MACHINE + "."
                         + cedar::aux::toString(cedar::aux::SettingsSingleton::getInstance()->getFFTWNumberOfThreads()) + "."
                         + cedar::aux::toString(cedar::aux::SettingsSingleton::getInstance()->getFFTWPlanningStrategyString()) + "."
                         + uniqueIdentifier + "."
                         + "wisdom";
    fftw_import_wisdom_from_filename(path.c_str());
    cedar::aux::conv::FFTW::mLoadedWisdoms.insert(uniqueIdentifier);
  }
}

void cedar::aux::conv::FFTW::saveWisdom(const std::string& uniqueIdentifier)
{
  cedar::aux::Path path = cedar::aux::getUserApplicationDataDirectory()
                          + "/.cedar/fftw/fftw."
                          + CEDAR_BUILT_ON_MACHINE + "."
                          + cedar::aux::toString(cedar::aux::SettingsSingleton::getInstance()->getFFTWNumberOfThreads()) + "."
                          + cedar::aux::toString(cedar::aux::SettingsSingleton::getInstance()->getFFTWPlanningStrategyString()) + "."
                          + uniqueIdentifier + "."
                          + "wisdom";
  path.createDirectories();
                       
  fftw_export_wisdom_to_filename(path.toString().c_str());
}

fftw_plan cedar::aux::conv::FFTW::getForwardPlan(unsigned int dimensionality, std::vector<unsigned int> sizes)
{
  CEDAR_ASSERT(sizes.size() == dimensionality);
  std::string unique_identifier = cedar::aux::toString(sizes.at(0));
  for (unsigned int i = 1; i < sizes.size(); ++i)
  {
    unique_identifier += "." + cedar::aux::toString(sizes.at(i));
  }
  auto entry = cedar::aux::conv::FFTW::mForwardPlans.find(unique_identifier);
  if (entry != cedar::aux::conv::FFTW::mForwardPlans.end())
  {
    return entry->second;
  }
  else
  {
#ifdef CEDAR_USE_FFTW_THREADED
    cedar::aux::conv::FFTW::initThreads();
#endif
    QWriteLocker plan_locker(&cedar::aux::conv::FFTW::mPlanLock);
    cedar::aux::conv::FFTW::loadWisdom(unique_identifier);
    std::vector<int> sizes_signed(sizes.size());
    for (unsigned int i = 0; i < sizes_signed.size(); ++i)
    {
      sizes_signed.at(i) = static_cast<int>(sizes.at(i));
    }

    cv::Mat matrix(dimensionality, &(sizes_signed.front()), CV_64F);

    unsigned int transformed_elements = 1;
    double number_of_elements = 1.0;
    for (unsigned int dim = 0 ; dim < cedar::aux::math::getDimensionalityOf(matrix) - 1; ++dim)
    {
      transformed_elements *= matrix.size[dim];
      number_of_elements *= matrix.size[dim];
    }
    transformed_elements *= matrix.size[cedar::aux::math::getDimensionalityOf(matrix) -1] / 2 + 1;
    number_of_elements *= matrix.size[cedar::aux::math::getDimensionalityOf(matrix) -1];

    fftw_complex* matrix_fourier = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * transformed_elements);
    fftw_plan matrix_plan_forward
      = fftw_plan_dft_r2c
        (
          cedar::aux::math::getDimensionalityOf(matrix),
          matrix.size,
          const_cast<double*>(matrix.clone().ptr<double>()),
          matrix_fourier,
          cedar::aux::SettingsSingleton::getInstance()->getFFTWPlanningStrategy()
        );
    fftw_free(matrix_fourier);
    if (matrix_plan_forward)
    {
      cedar::aux::conv::FFTW::mForwardPlans[unique_identifier] = matrix_plan_forward;
      cedar::aux::conv::FFTW::saveWisdom(unique_identifier);
      plan_locker.unlock();
      return matrix_plan_forward;
    }
    else
    {
      plan_locker.unlock();
      CEDAR_THROW
      (
        cedar::aux::NotFoundException,
        "FFTW could not find a forward transformation plan for a matrix with sizes " + unique_identifier
        + ". You can try to alter the planning strategy."
      );
    }
  }
}

fftw_plan cedar::aux::conv::FFTW::getBackwardPlan(unsigned int dimensionality, std::vector<unsigned int> sizes)
{
  CEDAR_ASSERT(sizes.size() == dimensionality);
  std::string unique_identifier = cedar::aux::toString(sizes.at(0));
  for (unsigned int i = 1; i < sizes.size(); ++i)
  {
    unique_identifier += "." + cedar::aux::toString(sizes.at(i));
  }
  auto entry = cedar::aux::conv::FFTW::mBackwardPlans.find(unique_identifier);
  if (entry != cedar::aux::conv::FFTW::mBackwardPlans.end())
  {
    return entry->second;
  }
  else
  {
#ifdef CEDAR_USE_FFTW_THREADED
    cedar::aux::conv::FFTW::initThreads();
#endif
    QWriteLocker plan_locker(&cedar::aux::conv::FFTW::mPlanLock);
    cedar::aux::conv::FFTW::loadWisdom(unique_identifier);
    std::vector<int> sizes_signed(sizes.size());
    for (unsigned int i = 0; i < sizes_signed.size(); ++i)
    {
      sizes_signed.at(i) = static_cast<int>(sizes.at(i));
    }

    cv::Mat matrix(dimensionality, &(sizes_signed.front()), CV_64F);

    unsigned int transformed_elements = 1;
    double number_of_elements = 1.0;
    for (unsigned int dim = 0 ; dim < cedar::aux::math::getDimensionalityOf(matrix) - 1; ++dim)
    {
      transformed_elements *= matrix.size[dim];
      number_of_elements *= matrix.size[dim];
    }
    transformed_elements *= matrix.size[cedar::aux::math::getDimensionalityOf(matrix) -1] / 2 + 1;
    number_of_elements *= matrix.size[cedar::aux::math::getDimensionalityOf(matrix) -1];

    fftw_complex* matrix_fourier = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * transformed_elements);
    fftw_plan matrix_plan_backward
      = fftw_plan_dft_c2r
        (
          cedar::aux::math::getDimensionalityOf(matrix),
          matrix.size,
          matrix_fourier,
          const_cast<double*>(matrix.ptr<double>()),
          cedar::aux::SettingsSingleton::getInstance()->getFFTWPlanningStrategy()
        );
    fftw_free(matrix_fourier);
    if (matrix_plan_backward)
    {
      cedar::aux::conv::FFTW::mBackwardPlans[unique_identifier] = matrix_plan_backward;
      cedar::aux::conv::FFTW::saveWisdom(unique_identifier);
      plan_locker.unlock();
      return matrix_plan_backward;
    }
    else
    {
      CEDAR_THROW
      (
        cedar::aux::NotFoundException,
        "FFTW could not find a backward transformation plan for a matrix with sizes " + unique_identifier
        + ". You can try to alter the planning strategy."
      );
    }
  }
}

void cedar::aux::conv::FFTW::initThreads()
{
#ifdef CEDAR_USE_FFTW_THREADED
  if (!mMultiThreadActivated)
  {
    // this should be done only once
    fftw_init_threads();
    omp_set_num_threads(cedar::aux::SettingsSingleton::getInstance()->getFFTWNumberOfThreads());
    fftw_set_timelimit(30.0);
    // from now on, all plans are generated for n threads
    fftw_plan_with_nthreads(cedar::aux::SettingsSingleton::getInstance()->getFFTWNumberOfThreads());
    // make sure that we do not initialize this again
    mMultiThreadActivated = true;
  }
#endif
}

void cedar::aux::conv::FFTW::kernelChanged() const
{
  QWriteLocker write_lock(&this->mKernelTransformLock);
  this->mRetransformKernel = true;
}

void cedar::aux::conv::FFTW::kernelListChanged()
{
  this->connect(this->getKernelList().get(), SIGNAL(combinedKernelUpdated()), SLOT(kernelChanged()));
}

#endif // CEDAR_FFTW
