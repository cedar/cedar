/*
 * WeightedSum.h
 *
 *  Created on: Mar 10, 2014
 *      Author: Jan Tekuelve
 */

#ifndef WEIGHTEDSUM_H
#define WEIGHTEDSUM_H

#include "cedar/processing/steps/WeightedSum.fwd.h"

#include <cedar/processing/Step.h> // if we are going to inherit from cedar::proc::Step, we have to include the header
#include <cedar/processing/ExternalData.h> // getInputSlot() returns ExternalData
#include <cedar/processing/typecheck/DerivedFrom.h>
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include <vector>
#include <iostream>
#include <fstream>

//forward includes
#include "cedar/auxiliaries/MatData.fwd.h"

class cedar::proc::steps::WeightedSum : public cedar::proc::Step
{

  //	//--------------------------------------------------------------------------------------------------------------------
  //	// macros
  //	//--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

public:
  //!@brief Enum class for FunctionTypes
  class FunctionType
  {
  public:
    //! the id of an enum entry
    typedef cedar::aux::EnumId Id;

    //! constructs the enum for all ids
    static void construct()
    {
      mType.type()->def(cedar::aux::Enum(Linear, "LINEAR"));
      mType.type()->def(cedar::aux::Enum(Noisy, "NOISY"));
    }

    //! @returns A const reference to the base enum object.
    static const cedar::aux::EnumBase& type()
    {
      return *(mType.type());
    }

    //! @returns A pointer to the base enum object.
    static const cedar::proc::DataRole::TypePtr& typePtr()
    {
      return mType.type();
    }

  public:
    //! flag for using linear weight relations with zero at the center
    static const Id Linear = 0;
    //! flag for linear weights with a noise term at the outskirts
    static const Id Noisy = 1;

  private:
    static cedar::aux::EnumType<FunctionType> mType;
  };

public:
  WeightedSum();
  virtual ~WeightedSum();

  void setMaxWeightValues(unsigned int dimension, double weightValue);

public slots:
  //!@brief a slot that is triggered if the matrix size is changed
  void updateMatrixSize();
  void updateFunctionType();
  void updateDimensionality();
private:
  cedar::proc::DataSlot::VALIDITY determineInputValidity(
      cedar::proc::ConstDataSlotPtr slot,
      cedar::aux::ConstDataPtr data) const;
  //!@brief Resets the field.
  void reset();
  void compute(const cedar::proc::Arguments& arguments);
  void initAllWeights();
  void initWeights(cedar::aux::MatDataPtr weightPtr, int index);
private:
  cedar::aux::MatDataPtr mOutput;
  cedar::aux::MatDataPtr mOutputTangential;
  cedar::aux::MatDataPtr mWeightsX;
  cedar::aux::MatDataPtr mWeightsY;

  cedar::aux::UIntParameterPtr _mDimensionality;
  cedar::aux::UIntVectorParameterPtr _mSizes;
  cedar::aux::DoubleVectorParameterPtr _maxWeightValues;
  cedar::aux::EnumParameterPtr _mFunctionType;
  cedar::aux::DoubleParameterPtr _mFunctionParameter1;
  cedar::aux::DoubleParameterPtr _mFunctionParameter2;
  cedar::aux::DoubleParameterPtr _mFunctionParameter3;
};

#endif /* WEIGHTEDSUM_H */
