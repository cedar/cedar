/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        StaticGain.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 28

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/processing/steps/StaticGain.h"

// PROJECT INCLUDES
#include "cedar/processing/DataSlot.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <iostream>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::StaticGain::StaticGain()
:
mOutput(new cedar::aux::MatData(cv::Mat())),
_mGainFactor(new cedar::aux::DoubleParameter(this, "gainFactor", 1.0, -10000.0, 10000.0))
{
  this->declareInput("input");
  this->declareOutput("output", mOutput);
  QObject::connect(_mGainFactor.get(), SIGNAL(valueChanged()), this, SLOT(gainChanged()));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::StaticGain::compute(const cedar::proc::Arguments&)
{
  this->mOutput->setData(this->mInput->getData() * this->_mGainFactor->getValue());
}

void cedar::proc::steps::StaticGain::gainChanged()
{
  this->onTrigger();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::StaticGain::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr CEDAR_DEBUG_ONLY(slot),
                                  cedar::aux::DataPtr data
                                ) const
{
  CEDAR_DEBUG_ASSERT(slot->getName() == "input")
  if (boost::shared_dynamic_cast<cedar::aux::MatData>(data))
  {
    return cedar::proc::DataSlot::VALIDITY_VALID;
  }
  else
  {
    return cedar::proc::DataSlot::VALIDITY_ERROR;
  }
}

void cedar::proc::steps::StaticGain::inputConnectionChanged(const std::string& inputName)
{
  CEDAR_DEBUG_ASSERT(inputName == "input");

  this->mInput = boost::shared_dynamic_cast<cedar::aux::MatData>(this->getInput(inputName));
  if (this->mInput)
  {
    const cv::Mat& input = this->mInput->getData();
    // make a copy to create a matrix of the same type, dimensions, ...
    this->mOutput->setData(input.clone());
  }

  this->onTrigger();
}
