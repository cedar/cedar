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

    File:        VariableGauss.h

    Maintainer:  

    Email:       

    Date:        

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_SOURCES_VARIABLE_GAUSS_H
#define CEDAR_PROC_SOURCES_VARIABLE_GAUSS_H

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/processing/steps/VariableGauss.fwd.h"

// SYSTEM INCLUDES


/*!@brief Generates a matrix with a box input at a specified position, amplitude, and extent.
 */
class cedar::proc::steps::VariableGauss : public cedar::proc::Step
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
  VariableGauss();

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
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

  void inputConnectionChanged(const std::string& inputName);

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

  void recompute();

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
  //!@brief the dimensionality of the box function
  cedar::aux::UIntParameterPtr _mDimensionality;
  //!@brief the vector of sizes of matrix containing the box function for each dimension
  cedar::aux::UIntVectorParameterPtr _mSizes;
  //!@brief the amplitude of the box function
  cedar::aux::DoubleParameterPtr _mAmplitude;
  //!@brief the vector of sigmas (i.e. width) of the Gauss function
  cedar::aux::DoubleVectorParameterPtr _mSigmas;

  //!@brief the reference level of the box function
  cedar::aux::DoubleParameterPtr _mReferenceLevel;

  //!@brief the vector of left bounds of the box function
  //cedar::aux::UIntVectorParameterPtr _mLeftBounds;
  cedar::aux::ConstMatDataPtr mInput;

private:

}; // class cedar::proc::source::VariableGauss

#endif // CEDAR_PROC_SOURCES_VARIABLE_GAUSS_H
