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

    File:        Supervisor.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 02 06

    Description: Source file for the class cedar::proc::experiment::Supervisor.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/Supervisor.h"
#include "cedar/processing/experiment/Experiment.h"
#include "cedar/auxiliaries/GlobalClock.h"

// SYSTEM INCLUDES
#include <QWriteLocker>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::Supervisor::Supervisor()
:
mpExperiment(nullptr)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::experiment::Supervisor::setExperiment(Experiment* experiment)
{
  if (this->mpExperiment != nullptr)
  {
    QObject::disconnect(this->mpExperiment, SIGNAL(experimentRunning(bool)), this, SIGNAL(experimentRunningChanged(bool)));
  }

  this->mpExperiment = experiment;
  QObject::connect(this->mpExperiment, SIGNAL(experimentRunning(bool)), this, SIGNAL(experimentRunningChanged(bool)));
}

cedar::proc::experiment::Experiment* cedar::proc::experiment::Supervisor::getExperiment()
{
  return this->mpExperiment;
}

void cedar::proc::experiment::Supervisor::log(std::string messageType, std::string message)
{
  cedar::unit::Time time= cedar::aux::GlobalClockSingleton::getInstance()->getTime();

  std::string message_str = cedar::aux::formatDuration(time) + "\t[" + messageType + "]\t" + message;
  cedar::aux::LogSingleton::getInstance()->message(message_str, "experiment supervisor");
}
