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

    File:        Step.h

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

#ifndef CEDAR_PROC_STEP_H
#define CEDAR_PROC_STEP_H

// LOCAL INCLUDES
#include "processing/namespace.h"
#include "processing/Trigger.h"
#include "processing/DataRole.h"
#include "auxiliaries/ParameterBase.h"
#include "auxiliaries/Base.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QThread>
#include <map>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::Step : public QThread
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  typedef std::map<std::string, cedar::aux::ParameterBasePtr> ParameterMap;
  struct DataEntry
  {
    public:
      DataEntry(bool isMandatory = true);

      void setData(DataPtr data);
      DataPtr getData();
      boost::shared_ptr<const Data> getData() const;

      bool isMandatory() const;

    private:
      DataPtr mData;
      bool mMandatory;
  };
  typedef std::map<std::string, DataEntry> SlotMap;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Step(bool runInThread = false, bool autoConnectTriggers = true);

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!\brief check if everything is prepared to execute compute()
  virtual void onStart();

  void onTrigger();

  virtual void compute(const cedar::proc::Arguments& arguments) = 0;

  void setNextArguments(cedar::proc::ArgumentsPtr arguments);

  cedar::proc::TriggerPtr& getFinishedTrigger();

  void setThreaded(bool isThreaded);

  void declareData(DataRole::Id role, const std::string& name, bool mandatory = true);
  void declareInput(const std::string& name, bool mandatory = true);
  void declareBuffer(const std::string& name, bool mandatory = true);
  void declareOutput(const std::string& name, bool mandatory = true);

  void setData(DataRole::Id role, const std::string& name, cedar::proc::DataPtr data);
  void setInput(const std::string& name, cedar::proc::DataPtr data);
  void setBuffer(const std::string& name, cedar::proc::DataPtr data);
  void setOutput(const std::string& name, cedar::proc::DataPtr data);

  cedar::proc::DataPtr getData(DataRole::Id role, const std::string& name);
  cedar::proc::DataPtr getInput(const std::string& name);
  cedar::proc::DataPtr getBuffer(const std::string& name);
  cedar::proc::DataPtr getOutput(const std::string& name);

  const cedar::proc::Step::SlotMap& getDataSlots(DataRole::Id role) const;

  virtual void readConfiguration(const cedar::aux::ConfigurationNode& node);

  static void connect(
                       cedar::proc::StepPtr source,
                       const std::string& sourceName,
                       cedar::proc::StepPtr target,
                       const std::string& targetName
                     );

  const ParameterMap& getParameters() const;
  ParameterMap& getParameters();

  void setName(const std::string& name);
  const std::string& getName() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void run();

  void registerParameter(cedar::aux::ParameterBasePtr parameter);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void checkMandatoryConnections();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  cedar::proc::TriggerPtr mFinished;
  std::map<DataRole::Id, SlotMap> mDataConnections;

private:
  /*!@brief Whether the connect function should automatically connect the triggers as well.
   */
  const bool mAutoConnectTriggers;
  bool mBusy;
  ArgumentsPtr mNextArguments;
  bool mMandatoryConnectionsAreSet;

  ParameterMap mParameters;

  cedar::aux::StringParameterPtr mName;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  bool mRunInThread;

}; // class cedar::proc::Step

#endif // CEDAR_PROC_STEP_H

