/*
 * WeightedSum.cpp
 *
 *  Created on: Mar 10, 2014
 *      Author: Jan Tekuelve
 */

#include "cedar/processing/steps/WeightedSum.h"
#include <cedar/auxiliaries/MatData.h>
#include "cedar/processing/Arguments.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include <cstdlib>
#include <ctime>


//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration
            (
                    new ElementDeclarationTemplate<cedar::proc::steps::WeightedSum>
                            (
                                    "Algebra",
                                    "cedar.processing.WeightedSum"
                            )
            );
    declaration->setIconPath(":/steps/weighted_sum.svg");
    declaration->setDescription
            (
                    "A looped step that multiplies each input value with a given weight and sums them up across each input dimension. The given weight function encodes the distance from the center of the input matrix. This step is used to calculate a velocity signal dependent on the distance between a peak position and the center of the input matrix."
            );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

cedar::aux::EnumType<cedar::proc::steps::WeightedSum::FunctionType> cedar::proc::steps::WeightedSum::FunctionType::mType("FunctionType::");

cedar::proc::steps::WeightedSum::WeightedSum()
    :
      mOutput(new cedar::aux::MatData(cv::Mat::zeros(2, 1, CV_32F))),
      mOutputTangential(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
      mWeightsX(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
      mWeightsY(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
      _mDimensionality(new cedar::aux::UIntParameter(this, "dimensionality", 2, 1, 2)),
      _mSizes(new cedar::aux::UIntVectorParameter(this, "sizes", 2, 99)),
      _maxWeightValues(new cedar::aux::DoubleVectorParameter(this, "maxWeightValues", 2, 1)),
      _mFunctionType(
          new cedar::aux::EnumParameter(this, "function type", cedar::proc::steps::WeightedSum::FunctionType::typePtr(),
              FunctionType::Linear)),
      _mFunctionParameter1(new cedar::aux::DoubleParameter(this, "noise_percentage", 0, 0, 1)),
      _mFunctionParameter2(new cedar::aux::DoubleParameter(this, "noise lower bound", 1)),
      _mFunctionParameter3(new cedar::aux::DoubleParameter(this, "noise upper bound", 1))

{

  initAllWeights();
  this->declareBuffer("weightsX", mWeightsX);
  this->declareBuffer("weightsY", mWeightsY);

  this->declareInput("uPlan");

  this->declareOutput("velocity", mOutput);

  this->declareOutput("tangential velocity", mOutputTangential);

  QObject::connect(_mSizes.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrixSize()));
  QObject::connect(_maxWeightValues.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrixSize()));
  QObject::connect(_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(updateDimensionality()));
  QObject::connect(_mFunctionType.get(), SIGNAL(valueChanged()), this, SLOT(updateFunctionType()));
  QObject::connect(_mFunctionParameter1.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrixSize()));
  QObject::connect(_mFunctionParameter2.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrixSize()));
  QObject::connect(_mFunctionParameter3.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrixSize()));
  srand(static_cast<unsigned>(time(0)));

}

cedar::proc::steps::WeightedSum::~WeightedSum()
{
  // TODO Auto-generated destructor stub
}

void cedar::proc::steps::WeightedSum::compute(const cedar::proc::Arguments& /*arguments*/)
{
  cv::Mat& output = mOutput->getData();
  cv::Mat& tangOutput = mOutputTangential->getData();

  cedar::aux::ConstDataPtr uPlan = this->getInputSlot("uPlan")->getData();
  if (boost::dynamic_pointer_cast<const cedar::aux::MatData>(uPlan))
  {
    cv::Mat inputValueMat = uPlan->getData<cv::Mat>().clone();

    unsigned int rowCount = inputValueMat.rows;
    unsigned int colCount = inputValueMat.cols;

    cv::Mat weightedSum = cv::Mat::zeros(_mDimensionality->getValue(), 1, CV_32F);

    for (unsigned int i = 0; i < rowCount; i++)
    {
      for (unsigned int j = 0; j < colCount; j++)
      {
        float inputValue = inputValueMat.at<float>(i, j);

        cv::Mat weights = cv::Mat::zeros(2, 1, CV_32F);
        weights.at<float>(0, 0) = mWeightsX->getData().at<float>(i, 0);
        weights.at<float>(1, 0) = mWeightsY->getData().at<float>(j, 0);

        for (unsigned int d = 0; d < _mDimensionality->getValue(); d++)
        {
          weightedSum.at<float>(d, 0) = weightedSum.at<float>(d, 0) + weights.at<float>(d, 0) * inputValue;
        }
      }
    }

    float tangentialSum = 0;
    for (unsigned int d = 0; d < _mDimensionality->getValue(); d++)
    {
      float curValue = weightedSum.at<float>(d, 0);
      tangentialSum = tangentialSum + (curValue * curValue);
    }

    output = weightedSum;
    tangOutput.at<float>(0, 0) = sqrt(tangentialSum);
  }
}

void cedar::proc::steps::WeightedSum::initAllWeights()
{
  initWeights(mWeightsX, 0);
  cv::Mat& weightsY = mWeightsY->getData();
  weightsY = cv::Mat::zeros(1, 1, CV_32F);
  if (_mDimensionality->getValue() > 1)
  {
    initWeights(mWeightsY, 1);
  }
}

void cedar::proc::steps::WeightedSum::initWeights(cedar::aux::MatDataPtr weightPtr, int index)
{
  cv::Mat& weights = weightPtr->getData();

  float maxValue = _maxWeightValues->getValue().at(index);
  unsigned int size = _mSizes->getValue().at(index);
  cv::Mat wMat = cv::Mat::zeros(size, 1, CV_32F);
  float middle = (size / 2) + 1;
  switch (_mFunctionType->getValue())
  {
    //Randomize _mFunctionParameter1 Percent of the weight matrix.
    case FunctionType::Noisy:
      for (unsigned int i = 0; i < size; i++)
      {
        float distance = (i - middle);
        float wI;
        if (abs(distance) <= (((((double) size) / 2) + 1) * (1 - _mFunctionParameter1->getValue())))
        {
          wI = (distance / middle) * maxValue;
        }
        else
        {
          float LOW = _mFunctionParameter2->getValue();
          float HIGH = _mFunctionParameter3->getValue();
          float random = LOW + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (HIGH - LOW)));
          wI = random;
          if (distance < 0)
            wI = wI * -1;
        }

        wMat.at<float>(i, 0) = wI;
      }
      break;
    case FunctionType::Linear:

      for (unsigned int i = 0; i < size; i++)
      {
        float distance = (i - middle);
        float wI = (distance / middle) * maxValue;
        wMat.at<float>(i, 0) = wI;
      }
      break;

  }

  weights = wMat;
}

void cedar::proc::steps::WeightedSum::setMaxWeightValues(unsigned int dimension, double weightValue)
{
  CEDAR_ASSERT(dimension < this->_maxWeightValues->size());
  _maxWeightValues->setValue(dimension, weightValue);
  initAllWeights();

}

void cedar::proc::steps::WeightedSum::updateMatrixSize()
{
  this->lock(cedar::aux::LOCK_TYPE_READ);
  initAllWeights();
  this->unlock();
}

void cedar::proc::steps::WeightedSum::updateFunctionType()
{
  switch (_mFunctionType->getValue())
  {
    case FunctionType::Linear:
      _mFunctionParameter1->setConstant(true);
      _mFunctionParameter2->setConstant(true);
      _mFunctionParameter3->setConstant(true);
      break;
    case FunctionType::Noisy:
      _mFunctionParameter1->setConstant(false);
      _mFunctionParameter2->setConstant(false);
      _mFunctionParameter3->setConstant(false);
      break;
  }

  this->lock(cedar::aux::LOCK_TYPE_READ);
  initAllWeights();
  this->unlock();
}

void cedar::proc::steps::WeightedSum::updateDimensionality()
{
  int new_dimensionality = static_cast<int>(_mDimensionality->getValue());
  _mSizes->resize(new_dimensionality, _mSizes->getDefaultValue());
  _mSizes->setDefaultSize(new_dimensionality);
  _maxWeightValues->resize(new_dimensionality, _maxWeightValues->getDefaultValue());
  _maxWeightValues->setDefaultSize(new_dimensionality);
  initAllWeights();
  cv::Mat& output = mOutput->getData();
  output = cv::Mat::zeros(new_dimensionality, 1, CV_32F);
}

void cedar::proc::steps::WeightedSum::reset()
{
  srand(static_cast<unsigned>(time(0)));
  initAllWeights();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::WeightedSum::determineInputValidity(
    cedar::proc::ConstDataSlotPtr,
    cedar::aux::ConstDataPtr data) const
{
  if (cedar::aux::ConstMatDataPtr input = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
  {
    unsigned int matrixRows = input->getData().rows;
    unsigned int matrixColumns = input->getData().cols;
    if ((matrixRows % 2 == 0) || (matrixColumns % 2 == 0))
      return cedar::proc::DataSlot::VALIDITY_ERROR;

    if (matrixRows == _mSizes->getValue().front())
    {
      if (_mDimensionality->getValue() > 1)
      {
        if (matrixColumns == _mSizes->getValue().at(1))
          return cedar::proc::DataSlot::VALIDITY_VALID;
      }
      else
        return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }

  return cedar::proc::DataSlot::VALIDITY_ERROR;

}
