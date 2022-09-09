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

    File:        ConvertToValueRange.cpp

    Maintainer:  Jan Tekülve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2022 08 02

    Description: Source file for the class cedar::proc::steps::ConvertToValueRange.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/ConvertToValueRange.h"

// CEDAR INCLUDES
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"

// SYSTEM INCLUDES

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
            new ElementDeclarationTemplate<cedar::proc::steps::ConvertToValueRange>
            (
                "Algebra",
                "cedar.processing.ConvertToValueRange"
                )
        );
        declaration->setIconPath(":/steps/convert_to_value_range.svg");
        declaration->setDescription
        (
            "Converts all values in the input matrix from a particular number range to a new value range. Values outside the specified input range are transformed using the same linear conversion and will exceed the given output range."
        );

        declaration->declare();

        return true;
    }

    bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ConvertToValueRange::ConvertToValueRange()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat())),
// parameters
_mInputLowerBound(new cedar::aux::DoubleParameter(this, "input lower bound", 0)),
_mInputUpperBound(new cedar::aux::DoubleParameter(this, "input upper bound", 1)),
_mOutputLowerBound(new cedar::aux::DoubleParameter(this, "output lower bound", 0)),
_mOutputUpperBound(new cedar::aux::DoubleParameter(this, "output upper bound", 1))
{
    cedar::proc::DataSlotPtr input = this->declareInput("input");
    this->declareOutput("converted output", mOutput);

    QObject::connect(_mInputLowerBound.get(), SIGNAL(valueChanged()), this, SLOT(rangeChanged()));
    QObject::connect(_mInputUpperBound.get(), SIGNAL(valueChanged()), this, SLOT(rangeChanged()));
    QObject::connect(_mOutputLowerBound.get(), SIGNAL(valueChanged()), this, SLOT(rangeChanged()));
    QObject::connect(_mOutputUpperBound.get(), SIGNAL(valueChanged()), this, SLOT(rangeChanged()));
}

cedar::proc::steps::ConvertToValueRange::~ConvertToValueRange()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::steps::ConvertToValueRange::compute(const cedar::proc::Arguments&)
{
    // the result is simply input * gain.
    if (mInput)
    {
        auto inputMat = this->mInput->getData();

        double inputRange = _mInputUpperBound->getValue() - _mInputLowerBound->getValue();
        double outputRange = _mOutputUpperBound->getValue() - _mOutputLowerBound->getValue();
        cv::Mat convertedMat = (((inputMat - _mInputLowerBound->getValue()) * outputRange) / inputRange) + _mOutputLowerBound->getValue();

        this->mOutput->setData(convertedMat);
    }
    else
    {
        this->mOutput->setData(cv::Mat());
    }
}


void cedar::proc::steps::ConvertToValueRange::inputConnectionChanged(const std::string& inputName)
{
    // Assign the input to the member. This saves us from casting in every computation step.
    this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
}

void cedar::proc::steps::ConvertToValueRange::rangeChanged()
{
    this->onTrigger();
}