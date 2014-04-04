/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

// CEDAR INCLUDES
#include "cedar/processing/Step.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/LoopedThread.fwd.h"
#include "cedar/auxiliaries/PluginProxy.fwd.h"
#include "cedar/processing/Manager.fwd.h"
#include "cedar/processing/PluginDeclaration.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#include <map>
#include <set>
#include <vector>


/*!@brief A manager for processing architectures.
 *
 * @deprecated This class will be removed in future releases.
 */
class cedar::proc::Manager
{
  //--------------------------------------------------------------------------------------------------------------------
  // friend
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::Singleton<cedar::proc::Manager>;

  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief a registry for LoopedThreads
  typedef std::set<cedar::aux::LoopedThreadPtr> ThreadRegistry;

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
  //!@brief access to thread registry
  //!@deprecated This function is replaced by functionality to access LoopedTriggers in cedar::proc::Networks.
  CEDAR_DECLARE_DEPRECATED(ThreadRegistry& threads());

  //!@brief register a thread (Manager holds a collection of threads, which can be started at once)
  void registerThread(cedar::aux::LoopedThreadPtr thread);

  //!@brief load a plugin
  //!@deprecated Call declare() on the plugin pointer instead.
  CEDAR_DECLARE_DEPRECATED(void load(cedar::aux::PluginProxyPtr plugin));

  //!@brief Loads the declarations from a plugin declaration.
  //!@deprecated Call declareAll() on the declaration pointer instead.
  CEDAR_DECLARE_DEPRECATED(void load(cedar::proc::PluginDeclarationPtr declaration));

  //!@brief Loads the default plugins specified in cedar's configuration file.
  //!@deprecated Use the function in cedar::proc::gui::Settings instead.
  CEDAR_DECLARE_DEPRECATED(void loadDefaultPlugins());

  /*!@brief start all registered threads
   *
   * @deprecated Use the startTriggers function in cedar::proc::Network instead.
   */
  CEDAR_DECLARE_DEPRECATED(void startThreads());

  /*!@brief stop all threads
   *
   * @deprecated Use the stopTriggers function in cedar::proc::Network instead.
   */
  CEDAR_DECLARE_DEPRECATED(void stopThreads(bool wait = false));

  //! Returns the singleton instance of the manager.
  CEDAR_DECLARE_DEPRECATED(static cedar::proc::Manager& getInstance());
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The standard constructor.
  Manager();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! a registry for all managed threads, which can be globally started or stopped (e.g., LoopedTrigger)
  ThreadRegistry mThreadRegistry;

}; // class cedar::Manager

CEDAR_PROC_SINGLETON(Manager);

#endif // CEDAR_PROC_MANAGER_H

