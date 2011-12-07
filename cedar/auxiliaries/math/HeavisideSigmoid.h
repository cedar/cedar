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

    File:        HeavysideSigmoid.h

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
#ifndef CEDAR_AUX_MATH_HEAVISIDE_SIGMOID_H
#define CEDAR_AUX_MATH_HEAVISIDE_SIGMOID_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/namespace.h"
#include "cedar/auxiliaries/math/sigmoids.h"
#include "cedar/auxiliaries/math/Sigmoid.h"

// SYSTEM INCLUDES

/*!@brief A sigmoid step function, going from 0.0 to 1.0 when the input reaches or exceeds the threshold.
 *
 * This class internally calls the free function cedar::aux::math::sigmoidHeaviside.
 */
class cedar::aux::math::HeavisideSigmoid : public cedar::aux::math::Sigmoid
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  HeavisideSigmoid(double threshold)
  :
  cedar::aux::math::Sigmoid(threshold)
  {
  }

  //!@brief Destructor
  virtual ~HeavisideSigmoid()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief this function calculates the Heaviside function for a given double value.
   */
  virtual double compute(double value) const
  {
    return cedar::aux::math::sigmoidHeaviside(value, mThreshold->getValue());
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
};


#endif  // CEDAR_AUX_MATH_HEAVISIDE_SIGMOID_H
