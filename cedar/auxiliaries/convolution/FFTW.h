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

    File:        FFTW.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 11 28

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_CONV_FFTW_H
#define CEDAR_AUX_CONV_FFTW_H

#include "cedar/configuration.h"

#ifdef CEDAR_USE_FFTW

// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/Engine.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/convolution/FFTW.fwd.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <fftw3.h>
#include <vector>
#include <map>
#include <string>
#include <set>

/*!@brief A convolution engine based on the FFTW library.
 */
class cedar::aux::conv::FFTW : public cedar::aux::conv::Engine
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  FFTW();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  cv::Mat convolve
  (
    const cv::Mat& matrix,
    cedar::aux::conv::BorderType::Id borderType = cedar::aux::conv::BorderType::Replicate,
    cedar::aux::conv::Mode::Id mode = cedar::aux::conv::Mode::Same,
    bool alternateEvenCenter = false
  ) const;

  cv::Mat convolve
  (
    const cv::Mat& matrix,
    const cv::Mat& kernel,
    cedar::aux::conv::BorderType::Id borderType = cedar::aux::conv::BorderType::Replicate,
    cedar::aux::conv::Mode::Id mode = cedar::aux::conv::Mode::Same,
    const std::vector<int>& anchor = std::vector<int>(),
    bool alternateEvenCenter = false
  ) const;

  cv::Mat convolve
  (
    const cv::Mat& matrix,
    cedar::aux::kernel::ConstKernelPtr kernel,
    cedar::aux::conv::BorderType::Id borderType = cedar::aux::conv::BorderType::Replicate,
    cedar::aux::conv::Mode::Id mode = cedar::aux::conv::Mode::Same,
    bool alternateEvenCenter = false
  ) const;

  cv::Mat convolve
  (
    const cv::Mat& matrix,
    cedar::aux::conv::ConstKernelListPtr kernelList,
    cedar::aux::conv::BorderType::Id borderType = cedar::aux::conv::BorderType::Replicate,
    cedar::aux::conv::Mode::Id mode = cedar::aux::conv::Mode::Same,
    bool alternateEvenCenter = false
  ) const;

  bool checkCapability
  (
    size_t matrixDim,
    size_t kernelDim,
    cedar::aux::conv::BorderType::Id borderType,
    cedar::aux::conv::Mode::Id mode
  ) const;

  bool checkCapability
  (
    cv::Mat matrix,
    cv::Mat kernel,
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

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief the internal version of the convolve method, currently called by all interface methods
  cv::Mat convolveInternal
  (
    const cv::Mat& matrix,
    const cv::Mat& kernel,
    cedar::aux::conv::BorderType::Id borderType,
    bool alternateEvenCenter
  ) const;

  //!@brief adjusts the size of the kernel (to matrix size) and flips sectors
  cv::Mat padKernel(const cv::Mat& matrix, const cv::Mat& kernel) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  static fftw_plan getForwardPlan(unsigned int dimensionality, std::vector<unsigned int> sizes);
  static fftw_plan getBackwardPlan(unsigned int dimensionality, std::vector<unsigned int> sizes);
  static void loadWisdom(const std::string& uniqueIdentifier);
  static void saveWisdom(const std::string& uniqueIdentifier);
  static void initThreads();

private slots:
  void kernelChanged() const;
  void kernelListChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief plan creation and destruction is not thread-safe, must be locked
  static QReadWriteLock mPlanLock;
  static bool mMultiThreadActivated;
  static std::set<std::string> mLoadedWisdoms;
  static std::map<std::string, fftw_plan> mForwardPlans;
  static std::map<std::string, fftw_plan> mBackwardPlans;
  mutable unsigned int mAllocatedSize;
  mutable fftw_complex* mMatrixBuffer;
  mutable fftw_complex* mKernelBuffer;
  mutable fftw_complex* mResultBuffer;
  // dirty flag if kernel has changed since last time
  mutable bool mRetransformKernel;
  mutable QReadWriteLock mKernelTransformLock;
}; // cedar::aux::conv::FFTW

#endif // CEDAR_FFTW
#endif // CEDAR_AUX_CONV_FFTW_H
