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

    File:        GaussInput.cpp

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
#include "cedar/processing/sources/GaussInput.h"
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
      new ElementDeclarationTemplate<cedar::proc::sources::GaussInput>
      (
        "Sources",
        "cedar.processing.sources.GaussInput"
      )
    );
    declaration->setIconPath(":/steps/gauss_input.svg");
    declaration->setDescription("Generates a tensor that contains a sampled Gauss function of your specification. Note: If you require that the parameters be set via inputs, please consider the VariableGauss step.");
    declaration->deprecatedName("cedar.processing.source.GaussInput");

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::sources::GaussInput::GaussInput()
:
cedar::proc::Step(),
mOutput(new cedar::aux::MatData(cv::Mat())),
_mDimensionality(new cedar::aux::UIntParameter(this, "dimensionality", 2, 1, 4)),
_mSizes(new cedar::aux::UIntVectorParameter(this, "sizes", 2, 50, 1, 1000.0)),
_mAmplitude(new cedar::aux::DoubleParameter(this, "amplitude", 1.0, cedar::aux::DoubleParameter::LimitType::full(), 0.5)),
_mCenters(new cedar::aux::DoubleVectorParameter(this, "centers", 2, 24.0, -10000.0, 10000.0)),
_mSigmas(new cedar::aux::DoubleVectorParameter(this, "sigma", 2, 3.0, 0.01, 1000.0, 0.5)),
_mIsCyclic(new cedar::aux::BoolParameter(this, "cyclic", false))
{
  this->declareOutput("Gauss input", mOutput);
  QObject::connect(_mAmplitude.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mSigmas.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mCenters.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mSizes.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrixSize()));
  QObject::connect(_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(updateDimensionality()));
  QObject::connect(_mIsCyclic.get(), SIGNAL(valueChanged()), this, SLOT(updateMatrix()));
  this->updateMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sources::GaussInput::setDimensionality(unsigned int dimensionality)
{
  this->_mDimensionality->setValue(dimensionality);
}

void cedar::proc::sources::GaussInput::setSize(unsigned int dimension, unsigned int size)
{
  this->_mSizes->setValue(dimension, size);
}

unsigned int cedar::proc::sources::GaussInput::getSize(unsigned int dimension) const
{
  return this->_mSizes->at(dimension);
}

void cedar::proc::sources::GaussInput::setCenter(unsigned int dimension, double center)
{
  CEDAR_ASSERT(dimension < this->_mCenters->size());
  this->_mCenters->setValue(dimension, center);
}

double cedar::proc::sources::GaussInput::getAmplitude() const
{
  return this->_mAmplitude->getValue();
}

void cedar::proc::sources::GaussInput::setAmplitude(double amplitude)
{
  this->_mAmplitude->setValue(amplitude);
}

void cedar::proc::sources::GaussInput::compute(const cedar::proc::Arguments&)
{
  try
  {
    this->mOutput->setData
                   (
                     cedar::aux::math::gaussMatrix
                     (
                       _mDimensionality->getValue(),
                       _mSizes->getValue(),
                       _mAmplitude->getValue(),
                       _mSigmas->getValue(),
                       _mCenters->getValue(),
                       _mIsCyclic->getValue()
                     )
                   );
  }
  catch (std::out_of_range& exc)
  {
    // this might happen if GaussInput is triggered and dimensionality is changed, just ignore
  }
}

void cedar::proc::sources::GaussInput::updateMatrix()
{
  this->onTrigger();
}

void cedar::proc::sources::GaussInput::updateDimensionality()
{
  int new_dimensionality = static_cast<int>(_mDimensionality->getValue());
  _mSigmas->resize(new_dimensionality, _mSigmas->getDefaultValue());
  _mSigmas->setDefaultSize(new_dimensionality);
  _mCenters->resize(new_dimensionality, _mCenters->getDefaultValue());
  _mCenters->setDefaultSize(new_dimensionality);
  _mSizes->resize(new_dimensionality, _mSizes->getDefaultValue());
  _mSizes->setDefaultSize(new_dimensionality);
  this->lock(cedar::aux::LOCK_TYPE_READ);
  this->compute(cedar::proc::Arguments());
  this->unlock();
  this->emitOutputPropertiesChangedSignal("Gauss input");
  this->onTrigger();
}

void cedar::proc::sources::GaussInput::updateMatrixSize()
{
  this->lock(cedar::aux::LOCK_TYPE_READ);
  this->compute(cedar::proc::Arguments());
  this->unlock();
  this->emitOutputPropertiesChangedSignal("Gauss input");
  this->onTrigger();
}
