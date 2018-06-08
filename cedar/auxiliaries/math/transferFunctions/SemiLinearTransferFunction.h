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

    File:        SemiLinearTransferFunction.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 05 16

    Description: Sigmoid functions

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MATH_SEMI_LINEAR_TRANSFER_FUNCTION_H
#define CEDAR_AUX_MATH_SEMI_LINEAR_TRANSFER_FUNCTION_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/TransferFunction.h"
#include "cedar/auxiliaries/DoubleParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/math/transferFunctions/SemiLinearTransferFunction.fwd.h"

// SYSTEM INCLUDES

/*!@brief Transfer function that is linear above a given threshold.
 *
 *        The equation for this function is:
 *        @f[
 *           \sigma(x) =
 *              \left\{
 *                \begin{array}{ll}
 *                  \theta: & x < \theta \\
 *                  \theta + \beta \cdot (x - \theta): & \text{otherwise}
 *                \end{array}
 *              \right.
 *        @f]
 *        where \f$\theta\f$ is the threshold set for this function.
 */
class cedar::aux::math::SemiLinearTransferFunction : public cedar::aux::math::TransferFunction
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  SemiLinearTransferFunction(double threshold = 0.0, double beta = 1.0);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief this function calculates the abs-based sigmoid function for a given double value.
   */
  virtual double compute(double value) const;

  cv::Mat compute(const cv::Mat& values) const;

  /*!@brief Returns the current beta value.
   */
  inline double getBeta() const
  {
    return this->_mBeta->getValue();
  }

  /*! Returns the current threshold value.
   */
  inline double getThreshold() const
  {
    return this->_mThreshold->getValue();
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
  //!@brief Threshold.
  cedar::aux::DoubleParameterPtr _mThreshold;

  //!@brief Steepness of the linear part.
  cedar::aux::DoubleParameterPtr _mBeta;
};

#endif  // CEDAR_AUX_MATH_SEMI_LINEAR_TRANSFER_FUNCTION_H
