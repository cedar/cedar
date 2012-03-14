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

    File:        Convolution.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 11 28

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_CONV_CONVOLUTION_H
#define CEDAR_AUX_CONV_CONVOLUTION_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/namespace.h"
#include "cedar/auxiliaries/convolution/BorderType.h"
#include "cedar/auxiliaries/convolution/KernelList.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/EnumParameter.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>


/*!@brief Base class for convolution engines.
 *
 * @todo describe more.
 */
class cedar::aux::conv::Convolution : public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  Convolution();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief This method convolves a given matrix with the kernel list stored in this convolution object.
   */
  virtual cv::Mat convolve(const cv::Mat& matrix) const = 0;

  //!@brief The convolution functor for a single matrix.
  cv::Mat operator()(const cv::Mat& matrix) const;

  //!@brief Method for accessing the kernel list.
  inline cedar::aux::conv::KernelList& getKernelList()
  {
    return this->mKernelList;
  }

  //!@brief Constant variant of the method for accessing the kernel list.
  inline const cedar::aux::conv::KernelList& getKernelList() const
  {
    return this->mKernelList;
  }

  //!@brief Get the border type.
  inline cedar::aux::conv::BorderType::Id getBorderType() const
  {
    return this->_mBorderType->getValue();
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! The list of kernel with which to convolve.
  cedar::aux::conv::KernelList mKernelList;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::aux::EnumParameterPtr _mBorderType;

}; // cedar::aux::conv::Convolution

#endif // CEDAR_AUX_CONV_CONVOLUTION_H
