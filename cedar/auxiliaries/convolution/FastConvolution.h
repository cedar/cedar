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

    File:        FastConvolution.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 11 28

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_CONV_FAST_CONVOLUTION_H
#define CEDAR_AUX_CONV_FAST_CONVOLUTION_H
#ifdef CEDAR_FFTW
// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/namespace.h"
#include "cedar/auxiliaries/convolution/Convolution.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <fftw3.h>

/*!@brief Base class for convolution.
 *
 * @todo describe more.
 */
class cedar::aux::conv::FastConvolution : public cedar::aux::conv::Convolution
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
  // none

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  cv::Mat convolve(const cv::Mat& matrix, const cv::Mat& kernel) const;

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

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // cedar::aux::conv::Convolution

#endif // CEDAR_FFTW
#endif // CEDAR_AUX_CONV_FAST_CONVOLUTION_H
