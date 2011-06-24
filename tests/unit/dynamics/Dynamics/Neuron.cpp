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

// PROJECT INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::Neuron::Neuron(double interactionWeight, double restingLevel)
:
mRestingLevel(restingLevel),
mInteractionWeight (interactionWeight),
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

void cedar::Neuron::readConfiguration(const cedar::proc::ConfigurationNode& node)
{
  this->cedar::proc::Step::readConfiguration(node);
  this->mRestingLevel = node.get<double>("restingLevel", -1000.0);
  this->mInteractionWeight = node.get<double>("interactionWeight", -1.0);
}

double cedar::Neuron::getActivity() const
{
  return this->mActivation->getData();
}

void cedar::Neuron::eulerStep(const cedar::unit::Time& time)
{
  using cedar::unit::Seconds;
  using cedar::unit::Milliseconds;

  double& activation = mActivation->getData();
  double input = this->getInput("input")->getData<double>();

  // nonlinearity
  double interaction = 0;
  if (input >= 0)
  {
    interaction = 1.0;
  }

  activation += Seconds(time) / Milliseconds(50.0) * (-1.0 * activation + mRestingLevel + mInteractionWeight * input);

  mOutput->getData() = activation;
}
