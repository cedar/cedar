/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Matrix.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 12 11

    Description: Header file for the class cedar::proc::typecheck::Matrix.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_TYPECHECK_MATRIX_H
#define CEDAR_PROC_TYPECHECK_MATRIX_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/TypeCheck.h"

// FORWARD DECLARATIONS
#include "cedar/processing/typecheck/Matrix.fwd.h"

// SYSTEM INCLUDES
#include <vector>


/*!@brief Typecheck that can perform various checks on a matrix.
 *
 *        Commonly, this is intended to test for combinations of matrix attributes, e.g., certain types and
 *        dimensionalities.
 */
class cedar::proc::typecheck::Matrix : public cedar::proc::typecheck::TypeCheck
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  Matrix();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Add an accepted dimensionality. By default, all dimensionalities are accepted.
  void addAcceptedDimensionality(unsigned int dimensionality);

  //! Adds a range of dimensionalities to be accepted by the check.
  void addAcceptedDimensionalityRange(unsigned int lowest, unsigned int highest);

  //! Add an accepted matrix type. By default, all dimensionalities are accepted.
  void addAcceptedType(int type);

  //! Sets whether the matrix can be empty
  void acceptsEmptyMatrix(bool accepts);

  //! Performs the actual check.
  cedar::proc::DataSlot::VALIDITY check(cedar::proc::ConstDataSlotPtr, cedar::aux::ConstDataPtr data) const;

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
private:
  std::vector<unsigned int> mAcceptedDimensionalities;

  std::vector<int> mAcceptedTypes;

  bool mAcceptsEmptyMatrix;

}; // class cedar::proc::typecheck::Matrix

#endif // CEDAR_PROC_TYPECHECK_MATRIX_H

