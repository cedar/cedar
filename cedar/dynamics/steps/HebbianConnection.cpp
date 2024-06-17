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
#include "cedar/processing/GroupXMLFileFormatV1.h"
#include "cedar/auxiliaries/math/transferFunctions/ExpSigmoid.h"
#include "cedar/auxiliaries/annotation/SizesRangeHint.h"

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

cedar::aux::EnumType<cedar::dyn::steps::HebbianConnection::LearningRule> cedar::dyn::steps::HebbianConnection::LearningRule::mType("LearningRule::");

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dyn::steps::HebbianConnection::HebbianConnection()
    :
      // parameters
        mLearningRule(
                new cedar::aux::EnumParameter(this, "learning rule", cedar::dyn::steps::HebbianConnection::LearningRule::typePtr(),
                                              LearningRule::OJA)),
        mTau(new cedar::aux::DoubleParameter(this, "time scale", 0.0)),
        mTauDecay(new cedar::aux::DoubleParameter(this, "time scale decay", 0.0)),
      mInputDimension(new cedar::aux::UIntParameter(this, "source dimension", 1, 0, 3)),
      mInputSizes(new cedar::aux::UIntVectorParameter(this, "source sizes", 1, 50,cedar::aux::UIntParameter::LimitType::positive(5000))),
      mAssociationDimension(new cedar::aux::UIntParameter(this, "target dimension", 2, 0, 3)),
      mAssociationSizes(new cedar::aux::UIntVectorParameter(this, "target sizes", 2, 50,cedar::aux::UIntParameter::LimitType::positive(5000))),
      mLearnRatePositive(new cedar::aux::DoubleParameter(this, "learning rate", 0.01)),
      mSigmoidF
              (
                      new cedar::dyn::steps::HebbianConnection::SigmoidParameter
                              (
                                      this,
                                      "f (source sigmoid)",
                                      cedar::aux::math::SigmoidPtr(new cedar::aux::math::HeavisideSigmoid(0.5))
                              )
              ),
      mSigmoidG
              (
                      new cedar::dyn::steps::HebbianConnection::SigmoidParameter
                              (
                                      this,
                         "g (reward sigmoid)",
                                      cedar::aux::math::SigmoidPtr(new cedar::aux::math::HeavisideSigmoid(0.5))
                              )
              ),
      mSigmoidH
              (
                      new cedar::dyn::steps::HebbianConnection::SigmoidParameter
                              (
                                      this,
                                      "h (target sigmoid)",
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
      mWeightSizeY(mAssociationDimension->getValue() > 1 ? mAssociationSizes->getValue().at(1) : 1),
      mWeightedTargetOutput((new cedar::aux::MatData(cv::Mat::zeros(100, 100, CV_32F)))),
      mWeightedTargetSumOutput((new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))))

{
  this->mXMLExportable = true;
  this->mXMLParameterWhitelist = {"learning rate","fixed reward duration", "reward duration", "manual weights", "weight centers", "weight sigmas", "weight amplitude"};

  //Initialize 3D Matrices
  std::vector<int> defaultThetaSizes{7,7,10};
  mBCMTheta = cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat(3,&defaultThetaSizes.at(0), CV_32F,cv::Scalar(0.5))));
  //Todo:Update this whenever Input Dimension Change etc.

  //Parameter Groups
  // create configurable to represent the group
  cedar::aux::ConfigurablePtr mBcmParameterGroup(new cedar::aux::Configurable());
  // add the group as a child
  this->addConfigurableChild("BCM Parameters", mBcmParameterGroup);
  mTauWeights = cedar::aux::DoubleParameterPtr(new cedar::aux::DoubleParameter(mBcmParameterGroup.get(), "tau weights", 1.0));
  mTauTheta = cedar::aux::DoubleParameterPtr(new cedar::aux::DoubleParameter(mBcmParameterGroup.get(), "tau theta", 10000.0));
  mMinThetaValue = cedar::aux::DoubleParameterPtr(new cedar::aux::DoubleParameter(mBcmParameterGroup.get(), "min theta value", 0.0));
  mUseFixedTheta = cedar::aux::BoolParameterPtr (new cedar::aux::BoolParameter (mBcmParameterGroup.get(), "use fixed theta", false));
  mFixedThetaValue = cedar::aux::DoubleParameterPtr(new cedar::aux::DoubleParameter(mBcmParameterGroup.get(), "fixed theta value", 0.5,0.0,1.0));
  this->toggleFixedTheta(); // Set Theta and the Parameter Constness accordingly




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

  //New BCM Stuff
  auto thetaBuffer = this->declareBuffer(mThetaMatrixName,mBCMTheta);
  thetaBuffer->setSerializable(true);

  auto weightTriggerOutput = this->declareOutput(mTriggerOutputName, mWeightOutput);
  this->declareOutput(mWeightedTargetOutputName, mWeightedTargetOutput);
  this->declareOutput(mWeightedTargetSumOutputName, mWeightedTargetSumOutput);

  this->mConnectionWeights->getData() = initializeWeightMatrix();
  mWeightOutput->setData(mConnectionWeights->getData());
  this->updateWeightedTargetOutput();

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
  QObject::connect(mLearningRule.get(), SIGNAL(valueChanged()), this, SLOT(updateLearningRule()));
  QObject::connect(mUseFixedTheta.get(),SIGNAL(valueChanged()),this,SLOT(toggleFixedTheta()));
  QObject::connect(mFixedThetaValue.get(),SIGNAL(valueChanged()),this,SLOT(applyFixedTheta()));

  this->updateLearningRule();
  this->updateAssociationSizesRange();
  this->updateInputSizesRange();
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dyn::steps::HebbianConnection::readConfigurationXML(const cedar::aux::ConfigurationNode &node)
{
  cedar::aux::Configurable::readConfigurationXML(node);

  //readDimensionsParameter
  std::vector<cedar::aux::math::Limits<double>> inputSizesRange;
  cedar::proc::GroupXMLFileFormatV1::readDimensionsParameter(this->mInputDimension, this->mInputSizes, inputSizesRange, node, "SourceDimensions");
  this->mConnectionWeights->setAnnotation(cedar::aux::annotation::AnnotationPtr(
          new cedar::aux::annotation::SizesRangeHint(inputSizesRange)));

  //readDimensionsParameter
  std::vector<cedar::aux::math::Limits<double>> assSizesRange;
  cedar::proc::GroupXMLFileFormatV1::readDimensionsParameter(this->mAssociationDimension, this->mAssociationSizes, assSizesRange, node, "TargetDimensions");
  this->mWeightOutput->setAnnotation(cedar::aux::annotation::AnnotationPtr(
          new cedar::aux::annotation::SizesRangeHint(assSizesRange)));

  //readActivationFunction
  cedar::proc::GroupXMLFileFormatV1::readActivationFunctionParameter(this->mSigmoidF.get(), node, "SourceFunction");
  cedar::proc::GroupXMLFileFormatV1::readActivationFunctionParameter(this->mSigmoidG.get(), node, "RewardFunction");
  cedar::proc::GroupXMLFileFormatV1::readActivationFunctionParameter(this->mSigmoidH.get(), node, "TargetFunction");


}

void cedar::dyn::steps::HebbianConnection::writeConfigurationXML(cedar::aux::ConfigurationNode& root) const
{

  cedar::aux::Configurable::writeConfigurationXML(root);

  // sigma parameter
  cedar::proc::GroupXMLFileFormatV1::writeActivationFunctionParameter(this->mSigmoidF.get(), root, "SourceFunction");
  cedar::proc::GroupXMLFileFormatV1::writeActivationFunctionParameter(this->mSigmoidG.get(), root, "RewardFunction");
  cedar::proc::GroupXMLFileFormatV1::writeActivationFunctionParameter(this->mSigmoidH.get(), root, "TargetFunction");

  //source dimensionality/sizes parameter
  cedar::proc::GroupXMLFileFormatV1::writeDimensionsParameter(this->mInputDimension, this->mInputSizes, this->mConnectionWeights->getAnnotation<cedar::aux::annotation::SizesRangeHint>()->getRange(),
                                                              root, "SourceDimensions");

  // target dimensionality/sizes parameter
  cedar::proc::GroupXMLFileFormatV1::writeDimensionsParameter(this->mAssociationDimension, this->mAssociationSizes, this->mWeightOutput->getAnnotation<cedar::aux::annotation::SizesRangeHint>()->getRange(),
                                                                 root, "TargetDimensions");
//
//  if(this -> mSetWeights -> getValue())
//  {
//    this->mWeightCenters->writeToNodeXML(root);
//    this->mWeightSigmas->writeToNodeXML(root);
//    this->mWeightAmplitude->writeToNodeXML(root);
//  }
}

bool cedar::dyn::steps::HebbianConnection::isXMLExportable(std::string& errorMsg){

  if(this->mLearningRule->getValue() != cedar::dyn::steps::HebbianConnection::LearningRule::OJA)
  {
    errorMsg = "The XML export only supports Oja's Rule as learning rule.";
    return false;
  }

  std::vector<std::tuple<boost::intrusive_ptr<aux::ObjectParameterTemplate<aux::math::TransferFunction>>, std::string>> sigmoid_functions;
  sigmoid_functions.emplace_back(this -> mSigmoidF, "source");
  sigmoid_functions.emplace_back(this -> mSigmoidG, "reward");
  sigmoid_functions.emplace_back(this -> mSigmoidH, "target");

  for (const auto& sigmoid_tuple : sigmoid_functions)
  {
    if(dynamic_cast<cedar::aux::math::Sigmoid*>(std::get<0>(sigmoid_tuple)->getValue().get()))
    {
      if(!(dynamic_cast<cedar::aux::math::ExpSigmoid*>(std::get<0>(sigmoid_tuple)->getValue().get()) ||
           dynamic_cast<cedar::aux::math::AbsSigmoid*>(std::get<0>(sigmoid_tuple)->getValue().get()) ||
           dynamic_cast<cedar::aux::math::HeavisideSigmoid*>(std::get<0>(sigmoid_tuple)->getValue().get())))
      {
        errorMsg = "The XML export only supports \"AbsSigmoid\", \"ExpSigmoid\" and \"HeavisideSigmoid\" as the transfer function.";
        return false;
      }
    }
    else if(!dynamic_cast<cedar::aux::math::LinearTransferFunction*>(std::get<0>(sigmoid_tuple)->getValue().get()))
    {
      errorMsg = "The XML export only supports sigmoids and LinearTransferFunction as transfer functions.";
      return false;
    }
  }

  if(this->getOutputSlot(mTriggerOutputName)->getDataConnections().size() > 1 ||
     this->getInputSlot(mAssoInputName)->getDataConnections().size() > 1)
  {
    return false;
  }
  return true;
}


void cedar::dyn::steps::HebbianConnection::updateAssociationDimension()
{
  int new_dim = static_cast<int>(mAssociationDimension->getValue());
  mAssociationSizes->resize(new_dim, mAssociationSizes->getDefaultValue());

  auto newWeightDimension = determineWeightDimension();


  mWeightCenters->resize(newWeightDimension, mWeightCenters->getDefaultValue());
  mWeightSigmas->resize(newWeightDimension, mWeightCenters->getDefaultValue());
  mWeightSizeX = determineWeightSizes(0);
  mWeightSizeY = determineWeightSizes(1);

  // Update the sizes annotation
  std::vector<cedar::aux::math::Limits<double>> sizesRange;
  for(unsigned int size : this->mAssociationSizes->getValue())
  {
    sizesRange.push_back(cedar::aux::math::Limits<double>(0, size - 1));
  }
  this->mWeightOutput->setAnnotation(cedar::aux::annotation::AnnotationPtr(new cedar::aux::annotation::SizesRangeHint(sizesRange)));

//  std::cout<<"updateAssociationDimension: WeightDim! " << determineWeightDimension() << " WeightX=" << determineWeightSizes(0) << " WeightY=" << determineWeightSizes(1) << std::endl;
  this->resetWeights();
}

void cedar::dyn::steps::HebbianConnection::updateInputDimension()
{
  int new_dim = static_cast<int>(mInputDimension->getValue());
  mInputSizes->resize(new_dim, mInputSizes->getDefaultValue());

  auto newWeightDimension = determineWeightDimension();


  mWeightCenters->resize(newWeightDimension, mWeightCenters->getDefaultValue());
  mWeightSigmas->resize(newWeightDimension, mWeightCenters->getDefaultValue());

//  std::cout<<"updateInputDimension: WeightDim! " << determineWeightDimension() << " WeightX=" << determineWeightSizes(0) << " WeightY=" << determineWeightSizes(1) << std::endl;

  std::vector<cedar::aux::math::Limits<double>> sizesRange;
  for(unsigned int size : this->mInputSizes->getValue())
  {
    sizesRange.push_back(cedar::aux::math::Limits<double>(0, size - 1));
  }
  this->mConnectionWeights->setAnnotation(cedar::aux::annotation::AnnotationPtr(new cedar::aux::annotation::SizesRangeHint(sizesRange)));

  this->resetWeights();
}

void cedar::dyn::steps::HebbianConnection::updateLearningRule()
{
  //Not really sure yet was this entails. A lot of parameters can probably be reused


  switch (this->mLearningRule->getValue())
  {
    case cedar::dyn::steps::HebbianConnection::LearningRule::OJA:
      if(mInputDimension->getValue()>2)
      {
        mInputDimension->setValue(2);
      }
      mInputDimension->setMaximum(2);
      if(mAssociationDimension->getValue()>2)
      {
        mAssociationDimension->setValue(2);
      }
      mAssociationDimension->setMaximum(2);
      mSetWeights->setConstant(false);
      mTau->setConstant(false);
      mTauDecay->setConstant(false);
      //Unfortunately no iteration
      mTauWeights->setConstant(true);
      mTauTheta->setConstant(true);
      mMinThetaValue->setConstant(true);
      mUseFixedTheta->setConstant(true);
      mFixedThetaValue->setConstant(true);
      break;

    case cedar::dyn::steps::HebbianConnection::LearningRule::BCM:
      mInputDimension->setMaximum(3);
      mInputDimension->setValue(3);
      mInputDimension->setMinimum(3);
      mAssociationDimension->setMaximum(3);
      mAssociationDimension->setValue(3);
      mAssociationDimension->setMinimum(3);
      mSetWeights->setValue(false);
      mSetWeights->setConstant(true);
      mTau->setConstant(true);
      mTauDecay->setConstant(true);

//      auto bcmParameterList = mBcmParameterGroup->getParameters(); //THIS CALL CRASHES. WHY?
//      for(auto parameter:mBcmParameterGroup->getParameters())
//      {
//        parameter->setConstant(false);
//      }
//      Set Parameters to Const individually
      mTauWeights->setConstant(false);
      mTauTheta->setConstant(false);
      mMinThetaValue->setConstant(false);
      mUseFixedTheta->setConstant(false);
      mFixedThetaValue->setConstant(!mUseFixedTheta->getValue());



      break;
  }

  this->mConnectionWeights->setData(initializeWeightMatrix());
  this->updateWeightedTargetOutput();
}

void cedar::dyn::steps::HebbianConnection::resetWeightedTargetOutput()
{
  this->mWeightedTargetOutput->setData(cv::Mat::zeros(100, 100, CV_32F));
  this->mWeightedTargetSumOutput->setData(cv::Mat::zeros(1, 1, CV_32F));
}

void cedar::dyn::steps::HebbianConnection::updateWeightedTargetOutput()
{
  if(!this->mAssoInput)
  {
    resetWeightedTargetOutput();
    return;
  }
  cv::Mat weights = this->mConnectionWeights->getData();
  cv::Mat targetField = this->mAssoInput->getData();

  if (weights.dims != 2 || targetField.dims != 2)
  {
    resetWeightedTargetOutput();
    return;
  }
  if (weights.size[0] != targetField.size[0] || weights.size[1] != targetField.size[1])
  {
    resetWeightedTargetOutput();
    return;
  }
  cv::Mat prod = weights.mul(targetField);
  this->mWeightedTargetOutput->setData(prod);
  this->mWeightedTargetSumOutput->setData(cv::Mat::ones(1, 1, CV_32F) * cv::sum(prod)[0]);
}

cv::Mat cedar::dyn::steps::HebbianConnection::initializeWeightMatrix()
{
  cv::Mat myWeightMat;
  switch (this->mLearningRule->getValue())
  {
    case cedar::dyn::steps::HebbianConnection::LearningRule::OJA:
      mWeightSizeX = determineWeightSizes(0);
      mWeightSizeY = determineWeightSizes(1);
      myWeightMat = cv::Mat::zeros(mWeightSizeX, mWeightSizeY, CV_32F);
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
      } else
      {
        std::vector<unsigned int> curSizes;
        curSizes.push_back(mWeightSizeX);
        curSizes.push_back(mWeightSizeY);

        if (mAssociationDimension->getValue() != 0 && mWeightSigmas->getValue().size()== mAssociationDimension->getValue() && mWeightCenters->getValue().size() == mAssociationDimension->getValue() )
        {
          myWeightMat = cedar::aux::math::gaussMatrix(mAssociationDimension->getValue(), curSizes,
                                                      mWeightAmplitude->getValue(), mWeightSigmas->getValue(),
                                                      mWeightCenters->getValue(), true);
        } else
        {
          myWeightMat.at<float>(0, 0) = mWeightAmplitude->getValue();
        }
      }
      break;
    case cedar::dyn::steps::HebbianConnection::LearningRule::BCM:
      if(mInputSizes->getValue().size() == 3 && mAssociationSizes->getValue().size() == 3)
      {
        //Allow other dimensionalities in the future
        int bcmWeightDimensionality = 4;
        std::vector<int> bcmWeightSizes(bcmWeightDimensionality);
        bcmWeightSizes[0] = mInputSizes->getValue().at(0);
        bcmWeightSizes[1] = mInputSizes->getValue().at(1);
        bcmWeightSizes[2] = mInputSizes->getValue().at(2);
        bcmWeightSizes[3] = mAssociationSizes->getValue().at(2);

        myWeightMat = cv::Mat(bcmWeightDimensionality, &bcmWeightSizes.at(0), CV_32F, cv::Scalar(0));
      }
  }
  return myWeightMat;
}

cv::Mat cedar::dyn::steps::HebbianConnection::initializeThetaMatrix()
{
  std::vector<int> defaultThetaSizes;
  for(unsigned int i = 0; i< mAssociationSizes->getValue().size();i++)
  {
    defaultThetaSizes.push_back((int) mAssociationSizes->getValue().at(i));
  }
  float defaultTheta = this->mUseFixedTheta->getValue() ? this->mFixedThetaValue->getValue() : 0.5;
  return cv::Mat(mAssociationSizes->getValue().size(), &defaultThetaSizes.at(0), CV_32F, cv::Scalar(defaultTheta));
}

cv::Mat cedar::dyn::steps::HebbianConnection::calculateDefaultOutput()
{
  cv::Mat outputMatrix = cv::Mat::zeros(mAssociationDimension->getValue() > 0 ? mAssociationSizes->getValue().at(0) : 1,mAssociationDimension->getValue() > 1 ? mAssociationSizes->getValue().at(1) : 1,CV_32F);

  //  Calculate an output for an Input of Zeros
  if(this->mInputDimension->getValue()!=0)
  {
  std::vector<int> inputSizes;
  for(unsigned int i = 0; i< mInputSizes->getValue().size();i++)
  {
    inputSizes.push_back((int) mInputSizes->getValue().at(i));
  }
  cv::Mat zeroInputMat = cv::Mat::zeros(mInputSizes->getValue().size(),&inputSizes.at(0),CV_32F);
  outputMatrix = calculateOutputMatrix(zeroInputMat);
  }


  return outputMatrix;
}

void cedar::dyn::steps::HebbianConnection::eulerStep(const cedar::unit::Time& time)
{
  if(mInputDimension->getValue() + mAssociationDimension->getValue() > 2 && this->mLearningRule->getValue() == cedar::dyn::steps::HebbianConnection::LearningRule::OJA)
  {
    this->setState(cedar::proc::Triggerable::STATE_INITIALIZING, "Current Set of Dimensions is not supported yet!");
    mWeightOutput->setData(cv::Mat::zeros(mWeightSizeX, mWeightSizeY, CV_32F));
    return;
  }

  if(this->mLearningRule->getValue() == cedar::dyn::steps::HebbianConnection::LearningRule::BCM)
  {
    bool isBCMPossible = mConnectionWeights->getData().dims == 4 &&
                         mAssociationSizes->getValue().at(0) == mInputSizes->getValue().at(0) &&
                         mAssociationSizes->getValue().at(1) == mInputSizes->getValue().at(1);

    if (!isBCMPossible)
    {
      this->setState(cedar::proc::Triggerable::STATE_INITIALIZING,
                     "Your current setting for the BCM learn rule is not yet implemented.\nCurrent implementation requires a 3D Input and a 3D Association, which have the same sizes in the first two dimensions");
      mWeightOutput->setData(cv::Mat::zeros(mWeightSizeX, mWeightSizeY, CV_32F));
      return;
    }
  }

  if (mAssoInput && mRewardTrigger && mReadOutTrigger )
  {
    //All inputs are connected
    const cv::Mat& rewardTrigger = mRewardTrigger->getData();
    if (mSigmoidG->getValue()->compute(rewardTrigger.at<float>(0, 0)) > 0) // This is for the RewardDuration Trick. Should be removed in later versions!
    {
      if (!mIsRewarded && mUseRewardDuration->getValue())
      {
        //Reward is present and start counting
        mElapsedTime = 0;
        mIsRewarded = true;

        //For 0d/0d case: Pause learning until either input or target field is active
        auto targetSigmoid = this->mSigmoidH->getValue()->compute(mAssoInput->getData());
        auto inputSigmoid = this->mSigmoidF->getValue()->compute(mReadOutTrigger->getData());
        if (mInputDimension->getValue() == 0 && mAssociationDimension->getValue() == 0)
        {
          if (inputSigmoid.at<float>(0, 0) < 0.5 && targetSigmoid.at<float>(0, 0) < 0.5)
          {
            mIsRewarded = false;
          }
        }
      }
      //Count the time the reward is present
      float curTime = time / cedar::unit::Time(1 * cedar::unit::milli * cedar::unit::seconds);
      mElapsedTime += curTime;
    }
    else if (mUseRewardDuration->getValue())
    {
      //No reward is present anymore. It can be reactivated
      mIsRewarded = false;
    }

    if ((mElapsedTime < mRewardDuration->getValue() && mIsRewarded) || !mUseRewardDuration->getValue())
    {
      //If there is no maximum reward duration =>update
      //If there is a maximum reward duration => update only if time is below the max time
      cv::Mat currentWeights = mConnectionWeights->getData();
      auto weightChange = calculateWeightChange(time,mReadOutTrigger->getData(),mAssoInput->getData(),mRewardTrigger->getData());
      currentWeights = currentWeights + weightChange;
      mConnectionWeights->setData(currentWeights);
      this->updateWeightedTargetOutput();
    }
  }

  if (mReadOutTrigger)
  {
      mWeightOutput->setData(calculateOutputMatrix(mReadOutTrigger->getData()));
  }
  else
  {
    mWeightOutput->setData(calculateDefaultOutput());
  }
  this->updateWeightedTargetOutput();
}

void cedar::dyn::steps::HebbianConnection::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == mAssoInputName)
  {
    this->mAssoInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
    this->updateWeightedTargetOutput();
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
    if (input && input->getDimensionality() == mAssociationDimension->getValue() && slot->getName() == mAssoInputName)
    {
      bool sizesMatch = true;
      for(unsigned int d = 0 ; d< input->getDimensionality();d++)
      {
        if(mAssociationSizes->getValue().at(d) != (unsigned int) input->getData().size[d])
          sizesMatch = false;
      }
      if(sizesMatch)
        return cedar::proc::DataSlot::VALIDITY_VALID;
    }

    if (input && input->getDimensionality() == mInputDimension->getValue() && slot->getName() == mReadOutInputName)
    {
      bool sizesMatch = true;
      for(unsigned int d = 0 ; d< input->getDimensionality();d++)
      {
        if(mInputSizes->getValue().at(d) !=  (unsigned int) input->getData().size[d])
          sizesMatch = false;
      }
      if(sizesMatch)
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
  resetWeightedTargetOutput();
  mWeightOutput->setData(cv::Mat::zeros(mWeightSizeX, mWeightSizeY, CV_32F));
}

void cedar::dyn::steps::HebbianConnection::resetWeights()
{
  if(mInputDimension->getValue() + mAssociationDimension->getValue() > 2 && this->mLearningRule->getValue() == cedar::dyn::steps::HebbianConnection::LearningRule::OJA)
  {
    this->setState(cedar::proc::Triggerable::STATE_INITIALIZING, "Current Set of Dimensions is not supported yet!");
  }

  this->mConnectionWeights->setData(initializeWeightMatrix());

  this->mWeightOutput->setData(calculateDefaultOutput());

  this->updateWeightedTargetOutput();

  if(this->mLearningRule->getValue() == cedar::dyn::steps::HebbianConnection::LearningRule::BCM)
  {
    mBCMTheta->setData(initializeThetaMatrix());
  }

  this->emitOutputPropertiesChangedSignal(mTriggerOutputName);
  this->revalidateInputSlot(mReadOutInputName);
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


//unsigned int cedar::dyn::steps::HebbianConnection::getDimensionality()
//{
//  return mAssociationDimension->getValue();
//}

//cv::Mat cedar::dyn::steps::HebbianConnection::getSizes()
//{
//  cv::Mat sizes = cv::Mat::zeros(2, 1, CV_32F);
//  sizes.at<float>(0, 0) = mWeightSizeX;
//  sizes.at<float>(1, 0) = mWeightSizeY;
//  return sizes;
//}

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
  this->updateAssociationSizesRange();
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
  this->updateWeightedTargetOutput();
}


 unsigned int cedar::dyn::steps::HebbianConnection::determineWeightSizes(unsigned int dimension)
 {
   auto assoDim = mAssociationDimension->getValue();
   auto inputDim = mInputDimension->getValue();

   switch(this->mLearningRule->getValue())
   {
     case cedar::dyn::steps::HebbianConnection::LearningRule::OJA:
     {
       if (dimension > determineWeightDimension() || determineWeightDimension() == 0)
       {
         return 1;
       }

       if (determineWeightDimension() == 1)
       {
         if (assoDim == 1)
         {
           return dimension > 0 ? 1 : mAssociationSizes->getValue().at(0);
         } else
         {
           return dimension > 0 ? 1 : mInputSizes->getValue().at(0);
         }
       }

       if (determineWeightDimension() == 2)
       {
         if (assoDim == 2)
         {
           return dimension == 0 ? mAssociationSizes->getValue().at(0) : mAssociationSizes->getValue().at(1);
         } else if (inputDim == 2)
         {
           return dimension == 0 ? mInputSizes->getValue().at(0) : mInputSizes->getValue().at(1);
         } else if (inputDim == 1 && assoDim == 1)
         {
           return dimension == 0 ? mInputSizes->getValue().at(0) : mAssociationSizes->getValue().at(0);
         }
       }
       break;
     }
     case cedar::dyn::steps::HebbianConnection::LearningRule::BCM:
     {
       //Again this assumes that BCM is only working with a fixed set of weights currently
       switch(dimension)
       {
         case 0: return mAssociationSizes->getValue().at(0);
         case 1: return mAssociationSizes->getValue().at(1);
         case 2: return mInputSizes->getValue().at(2);
         case 3: return mAssociationSizes->getValue().at(2);
       }
     }
   }

//   std::cout<<"cedar::dyn::steps::HebbianConnection::determineWeightSizes! Should not arrive here!"<<std::endl;
   return 1;

 }


unsigned int cedar::dyn::steps::HebbianConnection::determineWeightDimension()
{
  auto assoDim = mAssociationDimension->getValue();
  auto inputDim = mInputDimension->getValue();

//  std::cout<<"\ncedar::dyn::steps::HebbianConnection::determineWeightDimension!"<<std::endl;
//  std::cout<<"assoDim: " << assoDim << " inputDim: " << inputDim << std::endl;
//  std::cout<<"LearningRule: " << this->mLearningRule->getValue() << std::endl;

  switch(this->mLearningRule->getValue())
  {
    case cedar::dyn::steps::HebbianConnection::LearningRule::OJA:
    {
      if (assoDim + inputDim == 0)
      {
        return 0;
      } else if (assoDim + inputDim == 1)
      {
        return 1; // One is a node and the other a One-D Matrix
      } else if (assoDim + inputDim == 2)
      {
        return 2; // Either both are 1D which yields a 2D Matrix or one is 0D and the other is 2D
      } else
      {
//    std::cout<<"This Weight Combination is not yet implemented!"<<std::endl;
        return 0;
      }
    }
    case cedar::dyn::steps::HebbianConnection::LearningRule::BCM:
    {
      //TODO Currently BCM works for only 3 dims
      return 4;
    }
    default:
      //This should not happen
      std::cout<<"DetermineInputDimension did not recognize the Learning Rule! This should not happen!" <<std::endl;
      return 0;
  }
}

cv::Mat cedar::dyn::steps::HebbianConnection::calculateWeightChange(const cedar::unit::Time& delta_t, cv::Mat inputActivation,cv::Mat associationActivation,cv::Mat rewardValue)
{
  float learnRate = (float) mLearnRatePositive->getValue();
  cv::Mat currentWeights = mConnectionWeights->getData();

  auto targetSigmoid = this->mSigmoidH->getValue()->compute(associationActivation);
  auto rewardSigValue = this->mSigmoidG->getValue()->compute(rewardValue);
  cv::Mat inputSigmoid = this->mSigmoidF->getValue()->compute(inputActivation);

  switch(this->mLearningRule->getValue())
  {
    case cedar::dyn::steps::HebbianConnection::LearningRule::OJA:
    {
      //One case is outstar the other instar
      if (mInputDimension->getValue() == 0) // The old case! && And the 0 to 0 case! Be careful!
      {
        cv::Mat weightChangeMat = cv::Mat::zeros(mWeightSizeX, mWeightSizeY, CV_32F);
        for (unsigned int x = 0; x < mWeightSizeX; x++)
        {
          for (unsigned int y = 0; y < mWeightSizeY; y++)
          {
            if(mTau->getValue() > 0 && mTauDecay->getValue() == 0) {
              auto timeFactor = (delta_t / cedar::unit::Time(mTau->getValue() * cedar::unit::milli * cedar::unit::seconds));
              weightChangeMat.at<float>(x, y) = timeFactor * (learnRate * inputSigmoid.at<float>(0, 0) * rewardSigValue.at<float>(0, 0) * (targetSigmoid.at<float>(x, y) - currentWeights.at<float>(x, y)));
            } else if(mTau->getValue() > 0 && mTauDecay->getValue() > 0) {
              auto timeFactor1 = (delta_t / cedar::unit::Time(mTau->getValue() * cedar::unit::milli * cedar::unit::seconds)) * targetSigmoid.at<float>(x, y);
              auto timeFactor2 = (delta_t / cedar::unit::Time(mTauDecay->getValue() * cedar::unit::milli * cedar::unit::seconds)) * (1 - targetSigmoid.at<float>(x, y));
              weightChangeMat.at<float>(x, y) = (timeFactor1 + timeFactor2) * (learnRate * inputSigmoid.at<float>(0, 0) * rewardSigValue.at<float>(0, 0) * (targetSigmoid.at<float>(x, y) - currentWeights.at<float>(x, y)));
            } else {
              weightChangeMat.at<float>(x, y) = (learnRate * inputSigmoid.at<float>(0, 0) * rewardSigValue.at<float>(0, 0) * (targetSigmoid.at<float>(x, y) - currentWeights.at<float>(x, y)));
            }
          }
        }
        return weightChangeMat;
      }

      if (mAssociationDimension->getValue() == 0) // The reverse case
      {
        cv::Mat weightChangeMat = cv::Mat::zeros(mWeightSizeX, mWeightSizeY, CV_32F);
        for (unsigned int x = 0; x < mWeightSizeX; x++)
        {
          for (unsigned int y = 0; y < mWeightSizeY; y++)
          {
            if(mTau->getValue() > 0 && mTauDecay->getValue() == 0) {
              auto timeFactor = (delta_t / cedar::unit::Time(mTau->getValue() * cedar::unit::milli * cedar::unit::seconds));
              weightChangeMat.at<float>(x, y) = timeFactor * (learnRate * targetSigmoid.at<float>(0, 0) * rewardSigValue.at<float>(0, 0) * (inputSigmoid.at<float>(x, y) - currentWeights.at<float>(x, y)));
            } else if(mTau->getValue() > 0 && mTauDecay->getValue() > 0) {
              auto timeFactor1 = (delta_t / cedar::unit::Time(mTau->getValue() * cedar::unit::milli * cedar::unit::seconds)) * inputSigmoid.at<float>(x, y);
              auto timeFactor2 = (delta_t / cedar::unit::Time(mTauDecay->getValue() * cedar::unit::milli * cedar::unit::seconds)) * (1 - inputSigmoid.at<float>(x, y));
              weightChangeMat.at<float>(x, y) = (timeFactor1 + timeFactor2) * (learnRate * targetSigmoid.at<float>(0, 0) * rewardSigValue.at<float>(0, 0) * (inputSigmoid.at<float>(x, y) - currentWeights.at<float>(x, y)));
            } else {
              weightChangeMat.at<float>(x, y) = (learnRate * targetSigmoid.at<float>(0, 0) * rewardSigValue.at<float>(0, 0) * (inputSigmoid.at<float>(x, y) - currentWeights.at<float>(x, y)));
            }
          }
        }
        return weightChangeMat;
      }

      if (mAssociationDimension->getValue() == 1 && mInputDimension->getValue() == 1)
      {
        cv::Mat weightChangeMat = cv::Mat::zeros(mWeightSizeX, mWeightSizeY, CV_32F);

        for (unsigned int x = 0; x < mWeightSizeX; x++)
        {
          for (unsigned int y = 0; y < mWeightSizeY; y++)
          {
            if(mTau->getValue() > 0 && mTauDecay->getValue() == 0) {
              auto timeFactor = (delta_t / cedar::unit::Time(mTau->getValue() * cedar::unit::milli * cedar::unit::seconds));
              float combinedValue = inputSigmoid.at<float>(x, 0) * targetSigmoid.at<float>(y, 0);
              float weightChange = learnRate * (combinedValue - currentWeights.at<float>(x, y));
              weightChangeMat.at<float>(x, y) = timeFactor * weightChange;
            } else if(mTau->getValue() > 0 && mTauDecay->getValue() > 0) {
              float combinedValue = inputSigmoid.at<float>(x, 0) * targetSigmoid.at<float>(y, 0);
              auto timeFactor1 = (delta_t / cedar::unit::Time(mTau->getValue() * cedar::unit::milli * cedar::unit::seconds)) * combinedValue;
              auto timeFactor2 = (delta_t / cedar::unit::Time(mTauDecay->getValue() * cedar::unit::milli * cedar::unit::seconds)) * (1 - combinedValue);
              float weightChange = learnRate * (combinedValue - currentWeights.at<float>(x, y));
              weightChangeMat.at<float>(x, y) =  (timeFactor1 + timeFactor2) * weightChange;
            } else {
              float combinedValue = inputSigmoid.at<float>(x, 0) * targetSigmoid.at<float>(y, 0);
              float weightChange = learnRate * (combinedValue - currentWeights.at<float>(x, y));
              weightChangeMat.at<float>(x, y) =  weightChange;
            }
          }
        }
        return weightChangeMat;
      }
      //This should not happen!
      return cv::Mat::zeros(mWeightSizeX,mWeightSizeY,CV_32F);
    }
    case cedar::dyn::steps::HebbianConnection::LearningRule::BCM:
    {
      //Let's try naive method first and compare with optimization later
      if (currentWeights.dims == 4) //Right now only Raul's case needs to work
      {
        std::vector<int> bcmSizes {currentWeights.size[0],currentWeights.size[1],currentWeights.size[2],currentWeights.size[3]};
        cv::Mat weightChangeMat = cv::Mat(4, &bcmSizes.at(0), CV_32F, cv::Scalar(0));
        auto thetaMat = mBCMTheta->getData();

        if (rewardSigValue.at<float>(0, 0) > 0.5f)
        {
          //Update Weights
          float timeFactor = (delta_t / cedar::unit::Time(
                          mTauWeights->getValue() * cedar::unit::milli * cedar::unit::seconds));

          // For the Weightupdate this is faster than the regular for loops. Probably a decent size matrix is required to benefit from parallelization
          // Otherwise the multiple .at calls are probably not ideal
          weightChangeMat.forEach<float>([targetSigmoid,timeFactor,thetaMat,learnRate,inputSigmoid](float &p, const int * position) -> void {
          float sigmoidTarget = targetSigmoid.at<float>(position[0],position[1],position[3]);
          float featureValue = inputSigmoid.at<float>(position[0],position[1],position[2]);
          float thetaValue = thetaMat.at<float>(position[0],position[1],position[3]);
          p =  timeFactor * learnRate * sigmoidTarget * (sigmoidTarget - thetaValue) * (featureValue / thetaValue);
          });
        }

        //Update Theta
        //Todo: Put in separate Function for Optimizing later
        cv::Mat currentWeights = mConnectionWeights->getData();

        if (currentWeights.dims == 4 && !this->mUseFixedTheta->getValue()) // Todo: Change dim check
        {
          auto targetSigmoid = this->mSigmoidH->getValue()->compute(mAssoInput->getData());
          auto thetaMat = mBCMTheta->getData();
          float timeFactor = (delta_t/cedar::unit::Time(mTauTheta->getValue()* cedar::unit::milli * cedar::unit::seconds));
          float minTheta = (float) this->mMinThetaValue->getValue();


// This should be the fastest way, but somehow it takes longer. Maybe try on non-VM
//          thetaMat.forEach<float>([targetSigmoid,timeFactor,minTheta](float &p, const int * position) -> void {
//            float sigmoidTarget = targetSigmoid.at<float>(position[0],position[1],position[2]);
//              float squareTarget = sigmoidTarget * sigmoidTarget;
//              p =  std::max(minTheta, p +  timeFactor * ( squareTarget -p));
//          });

          for (int x = 0; x < currentWeights.size[0]; x++)
          {
            for (int y = 0; y < currentWeights.size[1]; y++)
            {
              for (int d = 0; d < currentWeights.size[3]; d++)
              {
                float sigmoidTarget = targetSigmoid.at<float>(x, y, d);
                float squareTarget = sigmoidTarget * sigmoidTarget;
                thetaMat.at<float>(x, y, d) = std::max( thetaMat.at<float>(x, y, d)  + timeFactor *  ( squareTarget - thetaMat.at<float>(x, y, d)), minTheta);
              }
            }
          }
          mBCMTheta->setData(thetaMat);
        }
        return weightChangeMat;
      }
      else
      {
        std::cout<<"The weight Matrix has not been initialized correctly. BCM expects dimensionality of 4. Current dimensionality is: " << currentWeights.dims << std::endl;
        break;
      }

    }
  }
  //This should not happen!
  return cv::Mat::zeros(mWeightSizeX,mWeightSizeY,CV_32F);
}

 cv::Mat cedar::dyn::steps::HebbianConnection::calculateOutputMatrix(cv::Mat inputMatrix)
{
//  std::cout<<"cedar::dyn::steps::HebbianConnection::calculateOutputMatrix with inputMatrix of dimensionality: " << inputMatrix.dims << std::endl;
  cv::Mat currentWeights = mConnectionWeights->getData();

  switch(this->mLearningRule->getValue())
  {
    case cedar::dyn::steps::HebbianConnection::LearningRule::OJA:
    {

      if (mInputDimension->getValue() == 0) // The old case! && And the 0 to 0 case! Be careful!
      {
        return currentWeights * mSigmoidF->getValue()->compute(inputMatrix.at<float>(0, 0));
      }

      if (mAssociationDimension->getValue() == 0) // The reverse case
      {
        cv::Mat overlap = currentWeights.mul(mSigmoidF->getValue()->compute(inputMatrix));
        cv::Mat outputMat = cv::Mat::zeros(1, 1, CV_32F);
        double overlapDouble = cv::sum(overlap)[0];
        outputMat.at<float>(0, 0) = (float) overlapDouble; //TODO: The output might get very large
        return outputMat;
      }

      if (mAssociationDimension->getValue() == 1 && mInputDimension->getValue() == 1)
      {
        cv::Mat outPutMatrix = cv::Mat::zeros(mWeightSizeY, 1, CV_32F);


        for (unsigned int y = 0; y < mWeightSizeY; y++)
        {
          float summedValue = 0;
          for (unsigned int x = 0; x < mWeightSizeX; x++)
          {
            summedValue += inputMatrix.at<float>(x, 0) * currentWeights.at<float>(x, y);
          }

          outPutMatrix.at<float>(y, 0) = summedValue; //Todo: Probably also some normalization
        }

        return outPutMatrix;
      }
      break;
    }


    case cedar::dyn::steps::HebbianConnection::LearningRule::BCM:
    {
      //Right now only Raul's case needs to work
      if (currentWeights.dims == 4 &&  mAssociationSizes->getValue().at(0) == mInputSizes->getValue().at(0) && mAssociationSizes->getValue().at(1) == mInputSizes->getValue().at(1) )
      {
        std::vector<int> outPutSizes{(int)mAssociationSizes->getValue().at(0),(int) mAssociationSizes->getValue().at(1), (int)mAssociationSizes->getValue().at(2)};
        cv::Mat outPutMatrix = cv::Mat(3,&outPutSizes.at(0) , CV_32F, cv::Scalar(0));

        //The Matrix Multiplication avoids iterating through the feature dimension and halved the computation time on my VM
        //Someone who knows more about reshaping OpenCV Matrices and Multiplication in more than 2D can probably improve this code even further.
        for (int x = 0; x < currentWeights.size[0]; x++)
        {
          for (int y = 0; y < currentWeights.size[1]; y++)
          {
            //Extract the 2D matrices out of the 3D and 4D Tensor
            std::vector<cv::Range> rangesFeature{cv::Range(x, x+1), cv::Range(y, y+1), cv::Range(0, currentWeights.size[2])};
            cv::Mat subFeatureMat = cv::Mat(inputMatrix, rangesFeature);
            std::vector<int> twoDMatrixSizeFeature {currentWeights.size[2],1};
            cv::Mat subFeatureMat2D = subFeatureMat.reshape(1,2,&twoDMatrixSizeFeature.at(0));

            std::vector<cv::Range> rangesWeights{cv::Range(x, x+1), cv::Range(y, y+1), cv::Range(0, currentWeights.size[2]),cv::Range(0, currentWeights.size[3])};
            cv::Mat subWeightMat4D = cv::Mat(currentWeights, rangesWeights);
            std::vector<int> twoDMatrixSizeWeight{currentWeights.size[2],currentWeights.size[3]};
            cv::Mat subWeightMat2D = subWeightMat4D.reshape(1,2,&twoDMatrixSizeWeight.at(0));

            //A lot of reshaping to do this Matrix Multiplication :-D
            cv::Mat outPutMatrix2D = subFeatureMat2D.t() * subWeightMat2D;

            for(int d=0;d<outPutMatrix2D.size[1];d++)
            {
              outPutMatrix.at<float>(x,y,d) = outPutMatrix2D.at<float>(0,d);
            }
          }
        }
        return outPutMatrix;
      }
      else
      {
//        std::cout<<"!Your current setting for the BCM learn rule is not yet implemented.\nCurrent implementation requires a 3D Input and a 3D Association, which have the same sizes in the first two input dimensions"<< std::endl;
      }
    }
  }

  return cv::Mat::zeros(mWeightSizeX,mWeightSizeY,CV_32F);

}

void cedar::dyn::steps::HebbianConnection::toggleFixedTheta()
{
  this->mFixedThetaValue->setConstant(!this->mUseFixedTheta->getValue());
  if(this->mUseFixedTheta->getValue())
  {
    this->applyFixedTheta();
  }
}

void cedar::dyn::steps::HebbianConnection::applyFixedTheta()
{
  cv::Mat thetaMat = this->mBCMTheta->getData();
  float fixedTheta = this->mFixedThetaValue->getValue();

  for (int x = 0; x < thetaMat.size[0]; x++)
  {
    for (int y = 0; y < thetaMat.size[1]; y++)
    {
      for (int d = 0; d < thetaMat.size[2]; d++)
      {
        thetaMat.at<float>(x,y,d) = fixedTheta; //at should not be used, but pointer arithmetic, but ideally the fastest way should be foreach ...
      }
    }
  }
//  // Parallel execution using C++11 lambda. Slower for some weird reason? Todo: Try  on non-virtual machine
//  thetaMat.forEach<float>([fixedTheta](float &p, const int * /*position*/) -> void {
//      p = fixedTheta;
//  });

  mBCMTheta->setData(thetaMat);
}

void cedar::dyn::steps::HebbianConnection::updateInputSizesRange()
{
  // Set the sizes annotation
  std::vector<cedar::aux::math::Limits<double>> sizesRange;
  for(unsigned int size : this->mInputSizes->getValue())
  {
    sizesRange.push_back(cedar::aux::math::Limits<double>(0, size - 1));
  }
  CEDAR_ASSERT(this->mConnectionWeights);
  this->mConnectionWeights->setAnnotation(cedar::aux::annotation::AnnotationPtr(
          new cedar::aux::annotation::SizesRangeHint(sizesRange)));
}

void cedar::dyn::steps::HebbianConnection::updateAssociationSizesRange()
{
  // Set the sizes annotation
  std::vector<cedar::aux::math::Limits<double>> sizesRange;
  for(unsigned int size : this->mAssociationSizes->getValue())
  {
    sizesRange.push_back(cedar::aux::math::Limits<double>(0, size - 1));
  }
  CEDAR_ASSERT(this->mWeightOutput);
  this->mWeightOutput->setAnnotation(cedar::aux::annotation::AnnotationPtr(
          new cedar::aux::annotation::SizesRangeHint(sizesRange)));
}
