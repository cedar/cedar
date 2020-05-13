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

 File:        HebbianConnection.cpp

 Maintainer:  Jan Tekülve
 Email:       jan.tekuelve@ini.rub.de
 Date:        2016 10 28

 Description: Source file for the class cedar::dyn::steps::HebbianConnection.

 Credits:

 ======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "HebbianConnection.h"

#include "cedar/processing/typecheck/IsMatrix.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"
#include "cedar/auxiliaries/math/Sigmoid.h"
#include "cedar/auxiliaries/math/transferFunctions/AbsSigmoid.h"
#include "cedar/auxiliaries/math/transferFunctions/HeavisideSigmoid.h"
#include "cedar/auxiliaries/math/transferFunctions/LinearTransferFunction.h"

// SYSTEM INCLUDES
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>


//Declaration

namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration
            (
                    new ElementDeclarationTemplate<cedar::dyn::steps::HebbianConnection>
                            (
                                    "DFT",
                                    "cedar.dynamics.HebbianConnection"
                            )
            );
    declaration->setIconPath(":/steps/hebbian_connection.svg");
    declaration->setDescription
            (
                    "A looped step that learns a projection between a node and a field according to a Hebbian rule.\nweightchange = learnrate * f(source)*g(reward)*(h(target)-weights)"
            );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dyn::steps::HebbianConnection::HebbianConnection()
    :
      // parameters
      mInputDimension(new cedar::aux::UIntParameter(this, "source dimension", 0, 0, 2)),
      mInputSizes(new cedar::aux::UIntVectorParameter(this, "source sizes", 0, 50,cedar::aux::UIntParameter::LimitType::positive(5000))),
      mAssociationDimension(new cedar::aux::UIntParameter(this, "target dimension", 2, 0, 2)),
      mAssociationSizes(new cedar::aux::UIntVectorParameter(this, "target sizes", 2, 50,cedar::aux::UIntParameter::LimitType::positive(5000))),
      mLearnRatePositive(new cedar::aux::DoubleParameter(this, "learning rate", 0.01)),
      mSigmoidF
              (
                      new cedar::dyn::steps::HebbianConnection::SigmoidParameter
                              (
                                      this,
                                      "f",
                                      cedar::aux::math::SigmoidPtr(new cedar::aux::math::HeavisideSigmoid(0.5))
                              )
              ),
      mSigmoidG
              (
                      new cedar::dyn::steps::HebbianConnection::SigmoidParameter
                              (
                                      this,
                                      "g",
                                      cedar::aux::math::SigmoidPtr(new cedar::aux::math::HeavisideSigmoid(0.5))
                              )
              ),
      mSigmoidH
              (
                      new cedar::dyn::steps::HebbianConnection::SigmoidParameter
                              (
                                      this,
                                      "h",
                                      cedar::aux::math::LinearTransferFunctionPtr(new cedar::aux::math::LinearTransferFunction())
                              )
              ),
      mUseRewardDuration(new cedar::aux::BoolParameter(this, "fixed reward duration", false)),
      mRewardDuration(new cedar::aux::DoubleParameter(this, "reward duration", 200)),
      mSetWeights(new cedar::aux::BoolParameter(this, "manual weights", false)),
      mWeightCenters(new cedar::aux::DoubleVectorParameter(this, "weight centers", mAssociationDimension->getValue(), 3)),
      mWeightSigmas(new cedar::aux::DoubleVectorParameter(this, "weight sigmas", mAssociationDimension->getValue(), 3)),
      mWeightAmplitude(new cedar::aux::DoubleParameter(this, "weight amplitude", 6)),
      mWeightInitBase(new cedar::aux::DoubleParameter(this, "weight init value", 0)),
      mWeightInitNoiseRange(new cedar::aux::DoubleParameter(this, "weight init noise", 0.001)),

      // outputs
      mConnectionWeights(new cedar::aux::MatData(cv::Mat::zeros(100, 100, CV_32F))),
      mWeightOutput((new cedar::aux::MatData(cv::Mat::zeros(100, 100, CV_32F)))),
//      mRewardTrigger(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
//      mReadOutTrigger(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
      mWeightSizeX(mAssociationDimension->getValue() > 0 ? mAssociationSizes->getValue().at(0) : 1),
      mWeightSizeY(mAssociationDimension->getValue() > 1 ? mAssociationSizes->getValue().at(1) : 1)

{
  //deal with deprecated Names
  this->mInputDimension->addDeprecatedName("input dimension");
  this->mInputSizes->addDeprecatedName("input sizes");
  this->mAssociationDimension->addDeprecatedName("association dimension");
  this->mAssociationSizes->addDeprecatedName("association sizes");

  // declare all data
  cedar::proc::DataSlotPtr assoInput = this->declareInput(mAssoInputName, false);
  cedar::proc::DataSlotPtr rewardInput = this->declareInput(mRewardInputName, false);
  cedar::proc::DataSlotPtr readOutInput = this->declareInput(mReadOutInputName, false);
  auto weightOutput = this->declareBuffer(mOutputName, mConnectionWeights);
  weightOutput->setSerializable(true);
  auto weightTriggerOutput = this->declareOutput(mTriggerOutputName, mWeightOutput);

  this->mConnectionWeights->getData() = initializeWeightMatrix();
  mWeightOutput->setData(mConnectionWeights->getData());

  mRewardDuration->setConstant(!mUseRewardDuration->getValue());
  mWeightAmplitude->setConstant(!mSetWeights->getValue());
  mWeightSigmas->setConstant(!mSetWeights->getValue());
  mWeightCenters->setConstant(!mSetWeights->getValue());

  mWeightInitNoiseRange->markAdvanced(true);
  mWeightInitBase->markAdvanced(true);

  this->registerFunction("reset Weights", boost::bind(&HebbianConnection::resetWeights, this), false);

  QObject::connect(mInputDimension.get(), SIGNAL(valueChanged()), this, SLOT(updateInputDimension()));
  QObject::connect(mAssociationDimension.get(), SIGNAL(valueChanged()), this, SLOT(updateAssociationDimension()));
  QObject::connect(mInputSizes.get(), SIGNAL(valueChanged()), this, SLOT(resetWeights()));
  QObject::connect(mAssociationSizes.get(), SIGNAL(valueChanged()), this, SLOT(resetWeights()));
  QObject::connect(mUseRewardDuration.get(), SIGNAL(valueChanged()), this, SLOT(toggleUseReward()));
  QObject::connect(mSetWeights.get(), SIGNAL(valueChanged()), this, SLOT(toggleUseManualWeights()));
  QObject::connect(mWeightCenters.get(), SIGNAL(valueChanged()), this, SLOT(resetWeights()));
  QObject::connect(mWeightSigmas.get(), SIGNAL(valueChanged()), this, SLOT(resetWeights()));
  QObject::connect(mWeightAmplitude.get(), SIGNAL(valueChanged()), this, SLOT(resetWeights()));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dyn::steps::HebbianConnection::updateAssociationDimension()
{
  int new_dim = static_cast<int>(mAssociationDimension->getValue());
  mAssociationSizes->resize(new_dim, mAssociationSizes->getDefaultValue());


  mWeightCenters->resize(determineWeightDimension(), mWeightCenters->getDefaultValue());
  mWeightSigmas->resize(determineWeightDimension(), mWeightCenters->getDefaultValue());
  mWeightSizeX = determineWeightSizes(0);
  mWeightSizeY = determineWeightSizes(1);

//  std::cout<<"updateAssociationDimension: WeightDim! " << determineWeightDimension() << " WeightX=" << determineWeightSizes(0) << " WeightY=" << determineWeightSizes(1) << std::endl;

  this->resetWeights();
}

void cedar::dyn::steps::HebbianConnection::updateInputDimension()
{
  int new_dim = static_cast<int>(mInputDimension->getValue());
  mInputSizes->resize(new_dim, 50);

  mWeightCenters->resize(determineWeightDimension(), mWeightCenters->getDefaultValue());
  mWeightSigmas->resize(determineWeightDimension(), mWeightCenters->getDefaultValue());

//  std::cout<<"updateInputDimension: WeightDim! " << determineWeightDimension() << " WeightX=" << determineWeightSizes(0) << " WeightY=" << determineWeightSizes(1) << std::endl;

  this->resetWeights();
}

cv::Mat cedar::dyn::steps::HebbianConnection::initializeWeightMatrix()
{
  mWeightSizeX = determineWeightSizes(0);
  mWeightSizeY = determineWeightSizes(1);
  cv::Mat myWeightMat = cv::Mat::zeros(mWeightSizeX, mWeightSizeY, CV_32F);
  if (!mSetWeights->getValue())
  {
    srand(static_cast<unsigned>(time(0)));
    float HIGH = mWeightInitNoiseRange->getValue();
    float LOW = mWeightInitBase->getValue();
    for (unsigned int x = 0; x < mWeightSizeX; x++)
    {
      for (unsigned int y = 0; y < mWeightSizeY; y++)
      {
        float random = LOW + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (HIGH - LOW)));
        myWeightMat.at<float>(x, y) = random;
      }
    }
  }
  else
  {
    std::vector<unsigned int> curSizes;
    curSizes.push_back(mWeightSizeX);
    curSizes.push_back(mWeightSizeY);
    if (mAssociationDimension->getValue() != 0)
    {
      myWeightMat = cedar::aux::math::gaussMatrix(mAssociationDimension->getValue(), curSizes,
                                                  mWeightAmplitude->getValue(), mWeightSigmas->getValue(),
                                                  mWeightCenters->getValue(), true);
    }
    else
    {
      myWeightMat.at<float>(0,0) = mWeightAmplitude->getValue();
    }
  }
  return myWeightMat;
}

void cedar::dyn::steps::HebbianConnection::eulerStep(const cedar::unit::Time& time)
{

  if(mInputDimension->getValue() + mAssociationDimension->getValue() > 2)
  {
    this->setState(cedar::proc::Triggerable::STATE_INITIALIZING, "Current Set of Dimensions is not supported yet!");
    mWeightOutput->setData(cv::Mat::zeros(mWeightSizeX, mWeightSizeY, CV_32F));
    return;
  }

  if (mAssoInput && mRewardTrigger && mReadOutTrigger )
  {
    const cv::Mat& rewardTrigger = mRewardTrigger->getData();
    if (mSigmoidG->getValue()->compute(rewardTrigger.at<float>(0, 0)) > 0) // This is for the RewardDuration Trick. Should be removed in later versions!
    {
      if (!mIsRewarded && mUseRewardDuration->getValue())
      {
        mElapsedTime = 0;
        mIsRewarded = true;
      }
      float curTime = time / cedar::unit::Time(1 * cedar::unit::milli * cedar::unit::seconds);
      mElapsedTime += curTime;

      if (mElapsedTime < mRewardDuration->getValue() || !mUseRewardDuration->getValue())
      {

        cv::Mat currentWeights = mConnectionWeights->getData();
        auto weightChange = calculateWeightChange(mReadOutTrigger->getData(),mAssoInput->getData(),mRewardTrigger->getData());
        currentWeights = currentWeights + weightChange;
        mConnectionWeights->setData(currentWeights);

      }
    }
    else if (mUseRewardDuration->getValue())
    {
      mIsRewarded = false;
    }
  }


  if (mReadOutTrigger)
  {
      mWeightOutput->setData(calculateOutputMatrix(mReadOutTrigger->getData()));
  }
  else
  {
    mWeightOutput->setData(cv::Mat::zeros(mWeightSizeX, mWeightSizeY, CV_32F));
  }
}

void cedar::dyn::steps::HebbianConnection::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == mAssoInputName)
  {
    this->mAssoInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }
  if (inputName == mRewardInputName)
  {
    this->mRewardTrigger = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }
  if (inputName == mReadOutInputName)
  {
    this->mReadOutTrigger = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }
}

cedar::proc::DataSlot::VALIDITY cedar::dyn::steps::HebbianConnection::determineInputValidity(cedar::proc::ConstDataSlotPtr slot, cedar::aux::ConstDataPtr data) const
{

  unsigned int assoSizeX, assoSizeY, inputSizeX, inputSizeY;
  switch (mAssociationSizes->getValue().size())
  {
    case 0:
      assoSizeX=1;
      assoSizeY=1;
      break;
    case 1:
      assoSizeX = mAssociationSizes->getValue().at(0);
      assoSizeY=1;
      break;
    case 2:
      assoSizeX = mAssociationSizes->getValue().at(0);
      assoSizeY=mAssociationSizes->getValue().at(1);
      break;
    default:
      assoSizeX=1;
      assoSizeY=1;
  }

  switch (mInputSizes->getValue().size())
  {
    case 0:
      inputSizeX=1;
      inputSizeY=1;
      break;
    case 1:
      inputSizeX = mInputSizes->getValue().at(0);
      inputSizeY=1;
      break;
    case 2:
      inputSizeX = mInputSizes->getValue().at(0);
      inputSizeY= mInputSizes->getValue().at(1);
      break;
    default:
      inputSizeX=1;
      inputSizeY=1;
  }

//  std::cout<<"DetermineInputValidity: Input " << inputSizeX << ", " << inputSizeY << " Asso " << assoSizeX << ", " << assoSizeY << std::endl;

  if (cedar::aux::ConstMatDataPtr input = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
  {
    if (input && input->getDimensionality() == mAssociationDimension->getValue() && slot->getName() == mAssoInputName && (unsigned int) input->getData().rows == assoSizeX
         && (unsigned int) input->getData().cols == assoSizeY)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }

    if (input && input->getDimensionality() == mInputDimension->getValue() && slot->getName() == mReadOutInputName && (unsigned int) input->getData().rows == inputSizeX
        && (unsigned int) input->getData().cols == inputSizeY)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }

    if (input && input->getDimensionality() == 0 && slot->getName() == mRewardInputName )
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }

  }
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::dyn::steps::HebbianConnection::reset()
{
}

void cedar::dyn::steps::HebbianConnection::resetWeights()
{
  if(mInputDimension->getValue() + mAssociationDimension->getValue() > 2)
  {
    this->setState(cedar::proc::Triggerable::STATE_INITIALIZING, "Current Set of Dimensions is not supported yet!");
  }

  this->mConnectionWeights->setData(initializeWeightMatrix());
  cv::Mat emptyOutputMat = cv::Mat::zeros(mAssociationDimension->getValue() > 0 ? mAssociationSizes->getValue().at(0) : 1,mAssociationDimension->getValue() > 1 ? mAssociationSizes->getValue().at(1) : 1,CV_32F);
  this->mWeightOutput->setData(emptyOutputMat);


  this->emitOutputPropertiesChangedSignal(mTriggerOutputName);
  this->revalidateInputSlot(mAssoInputName);
}

void cedar::dyn::steps::HebbianConnection::toggleUseReward()
{
  mRewardDuration->setConstant(!mUseRewardDuration->getValue());
}

void cedar::dyn::steps::HebbianConnection::toggleUseManualWeights()
{
  if (mSetWeights->getValue())
  {
    this->resetWeights();
  }
  mWeightAmplitude->setConstant(!mSetWeights->getValue());
  mWeightSigmas->setConstant(!mSetWeights->getValue());
  mWeightCenters->setConstant(!mSetWeights->getValue());
}


unsigned int cedar::dyn::steps::HebbianConnection::getDimensionality()
{
  return mAssociationDimension->getValue();
}

cv::Mat cedar::dyn::steps::HebbianConnection::getSizes()
{
  cv::Mat sizes = cv::Mat::zeros(2, 1, CV_32F);
  sizes.at<float>(0, 0) = mWeightSizeX;
  sizes.at<float>(1, 0) = mWeightSizeY;
  return sizes;
}

std::string cedar::dyn::steps::HebbianConnection::getOutputName()
{
  return mOutputName;
}

std::string cedar::dyn::steps::HebbianConnection::getAssoInputName()
{
  return mAssoInputName;
}

std::string cedar::dyn::steps::HebbianConnection::getRewardInputName()
{
  return mRewardInputName;
}

std::string cedar::dyn::steps::HebbianConnection::getReadOutInputName()
{
  return mReadOutInputName;
}
std::string cedar::dyn::steps::HebbianConnection::getTriggerOutputName()
{
  return mTriggerOutputName;
}

void cedar::dyn::steps::HebbianConnection::setDimensionality(unsigned int dim)
{
  mAssociationDimension->setValue(dim);
}
void cedar::dyn::steps::HebbianConnection::setSize(unsigned int dim, unsigned int size)
{
  if (dim < mAssociationSizes->size())
    mAssociationSizes->setValue(dim, size);
}
void cedar::dyn::steps::HebbianConnection::setWeights(cv::Mat newWeights)
{
  this->mConnectionWeights->setData(newWeights);
  this->mWeightOutput->setData(newWeights);
}


 unsigned int cedar::dyn::steps::HebbianConnection::determineWeightSizes(unsigned int dimension)
 {
   auto assoDim = mAssociationDimension->getValue();
   auto inputDim = mInputDimension->getValue();

   if(dimension > determineWeightDimension() || determineWeightDimension() == 0)
   {
     return 1;
   }

   if(determineWeightDimension() == 1 )
   {
     if (assoDim == 1)
     {
       return dimension > 0 ? 1 : mAssociationSizes->getValue().at(0);
     }
     else
     {
       return dimension > 0 ? 1 : mInputSizes->getValue().at(0);
     }
   }

   if(determineWeightDimension() == 2)
   {
     if (assoDim == 2)
     {
       return dimension == 0 ? mAssociationSizes->getValue().at(0) : mAssociationSizes->getValue().at(1);
     }
     else if(inputDim == 2)
     {
       return dimension == 0 ? mInputSizes->getValue().at(0) : mInputSizes->getValue().at(1);
     }
     else if (inputDim == 1 && assoDim ==1)
     {
       return dimension == 0 ? mInputSizes->getValue().at(0) : mAssociationSizes->getValue().at(0);
     }
   }

//   std::cout<<"cedar::dyn::steps::HebbianConnection::determineWeightSizes! Should not arrive here!"<<std::endl;
   return 1;

 }


unsigned int cedar::dyn::steps::HebbianConnection::determineWeightDimension()
{
  auto assoDim = mAssociationDimension->getValue();
  auto inputDim = mInputDimension->getValue();

  if(assoDim + inputDim == 0)
  {
    return 0;
  }
  else if(assoDim + inputDim == 1)
  {
    return 1; // One is a node and the other a One-D Matrix
  } else if(assoDim + inputDim == 2)
  {
   return 2; // Either both are 1D which yields a 2D Matrix or one is 0D and the other is 2D
  } else
  {
//    std::cout<<"This Weight Combination is not yet implemented!"<<std::endl;
    return 0;
  }

}

cv::Mat cedar::dyn::steps::HebbianConnection::calculateWeightChange(cv::Mat inputActivation,cv::Mat associationActivation,cv::Mat rewardValue)
{
  double learnRate = mLearnRatePositive->getValue();
  cv::Mat currentWeights = mConnectionWeights->getData();

  auto inputSigmoid = this->mSigmoidF->getValue()->compute(inputActivation);
  auto targetSigmoid = this->mSigmoidH->getValue()->compute(associationActivation);
  auto rewardSigValue = this->mSigmoidG->getValue()->compute(rewardValue);

  //One case is outstar the other instar
  if(mInputDimension->getValue() == 0) // The old case! && And the 0 to 0 case! Be careful!
  {
    return learnRate * inputSigmoid.at<float>(0,0) * rewardSigValue.at<float>(0,0)  * (targetSigmoid- currentWeights);
  }

  if(mAssociationDimension->getValue() == 0) // The reverse case
  {
    return learnRate * rewardSigValue.at<float>(0,0) * targetSigmoid.at<float>(0,0)   * (inputSigmoid- currentWeights);
  }

  if(mAssociationDimension->getValue() == 1 && mInputDimension->getValue() == 1)
  {
    cv::Mat weightChangeMat = cv::Mat::zeros(mWeightSizeX,mWeightSizeY,CV_32F);

    for (unsigned int x = 0; x < mWeightSizeX; x++)
        {
          for (unsigned int y = 0; y < mWeightSizeY; y++)
          {
            float combinedValue = inputSigmoid.at<float>(x,0) * targetSigmoid.at<float>(y,0);
            float weightChange = learnRate * ( combinedValue    - currentWeights.at<float>(x, y));
            weightChangeMat.at<float>(x, y) = weightChange;
          }
        }
    return weightChangeMat;
  }

//  std::cout<<"calculateWeightChange?????? If you read this the current dimensions are not learnable yet!" <<std::endl;

  return cv::Mat::zeros(mWeightSizeX,mWeightSizeY,CV_32F);


}

 cv::Mat cedar::dyn::steps::HebbianConnection::calculateOutputMatrix(cv::Mat inputMatrix)
{
  if(mInputDimension->getValue() == 0) // The old case! && And the 0 to 0 case! Be careful!
  {
   return mConnectionWeights->getData() * mSigmoidF->getValue()->compute(inputMatrix.at<float>(0, 0));
  }

  if(mAssociationDimension->getValue() == 0) // The reverse case
  {
    cv::Mat overlap =  mConnectionWeights->getData().mul( mSigmoidF->getValue()->compute(inputMatrix));
    cv::Mat outputMat = cv::Mat::zeros(1,1,CV_32F);
    double overlapDouble = cv::sum(overlap) [0];
    outputMat.at<float>(0,0) = (float) overlapDouble; //TODO: The output might get very large
    return outputMat;
  }

  if(mAssociationDimension->getValue() == 1 && mInputDimension->getValue() == 1)
  {
    cv::Mat outPutMatrix = cv::Mat::zeros(mWeightSizeY,1,CV_32F);


    for (unsigned int y = 0; y < mWeightSizeY; y++)
    {
      float summedValue = 0;
      for (unsigned int x = 0; x < mWeightSizeX; x++)
      {
        summedValue += inputMatrix.at<float>(x,0) * mConnectionWeights->getData().at<float>(x,y);
      }

      outPutMatrix.at<float>(y,0) =summedValue; //Todo: Probably also some normalization
    }

    return outPutMatrix;
  }


//  std::cout<<"calculateOutputMatrix!!!! If you read this the current dimensions are not learnable yet!" <<std::endl;

  return cv::Mat::zeros(mWeightSizeX,mWeightSizeY,CV_32F);

}
