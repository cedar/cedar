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
#include "cedar/auxiliaries/convolution/KernelList.h"
#include "cedar/auxiliaries/convolution/BorderType.h"
#include "cedar/auxiliaries/convolution/Mode.h"
#include "cedar/auxiliaries/Configurable.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/kernel/Separable.fwd.h"
#include "cedar/auxiliaries/convolution/Engine.fwd.h"

// SYSTEM INCLUDES
#include <vector>
#include <QObject>


/*!@brief Base class for convolution engines.
 *
 *        A convolution engine provides an interchangeable implementation for the convolution functions based on an
 *        underlying framework such as FFTW or Open CV. It also encapsulates technical details such as transformations
 *        of kernels and images.
 *
 *        In order to actually convolve an image, the cedar::aux::conv::Convolution class should be used.
 *
 * @see   cedar::aux::conv::Convolution
 */
class cedar::aux::conv::Engine : public QObject, public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief   This method convolves a given matrix with the kernel list stored in this convolution object.
   *
   * @returns This method should return \f$ R = \sum\limits_{l} I * K_l\f$ where \f$I\f$ is the input image and
   *          \f$K_l\f$ are the kernels stored in the engine's kernel list.
   */
  virtual cv::Mat convolve
  (
    const cv::Mat& matrix,
    cedar::aux::conv::BorderType::Id borderType = cedar::aux::conv::BorderType::Replicate,
    cedar::aux::conv::Mode::Id mode = cedar::aux::conv::Mode::Same,
    bool alternateEvenCenter = false
  ) const = 0;

  /*!@brief   Convolves two matrices with each other.
   */
  virtual cv::Mat convolve
  (
    const cv::Mat& matrix,
    const cv::Mat& kernel,
    cedar::aux::conv::BorderType::Id borderType = cedar::aux::conv::BorderType::Replicate,
    cedar::aux::conv::Mode::Id mode = cedar::aux::conv::Mode::Same,
    const std::vector<int>& anchor = std::vector<int>(),
    bool alternateEvenCenter = false
  ) const = 0;

  /*!@brief Convolves a matrix with a kernel.
   */
  virtual cv::Mat convolve
  (
    const cv::Mat& matrix,
    cedar::aux::kernel::ConstKernelPtr kernel,
    cedar::aux::conv::BorderType::Id borderType = cedar::aux::conv::BorderType::Replicate,
    cedar::aux::conv::Mode::Id mode = cedar::aux::conv::Mode::Same,
    bool alternateEvenCenter = false
  ) const = 0;


  /*!@brief   Convolve with a separable kernel.
   *
   * @remarks As a default, this method just calls the normal convolve function. Override this in order to implement
   *          faster convolution for separable kernels.
   */
  virtual cv::Mat convolveSeparable
  (
    const cv::Mat& matrix,
    cedar::aux::kernel::ConstSeparablePtr kernel,
    cedar::aux::conv::BorderType::Id borderType = cedar::aux::conv::BorderType::Replicate,
    cedar::aux::conv::Mode::Id mode = cedar::aux::conv::Mode::Same,
    bool alternateEvenCenter = false
  ) const;

  /*!@brief Convolves a matrix with a kernel list.
   */
  virtual cv::Mat convolve
  (
    const cv::Mat& matrix,
    cedar::aux::conv::ConstKernelListPtr kernelList,
    cedar::aux::conv::BorderType::Id borderType = cedar::aux::conv::BorderType::Replicate,
    cedar::aux::conv::Mode::Id mode = cedar::aux::conv::Mode::Same,
    bool alternateEvenCenter = false
  ) const = 0;

  //!@brief method for setting the kernel list
  virtual void setKernelList(cedar::aux::conv::KernelListPtr kernelList);

  //!@brief method for accessing the kernel list
  inline cedar::aux::conv::KernelListPtr getKernelList()
  {
    return this->mKernelList;
  }

  //!@brief Constant variant of the method for accessing the kernel list.
  inline cedar::aux::conv::ConstKernelListPtr getKernelList() const
  {
    return this->mKernelList;
  }

  //!@brief Checks if the convolution is capable of a certain type of operation.
  virtual bool checkCapability
  (
    size_t matrixDim,
    size_t kernelDim,
    cedar::aux::conv::BorderType::Id borderType,
    cedar::aux::conv::Mode::Id mode = cedar::aux::conv::Mode::Same
  ) const = 0;

  //!@brief Checks if the given two matrices can be convolved by the engine.
  virtual bool checkCapability
  (
    cv::Mat matrix,
    cv::Mat kernel,
    cedar::aux::conv::BorderType::Id borderType,
    cedar::aux::conv::Mode::Id mode = cedar::aux::conv::Mode::Same
  ) const = 0;

  //!@brief Checks if a matrix is capable of a certain type of convolution.
  virtual bool checkBorderTypeCapability
  (
    cedar::aux::conv::BorderType::Id borderType
  ) const = 0;

  //!@brief Checks if a matrix is capable of a certain type of convolution.
  virtual bool checkModeCapability
  (
    cedar::aux::conv::Mode::Id mode
  ) const = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
signals:
  //! Signals that the kernel list stored in the engine has changed.
  void kernelListChanged();

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
  cedar::aux::conv::KernelListPtr mKernelList;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::conv::Engine

#endif // CEDAR_AUX_CONV_ENGINE_H

