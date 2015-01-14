/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        BoxInput.h

    Maintainer:  Mathis Richter

    Email:       mathis.richter@ini.rub.de

    Date:        2013 05 03

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_SOURCES_BOX_INPUT_H
#define CEDAR_PROC_SOURCES_BOX_INPUT_H

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/processing/sources/BoxInput.fwd.h"

// SYSTEM INCLUDES


/*!@brief Generates a matrix with a box input at a specified position, amplitude, and extent.
 */
class cedar::proc::sources::BoxInput : public cedar::proc::Step
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
  BoxInput();

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Sets the left edge of the box in the given dimension.
  void setLeftBound(unsigned int dimension, unsigned int leftBound);

  //!@brief Sets the amplitude of the box input.
  void setAmplitude(double amplitude);

  //!@brief Returns the amplitude of the box input.
  double getAmplitude() const;

  //!@brief Sets the reference level of the box input.
  void setReferenceLevel(double referenceLevel);

  //!@brief Returns the reference level of the box input.
  double getReferenceLevel() const;

  //!@brief Sets the width of the box input.
  void setWidth(unsigned int dimension, unsigned int width);

public slots:
  //!@brief a slot that is triggered if any of the box function parameters are changed
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
  //!@brief refreshes the internal matrix containing the box input
  void compute(const cedar::proc::Arguments& arguments);

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
  //!@brief the amplitude of the box function
  cedar::aux::DoubleParameterPtr _mAmplitude;
  //!@brief the reference level of the box function
  cedar::aux::DoubleParameterPtr _mReferenceLevel;
  //!@brief the dimensionality of the box function
  cedar::aux::UIntParameterPtr _mDimensionality;
  //!@brief the vector of widths of the box function (one for each dimension)
  cedar::aux::UIntVectorParameterPtr _mWidths;
  //!@brief the vector of left bounds of the box function
  cedar::aux::UIntVectorParameterPtr _mLeftBounds;
  //!@brief the vector of sizes of matrix containing the box function for each dimension
  cedar::aux::UIntVectorParameterPtr _mSizes;

private:

}; // class cedar::proc::source::BoxInput

#endif // CEDAR_PROC_SOURCES_BOX_INPUT_H
