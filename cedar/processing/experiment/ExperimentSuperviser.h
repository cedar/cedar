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

    File:        ExperimentController.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 02 06

    Description: Header file for the class cedar::proc::experiment::ExperimentController.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_EXPERIMENT_CONTROLLER_H
#define CEDAR_PROC_EXPERIMENT_EXPERIMENT_CONTROLLER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/LoopedThread.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/ExperimentSuperviser.fwd.h"
#include "cedar/processing/experiment/Experiment.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>

/*!@brief This thread should continuously perform the action sequences of the experiment
 *
 */
//@todo Superviser
class cedar::proc::experiment::ExperimentSuperviser : public cedar::aux::LoopedThread
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------

  // uses singleton template.
  friend class cedar::aux::Singleton<ExperimentSuperviser>;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:

  //!@brief A data structure to store all the DataPtr with time stamp (in ms) in a list.
  struct LogData
  {
    cedar::unit::Time mLogTime;
    std::string  mMessageType;
    std::string mMessage;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The private constructor.
  ExperimentSuperviser();
public:
  //!@brief Destructor
  virtual ~ExperimentSuperviser();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Sets the experiment.
  void setExperiment(Experiment* experiment);

  //!@brief Returns the experimen
  Experiment* getExperiment();

  //!@brief Logs a message and writes it to the log file;
  void log(std::string messageType, std::string message);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Calls write in the specified interval.
  void step(cedar::unit::Time);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! The experiment
  //!@todo Why isn't this a shared ptr?
  Experiment* mpExperiment;

  //!@brief Lock during the logging process
  QReadWriteLock* mLogLock;

  //!@brief A memory list of the log
  std::vector<LogData> mLogList;
}; // class cedar::proc::experiment::ExperimentController



#include "cedar/auxiliaries/Singleton.h"

namespace cedar
{
  namespace proc
  {
    namespace experiment
    {
      CEDAR_INSTANTIATE_AUX_TEMPLATE(cedar::aux::Singleton<cedar::proc::experiment::ExperimentSuperviser>);
      typedef cedar::aux::Singleton<cedar::proc::experiment::ExperimentSuperviser> ExperimentSuperviserSingleton;
    }
  }
}

#endif // CEDAR_PROC_EXPERIMENT_EXPERIMENT_CONTROLLER_H

