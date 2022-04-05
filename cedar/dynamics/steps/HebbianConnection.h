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

    File:        HebbianConnection.h

    Maintainer:  Jan Tekülve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2016 10 28

    Description: Header file for the class cedar::dyn::steps::HebbianConnection.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DYN_HEBBIAN_CONNECTION_H
#define CEDAR_DYN_HEBBIAN_CONNECTION_H

// CEDAR INCLUDES
#include "cedar/dynamics/Dynamics.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/math/functions.h"
#include "cedar/processing/steps/TransferFunction.h"
//#include <boost/enable_shared_from_this.hpp>

#include "HebbianConnection.fwd.h"

// SYSTEM INCLUDES

/*!@brief   This is a static gain step that allows multiplying a matrix input with a factor.
 *
 *          This step implements the cedar::proc::Step interface and allows a user to easily multiply any matrix with a
 *          gain factor. This step is called a static gain step because the gain factor is determined by a parameter,
 *          rather than being an input that might change over time.
 *
 * @remarks This step declares the following interface:
 *          input - any matrix data
 *          output - the output, i.e., input multiplied by gainFactor
 *
 *          Parameters of the step are:
 *          gainFactor - the gain factor.
 */
class cedar::dyn::steps::HebbianConnection : public cedar::dyn::Dynamics
//public boost::enable_shared_from_this<cedar::dyn::steps::HebbianConnection>
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  typedef cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction> SigmoidParameter;
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(SigmoidParameter);

public:
    //!@brief Enum class for Learnrules
    class LearningRule
    {
    public:
        //! the id of an enum entry
        typedef cedar::aux::EnumId Id;

        //! constructs the enum for all ids
        static void construct()
        {
          mType.type()->def(cedar::aux::Enum(OJA, "Oja's Rule"));
          mType.type()->def(cedar::aux::Enum(BCM, "BCM Rule"));
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
        //! flag for using Oja's rule
        static const Id OJA = 0;
        //! flag for using the BCM Rule adapted by Law and Cooper, 1994
        static const Id BCM = 1;

    private:
        static cedar::aux::EnumType<LearningRule> mType;
    };


  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  HebbianConnection();


  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
//  unsigned int getDimensionality();
//  cv::Mat getSizes();
  std::string getOutputName();
  std::string getAssoInputName();
  std::string getRewardInputName();
  std::string getReadOutInputName();
  std::string getTriggerOutputName();
  void setDimensionality(unsigned int dim);
  void setSize(unsigned int dim, unsigned int size);
  void setWeights(cv::Mat newWeights);

public slots:
  //!@brief This slot is connected to the valueChanged() event of the gain value parameter.
 void updateAssociationDimension();
 void updateInputDimension();
 void resetWeights();
 void toggleUseReward();
 void toggleUseManualWeights();
 void updateLearningRule();
 void toggleFixedTheta();
 void applyFixedTheta();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);

  cv::Mat initializeWeightMatrix();

  cv::Mat initializeThetaMatrix();

  cv::Mat calculateDefaultOutput();

  //!@brief Updates the output matrix.
  void eulerStep(const cedar::unit::Time& time);

  cedar::proc::DataSlot::VALIDITY determineInputValidity(
      cedar::proc::ConstDataSlotPtr slot,
      cedar::aux::ConstDataPtr data) const;

  void reset();

  unsigned int determineWeightDimension();

  unsigned int determineWeightSizes(unsigned int dimension);

  cv::Mat calculateWeightChange(const cedar::unit::Time& delta_t, cv::Mat inputActivation,cv::Mat associationActivation, cv::Mat rewardValue);

  cv::Mat calculateOutputMatrix( cv::Mat mat);


  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  cedar::aux::EnumParameterPtr mLearningRule;
  cedar::aux::UIntParameterPtr mInputDimension;
  cedar::aux::UIntVectorParameterPtr mInputSizes;
  cedar::aux::UIntParameterPtr mAssociationDimension;
  cedar::aux::UIntVectorParameterPtr mAssociationSizes;
  cedar::aux::DoubleParameterPtr mLearnRatePositive;
  cedar::aux::DoubleParameterPtr mTauWeights;
  cedar::aux::DoubleParameterPtr mTauTheta;
  cedar::aux::DoubleParameterPtr mMinThetaValue;
  cedar::aux::BoolParameterPtr  mUseFixedTheta;
  cedar::aux::DoubleParameterPtr mFixedThetaValue;
  SigmoidParameterPtr mSigmoidF;
  SigmoidParameterPtr mSigmoidG;
  SigmoidParameterPtr mSigmoidH;
  cedar::aux::BoolParameterPtr mUseRewardDuration;
  cedar::aux::DoubleParameterPtr mRewardDuration;
  cedar::aux::BoolParameterPtr mSetWeights;
  cedar::aux::DoubleVectorParameterPtr mWeightCenters;
  cedar::aux::DoubleVectorParameterPtr mWeightSigmas;
  cedar::aux::DoubleParameterPtr mWeightAmplitude;
  cedar::aux::DoubleParameterPtr mWeightInitBase;
  cedar::aux::DoubleParameterPtr mWeightInitNoiseRange;


private:
  cedar::aux::ConfigurablePtr mBcmParameterGroup;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The data containing the output.
  cedar::aux::MatDataPtr mConnectionWeights;
  cedar::aux::MatDataPtr mWeightOutput;
  cedar::aux::MatDataPtr mBCMTheta;

private:
  std::string mAssoInputName = "target field";
  std::string mOutputName = "weights";
  std::string mTriggerOutputName = "learned output";
  std::string mRewardInputName = "reward signal";
  std::string mReadOutInputName= "source node";

  std::string mThetaMatrixName = "bcm theta";
  std::string mThetaCounterName  = "theta counter";
  bool mIsRewarded = false;
  int mElapsedTime = 0;

  cedar::aux::ConstMatDataPtr mAssoInput;
  cedar::aux::ConstMatDataPtr mRewardTrigger;
  cedar::aux::ConstMatDataPtr mReadOutTrigger;
  unsigned int mWeightSizeX;
  unsigned int mWeightSizeY;


};// class cedar::dyn::steps::ImprintHebb



#endif // CEDAR_DYN_HEBBIAN_CONNECTION_H

