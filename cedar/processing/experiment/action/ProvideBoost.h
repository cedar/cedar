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

    File:        ProvideBoost.h

    Maintainer:  Daniel Sabinasz
    Email:       daniel.sabinasz@ini.rub.de
    Date:        2021 08 04

    Description: Header file for the class cedar::proc::experiment::ProvideBoost.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_ACTION_ACTIVATE_NODE_H
#define CEDAR_PROC_EXPERIMENT_ACTION_ACTIVATE_NODE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/action/Action.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/processing/experiment/NeuralFieldBoostParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/action/ProvideBoost.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#include <vector>
#include <string>
#include <cedar/auxiliaries/BoolParameter.h>


/*!@brief Sets the parameter of a step to a desired value
 */
class cedar::proc::experiment::action::ProvideBoost : public cedar::proc::experiment::action::Action
{
public:
  //!@brief The standard constructor.
  ProvideBoost();

public:
  //!@brief Sets the parameter of a step to a desired value
  void run();

  void preExperiment();

  void postExperiment();

  bool checkValidity(std::vector<std::string>& errors, std::vector<std::string>& warnings) const;

protected:
    //! Returns the step parameter chosen for this action.
    cedar::proc::experiment::NeuralFieldBoostParameterPtr getNeuralFieldBoostParameter() const;

private:

private:
    cedar::proc::experiment::NeuralFieldBoostParameterPtr _mNeuralFieldBoostParameter;
}; // class cedar::proc::experiment::ProvideBoost

#endif // CEDAR_PROC_EXPERIMENT_ACTION_ACTIVATE_NODE_H

