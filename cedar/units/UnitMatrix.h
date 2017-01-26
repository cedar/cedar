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

    File:        UnitMatrix.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2013 02 14

    Description: Holds a cv::Mat and the unit in which the values in this matrix are expressed.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_UNITS_UNIT_MATRIX_H
#define CEDAR_UNITS_UNIT_MATRIX_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/units/UnitMatrix.fwd.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#ifndef Q_MOC_RUN
  #include <boost/units/quantity.hpp>
  #include <boost/units/io.hpp>
#endif // Q_MOC_RUN
#include <ostream>

//!@brief Holds a cv::Mat and the unit in which the values in this matrix are expressed.
template<typename DimensionType>
struct cedar::unit::UnitMatrix
{
private:
  typedef cedar::unit::UnitMatrix<DimensionType> SelfType;
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief The standard constructor.
  //!@todo It is unclear here, to what unit will be initialized.
  UnitMatrix()
  {
  }

  //! Constructor that takes a matrix and a unit
  UnitMatrix(const cv::Mat& matrix, const boost::units::quantity<DimensionType>& unit)
  :
  matrix(matrix),
  unit(unit)
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // methods
  //--------------------------------------------------------------------------------------------------------------------

  //! Returns a deep copy of the unit matrix.
  SelfType clone() const
  {
    return SelfType(this->matrix.clone(), this->unit);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
  //! matrix
  cv::Mat matrix;
  //! the unit in which the values of the matrix are expressed
  boost::units::quantity<DimensionType> unit;
}; // struct cedar::units::UnitMatrix

//!@brief stream operator for the UnitMatrix class
template<class DimensionType>
inline std::ostream& operator<<(std::ostream& outStream, const cedar::unit::UnitMatrix<DimensionType>& unitMatrix)
{
  outStream << unitMatrix.matrix;
  outStream << " ";
  outStream << unitMatrix.unit;
  return outStream;
}

#endif // CEDAR_UNITS_UNIT_MATRIX_H

