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

    File:        ProcessingStep.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 05 23

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_COMP_PROCESSING_STEP_H
#define CEDAR_AUX_COMP_PROCESSING_STEP_H

// LOCAL INCLUDES
#include "auxiliaries/computation/namespace.h"
#include "auxiliaries/computation/Trigger.h"
#include "auxiliaries/Base.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QThread>
#include <map>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::aux::comp::ProcessingStep : public cedar::aux::Base, public QThread
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ProcessingStep(bool runInThread = false, bool autoConnectTriggers = true);

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void onTrigger();

  virtual void compute(const cedar::aux::comp::Arguments& arguments) = 0;

  void setNextArguments(cedar::aux::comp::ArgumentsPtr arguments);

  cedar::aux::comp::TriggerPtr& getFinishedTrigger();

  void setThreaded(bool isThreaded);

  void declareData(DataRole role, const std::string& name);
  void declareInput(const std::string& name);
  void declareBuffer(const std::string& name);
  void declareOutput(const std::string& name);

  void setData(DataRole role, const std::string& name, cedar::aux::comp::DataPtr data);
  void setInput(const std::string& name, cedar::aux::comp::DataPtr data);
  void setBuffer(const std::string& name, cedar::aux::comp::DataPtr data);
  void setOutput(const std::string& name, cedar::aux::comp::DataPtr data);

  cedar::aux::comp::DataPtr getData(DataRole role, const std::string& name);
  cedar::aux::comp::DataPtr getInput(const std::string& name);
  cedar::aux::comp::DataPtr getBuffer(const std::string& name);
  cedar::aux::comp::DataPtr getOutput(const std::string& name);

  static void connect(
                       cedar::aux::comp::ProcessingStepPtr source,
                       const std::string& sourceName,
                       cedar::aux::comp::ProcessingStepPtr target,
                       const std::string& targetName
                     );

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void run();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  cedar::aux::comp::TriggerPtr mFinished;

  typedef std::map<std::string, DataPtr> SlotMap;
  std::map<DataRole, SlotMap > mDataConnections;

private:
  /*!@brief Whether the connect function should automatically connect the triggers as well.
   */
  const bool mAutoConnectTriggers;

  bool mBusy;
  bool mRunInThread;
  ArgumentsPtr mNextArguments;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::comp::ProcessingStep

#endif // CEDAR_AUX_COMP_PROCESSING_STEP_H

