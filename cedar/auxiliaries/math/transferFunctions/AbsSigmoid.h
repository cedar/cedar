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

    File:        AbsSigmoid.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 05

    Description: Sigmoid functions

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MATH_ABS_SIGMOID_H
#define CEDAR_AUX_MATH_ABS_SIGMOID_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/Sigmoid.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/math/transferFunctions/AbsSigmoid.fwd.h"

// SYSTEM INCLUDES

/*!@brief Sigmoid function that is based on absolute values.
 *
 *        This function behaves similar to cedar::aux::math::ExpSigmoid, but computing it is less costly.
 *
 *        The equation for this sigmoid is:
 *        @f[
 *           \sigma(x) = \frac{1}{2} \left(1 + \frac{\beta (x - \theta)}{1 + \beta |x - \theta|}\right)
 *        @f]
 *        where \f$\theta\f$ is the threshold set for this function and \f$\beta\f$ is the steepness of the sigmoid.
 */
class cedar::aux::math::AbsSigmoid : public cedar::aux::math::Sigmoid
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  AbsSigmoid(double threshold = 0.0, double beta = 100.0);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief this function calculates the abs-based sigmoid function for a given double value.
   */
  virtual double compute(double value) const;

  //! Overriden for efficiency.
  virtual cv::Mat compute(const cv::Mat& values) const;

  //! Returns the beta (slope) of the sigmoid.
  inline double getBeta() const
  {
    return this->_mBeta->getValue();
  }

  //! Sets the beta (slope) of the sigmoid.
  inline void setBeta(double beta)
  {
    this->_mBeta->setValue(beta);
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
  //!@brief steepness of the abs-sigmoid
  cedar::aux::DoubleParameterPtr _mBeta;
private:
  // none yet
};

#endif  // CEDAR_AUX_MATH_ABS_SIGMOID_H
