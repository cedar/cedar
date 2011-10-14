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

    File:        GaussInput.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 19

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_SOURCE_GAUSS_INPUT_H
#define CEDAR_PROC_SOURCE_GAUSS_INPUT_H

// LOCAL INCLUDES
#include "processing/source/namespace.h"
#include "processing/namespace.h"
#include "dynamics/namespace.h"
#include "processing/Step.h"
#include "auxiliaries/NumericParameter.h"
#include "auxiliaries/NumericVectorParameter.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::source::GaussInput : public cedar::proc::Step
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
  GaussInput();

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief refreshes the internal matrix containing the Gaussian input
  void compute(const cedar::proc::Arguments& arguments);

public slots:
  //!@brief a slot that is triggered if any of the Gauss function parameters are changed
  void updateMatrix();
  //!@brief a slot to process changes in dimensionality, including reinitializing the buffers
  void updateDimensionality();
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief the buffer containing the output
  cedar::aux::MatDataPtr mOutput;
private:

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief the amplitude of the Gauss function
  cedar::aux::DoubleParameterPtr _mAmplitude;
  //!@brief the dimensionality of the Gauss function
  cedar::aux::UIntParameterPtr _mDimensionality;
  //!@brief the vector of sigmas (i.e. width) of the Gauss function
  cedar::aux::DoubleVectorParameterPtr _mSigmas;
  //!@brief the vector of centers of the Gauss function
  cedar::aux::DoubleVectorParameterPtr _mCenters;
  //!@brief the vector of sizes of matrix containing the Gauss function for each dimension
  cedar::aux::UIntVectorParameterPtr _mSizes;

private:

}; // class cedar::proc::source::GaussInput

#endif // CEDAR_PROC_SOURCE_GAUSS_INPUT_H

