/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        VariableBox.cpp

    Maintainer:  Mathis Richter

    Email:       mathis.richter@ini.rub.de

    Date:        2013 05 03

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/VariableBox.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/math/functions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <iostream>
#include <vector>

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
      new ElementDeclarationTemplate<cedar::proc::steps::VariableBox>
      (
        "Arrays",
        "cedar.processing.steps.VariableBox"
      )
    );
    declaration->setIconPath(":/steps/variable_box.svg");
    declaration->setDescription("Generates a matrix that contains a box with left bound defined via an input.");
    declaration->deprecatedName("cedar.processing.steps.VariableBox");

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::VariableBox::VariableBox()
:
cedar::proc::Step(),
mOutput(new cedar::aux::MatData(cv::Mat())),
_mDimensionality(new cedar::aux::UIntParameter(this, "dimensionality", 2, 1, 4)),
_mSizes(new cedar::aux::UIntVectorParameter(this, "sizes", 2, 50, 1, 1000)),
_mAmplitude(new cedar::aux::DoubleParameter(this, "amplitude", 1.0, cedar::aux::DoubleParameter::LimitType::full(), 0.5)),
_mWidths(new cedar::aux::UIntVectorParameter(this, "widths", 2, 1, 1, 10000)),
_mReferenceLevel(new cedar::aux::DoubleParameter(this, "reference level", 0.0))
{
  cedar::proc::DataSlotPtr input = this->declareInput("left bounds");

  this->declareOutput("box", mOutput);
  QObject::connect(_mAmplitude.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mReferenceLevel.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mWidths.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mSizes.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(updateDimensionality()));
  this->updateMatrix();
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


double cedar::proc::steps::VariableBox::getAmplitude() const
{
  return this->_mAmplitude->getValue();
}

void cedar::proc::steps::VariableBox::setAmplitude(double amplitude)
{
  this->_mAmplitude->setValue(amplitude);
}

void cedar::proc::steps::VariableBox::setReferenceLevel(double referenceLevel)
{
  this->_mReferenceLevel->setValue(referenceLevel);
}

double cedar::proc::steps::VariableBox::getReferenceLevel() const
{
  return this->_mReferenceLevel->getValue();
}

void cedar::proc::steps::VariableBox::updateMatrix()
{
  this->lock(cedar::aux::LOCK_TYPE_READ);
  this->compute(cedar::proc::Arguments());
  this->unlock();
  this->emitOutputPropertiesChangedSignal("box");
  this->onTrigger();
}

void cedar::proc::steps::VariableBox::updateDimensionality()
{
  int new_dimensionality = static_cast<int>(_mDimensionality->getValue());
  _mWidths->resize(new_dimensionality, _mWidths->getDefaultValue());
  _mWidths->setDefaultSize(new_dimensionality);
  _mSizes->resize(new_dimensionality, _mSizes->getDefaultValue());
  _mSizes->setDefaultSize(new_dimensionality);
  this->lock(cedar::aux::LOCK_TYPE_READ);
  this->compute(cedar::proc::Arguments());
  this->unlock();
  this->emitOutputPropertiesChangedSignal("box");
  this->onTrigger();
}

void cedar::proc::steps::VariableBox::inputConnectionChanged(const std::string& inputName)
{
  // TODO: you may want to replace this code by using a cedar::proc::InputSlotHelper

  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "left bounds");

  // Assign the input to the member. This saves us from casting in every computation step.
  this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));

  bool output_changed = false;
  if (!this->mInput)
  {
    // no input -> no output
    this->mOutput->setData(cv::Mat());
    output_changed = true;
  }
  else
  {
    // Let's get a reference to the input matrix.
    const cv::Mat& input = this->mInput->getData();

    // check if the input is different from the output
    if (input.type() != this->mOutput->getData().type() || input.size != this->mOutput->getData().size)
    {
      output_changed = true;
    }

    // Make a copy to create a matrix of the same type, dimensions, ...
    this->recompute();
  }

  if (output_changed)
  {
    this->emitOutputPropertiesChangedSignal("box");
  }
}

void cedar::proc::steps::VariableBox::compute(const cedar::proc::Arguments&)
{
  this->recompute();
}

void cedar::proc::steps::VariableBox::recompute()
{
  auto input = getInput("left bounds");

  if (!input)
    return;

  auto data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input);

  if (!data)
    return;

  cv::Mat mat = input->getData<cv::Mat>();
  std::vector<unsigned int> tmpvec{ static_cast<unsigned int>(mat.at<float>(0,0)), 
                                    static_cast<unsigned int>(mat.at<float>(1,0) ) };

  this->mOutput->setData
                 (
                   cedar::aux::math::boxMatrix
                   (
                     _mDimensionality->getValue(),
                     _mSizes->getValue(),
                     _mAmplitude->getValue(),
                     _mReferenceLevel->getValue(),
                     _mWidths->getValue(),
                     tmpvec
                   )
                 );
}


