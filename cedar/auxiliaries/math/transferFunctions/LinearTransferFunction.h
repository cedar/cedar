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

    File:        LinearTransferFunction.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 05 16

    Description: Sigmoid functions

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MATH_LINEAR_TRANSFER_FUNCTION_H
#define CEDAR_AUX_MATH_LINEAR_TRANSFER_FUNCTION_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/TransferFunction.h"
#include "cedar/auxiliaries/DoubleParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/math/transferFunctions/LinearTransferFunction.fwd.h"

// SYSTEM INCLUDES

/*!@brief Sigmoid function that is linear, i.e., multiplies the values with a scalar.
 *
 *        The equation for this function is:
 *        @f[
 *           \sigma(x) = x
 *        @f]
 *        where \f$\theta\f$ is the threshold set for this function.
 */
class cedar::aux::math::LinearTransferFunction : public cedar::aux::math::TransferFunction
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  LinearTransferFunction();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief this function calculates the abs-based sigmoid function for a given double value.
   */
  virtual double compute(double value) const;

  //! Returns the offset of the linear function, i.e., \f$ m \f$ in \f$ f(x) = m \cdot x + b \f$.
  inline double getFactor() const
  {
    return this->_mFactor->getValue();
  }

  //! Returns the offset of the linear function, i.e., \f$ b \f$ in \f$ f(x) = m \cdot x + b \f$.
  inline double getOffset() const
  {
    return this->_mOffset->getValue();
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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Factor with which the input is multiplied.
  cedar::aux::DoubleParameterPtr _mFactor;

  //! Offset added to the input.
  cedar::aux::DoubleParameterPtr _mOffset;
};

#endif  // CEDAR_AUX_MATH_LINEAR_TRANSFER_FUNCTION_H
