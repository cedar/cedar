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

    Description: Processing step, which projects neuronal activation between processing steps of different
                 dimensionality.

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


/*!@brief Processing step, which projects neuronal activation between processing steps of different dimensionality.
 *
 * This processing step can be set up to project the output activation of a step A onto step B, where A and B may have
 * a different dimensionality. The projection supports the reduction of dimensionality (A.dims() > B.dims()), the
 * expansion (A.dims() < B.dims()) and also permutation of dimensions (e.g., for A.dims() == B.dims()).
 */
class cedar::proc::steps::Projection : public cedar::proc::Step
{
private:
  //! typedef for the projection method function pointer
  //! (function pointer to a void method in cedar::proc::steps::Projection)
  typedef void (cedar::proc::steps::Projection::*ProjectionFunctionPtr)();

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief standard constructor
  Projection();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief computes the projected output whenever this step is triggered
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
  //!@brief this slot is triggered whenever a new connection is connected as an input to the projection step
  void inputConnectionChanged(const std::string& inputName);
  //!@brief this slot is triggered whenever the dimensionality of its output is changed by the user
  void outputDimensionalityChanged();
  //!@brief this slot is triggered whenever the size of any dimension of the output is changed by the user
  void outputDimensionSizesChanged();
  //!@brief chooses the appropriate projection method for the current input and output
  void reconfigure();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief initializes or reconfigures the output matrix
  void initializeOutputMatrix();

  // projection methods

  //!@brief expands a 0D input to an ND output
  void expand0DtoND();
  //!@brief expands and permutes MD input to ND output (M <= N)
  void expandMDtoND();
  //!@brief compresses ND input to 0D output
  void compressNDto0D();
  //!@brief compresses 3D input to 2D output
  void compress3Dto2D();
  //!@brief compresses 3D input to 1D output
  void compress3Dto1D();
  //!@brief compresses 2D input to 1D output
  void compress2Dto1D();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief MatrixData representing the input
  cedar::aux::MatDataPtr mInput;
  //!@brief the buffer containing the output
  cedar::aux::MatDataPtr mOutput;

private:
  //!@brief function pointer to one of the projection member functions
  ProjectionFunctionPtr mpProjectionMethod;
  //!@brief vector holding all indices of dimensions that have to be compressed
  //! this is only in use when the projection is set up to compress the dimensionality of the input
  std::vector<unsigned int> mIndicesToCompress;
  //!@brief dimensionality of the input
  //! this is determined when another step is connected to the input of the projection
  unsigned int mInputDimensionality;

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

  //!@brief dimensionality of the output
  //!@todo determine when the projection is connected to an output step
  cedar::aux::UIntParameterPtr _mOutputDimensionality;

  //!@brief sizes of all dimensions of the output of the projection
  cedar::aux::UIntVectorParameterPtr _mOutputDimensionSizes;
  //!@brief type of compression used when reducing the dimensionality (maximum, minimum, average, sum)
  cedar::aux::UIntParameterPtr _mCompressionType;
}; // class cedar::proc::steps::Projection

#endif // CEDAR_PROC_STEPS_PROJECTION_H

