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

    File:        BoxInput.cpp

    Maintainer:  Mathis Richter

    Email:       mathis.richter@ini.rub.de

    Date:        2013 05 03

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/sources/BoxInput.h"
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
      new ElementDeclarationTemplate<cedar::proc::sources::BoxInput>
      (
        "Sources",
        "cedar.processing.sources.BoxInput"
      )
    );
    declaration->setIconPath(":/steps/box_input.svg");
    declaration->setDescription("Generates a tensor that contains a 'box' (i.e. square or cubic region) of values to your specifications. Note: If you require that the parameters be set via inputs, please consider the VariableBox step.");
    declaration->deprecatedName("cedar.processing.source.BoxInput");

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::sources::BoxInput::BoxInput()
:
cedar::proc::Step(),
mOutput(new cedar::aux::MatData(cv::Mat())),
_mDimensionality(new cedar::aux::UIntParameter(this, "dimensionality", 2, 1, 4)),
_mSizes(new cedar::aux::UIntVectorParameter(this, "sizes", 2, 50, 1, 1000)),
_mAmplitude(new cedar::aux::DoubleParameter(this, "amplitude", 1.0, cedar::aux::DoubleParameter::LimitType::full(), 0.5)),
_mWidths(new cedar::aux::UIntVectorParameter(this, "widths", 2, 1, 1, 10000)),
_mLeftBounds(new cedar::aux::UIntVectorParameter(this, "left bounds", 2, 0, 0, 10000)),
_mReferenceLevel(new cedar::aux::DoubleParameter(this, "reference level", 0.0))
{
  this->declareOutput("box input", mOutput);
  QObject::connect(_mAmplitude.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mReferenceLevel.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mWidths.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mLeftBounds.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mSizes.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(updateDimensionality()));
  this->updateMatrix();
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sources::BoxInput::setLeftBound(unsigned int dimension, unsigned int leftBound)
{
  CEDAR_ASSERT(dimension < this->_mLeftBounds->size());
  this->_mLeftBounds->setValue(dimension, leftBound);
}

double cedar::proc::sources::BoxInput::getAmplitude() const
{
  return this->_mAmplitude->getValue();
}

void cedar::proc::sources::BoxInput::setAmplitude(double amplitude)
{
  this->_mAmplitude->setValue(amplitude);
}

void cedar::proc::sources::BoxInput::setReferenceLevel(double referenceLevel)
{
  this->_mReferenceLevel->setValue(referenceLevel);
}

double cedar::proc::sources::BoxInput::getReferenceLevel() const
{
  return this->_mReferenceLevel->getValue();
}

void cedar::proc::sources::BoxInput::compute(const cedar::proc::Arguments&)
{
  this->mOutput->setData
                 (
                   cedar::aux::math::boxMatrix
                   (
                     _mDimensionality->getValue(),
                     _mSizes->getValue(),
                     _mAmplitude->getValue(),
                     _mReferenceLevel->getValue(),
                     _mWidths->getValue(),
                     _mLeftBounds->getValue()
                   )
                 );
}

void cedar::proc::sources::BoxInput::updateMatrix()
{
  this->lock(cedar::aux::LOCK_TYPE_READ);
  this->compute(cedar::proc::Arguments());
  this->unlock();
  this->emitOutputPropertiesChangedSignal("box input");
  this->onTrigger();
}

void cedar::proc::sources::BoxInput::updateDimensionality()
{
  int new_dimensionality = static_cast<int>(_mDimensionality->getValue());
  _mWidths->resize(new_dimensionality, _mWidths->getDefaultValue());
  _mWidths->setDefaultSize(new_dimensionality);
  _mLeftBounds->resize(new_dimensionality, _mLeftBounds->getDefaultValue());
  _mLeftBounds->setDefaultSize(new_dimensionality);
  _mSizes->resize(new_dimensionality, _mSizes->getDefaultValue());
  _mSizes->setDefaultSize(new_dimensionality);
  this->lock(cedar::aux::LOCK_TYPE_READ);
  this->compute(cedar::proc::Arguments());
  this->unlock();
  this->emitOutputPropertiesChangedSignal("box input");
  this->onTrigger();
}
