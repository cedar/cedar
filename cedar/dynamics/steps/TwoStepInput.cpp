/*======================================================================================================================

 Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

 File:        TwoStepInput.cpp

 Maintainer:  Oliver Lomp,
 Mathis Richter,
 Stephan Zibner
 Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
 mathis.richter@ini.ruhr-uni-bochum.de,
 stephan.zibner@ini.ruhr-uni-bochum.de
 Date:        2011 07 19

 Description:

 Credits:

 ======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/dynamics/steps/TwoStepInput.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/math/functions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <iostream>
#include <vector>

namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration
            (
                    new ElementDeclarationTemplate<cedar::dyn::steps::TwoStepInput>
                            (
                                    "Sources",
                                    "cedar.dynamics.steps.TwoStepInput"
                            )
            );
    declaration->setIconPath(":/steps/two_step_input.svg");
    declaration->setDescription("A special GaussInput that allows to switch the center of the Gaussian after a specific amount of time. This might represent a stimulus in the two-step paradigm");
    declaration->deprecatedName("cedar.dynamics.steps.TwoStepInput");

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dyn::steps::TwoStepInput::TwoStepInput()
    :
      mOutput(new cedar::aux::MatData(cv::Mat::zeros(50, 50, CV_32F))),
      _mAmplitude(new cedar::aux::DoubleParameter(this, "amplitude", 1.0)),
      _mTimetilChange(new cedar::aux::DoubleParameter(this, "ISI", 500)),
      _mDimensionality(new cedar::aux::UIntParameter(this, "dimensionality", 2, 1, 4)),
      _mSigmas(new cedar::aux::DoubleVectorParameter(this, "sigma", 2, 3.0, 0.01, 1000.0)),
      _mCenters(new cedar::aux::DoubleVectorParameter(this, "centers", 2, 10.0, -10000.0, 10000.0)),
      _mSecondCenters(new cedar::aux::DoubleVectorParameter(this, "secondCenters", 2, 40.0, -10000.0, 10000.0)),
      _mSizes(new cedar::aux::UIntVectorParameter(this, "sizes", 2, 50, 1, 1000.0)),
      _mIsCyclic(new cedar::aux::BoolParameter(this, "cyclic", false)),
//      _mStartBlank(new cedar::aux::BoolParameter(this, "start blank", false)),
      _mIsContinuous(new cedar::aux::BoolParameter(this, "continuous move", false)),
      _mStopMoving(new cedar::aux::BoolParameter(this, "stop moving", true)),
      _mMovementTime(new cedar::aux::DoubleParameter(this, "MovementTime", 500)),
      _backAndForth(new cedar::aux::BoolParameter(this, "Move Back and Forth", false)),
      _externalSwitch(new cedar::aux::BoolParameter(this, "external Switch", false))
{
  this->declareOutput("twostep", mOutput);
  this->declareInput("start_countdown", false);
  this->declareInput("external_switch", false);

  QObject::connect(_mSizes.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrixSize()));
  QObject::connect(_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(updateDimensionality()));


  curCenter = _mCenters->getValue();
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dyn::steps::TwoStepInput::setDimensionality(unsigned int dimensionality)
{
  this->_mDimensionality->setValue(dimensionality);
}

void cedar::dyn::steps::TwoStepInput::setSize(unsigned int dimension, unsigned int size)
{
  this->_mSizes->setValue(dimension, size);
}

void cedar::dyn::steps::TwoStepInput::setCenter(unsigned int dimension, double center)
{
  CEDAR_ASSERT(dimension < this->_mCenters->size());
  this->_mCenters->setValue(dimension, center);
}

void cedar::dyn::steps::TwoStepInput::setSecondCenter(unsigned int dimension, double center)
{
  CEDAR_ASSERT(dimension < this->_mSecondCenters->size());
  this->_mSecondCenters->setValue(dimension, center);
}

double cedar::dyn::steps::TwoStepInput::getCenter(unsigned int dimension)
{
  CEDAR_ASSERT(dimension < this->_mCenters->size());
  return _mCenters->getValue().at(dimension);

}

double cedar::dyn::steps::TwoStepInput::getSecondCenter(unsigned int dimension)
{
  CEDAR_ASSERT(dimension < this->_mSecondCenters->size());
  return _mSecondCenters->getValue().at(dimension);

}

cv::Mat cedar::dyn::steps::TwoStepInput::getCurrentTarget()
{
  cv::Mat targetMat = cv::Mat::zeros(2, 1, CV_32F);
  targetMat.at<float>(0, 0) = curCenter.at(0);
  targetMat.at<float>(1, 0) = curCenter.at(1);

  return targetMat;

}

double cedar::dyn::steps::TwoStepInput::getAmplitude() const
{
  return this->_mAmplitude->getValue();
}

void cedar::dyn::steps::TwoStepInput::setAmplitude(double amplitude)
{
  this->_mAmplitude->setValue(amplitude);
}

void cedar::dyn::steps::TwoStepInput::eulerStep(const cedar::unit::Time& time)
{
  //	std::cout<<"euler Step "<<std::endl;

  float curTime = time / cedar::unit::Time(1 * cedar::unit::milli * cedar::unit::seconds);

  if (mStartTimer)
  {
    if (mStartTimer->getData().at<float>(0, 0) > 0.1)
      elapsedTime += curTime;
    else
      elapsedTime = 0;
  }

  //	std::cout<< "Elapsed Time: " << elapsedTime << std::endl;

  try
  {
//    if (elapsedTime == 0 && _mStartBlank->getValue())
//    {
//
//      this->mOutput->setData(cv::Mat::zeros(50, 50, CV_32F)
//      //					cv::Mat::zeros(_mDimensionality->getValue(),sizes,CV_32F)
//          );
//
//      //			TODO HIER MIT ZEROS INTIALISIEREN !!!!
//    } else

    if (_externalSwitch->getValue() && mSwitchSignal)
    {
      if (!isSwitch && mSwitchSignal->getData().at<float>(0, 0) > 0.5)
      {
//        std::cout<<"Switch is false and Input is > 0.5"<<std::endl;
        isSwitch = true;
        //Exchange current Target!
        if (curCenter.at(0) == _mCenters->getValue().at(0) && curCenter.at(1) == _mCenters->getValue().at(1))
        {
//          std::cout<<"Center was the first one -> Now the Second!"<<std::endl;
          curCenter = _mSecondCenters->getValue();
        }
        else
        {
//          std::cout<<"Center was not first one -> Now the First!"<<std::endl;
          curCenter = _mCenters->getValue();
        }
      }
      else if (isSwitch && mSwitchSignal->getData().at<float>(0, 0) < 0.5)
      {
//        std::cout<<"Switch was on, but the Signal below 0.5! Switch -> Off"<<std::endl;
        isSwitch = false;
      }
    }
    else
    {

      if (elapsedTime < _mTimetilChange->getValue())
      {
        curCenter = _mCenters->getValue();
//      this->mOutput->setData(
//          cedar::aux::math::gaussMatrix(_mDimensionality->getValue(), _mSizes->getValue(), _mAmplitude->getValue(),
//              _mSigmas->getValue(), _mCenters->getValue(), _mIsCyclic->getValue()));
      }
      else if (elapsedTime >= _mTimetilChange->getValue())
      {
        if (!_mIsContinuous->getValue())
        {
          curCenter = _mSecondCenters->getValue();
//        this->mOutput->setData(
//            cedar::aux::math::gaussMatrix(_mDimensionality->getValue(), _mSizes->getValue(), _mAmplitude->getValue(),
//                _mSigmas->getValue(), _mSecondCenters->getValue(), _mIsCyclic->getValue()));
        }
        else
        {
          float oldCenterX = this->getCenter(0);
          float oldCenterY = this->getCenter(1);
          float newCenterX = this->getSecondCenter(0);
          float newCenterY = this->getSecondCenter(1);

          curMovementTime += curTime;

          float factor = curMovementTime / _mMovementTime->getValue();

          if (factor > 1 && _mStopMoving->getValue())
            factor = 1;

          if (factor >= 1 && _backAndForth->getValue())
          {
            curMovementTime = 0;
            factor = 0;
            if (moveForward)
            {
              moveForward = false;

            }
            else
            {
              moveForward = true;
            }
          }

          float newPosX, newPosY;
          if (moveForward)
          {
            newPosX = oldCenterX + (factor * (newCenterX - oldCenterX));
            newPosY = oldCenterY + (factor * (newCenterY - oldCenterY));
          }
          else
          {
            newPosX = newCenterX - (factor * (newCenterX - oldCenterX));
            newPosY = newCenterY - (factor * (newCenterY - oldCenterY));
          }

          std::vector<double> transientCenter;
          transientCenter.push_back(newPosX);
          transientCenter.push_back(newPosY);

          curCenter = transientCenter;

//        this->mOutput->setData(
//            cedar::aux::math::gaussMatrix(_mDimensionality->getValue(), _mSizes->getValue(), _mAmplitude->getValue(),
//                _mSigmas->getValue(), transientCenter, _mIsCyclic->getValue()));

        }

      }
    }

    auto setCenter = curCenter;
    this->mOutput->setData(
        cedar::aux::math::gaussMatrix(_mDimensionality->getValue(), _mSizes->getValue(), _mAmplitude->getValue(),
            _mSigmas->getValue(), setCenter, _mIsCyclic->getValue()));

  }
  catch (std::out_of_range& exc)
  {
    // this might happen if TwoStepInput is triggered and dimensionality is changed, just ignore
  }

}

void cedar::dyn::steps::TwoStepInput::setISI(double isi)
{
  this->_mTimetilChange->setValue(isi);
}

void cedar::dyn::steps::TwoStepInput::setMovementTime(double time)
{
  this->_mMovementTime->setValue(time);
}

void cedar::dyn::steps::TwoStepInput::setContinuous(bool isContinous)
{
  this->_mIsContinuous->setValue(isContinous);
}

void cedar::dyn::steps::TwoStepInput::setStopMoving(bool doesStop)
{
  this->_mStopMoving->setValue(doesStop);
}

void cedar::dyn::steps::TwoStepInput::setStartBlank(bool startBlank)
{
  this->_mStopMoving->setValue(startBlank);
}

void cedar::dyn::steps::TwoStepInput::updateMatrix()
{
  //	this->onTrigger();
}



void cedar::dyn::steps::TwoStepInput::updateDimensionality()
{
  //	std::cout<<"update Dimensionality "<<std::endl;

  int new_dimensionality = static_cast<int>(_mDimensionality->getValue());
  _mSigmas->resize(new_dimensionality, _mSigmas->getDefaultValue());
  _mSigmas->setDefaultSize(new_dimensionality);
  _mCenters->resize(new_dimensionality, _mCenters->getDefaultValue());
  _mCenters->setDefaultSize(new_dimensionality);
  _mSecondCenters->resize(new_dimensionality, _mSecondCenters->getDefaultValue());
  _mSecondCenters->setDefaultSize(new_dimensionality);
  _mSizes->resize(new_dimensionality, _mSizes->getDefaultValue());
  _mSizes->setDefaultSize(new_dimensionality);
  //	this->lock(cedar::aux::LOCK_TYPE_READ);
  //	//	this->compute(cedar::proc::Arguments());
  this->reset();
  //	this->unlock();
  //	this->emitOutputPropertiesChangedSignal("twostep");
  //	this->onTrigger();

}

void cedar::dyn::steps::TwoStepInput::updateMatrixSize()
{
  //	std::cout<<"update MatrixSize "<<std::endl;

  //	this->lock(cedar::aux::LOCK_TYPE_READ);
  //	//	this->compute(cedar::proc::Arguments());
  this->mOutput->setData(
      cedar::aux::math::gaussMatrix(_mDimensionality->getValue(), _mSizes->getValue(), _mAmplitude->getValue(),
          _mSigmas->getValue(), _mCenters->getValue(), _mIsCyclic->getValue()));
  this->reset();
  //	this->unlock();
  //	this->emitOutputPropertiesChangedSignal("twostep");
  ////	this->onTrigger();
  //	std::cout<<"update MatrixSize finished "<<std::endl;

}

void cedar::dyn::steps::TwoStepInput::reset()
{
  elapsedTime = 0;
  curMovementTime = 0;
  moveForward = true;
  curCenter = _mCenters->getValue();
}

cedar::proc::DataSlot::VALIDITY cedar::dyn::steps::TwoStepInput::determineInputValidity(
    cedar::proc::ConstDataSlotPtr slot,
    cedar::aux::ConstDataPtr data) const
{
  cedar::aux::ConstMatDataPtr _input = boost::dynamic_pointer_cast < cedar::aux::ConstMatData > (data);
  if (_input)
  {
//    std::cout<<"DetermineInputValidity: Input is ConstMatData"<< std::endl;
    if (_input->getDimensionality() == 0 && slot->getName() == "start_countdown"
        && cedar::aux::math::get1DMatrixSize(_input->getData()) == 1)
    {
//      std::cout<<"DetermineInputValidity: Start Countdown correctly assigned"<< std::endl;
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }

    if (_input->getDimensionality() == 0 && slot->getName() == "external_switch")
    {
      auto mydata = _input->getData();
      auto rows = mydata.rows;
      auto cols = mydata.cols;
      if (rows==1 && cols == 1)
      {
        return cedar::proc::DataSlot::VALIDITY_VALID;
      }
    }
  }

  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::dyn::steps::TwoStepInput::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == "start_countdown")
  {
    mStartTimer = boost::dynamic_pointer_cast < cedar::aux::ConstMatData > (this->getInput(inputName));
  }
  else if (inputName == "external_switch")
  {
    mSwitchSignal = boost::dynamic_pointer_cast < cedar::aux::ConstMatData > (this->getInput(inputName));
  }
  else
  {
    throw cedar::aux::UnknownNameException();
  }
}
