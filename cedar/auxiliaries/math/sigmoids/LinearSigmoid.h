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

    File:        LinearSigmoid.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 05 16

    Description: Sigmoid functions

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MATH_LINEAR_SIGMOID_H
#define CEDAR_AUX_MATH_LINEAR_SIGMOID_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/namespace.h"
#include "cedar/auxiliaries/math/Sigmoid.h"

// SYSTEM INCLUDES

/*!@brief Sigmoid function that is linear, i.e., multiplies the values with a scalar.
 */
class cedar::aux::math::LinearSigmoid : public cedar::aux::math::Sigmoid
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  LinearSigmoid(double threshold = 0.0, double beta = 1.0);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief this function calculates the abs-based sigmoid function for a given double value.
   */
  virtual double compute(double value) const;

  /*!@brief Returns the current beta value.
   */
  inline double getBeta() const
  {
    return this->_mBeta->getValue();
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
  //!@brief Scalar with which all values are multiplied.
  cedar::aux::DoubleParameterPtr _mBeta;
};

#endif  // CEDAR_AUX_MATH_LINEAR_SIGMOID_H
