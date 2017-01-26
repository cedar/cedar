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

    File:        Separable.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 07 14

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_KERNEL_SEPARABLE_H
#define CEDAR_AUX_KERNEL_SEPARABLE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/kernel/Kernel.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/kernel/Separable.fwd.h"

// SYSTEM INCLUDES
#include <vector>

//!@brief Meta class to derive separable kernels when implementing kernels.
class cedar::aux::kernel::Separable : public cedar::aux::kernel::Kernel
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Construct an instance of a separable kernel with some parameters.
  Separable(unsigned int dimensionality = 2);
  //!@brief Destructor
  virtual ~Separable();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*!@brief this returns the one-dimensional part of a separable kernel for a specified dimension
   * @param dimension the desired dimension
   */
  virtual const cv::Mat& getKernelPart(unsigned int dimension) const;

  //!@brief returns the amount of kernel parts, from which the kernel is assembled
  inline size_t kernelPartCount() const
  {
    return this->mKernelParts.size();
  }

  void calculate();

  //!@brief returns the size for a given dimension
  unsigned int getSize(size_t dimension) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief set one kernel part of the specified dimension to the given matrix
  void setKernelPart(unsigned int dimension, const cv::Mat& mat);

  //! update the resulting kernel matrix by fusing the separate dimensions
  void updateKernelMatrix();
  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  virtual void calculateParts() = 0;

private slots:
  void dimensionalityChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief the kernel parts for each dimension
  std::vector<cv::Mat> mKernelParts; //!< the separate parts of a separable kernel

private:
  // none yet

}; // class cedar::aux::kernel::Separable

#endif // CEDAR_AUX_KERNEL_SEPARABLE_H

