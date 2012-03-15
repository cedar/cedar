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

    File:        Engine.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 15

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_CONV_ENGINE_H
#define CEDAR_AUX_CONV_ENGINE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/namespace.h"
#include "cedar/auxiliaries/convolution/KernelList.h"
#include "cedar/auxiliaries/convolution/BorderType.h"
#include "cedar/auxiliaries/Configurable.h"

// SYSTEM INCLUDES


/*!@brief Base class for convolution engines.
 *
 * @todo describe more.
 */
class cedar::aux::conv::Engine : public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief This method convolves a given matrix with the kernel list stored in this convolution object.
   */
  virtual cv::Mat convolve
  (
    const cv::Mat& matrix,
    cedar::aux::conv::BorderType::Id borderType,
    const std::vector<unsigned int>& anchor = std::vector<unsigned int>()
  ) const = 0;

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

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

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
  // none yet

}; // class cedar::aux::conv::Engine

#endif // CEDAR_AUX_CONV_ENGINE_H

