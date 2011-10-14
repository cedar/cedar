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

    File:        <filename>

    Maintainer:  <first name> <last name>
    Email:       <email address>
    Date:        <creation date YYYY MM DD>

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "Neuron.h"
#include "auxiliaries/NumericParameter.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::Neuron::Neuron(double interactionWeight, double restingLevel)
:
mRestingLevel(new cedar::aux::DoubleParameter(this, "restingLevel", restingLevel, -100, 0)),
mInteractionWeight(new cedar::aux::DoubleParameter(this, "interactionWeight", interactionWeight, -100, 100)),
mActivation(new cedar::dyn::DoubleActivation(0.0)),
mOutput(new cedar::dyn::DoubleActivation(0.0))
{
  this->declareInput("input");
  this->declareOutput("output");
  this->setOutput("output", mOutput);
}

cedar::Neuron::~Neuron()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

double cedar::Neuron::getActivity() const
{
  return this->mActivation->getData();
}

void cedar::Neuron::eulerStep(const cedar::unit::Duration& time)
{
  using cedar::unit::Seconds;
  using cedar::unit::Milliseconds;

  double& activation = mActivation->getData();
  double input = this->getInput("input")->getData<double>();
  double resting_level = mRestingLevel->getValue();
  double interaction_weight = mInteractionWeight->getValue();

  // nonlinearity
  double interaction = 0;
  if (input >= 0)
  {
    interaction = 1.0;
  }

  activation += Seconds(time) / Milliseconds(50.0) * (-1.0 * activation + resting_level + interaction_weight * interaction);

  mOutput->getData() = activation;
}
