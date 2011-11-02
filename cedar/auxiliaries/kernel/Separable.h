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

    File:        Separable.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 07 14

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_KERNEL_SEPARABLE_H
#define CEDAR_AUX_KERNEL_SEPARABLE_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/kernel/namespace.h"
#include "cedar/auxiliaries/kernel/Kernel.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

/*!@brief Meta class to derive separable kernels when implementing kernels.
 * @todo  The mKernel matrix should be automatically calculated here, rather than having to implement it in every
 *        derived class.
 */
class cedar::aux::kernel::Separable : public cedar::aux::kernel::Kernel
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Separable();
  Separable(unsigned int dimensionality, const std::string& kernelFile = "dummy_matrix_file.yml");
  //!@brief Destructor
  virtual ~Separable();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*!\brief this returns the one-dimensional part of a separable kernel for a specified dimension
   * @param dim the desired dimension
   */
  virtual const cv::Mat& getKernelPart(unsigned int dimension) const;

  cv::Mat convolveWith(const cv::Mat& mat) const;

  void setNumParts(unsigned int numParts);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void setKernelPart(unsigned int dimension, const cv::Mat& mat);

  //!@todo make private
  std::vector<cv::Mat> mKernelParts; //!< the separate parts of a separable kernel
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:

private:
  // none yet

}; // class cedar::aux::kernel::Separable

#endif // CEDAR_AUX_KERNEL_SEPARABLE_H

