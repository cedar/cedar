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

    File:        Manager.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 24

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_MANAGER_H
#define CEDAR_PROC_MANAGER_H

// LOCAL INCLUDES
#include "auxiliaries/namespace.h"
#include "processing/namespace.h"
#include "processing/Registry.h"
#include "processing/StepDeclaration.h"
#include "processing/TriggerDeclaration.h"
#include "processing/FrameworkSettings.h"
#include "processing/Step.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <map>
#include <set>
#include <vector>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::Manager
{
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
public:
  typedef cedar::proc::Registry<Step, StepDeclaration> StepRegistry;
  typedef cedar::proc::Registry<Trigger, TriggerDeclaration> TriggerRegistry;
  typedef std::set<cedar::aux::LoopedThreadPtr> ThreadRegistry; //!<@todo Use a name?
  typedef std::set<cedar::proc::GroupPtr> GroupRegistry; //!<@todo Use a name instead?

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Destructor
  ~Manager();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!\brief get the singleton instance of Manager
  static Manager& getInstance();
  cedar::proc::FrameworkSettings& settings();

  //!\brief access to step registry
  StepRegistry& steps();
  //!\brief access to trigger registry
  TriggerRegistry& triggers();
  //!\brief access to thread registry
  ThreadRegistry& threads();
  //!\brief access to group registry
  GroupRegistry& groups();

  void registerThread(cedar::aux::LoopedThreadPtr thread);
  cedar::proc::GroupPtr allocateGroup();
  void removeGroup(cedar::proc::GroupPtr group);

  cedar::proc::GroupPtr getGroup(const std::string& name);

  void load(cedar::proc::PluginProxyPtr plugin);

  void startThreads();
  void stopThreads();
  void connect(
                cedar::proc::StepPtr source,
                const std::string& sourceName,
                cedar::proc::StepPtr target,
                const std::string& targetName
              );
  void disconnect(
                   cedar::proc::StepPtr source,
                   const std::string& sourceName,
                   cedar::proc::StepPtr target,
                   const std::string& targetName
                 );
  void connect(
                cedar::proc::TriggerPtr trigger,
                cedar::proc::StepPtr target
              );
  void connect(
                cedar::proc::TriggerPtr trigger,
                cedar::proc::TriggerPtr target
              );

  void getConnections(
                       cedar::proc::StepPtr source,
                       const std::string& sourceDataName,
                       std::vector<cedar::proc::Connection*>& connections //!@todo should be const pointers?
                     );

  //!\ remove and disconnect a step
  void removeStep(cedar::proc::StepPtr step);
  //!\ remove and disconnect a trigger
  void removeTrigger(cedar::proc::TriggerPtr trigger);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!\brief this function disconnects all incoming and outgoing connections of a single step
  void disconnect(cedar::proc::StepPtr deletedStep);
  //!\brief this function disconnects all incoming and outgoing connections of a single trigger
  void disconnect(cedar::proc::TriggerPtr deletedTrigger);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The standard constructor.
  Manager();
  /*!\brief this deletes a single connection from the mConnections vector. Afterwards, previously obtained iterators
   * will become invalid, so watch out.*/
  void deleteConnection(cedar::proc::Connection* connection);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  //! the manager singleton instance
  static Manager mManager;

  //! a registry for all managed steps
  StepRegistry mStepRegistry;
  //! a registry for all managed triggers
  TriggerRegistry mTriggerRegistry;
  //! a registry for all managed threads, which can be globally started or stopped (e.g., LoopedTrigger)
  ThreadRegistry mThreadRegistry;
  //! a registry for all managed groups
  GroupRegistry mGroupRegistry;

  cedar::proc::FrameworkSettings mSettings;
  std::vector<cedar::proc::Connection*> mConnections;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::Manager

#endif // CEDAR_PROC_MANAGER_H

