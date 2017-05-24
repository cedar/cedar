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

    File:        ExpSigmoid.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 05

    Description: Sigmoid function

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MATH_EXP_SIGMOID_H
#define CEDAR_AUX_MATH_EXP_SIGMOID_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/Sigmoid.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/math/transferFunctions/ExpSigmoid.fwd.h"

// SYSTEM INCLUDES


/*!@brief Sigmoid function that is based on the exponential function.
 *
 *        The equation for this sigmoid is:
 *        @f[
 *           \sigma(x) = \frac{1}{1 + e^{-\beta \cdot (x - \theta)}}
 *        @f]
 *        where \f$\theta\f$ is the threshold set for this function and \f$\beta\f$ is the steepness of the sigmoid.
 */
class cedar::aux::math::ExpSigmoid : public cedar::aux::math::Sigmoid
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ExpSigmoid(double threshold = 0.0, double beta = 100.0);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief this function calculates the exp-based sigmoid function for a given double value.
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
  //!@brief steepness of the exp-sigmoid
  cedar::aux::DoubleParameterPtr _mBeta;

private:
  // none yet
};

#endif  // CEDAR_AUX_MATH_EXP_SIGMOID_H
