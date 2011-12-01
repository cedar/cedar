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
#include "cedar/auxiliaries/namespace.h"
#include "cedar/processing/namespace.h"
#include "cedar/processing/FrameworkSettings.h"
#include "cedar/processing/Step.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <map>
#include <set>
#include <vector>
#include <QObject>


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
  typedef std::set<cedar::aux::LoopedThreadPtr> ThreadRegistry; //!<@todo Use a name?

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
  //!@brief get the singleton instance of Manager
  static Manager& getInstance();
  //!@brief access the FrameworkSettings
  cedar::proc::FrameworkSettings& settings();

  //!\brief access to thread registry
  ThreadRegistry& threads();

  //!@brief register a thread (Manager holds a collection of threads, which can be started at once)
  void registerThread(cedar::aux::LoopedThreadPtr thread);

  //!@brief load a plugin
  void load(cedar::proc::PluginProxyPtr plugin);

  //!@brief start all registered threads
  void startThreads();

  //!@brief stop all threads
  void stopThreads(bool wait = false);

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
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  //! the manager singleton instance
  static Manager mManager;

  //! a registry for all managed threads, which can be globally started or stopped (e.g., LoopedTrigger)
  ThreadRegistry mThreadRegistry;

  //!@brief the framework settings
  cedar::proc::FrameworkSettings mSettings;

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

