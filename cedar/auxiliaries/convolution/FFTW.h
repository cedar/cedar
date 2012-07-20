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
#include "cedar/auxiliaries/convolution/namespace.h"
#include "cedar/auxiliaries/convolution/Engine.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <fftw3.h>

/*!@brief Base class for convolution.
 *
 * @todo describe more.
 */
class cedar::aux::conv::FFTW : public cedar::aux::conv::Engine
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  cv::Mat convolve
  (
    const cv::Mat& matrix,
    cedar::aux::conv::BorderType::Id borderType = cedar::aux::conv::BorderType::Replicate,
    cedar::aux::conv::Mode::Id mode = cedar::aux::conv::Mode::Same
  ) const;

  cv::Mat convolve
  (
    const cv::Mat& matrix,
    const cv::Mat& kernel,
    cedar::aux::conv::BorderType::Id borderType = cedar::aux::conv::BorderType::Replicate,
    cedar::aux::conv::Mode::Id mode = cedar::aux::conv::Mode::Same,
    const std::vector<int>& anchor = std::vector<int>()
  ) const;

  cv::Mat convolve
  (
    const cv::Mat& matrix,
    cedar::aux::kernel::ConstKernelPtr kernel,
    cedar::aux::conv::BorderType::Id borderType = cedar::aux::conv::BorderType::Replicate,
    cedar::aux::conv::Mode::Id mode = cedar::aux::conv::Mode::Same
  ) const;

  cv::Mat convolve
  (
    const cv::Mat& matrix,
    cedar::aux::conv::ConstKernelListPtr kernelList,
    cedar::aux::conv::BorderType::Id borderType = cedar::aux::conv::BorderType::Replicate,
    cedar::aux::conv::Mode::Id mode = cedar::aux::conv::Mode::Same
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

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief the internal version of the convolve method, currently called by all interface methods
  cv::Mat convolveInternal(const cv::Mat& matrix, const cv::Mat& kernel, cedar::aux::conv::BorderType::Id borderType) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  cv::Mat padKernel(const cv::Mat& matrix, const cv::Mat& kernel) const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

}; // cedar::aux::conv::FFTW

#endif // CEDAR_FFTW
#endif // CEDAR_AUX_CONV_FFTW_H
