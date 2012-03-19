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
 * @remarks The choice of convolution engine should not affect the outcome of the convolution, only the speed thereof.
 *
 * @see     cedar::aux::conv::Engine, cedar::aux::conv::BorderType, cedar::aux::conv::Mode,
 *          cedar::aux::conv::KernelList, cedar::aux::conv
 *
 * @todo    Move the anchor to the kernel base class
 *
 * @todo    Provide an interface to all the conv::Engine functions.
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
   */
  inline cv::Mat convolve(const cv::Mat& matrix) const
  {
    return this->getEngine()->convolve(matrix, this->getBorderType(), this->getMode());
  }

  //!@brief The convolution functor for a single matrix.
  cv::Mat operator()(const cv::Mat& matrix) const;

  //!@brief Method for accessing the kernel list.
  inline cedar::aux::conv::KernelList& getKernelList()
  {
    return this->getEngine()->getKernelList();
  }

  //!@brief Constant variant of the method for accessing the kernel list.
  inline const cedar::aux::conv::KernelList& getKernelList() const
  {
    return this->getEngine()->getKernelList();
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

  inline cedar::aux::conv::Mode::Id getMode() const
  {
    return this->_mMode->getValue();
  }

  //!@brief Returns a pointer to the combined kernel.
  inline cedar::aux::MatDataPtr getCombinedKernel()
  {
    return this->mCombinedKernel;
  }

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

  void slotKernelAdded(size_t);
  void slotKernelChanged(size_t);
  void slotKernelRemoved(size_t);

private slots:
  void selectedEngineChanged();

  void updateCombinedKernel();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::aux::MatDataPtr mCombinedKernel;

  boost::signals2::connection mKernelAddedConnection;

  boost::signals2::connection mKernelChangedConnection;

  boost::signals2::connection mKernelRemovedConnection;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::aux::EnumParameterPtr _mBorderType;

  cedar::aux::EnumParameterPtr _mMode;

  cedar::aux::conv::EngineParameterPtr _mEngine;

}; // cedar::aux::conv::Convolution

#endif // CEDAR_AUX_CONV_CONVOLUTION_H
