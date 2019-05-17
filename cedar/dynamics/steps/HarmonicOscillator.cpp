/*
 * HarmonicOscillator.cpp
 *
 *  Created on: Apr 4, 2014
 *      Author: tekuejpp
 */

#include "cedar/dynamics/steps/HarmonicOscillator.h"
#include <cedar/auxiliaries/MatData.h>
#include "cedar/processing/Arguments.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"


namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration
            (
                    new ElementDeclarationTemplate<cedar::dyn::steps::HarmonicOscillator>
                            (
                                    "Numerics",
                                    "cedar.dynamics.HarmonicOscillator"
                            )
            );
    declaration->setIconPath(":/steps/harmonic_oscillator.svg");
    declaration->setDescription
            (
                    "A looped step that employs the harmonic Oscillator Formula pdotdot = -K(p-x)-B*pdot. K and B are automatically set to the critically damped solution."
            );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}


cedar::dyn::steps::HarmonicOscillator::HarmonicOscillator()
    :
      mOutput(new cedar::aux::MatData(cv::Mat::zeros(2, 1, CV_32F))),
      mOutputDot(new cedar::aux::MatData(cv::Mat::zeros(2, 1, CV_32F))),
      mOutputDotDot(new cedar::aux::MatData(cv::Mat::zeros(2, 1, CV_32F))),
      _mDimensionality(new cedar::aux::UIntParameter(this, "dimensionality", 2, 1, 7)),
      _initPos(new cedar::aux::DoubleVectorParameter(this, "Initial Position", 2, 4)),
      _K(new cedar::aux::DoubleVectorParameter(this, "K", 2, 4)),
      _B(new cedar::aux::DoubleVectorParameter(this, "B", 2, 4)),
      _D(new cedar::aux::DoubleParameter(this, "D", 1)),
      _cyclicRestrictions(new cedar::aux::BoolParameter(this, "cyclic restrictions", false)),
      _lowerRestrictions(new cedar::aux::DoubleVectorParameter(this, "lower restrictions", 1, 0)),
      _upperRestrictions(new cedar::aux::DoubleVectorParameter(this, "upper restrictions", 1, 0)),
      _mPData(new cedar::aux::MatData(cv::Mat(2, 1, CV_32F, _initPos->getDefaultValue()))),
      _mPDotData(new cedar::aux::MatData(cv::Mat::zeros(2, 1, CV_32F)))
{
  this->declareInput("lambda");
  this->declareInput("overwriteInput", false);
  this->declareInput("overwritePeakDetector", false);

  this->declareOutput("p", mOutput);
  this->declareOutput("pDot", mOutputDot);
  this->declareOutput("pDotDot", mOutputDotDot);

  QObject::connect(_initPos.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrixSize()));
  QObject::connect(_K.get(), SIGNAL(valueChanged()), this, SLOT(updateDampingK()));
  QObject::connect(_B.get(), SIGNAL(valueChanged()), this, SLOT(updateDampingB()));
  QObject::connect(_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(updateDimensionality()));

}

cedar::dyn::steps::HarmonicOscillator::~HarmonicOscillator()
{
}

void cedar::dyn::steps::HarmonicOscillator::eulerStep(const cedar::unit::Time& time)
{

  cedar::aux::ConstDataPtr lambda = this->getInputSlot("lambda")->getData();

  cv::Mat& output = mOutput->getData();

  cv::Mat& outputDot = mOutputDot->getData();

  cv::Mat& outputDotDot = mOutputDotDot->getData();

  cv::Mat& pDot = _mPDotData->getData();
  cv::Mat& p = _mPData->getData();

  if (boost::dynamic_pointer_cast<const cedar::aux::MatData>(lambda))
  {
    cv::Mat lambdaMat = lambda->getData<cv::Mat>().clone();
    cv::Mat pDotDot = cv::Mat::zeros(_mDimensionality->getValue(), 1, CV_32F);

    cedar::aux::ConstDataPtr overWriteInput = this->getInputSlot("overwriteInput")->getData();
    cedar::aux::ConstDataPtr overWritePeakDetector = this->getInputSlot("overwritePeakDetector")->getData();
    if (boost::dynamic_pointer_cast<const cedar::aux::MatData>(overWriteInput) && boost::dynamic_pointer_cast<const cedar::aux::MatData>(overWritePeakDetector))
    {
      auto overWriteMat = overWriteInput->getData<cv::Mat>().clone();
      auto overWritePeak = overWritePeakDetector->getData<cv::Mat>().clone();
      if (overWritePeak.at<float>(0, 0) > 0.5)
      {
        p = overWriteMat;
        lambdaMat = overWriteMat;
      }
    }

    cv::Mat K = cv::Mat::zeros(_mDimensionality->getValue(), _mDimensionality->getValue(), CV_32F);
    cv::Mat B = cv::Mat::zeros(_mDimensionality->getValue(), _mDimensionality->getValue(), CV_32F);
    for (unsigned int d = 0; d < _mDimensionality->getValue(); d++)
    {
      float newK = _K->getValue().at(d);
      K.at<float>(d, d) = newK;
      B.at<float>(d, d) = sqrt(newK) * 2 * _D->getValue();
    }

    cv::Mat difMat = p - lambdaMat;
    if (_cyclicRestrictions->getValue())
    {
      for (unsigned int i = 0; i < _mDimensionality->getValue(); i++)
      {
        if (abs(p.at<float>(i, 0) - lambdaMat.at<float>(i, 0)) > ((_upperRestrictions->getValue().at(i) + _lowerRestrictions->getValue().at(i)) / 2.0))
        {
          if (p.at<float>(i, 0) < lambdaMat.at<float>(i, 0))
            difMat.at<float>(i, 0) = p.at<float>(i, 0) - lambdaMat.at<float>(i, 0) + _upperRestrictions->getValue().at(i);
          else
            difMat.at<float>(i, 0) = p.at<float>(i, 0) - lambdaMat.at<float>(i, 0) - _upperRestrictions->getValue().at(i);
        }
        else
        {
          difMat.at<float>(i, 0) = p.at<float>(i, 0) - lambdaMat.at<float>(i, 0);
        }
      }

    }

//    std::cout << "Lambda: " << lambdaMat << std::endl;
    pDotDot = (-1 * (K) * (difMat)) - ((B) * pDot);

//    std::cout << "PDotDot: " << pDotDot << std::endl;

    pDot += (time / cedar::unit::Time(1.0 * cedar::unit::second)) * pDotDot;
//    std::cout << "PDot: " << pDot << std::endl;

    p += (time / cedar::unit::Time(1.0 * cedar::unit::second)) * pDot;

//    std::cout << "P: " << p << std::endl;

    if (_cyclicRestrictions->getValue())
    {
      for (unsigned int i = 0; i < _mDimensionality->getValue(); i++)
      {
        if (p.at<float>(i, 0) > _upperRestrictions->getValue().at(i))
        {
          double dif = p.at<float>(i, 0) - _upperRestrictions->getValue().at(i);
          p.at<float>(i, 0) = _lowerRestrictions->getValue().at(i) + dif;
        }

        if (p.at<float>(i, 0) < _lowerRestrictions->getValue().at(i))
        {
          double dif = p.at<float>(i, 0) - _lowerRestrictions->getValue().at(i);
          p.at<float>(i, 0) = _upperRestrictions->getValue().at(i) + dif;
        }
      }
    }

    output = p;
    outputDot = pDot;
    outputDotDot = pDotDot;
  }
}

cedar::aux::MatDataPtr cedar::dyn::steps::HarmonicOscillator::getCurrentPosition()
{
  return this->_mPData;
}

cedar::aux::MatDataPtr cedar::dyn::steps::HarmonicOscillator::getCurrentVelocity()
{
  return this->_mPDotData;
}

void cedar::dyn::steps::HarmonicOscillator::setK(unsigned int dimension, double k)
{
  CEDAR_ASSERT(dimension < this->_K->size());
  _K->setValue(dimension, k);
  updateDampingK();
}

void cedar::dyn::steps::HarmonicOscillator::setB(unsigned int dimension, double b)
{
  CEDAR_ASSERT(dimension < this->_B->size());
  _B->setValue(dimension, b);
  updateDampingB();
}

void cedar::dyn::steps::HarmonicOscillator::setK(double k)
{
  for (unsigned int d = 0; d < _mDimensionality->getValue(); d++)
  {
    _K->setValue(d, k);
  }
  updateDampingK();
}

void cedar::dyn::steps::HarmonicOscillator::setB(double b)
{
  for (unsigned int d = 0; d < _mDimensionality->getValue(); d++)
  {
    _B->setValue(d, b);
  }
  updateDampingB();
}

std::vector<double> cedar::dyn::steps::HarmonicOscillator::getK()
{
  return _K->getValue();
}

std::vector<double> cedar::dyn::steps::HarmonicOscillator::getB()
{
  return _B->getValue();
}

void cedar::dyn::steps::HarmonicOscillator::setInitPos(float dimension, float value)
{
  _initPos->setValue(dimension, value);
}

void cedar::dyn::steps::HarmonicOscillator::updateMatrixSize()
{
  cv::Mat& _mPDot = _mPDotData->getData();
  cv::Mat& _mP = _mPData->getData();

  for (unsigned int i = 0; i < _mDimensionality->getValue(); i++)
  {
    _mP.at<float>(i, 0) = _initPos->getValue().at(i);
  }
  _mPDot = cv::Mat::zeros(_mDimensionality->getValue(), 1, CV_32F);

  this->emitOutputPropertiesChangedSignal("p");

}

void cedar::dyn::steps::HarmonicOscillator::updateDimensionality()
{
  int new_dimensionality = static_cast<int>(_mDimensionality->getValue());
  _initPos->resize(new_dimensionality, _initPos->getDefaultValue());
  _initPos->setDefaultSize(new_dimensionality);
  _K->resize(new_dimensionality, _K->getDefaultValue());
  _K->setDefaultSize(new_dimensionality);
  _B->resize(new_dimensionality, _B->getDefaultValue());
  _B->setDefaultSize(new_dimensionality);

  cv::Mat& _mPDot = _mPDotData->getData();
  cv::Mat& _mP = _mPData->getData();
  _mP = cv::Mat::zeros(new_dimensionality, 1, CV_32F);

  for (int i = 0; i < new_dimensionality; i++)
  {
    _mP.at<float>(i, 0) = _initPos->getValue().at(i);
  }
  _mPDot = cv::Mat::zeros(new_dimensionality, 1, CV_32F);

  cv::Mat& output = mOutput->getData();
  output = cv::Mat::zeros(new_dimensionality, 1, CV_32F);

  cv::Mat& outputdot = mOutputDot->getData();
  outputdot = cv::Mat::zeros(new_dimensionality, 1, CV_32F);

  cv::Mat& outputdotdot = mOutputDotDot->getData();
  outputdotdot = cv::Mat::zeros(new_dimensionality, 1, CV_32F);

  this->emitOutputPropertiesChangedSignal("p");
  this->onTrigger();
}

void cedar::dyn::steps::HarmonicOscillator::updateDampingB()
{
  std::vector<double> bNew = _B->getValue();

  for (unsigned int i = 0; i < bNew.size(); i++)
  {
    _K->setValue(i, (bNew.at(i) / (2 * _D->getValue())) * (bNew.at(i) / (2 * _D->getValue())));
  }

}

void cedar::dyn::steps::HarmonicOscillator::updateDampingK()
{
  std::vector<double> kNew = _K->getValue();

  for (unsigned int i = 0; i < kNew.size(); i++)
  {
    if (kNew.at(i) < 0)
      kNew.at(i) = 0;
    _B->setValue(i, sqrt(kNew.at(i)) * 2 * _D->getValue());
  }

}

void cedar::dyn::steps::HarmonicOscillator::reset()
{
  cv::Mat& _mPDot = _mPDotData->getData();
  cv::Mat& _mP = _mPData->getData();
  _mPDot = cv::Mat::zeros(_mDimensionality->getValue(), 1, CV_32F);
  for (unsigned int i = 0; i < _mDimensionality->getValue(); i++)
  {
    _mP.at<float>(i, 0) = _initPos->getValue().at(i);
  }
}

cedar::proc::DataSlot::VALIDITY cedar::dyn::steps::HarmonicOscillator::determineInputValidity(cedar::proc::ConstDataSlotPtr slot, cedar::aux::ConstDataPtr data) const
{
  if (cedar::aux::ConstMatDataPtr input = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
  {
    if (input && input->getDimensionality() == 1 && cedar::aux::math::get1DMatrixSize(input->getData()) == _mDimensionality->getValue() && (slot->getName() == "lambda" || slot->getName() == "overwriteInput"))
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
    if (input->getDimensionality() == 0 && slot->getName() == "overwritePeakDetector")
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }

  return cedar::proc::DataSlot::VALIDITY_ERROR;

}
