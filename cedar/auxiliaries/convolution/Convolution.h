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


/*!@brief Base class for convolution engines.
 *
 * @todo The anchor value for the convolution can currently not be set.
 *
 * @todo Maybe the actual engine should be shared/shareable across mutliple convolution objects?
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
  Convolution();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief This method convolves a given matrix with the kernel list stored in this convolution object.
   */
  inline cv::Mat convolve(const cv::Mat& matrix) const
  {
    return this->getEngine()->convolve(matrix, this->getBorderType());
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

  cedar::aux::conv::EngineParameterPtr _mEngine;

}; // cedar::aux::conv::Convolution

#endif // CEDAR_AUX_CONV_CONVOLUTION_H
