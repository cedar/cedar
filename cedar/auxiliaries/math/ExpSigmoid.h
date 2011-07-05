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

// LOCAL INCLUDES
#include "auxiliaries/math/namespace.h"
#include "auxiliaries/math/sigmoids.h"
#include "auxiliaries/math/Sigmoid.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

/*!@brief Abstract description of the sigmoid base class.
 *
 * More detailed description of the sigmoid base class.
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
  ExpSigmoid(double beta, double threshold = 0)
  :
  cedar::aux::math::Sigmoid(threshold),
  mBeta(beta)
  {
  }

  //!@brief Destructor
  virtual ~ExpSigmoid()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  virtual double compute(double value)
  {
    return cedar::aux::math::sigmoid(value, mBeta, mThreshold);
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
  double mBeta;
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

};


#endif  // CEDAR_AUX_MATH_EXP_SIGMOID_H
