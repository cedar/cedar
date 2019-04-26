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
                    "A looped step that learns a projection between a node and a field according to a Hebbian rule.\nweightchange = learnrate * sigmoid(node)*sigmoid(reward)*(weights-field)"
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
      mAssociationDimension(new cedar::aux::UIntParameter(this, "association dimension", 2, 0, 2)),
      mAssociationSizes(new cedar::aux::UIntVectorParameter(this, "association sizes", 2, 50)),
      mLearnRatePositive(new cedar::aux::DoubleParameter(this, "learning rate", 0.01)),
      mUseRewardDuration(new cedar::aux::BoolParameter(this, "fixed reward duration", false)),
      mRewardDuration(new cedar::aux::DoubleParameter(this, "reward duration", 200)),
      mSetWeights(new cedar::aux::BoolParameter(this, "manual weights", false)),
      mWeightCenters(new cedar::aux::DoubleVectorParameter(this, "weight centers", mAssociationDimension->getValue(), 3)),
      mWeightSigmas(new cedar::aux::DoubleVectorParameter(this, "weight sigmas", mAssociationDimension->getValue(), 3)),
      mWeightAmplitude(new cedar::aux::DoubleParameter(this, "weight amplitude", 6)),
      // outputs
      mConnectionWeights(new cedar::aux::MatData(cv::Mat::zeros(100, 100, CV_32F))),
      mWeightOutput((new cedar::aux::MatData(cv::Mat::zeros(100, 100, CV_32F)))),
      mRewardTrigger(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
      mReadOutTrigger(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
      mWeightSizeX(mAssociationDimension->getValue() > 0 ? mAssociationSizes->getValue().at(0) : 1),
      mWeightSizeY(mAssociationDimension->getValue() > 1 ? mAssociationSizes->getValue().at(1) : 1)
{

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

  this->registerFunction("reset Weights", boost::bind(&HebbianConnection::resetWeights, this), false);

  QObject::connect(mAssociationDimension.get(), SIGNAL(valueChanged()), this, SLOT(updateAssociationDimension()));
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
  mWeightCenters->resize(new_dim, mWeightCenters->getDefaultValue());
  mWeightSigmas->resize(new_dim, mWeightCenters->getDefaultValue());
  mWeightSizeX = mAssociationDimension->getValue() > 0 ? mAssociationSizes->getValue().at(0) : 1;
  mWeightSizeY = mAssociationDimension->getValue() > 1 ? mAssociationSizes->getValue().at(1) : 1;
  this->resetWeights();
}

cv::Mat cedar::dyn::steps::HebbianConnection::initializeWeightMatrix()
{
  mWeightSizeX = mAssociationDimension->getValue() > 0 ? mAssociationSizes->getValue().at(0) : 1;
  mWeightSizeY = mAssociationDimension->getValue() > 1 ? mAssociationSizes->getValue().at(1) : 1;
  cv::Mat myWeightMat = cv::Mat::zeros(mWeightSizeX, mWeightSizeY, CV_32F);
  if (!mSetWeights->getValue())
  {
//    std::cout<<"InitWeights: RANDOM!"<<std::endl;
    srand(static_cast<unsigned>(time(0)));
    float HIGH = 0.1;
    float LOW = 0;
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
//    std::cout<<"InitWeights: MANUAL"<<std::endl;
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
  if (mAssoInput && mRewardTrigger && mReadOutTrigger )
  {
    const cv::Mat& trigger = mRewardTrigger->getData();
    if (trigger.at<float>(0, 0) > 0.5) // It is assumed that a change greater than 0.5 is not noise
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
        //Apply Learning Rule
        double learnRate = mLearnRatePositive->getValue();
        cv::Mat currentWeights = mConnectionWeights->getData();
        cv::Mat currentAssoMat = mAssoInput->getData();
//        auto mySigmoid = cedar::aux::math::SigmoidPtr(new cedar::aux::math::HeavisideSigmoid(0.5));
//        float sigmoidedNodeInput =  mySigmoid->compute(mReadOutTrigger->getData().at<float>(0, 0));

        for (unsigned int x = 0; x < mWeightSizeX; x++)
        {
          for (unsigned int y = 0; y < mWeightSizeY; y++)
          {


            float weightChange = learnRate * mReadOutTrigger->getData().at<float>(0, 0)   * (currentAssoMat.at<float>(x, y) - currentWeights.at<float>(x, y));
            currentWeights.at<float>(x, y) = currentWeights.at<float>(x, y) + weightChange;
          }
        }
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
    const cv::Mat& trigger = mReadOutTrigger->getData();
    if (trigger.at<float>(0, 0) > 0.5) // It is assumed that a change greater than 0.5 is intentional
    {
      mWeightOutput->setData(mConnectionWeights->getData());
    }
    else
    {
      mWeightOutput->setData(cv::Mat::zeros(mWeightSizeX, mWeightSizeY, CV_32F));
    }
  }
  else
    mWeightOutput->setData(cv::Mat::zeros(mWeightSizeX, mWeightSizeY, CV_32F));
}

void cedar::dyn::steps::HebbianConnection::inputConnectionChanged(const std::string& inputName)
{
// Assign the input to the member. This saves us from casting in every computation step.
//  std::cout << "InputConnectionChanged! Inputname: " << inputName << std::endl;
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
  if (cedar::aux::ConstMatDataPtr input = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
  {
    if (input && input->getDimensionality() == mAssociationDimension->getValue() && slot->getName() == mAssoInputName && (unsigned int) input->getData().rows == mWeightSizeX
        && (unsigned int) input->getData().cols == mWeightSizeY)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
    if (input && input->getDimensionality() == 0 && (slot->getName() == mRewardInputName || slot->getName() == mReadOutInputName))
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
  this->mConnectionWeights->setData(initializeWeightMatrix());
  this->mWeightOutput->setData(mConnectionWeights->getData());
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
