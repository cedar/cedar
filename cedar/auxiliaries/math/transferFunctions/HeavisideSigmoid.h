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
#include "cedar/auxiliaries/math/Sigmoid.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/math/transferFunctions/HeavisideSigmoid.fwd.h"

// SYSTEM INCLUDES


/*!@brief A sigmoid step function, going from 0.0 to 1.0 when the input reaches or exceeds the threshold.
 *
 *        This class internally calls the free function cedar::aux::math::sigmoidHeaviside.
 *
 *        The equation for this function is:
 *        @f[
 *           \sigma(x) =
 *              \left\{
 *                \begin{array}{ll}
 *                  0:& x < \theta \\
 *                  1:& \text{otherwise}
 *                \end{array}
 *              \right.
 *        @f]
 *        where \f$\theta\f$ is the threshold set for this function.
 */
class cedar::aux::math::HeavisideSigmoid : public cedar::aux::math::Sigmoid
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  HeavisideSigmoid(double threshold = 0.0)
  :
  cedar::aux::math::Sigmoid(threshold)
  {
    mThreshold->markAdvanced(false); // make users aware that the threshold is important 
                                     // since a common mistake is to forget that Heaviside(0) = 1 in this model
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief this function calculates the Heaviside function for a given double value.
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

#endif  // CEDAR_AUX_MATH_HEAVISIDE_SIGMOID_H
