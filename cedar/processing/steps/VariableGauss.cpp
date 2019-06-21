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

    File:        VariableGauss.cpp

    Maintainer:  

    Email:       

    Date:        

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/VariableGauss.h"
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
      new ElementDeclarationTemplate<cedar::proc::steps::VariableGauss>
      (
        "Arrays",
        "cedar.processing.steps.VariableGauss"
      )
    );
    declaration->setIconPath(":/steps/variable_gauss.svg");
    declaration->setDescription("Similar to the GaussInput step: Generates a tensor that contains a Gauss shape with center(!) defined via an input. TODO: make other parameters optionally adjustable via input.");
    declaration->deprecatedName("cedar.processing.steps.VariableGauss");

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::VariableGauss::VariableGauss()
:
cedar::proc::Step(),
mOutput(new cedar::aux::MatData(cv::Mat())),
_mDimensionality(new cedar::aux::UIntParameter(this, "dimensionality", 2, 1, 4)),
_mSizes(new cedar::aux::UIntVectorParameter(this, "sizes", 2, 50, 1, 1000)),
_mAmplitude(new cedar::aux::DoubleParameter(this, "amplitude", 1.0, cedar::aux::DoubleParameter::LimitType::full(), 0.5)),
_mSigmas(new cedar::aux::DoubleVectorParameter(this, "sigma", 2, 3.0, 0.01, 1000.0, 0.5)),
_mReferenceLevel(new cedar::aux::DoubleParameter(this, "reference level", 0.0))
{
  cedar::proc::DataSlotPtr input = this->declareInput("centers");

  this->declareOutput("box", mOutput);
  QObject::connect(_mAmplitude.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mReferenceLevel.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mSizes.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(updateDimensionality()));
  QObject::connect(_mSigmas.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  this->updateMatrix();
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


double cedar::proc::steps::VariableGauss::getAmplitude() const
{
  return this->_mAmplitude->getValue();
}

void cedar::proc::steps::VariableGauss::setAmplitude(double amplitude)
{
  this->_mAmplitude->setValue(amplitude);
}

void cedar::proc::steps::VariableGauss::setReferenceLevel(double referenceLevel)
{
  this->_mReferenceLevel->setValue(referenceLevel);
}

double cedar::proc::steps::VariableGauss::getReferenceLevel() const
{
  return this->_mReferenceLevel->getValue();
}

void cedar::proc::steps::VariableGauss::updateMatrix()
{
  this->lock(cedar::aux::LOCK_TYPE_READ);
  this->compute(cedar::proc::Arguments());
  this->unlock();
  this->emitOutputPropertiesChangedSignal("box");
  this->onTrigger();
}

void cedar::proc::steps::VariableGauss::updateDimensionality()
{
  int new_dimensionality = static_cast<int>(_mDimensionality->getValue());
  _mSizes->resize(new_dimensionality, _mSizes->getDefaultValue());
  _mSizes->setDefaultSize(new_dimensionality);
  _mSigmas->resize(new_dimensionality, _mSigmas->getDefaultValue());
  _mSigmas->setDefaultSize(new_dimensionality);
  this->lock(cedar::aux::LOCK_TYPE_READ);
  this->compute(cedar::proc::Arguments());
  this->unlock();
  this->emitOutputPropertiesChangedSignal("box");
  this->onTrigger();
}

void cedar::proc::steps::VariableGauss::inputConnectionChanged(const std::string& inputName)
{
  // TODO: you may want to replace this code by using a cedar::proc::InputSlotHelper

  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "centers");

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

void cedar::proc::steps::VariableGauss::compute(const cedar::proc::Arguments&)
{
  this->recompute();
}

void cedar::proc::steps::VariableGauss::recompute()
{
  auto input = getInput("centers");

  if (!input)
    return;

  auto data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input);

  if (!data)
    return;

  cv::Mat mat = input->getData<cv::Mat>();

  if (mat.empty())
    return;

  std::vector<double> tmpvec{ static_cast<double>(mat.at<float>(0,0)) };

  if (mat.rows > 1 )
    tmpvec.push_back( static_cast<double>(mat.at<float>(1,0) ) );

  this->mOutput->setData
                 (
                   cedar::aux::math::gaussMatrix
                   (
                     _mDimensionality->getValue(),
                     _mSizes->getValue(),
                     _mAmplitude->getValue(),
                     _mSigmas->getValue(),
                     tmpvec,
                     false // cyclic
                   )
                 );
}


