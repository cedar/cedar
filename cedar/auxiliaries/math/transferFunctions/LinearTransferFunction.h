/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/auxiliaries/math/transferFunctions/namespace.h"
#include "cedar/auxiliaries/math/TransferFunction.h"

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
};

#endif  // CEDAR_AUX_MATH_LINEAR_TRANSFER_FUNCTION_H
