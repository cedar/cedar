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

    File:        Supervisor.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 02 06

    Description: Header file for the class cedar::proc::experiment::Supervisor.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_SUPERVISOR_H
#define CEDAR_PROC_EXPERIMENT_SUPERVISOR_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/LoopedThread.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/Supervisor.fwd.h"
#include "cedar/processing/experiment/Experiment.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>
#include <QObject>
#include <string>

/*!@brief This thread should continuously perform the action sequences of the experiment
 */
class cedar::proc::experiment::Supervisor : public QObject
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------

  // uses singleton template.
  friend class cedar::aux::Singleton<Supervisor>;

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
  Supervisor();

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

signals:
  //!@brief signal emitted if state of running changes
  void experimentRunningChanged(bool running);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none

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
}; // class cedar::proc::experiment::Supervisor



#include "cedar/auxiliaries/Singleton.h"

namespace cedar
{
  namespace proc
  {
    namespace experiment
    {
      CEDAR_INSTANTIATE_PROC_TEMPLATE(cedar::aux::Singleton<cedar::proc::experiment::Supervisor>);
      typedef cedar::aux::Singleton<cedar::proc::experiment::Supervisor> SupervisorSingleton;
    }
  }
}

#endif // CEDAR_PROC_EXPERIMENT_SUPERVISOR_H

