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
#include "cedar/auxiliaries/convolution/Engine.h"
#include "cedar/auxiliaries/convolution/KernelList.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/ObjectParameterTemplate.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <QObject>


/*!@brief   Base class for convolution engines.
 *
 *          This class is the high-level interface for performing convolutions within cedar. An an underlying
 *          cedar::aux::conv::Engine decides what implementation of the convolution is actually used. There are multiple
 *          ways of convolving with this class, namely the different @em convolve methods.
 *
 *          Some methods can be called with separate kernels or kernel lists, however, the fastest method is usually to
 *          have a persistent kernel as some implementations (e.g., FFT convolution) store a transformed version of the
 *          kernel list that is only recalculated as needed.
 *
 * @remarks The choice of convolution engine should not affect the outcome of the convolution, only the speed of it.
 *
 * @see     cedar::aux::conv::Engine, cedar::aux::conv::BorderType, cedar::aux::conv::Mode,
 *          cedar::aux::conv::KernelList, cedar::aux::conv
 *
 * @todo    Should the kernel list be stored within this object rather than each engine?
 *
 */
class cedar::aux::conv::Convolution : public QObject, public cedar::aux::Configurable
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The default constructor.
  Convolution();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief This method convolves a given matrix with the settings stored in this convolution object.
   *
   *        Calling this method returns the sum of the convolutions between the input matrix and each kernel stored in
   *        the kernel list in this convolution object, i.e.,
   *        \f[
   *           R = \sum\limits_{l} I * k_l
   *        \f]
   *        where \f$R\f$ is the return value of the function, \f$I\f$ is the input image and \f$k_l\f$ are the kernels
   *        stored in the kernel list.
   *
   * @param matrix The matrix to be convolved with the kernels.
   */
  inline cv::Mat convolve(const cv::Mat& matrix) const
  {
    return this->getEngine()->convolve(matrix, this->getBorderType(), this->getMode());
  }

  /*!@brief Convolves two matrices with each other using the selected engine of the convolution object.
   *
   * @param matrix The matrix to be convolved.
   * @param kernel The kernel with which to convolve the matrix.
   * @param anchor The anchor of the convolution. This can be positive or negative and means an offset from the kernel's
   *               center; values with a magnitude greater than half the kernel size are mapped to the nearest pixel on
   *               the edge of the kernel.
   */
  inline cv::Mat convolve
  (
    const cv::Mat& matrix,
    const cv::Mat& kernel,
    const std::vector<int>& anchor = std::vector<int>()
  ) const
  {
    return this->getEngine()->convolve(matrix, kernel, this->getBorderType(), this->getMode(), anchor);
  }

  /*!@brief Convolution of a matrix with a single kernel.
   */
  inline cv::Mat convolve
  (
    const cv::Mat& matrix,
    cedar::aux::kernel::ConstKernelPtr kernel
  ) const
  {
    return this->getEngine()->convolve(matrix, kernel, this->getBorderType(), this->getMode());
  }

  /*!@brief Convolve with a separable kernel using the set convolution engine.
   */
  inline cv::Mat convolve
  (
    const cv::Mat& matrix,
    cedar::aux::kernel::ConstSeparablePtr kernel
  ) const
  {
    return this->getEngine()->convolve(matrix, kernel, this->getBorderType(), this->getMode());
  }

  /*!@brief   Convolves a matrix with a kernel list.
   *
   * @remarks This method is provided for convenience. It is likely faster to set the same kernel list to a (new)
   *          convolution object permanently and reuse said object because fewer transformations will have to be
   *          computed when the objects are reused.
   */
  inline cv::Mat convolve
  (
    const cv::Mat& matrix,
    const cedar::aux::conv::KernelList& kernelList
  ) const
  {
    return this->getEngine()->convolve(matrix, kernelList, this->getBorderType(), this->getMode());
  }


  //!@brief The convolution functor for a matrix.
  inline cv::Mat operator()(const cv::Mat& matrix) const
  {
    return this->convolve(matrix);
  }

  //!@brief The convolution functor for a matrix and a kernel matrix.
  inline cv::Mat operator()
  (
    const cv::Mat& matrix,
    const cv::Mat kernel,
    const std::vector<int>& anchor = std::vector<int>()
  ) const
  {
    return this->convolve(matrix, kernel, anchor);
  }

  //!@brief The convolution functor for a matrix and a kernel.
  inline cv::Mat operator()
  (
    const cv::Mat& matrix,
    cedar::aux::kernel::ConstKernelPtr kernel
  ) const
  {
    return this->convolve(matrix, kernel);
  }

  //!@brief The convolution functor for a matrix and a separable kernel.
  inline cv::Mat operator()
  (
    const cv::Mat& matrix,
    cedar::aux::kernel::ConstSeparablePtr kernel
  ) const
  {
    return this->convolve(matrix, kernel);
  }

  //!@brief The convolution functor for a matrix and a kernel list.
  inline cv::Mat operator()
  (
    const cv::Mat& matrix,
    const cedar::aux::conv::KernelList& kernelList
  ) const
  {
    return this->convolve(matrix, kernelList);
  }

  //!@brief Returns the convolution engine.
  inline cedar::aux::conv::ConstEnginePtr getEngine() const
  {
    return this->_mEngine->getValue();
  }

  //!@brief Get the border type.
  inline cedar::aux::conv::BorderType::Id getBorderType() const
  {
    return this->_mBorderType->getValue();
  }

  //!@brief Returns the mode of the convolution.
  inline cedar::aux::conv::Mode::Id getMode() const
  {
    return this->_mMode->getValue();
  }

  //!@brief Returns a pointer to the combined kernel.
  inline cedar::aux::MatDataPtr getCombinedKernel()
  {
    return this->mCombinedKernel;
  }

  //!@brief Method for accessing the kernel list.
  inline cedar::aux::conv::KernelListPtr getKernelList()
  {
    return this->mKernelList;
  }

  //!@brief Constant variant of the method for accessing the kernel list.
  inline cedar::aux::conv::ConstKernelListPtr getKernelList() const
  {
    return this->mKernelList;
  }
signals:
  void configurationChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Returns the convolution engine.
  inline cedar::aux::conv::EnginePtr getEngine()
  {
    return this->_mEngine->getValue();
  }

  /*!@brief Reacts to the addition of a new kernel to the kernel list.
   */
  void slotKernelAdded(size_t);

  /*!@brief Reacts to a change of a kernel in the kernel list.
   */
  void slotKernelChanged(size_t);

  /*!@brief Reacts to the removal of a kernel from the kernel list.
   */
  void slotKernelRemoved(size_t);

  /*!@brief Updates the engine capabilities.
   */
  void updateEngineCapabilities();

private slots:
  //!@brief Reacts to a change of the convolution engine.
  void selectedEngineChanged();

  /*!@brief (Re)calculates the combined kernel.
   */
  void updateCombinedKernel();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  /*! Matrix representing the combined kernel. Convolving an image with this kernel is equivalent to convolving it with
   *  the kernel list.
   *
   *  @todo Make a unit test for this.
   */
  cedar::aux::MatDataPtr mCombinedKernel;

  //! Connection to the kernel added signal of the kernel list.
  boost::signals2::connection mKernelAddedConnection;

  //! Connection to the kernel changed signal of the kernel list.
  boost::signals2::connection mKernelChangedConnection;

  //! Connection to the kernel removed signal of the kernel list.
  boost::signals2::connection mKernelRemovedConnection;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  /*! The type of border handling applied to convolutions performed with this object.
   *
   *  @see cedar::aux::conv::BorderType
   */
  cedar::aux::EnumParameterPtr _mBorderType;

  /*! The mode of this convolution object.
   *
   *  @see cedar::aux::conv::Mode
   */
  cedar::aux::EnumParameterPtr _mMode;

  //! The list of kernel with which to convolve.
  cedar::aux::conv::KernelListPtr mKernelList;

  //! The engine used for convolutions performed with this object.
  cedar::aux::conv::EngineParameterPtr _mEngine;

}; // cedar::aux::conv::Convolution

#endif // CEDAR_AUX_CONV_CONVOLUTION_H
