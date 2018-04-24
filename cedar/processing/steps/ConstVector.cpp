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

    File:        ConstVector.cpp

    Maintainer: Raul Grieben
    Email:      Raul.Grieben@ini.rub.e
    Date:       2017 08 07

    Description: Processing step, which projects neuronal activation between processing steps of different
                 dimensionality.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/processing/steps/ConstVector.h"

// PROJECT INCLUDES
#include "cedar/processing/ElementDeclaration.h"

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr stv_decl
        (
            new ElementDeclarationTemplate<cedar::proc::steps::ConstVector>
                (
                    "Sources",
                    "cedar.processing.ConstVector"
                )
        );
    stv_decl->setIconPath(":/steps/const_vector.svg");
    stv_decl->setDescription
        (
            "Creates a constant vector."
        );

    stv_decl->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ConstVector::ConstVector()
    :
    Step(false),
    mOutput(new cedar::aux::MatData(cv::Mat::zeros(1,1, CV_32F))),
    _mOutputDimension (new cedar::aux::UIntParameter(this, "number of vector entries", 1,1,255)),
    _mValues(new cedar::aux::DoubleVectorParameter(this, "entries", 1, 0.0, -1000000000.0, 1000000000.0))
{
  declareOutput("vector", mOutput);
  QObject::connect(_mOutputDimension.get(), SIGNAL(valueChanged()), this, SLOT(vectorDimensionChanged()));
  QObject::connect(_mValues.get(), SIGNAL(valueChanged()), this, SLOT(updateVector()));
}

void cedar::proc::steps::ConstVector::updateVector()
{
  this->onTrigger();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::steps::ConstVector::compute(const cedar::proc::Arguments&)
{
  mOutput->getData().setTo(0);
  const std::vector<double>& values =_mValues->getValue();
  for (unsigned i=0; i<values.size(); i++)
  {
    if (values[i])
    {
      mOutput->getData().row(i) = static_cast<float>(values[i]);
    }
  }
}

void cedar::proc::steps::ConstVector::vectorDimensionChanged()
{
  int newsize = static_cast<int>(_mOutputDimension->getValue());
  _mValues->resize(newsize, _mValues->getDefaultValue());
  _mValues->setDefaultSize(newsize);
  mOutput->getData() = cv::Mat::zeros(newsize, 1, mOutput->getData().type());
  this->emitOutputPropertiesChangedSignal("vector");
  this->onTrigger();
}
