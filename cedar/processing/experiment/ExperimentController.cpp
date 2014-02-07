/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ExperimentController.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 02 06

    Description: Source file for the class cedar::proc::experiment::ExperimentController.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/ExperimentController.h"
#include "cedar/processing/experiment/Experiment.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::ExperimentController::ExperimentController(Experiment* parent)
:
init(true)
{
  this->mpExperiment = parent;

  this->connectToStartSignal(boost::bind(&cedar::proc::experiment::ExperimentController::prepareStart, this));
  this->connectToQuitSignal(boost::bind(&cedar::proc::experiment::ExperimentController::processQuit, this));
}

cedar::proc::experiment::ExperimentController::~ExperimentController()
{
}



//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::proc::experiment::ExperimentController::step(cedar::unit::Time)
{
  this->mpExperiment->executeAcionSequences();
}

void cedar::proc::experiment::ExperimentController::prepareStart()
{
  this->mpExperiment->executeAcionSequences();
  this->init = false;
}

void cedar::proc::experiment::ExperimentController::processQuit()
{
  this->init = true;
}

bool cedar::proc::experiment::ExperimentController::isOnInit()
{
  return init;
}

