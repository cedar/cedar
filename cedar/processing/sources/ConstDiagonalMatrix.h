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

    File:        ConstDiagonalMatrix.h

    Maintainer:  
    Email:       
    Date:        

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_SOURCES_CONST_DIAGONAL_MATRIX_H
#define CEDAR_PROC_SOURCES_CONST_DIAGONAL_MATRIX_H

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/processing/sources/ConstDiagonalMatrix.fwd.h"

// SYSTEM INCLUDES


/*!@brief Generates a matrix with a Gaussian.
 *
 *        The output matrix will contain values of a Gauss function, sampled based on the indices of the matrix taken as
 *        x,y,... coordinates.
 */
class cedar::proc::sources::ConstDiagonalMatrix : public cedar::proc::Step
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
  ConstDiagonalMatrix();

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

public slots:
  //!@brief a slot that is triggered if any of the Gauss function parameters are changed
  void updateMatrix();

  //!@brief a slot that is triggered if the matrix size is changed
  void updateMatrixSize();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief refreshes the internal matrix containing the Gaussian input
  void compute(const cedar::proc::Arguments& arguments);

  void calculateOutput();

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
  //!@brief the vector of sizes of matrix containing the Gauss function for each dimension
  cedar::aux::UIntVectorParameterPtr _mSizes;

  cedar::aux::DoubleParameterPtr _mConst;

private:

}; // class cedar::proc::source::ConstDiagonalMatrix

#endif // CEDAR_PROC_SOURCES_CONST_DIAGONAL_MATRIX_H
