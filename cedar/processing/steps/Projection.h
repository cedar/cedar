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

// CEDAR INCLUDES
#include "cedar/processing/steps/namespace.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/ProjectionMappingParameter.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/NumericParameter.h"
#include "cedar/auxiliaries/NumericVectorParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"

// SYSTEM INCLUDES


/*!@brief Processing step, which projects neuronal activation between processing steps of different dimensionality.
 *
 * This processing step can be set up to project the output activation of a step A onto step B, where A and B may have
 * a different dimensionality. The projection supports the reduction of dimensionality (A.dims() > B.dims()), the
 * expansion (A.dims() < B.dims()) and also permutation of dimensions (e.g., for A.dims() == B.dims()).
 */
class cedar::proc::steps::Projection : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! typedef for the projection method function pointer
  //! (function pointer to a void method in cedar::proc::steps::Projection)
  typedef void (cedar::proc::steps::Projection::*ProjectionFunctionPtr)();

public:
  /*!@brief The type of compression used by the projection.
   */
  class CompressionType
  {
  public:
    //! Type of the enum's identifiers.
    typedef cedar::aux::EnumId Id;

    //! Pointer to the enum's base type.
    typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

    /*!@brief Static construction method that defines all values.
     */
    static void construct()
    {
      mType.type()->def(cedar::aux::Enum(SUM, "SUM", "Sum"));
      mType.type()->def(cedar::aux::Enum(AVERAGE, "AVERAGE", "Average"));
      mType.type()->def(cedar::aux::Enum(MAXIMUM, "MAXIMUM", "Maximum"));
      mType.type()->def(cedar::aux::Enum(MINIMUM, "MINIMUM", "Minimum"));
    }

    //!@brief Returns the base object of the enum.
    static const cedar::aux::EnumBase& type()
    {
      return *mType.type();
    }

    //!@brief Returns a pointer to the base object of the enum.
    static const TypePtr& typePtr()
    {
      return mType.type();
    }

  public:

    //! Compression is done by calculating the sum along the compressed direction.
    static const Id SUM = CV_REDUCE_SUM;

    //! Compression is done by averaging along the compressed direction.
    static const Id AVERAGE = CV_REDUCE_AVG;

    //! Compression is done by calculating the maximum along the compressed direction.
    static const Id MAXIMUM = CV_REDUCE_MAX;

    //! Compression is done by calculating the minimum along the compressed direction.
    static const Id MINIMUM = CV_REDUCE_MIN;

  private:
    //!@brief The type object of the enum.
    static cedar::aux::EnumType<CompressionType> mType;
  };

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
  //!@brief check if all inputs are valid, e.g. have the right size and dimensionality
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
  //!@brief expands a 1D input to an 2D output
  void expand1Dto2D();
  //!@brief expands and permutes MD input to ND output (M <= N)
  void expandMDtoND();
  //!@brief compresses ND input to 0D output by computing the minimum over all positions
  void compressNDto0Dmin();
  //!@brief compresses ND input to 0D output by computing the maximum over all positions
  void compressNDto0Dmax();
  //!@brief compresses ND input to 0D output by computing the sum over all positions
  void compressNDto0Dsum();
  //!@brief compresses ND input to 0D output by computing the mean over all positions
  void compressNDto0Dmean();
  //!@brief compresses 3D input to 2D output
  void compress3Dto2D();
  //!@brief compresses 3D input to 1D output
  void compress3Dto1D();
  //!@brief compresses 2D input to 1D output
  void compress2Dto1D();

  //!@brief gets called once by cedar::proc::LoopedTrigger once prior to starting the trigger
  virtual void onStart();
  //!@brief gets called once by cedar::proc::LoopedTrigger after it stops
  virtual void onStop();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief MatrixData representing the input
  cedar::aux::ConstMatDataPtr mInput;
  //!@brief the buffer containing the output
  cedar::aux::MatDataPtr mOutput;

private:
  //!@brief function pointer to one of the projection member functions
  ProjectionFunctionPtr mpProjectionMethod;
  //!@brief vector holding all indices of dimensions that have to be compressed
  //! this is only in use when the projection is set up to compress the dimensionality of the input
  std::vector<unsigned int> mIndicesToCompress;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief mapping between the input and output dimensions
  cedar::proc::ProjectionMappingParameterPtr _mDimensionMappings;

  //!@brief dimensionality of the output
  //!@todo determine when the projection is connected to an output step
  cedar::aux::UIntParameterPtr _mOutputDimensionality;

  //!@brief sizes of all dimensions of the output of the projection
  cedar::aux::UIntVectorParameterPtr _mOutputDimensionSizes;
  //!@brief type of compression used when reducing the dimensionality (maximum, minimum, average, sum)
  cedar::aux::EnumParameterPtr _mCompressionType;
}; // class cedar::proc::steps::Projection

#endif // CEDAR_PROC_STEPS_PROJECTION_H
