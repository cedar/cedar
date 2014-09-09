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
mpExperiment(nullptr),
mLogLock(new QReadWriteLock)
{

}

cedar::proc::experiment::Supervisor::~Supervisor()
{
  delete mLogLock;
}



//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::proc::experiment::Supervisor::step(cedar::unit::Time)
{
  bool init = this->mpExperiment->hasStopped();
  this->mpExperiment->executeAcionSequences(init);
}

void cedar::proc::experiment::Supervisor::setExperiment(Experiment* experiment)
{
  if (this->mpExperiment != nullptr)
  {
    QObject::disconnect(this->mpExperiment, SIGNAL(experimentRunning(bool)), this, SIGNAL(experimentRunning(bool)));
  }
  this->mpExperiment = experiment;
  QObject::connect(this->mpExperiment, SIGNAL(experimentRunning(bool)), this, SIGNAL(experimentRunning(bool)));
}
cedar::proc::experiment::Experiment* cedar::proc::experiment::Supervisor::getExperiment()
{
  return this->mpExperiment;
}

void cedar::proc::experiment::Supervisor::log(std::string messageType, std::string message)
{
  QWriteLocker locker(mLogLock);

  cedar::unit::Time time= cedar::aux::GlobalClockSingleton::getInstance()->getTime();

  /* @todo Save Log in memory later
   *
   * const LogData data = {time, messageType, message};
   * mLogList.push_back(data);
   */

  // Get file name
  std::string file_name = mpExperiment->getFileName();
  int lastindex = file_name.find_last_of(".");
  file_name = file_name.substr(0, lastindex) + ".log";

  //Open stream
  std::ofstream output;
  output.open(file_name, std::ios::out | std::ios::app);
  output << time << "\t" << messageType << "\t" << message << std::endl;
  output.close();
}
