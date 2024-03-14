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

    File:        ArrayFire.h

    Maintainer:  
    Email:       
    Date:       

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_CONVOLUTION_ARRAYFIRE_H
#define CEDAR_AUX_CONVOLUTION_ARRAYFIRE_H

#include "cedar/configuration.h"

#ifdef CEDAR_USE_ARRAYFIRE

// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/Engine.h"
#include "cedar/auxiliaries/opencv_helper.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/convolution/ArrayFire.fwd.h"

#include <arrayfire.h>


/*!@brief A convolution engine based on ArrayFire's filter engine.
 */
class cedar::aux::conv::ArrayFire : public cedar::aux::conv::Engine
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  ArrayFire();

  ~ArrayFire();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  cv::Mat convolve
  (
    const cv::Mat& matrix,
    cedar::aux::conv::BorderType::Id borderType,
    cedar::aux::conv::Mode::Id mode,
    bool alternateEvenCenter = false
  ) const;

  cv::Mat convolve
  (
    const cv::Mat& matrix,
    const cv::Mat& kernel,
    cedar::aux::conv::BorderType::Id borderType,
    cedar::aux::conv::Mode::Id mode,
    const std::vector<int>& anchor,
    bool alternateEvenCenter = false
  ) const;

  cv::Mat convolve
  (
    const cv::Mat& matrix,
    cedar::aux::kernel::ConstKernelPtr kernel,
    cedar::aux::conv::BorderType::Id borderType,
    cedar::aux::conv::Mode::Id mode,
    bool alternateEvenCenter = false
  ) const;

  cv::Mat convolve
  (
    const cv::Mat& matrix,
    cedar::aux::conv::ConstKernelListPtr kernelList,
    cedar::aux::conv::BorderType::Id borderType,
    cedar::aux::conv::Mode::Id mode,
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
  // none yet
  cv::Mat convolveInternal
          (
                  const cv::Mat& matrix,
                  const cv::Mat& kernel,
                  cedar::aux::conv::BorderType::Id borderType,
                  cedar::aux::conv::Mode::Id mode,
                  bool alternateEvenCenter
          ) const;
  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::conv::ArrayFire

#endif // CEDAR_AUX_CONVOLUTION_ARRAYFIRE_H

#endif