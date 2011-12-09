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

    File:        Noise.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 11 10

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DYN_NOISE_H
#define CEDAR_DYN_NOISE_H

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/dynamics/namespace.h"
#include "cedar/dynamics/Dynamics.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"
#include "cedar/auxiliaries/namespace.h"

// SYSTEM INCLUDES


/*!@brief A step that generates noise.
 *
 * @todo More descriptions
 * @todo Does this take the sqrt(dt) problem into account? (may need to divide the output by dt/sqrt(dt))
 */
class cedar::dyn::Noise : public cedar::dyn::Dynamics
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Noise();

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

public slots:
  //!@brief handle a change in dimensionality, which leads to creating new matrices
  void dimensionalityChanged();
  //!@brief handle a change in size along dimensions, which leads to creating new matrices
  void dimensionSizeChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief compute the euler step
   *
   * This is the equation:
   * \f[
   *
   * \f]
   */
  void eulerStep(const cedar::unit::Time& time);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief update the size and dimensionality of internal matrices
  void updateMatrices();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  //!@brief this MatData matrix contains the current random numbers
  cedar::aux::MatDataPtr mRandomMatrix;
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  //!@brief the matrix dimensionality - may range from 1 to 16 in principle, but more like 6 or 7 in reality
  cedar::aux::UIntParameterPtr _mDimensionality; //!@todo not the only class needing this - think about parent class
  //!@brief the matrix sizes in each dimension
  cedar::aux::UIntVectorParameterPtr _mSizes;
  //!@brief the mean of the normal distribution
  cedar::aux::DoubleParameterPtr _mMean;
  //!@brief the standard deviation of the normal distribution
  cedar::aux::DoubleParameterPtr _mStandardDeviation;

private:
  // none yet

}; // class cedar::dyn::Noise

#endif // CEDAR_DYN_NOISE_H

