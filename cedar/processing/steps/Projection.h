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

    File:        Projection.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2011 10 31

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_PROJECTION_H
#define CEDAR_PROC_STEPS_PROJECTION_H

// LOCAL INCLUDES
#include "cedar/processing/steps/namespace.h"

// PROJECT INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/NumericParameter.h"
#include "cedar/auxiliaries/NumericVectorParameter.h"

// SYSTEM INCLUDES


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::steps::Projection : public cedar::proc::Step
{
private:
  typedef void (cedar::proc::steps::Projection::*ProjectionFunctionPtr)();

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Projection();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief refreshes the internal matrix containing the input multiplied with the gain factor
  void compute(const cedar::proc::Arguments& arguments);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  cedar::proc::DataSlot::VALIDITY determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr slot,
                                  cedar::aux::DataPtr data
                                ) const;

protected slots:
  //!@brief a slot that is triggered if the gain factor is changed
  void reconfigure();
  void inputConnectionChanged(const std::string& inputName);
  void inputDimensionalityChanged();
  void outputDimensionalityChanged();
  void inputDimensionSizesChanged();
  void outputDimensionSizesChanged();
  void initializeInputMatrix();
  void initializeOutputMatrix();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void expand0D();
  void expandND();
  void compressNDto0D();
  void compress3Dto2D();
  void compress3Dto1D();
  void compress2Dto1D();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief MatrixData representing the input.
  cedar::aux::MatDataPtr mInput;
  //!@brief the buffer containing the output
  cedar::aux::MatDataPtr mOutput;

private:
  ProjectionFunctionPtr mpProjectionMethod;

  std::vector<unsigned int> mIndicesToCompress;
  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief mapping between the input and output dimensions
  //! each index i of the vector represents the corresponding dimension in the input,
  //! the value at index i represents the corresponding dimension in the output
  cedar::aux::UIntVectorParameterPtr _mDimensionMappings;

  //!@brief dimensionality of the input
  //!@todo determine when an input step is connected to the projection
  cedar::aux::UIntParameterPtr _mInputDimensionality;
  //!@brief dimensionality of the output
  //!@todo determine when the projection is connected to an output step
  cedar::aux::UIntParameterPtr _mOutputDimensionality;

  cedar::aux::UIntVectorParameterPtr _mInputDimensionSizes;
  cedar::aux::UIntVectorParameterPtr _mOutputDimensionSizes;
  cedar::aux::UIntParameterPtr _mCompressionType;
}; // class cedar::proc::steps::Projection

#endif // CEDAR_PROC_STEPS_PROJECTION_H

