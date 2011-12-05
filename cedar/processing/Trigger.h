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

    File:        Trigger.h

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

#ifndef CEDAR_PROC_TRIGGER_H
#define CEDAR_PROC_TRIGGER_H

// LOCAL INCLUDES
#include "cedar/processing/Element.h"
#include "cedar/processing/Triggerable.h"
#include "cedar/processing/namespace.h"


// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/enable_shared_from_this.hpp>
#include <vector>

/*!@brief A base class for all sorts of Trigger. Trigger provides a generic interface for the trigger concept in cedar.
 * Trigger can have listeners, to which they send a trigger signal.
 *
 */
class cedar::proc::Trigger : public cedar::proc::Element,
                             public boost::enable_shared_from_this<cedar::proc::Trigger>,
                             public cedar::proc::Triggerable
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::proc::Manager;
  friend class cedar::proc::Network;
  friend class cedar::proc::TriggerConnection;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Trigger(const std::string& name = "", bool isLooped = false);

  //!@brief Destructor
  virtual ~Trigger();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief sends trigger signal and arguments to all listeners
  void trigger(cedar::proc::ArgumentsPtr arguments = cedar::proc::ArgumentsPtr());

  //!@brief handles an incoming trigger signal if Trigger instance is listener
  void onTrigger(cedar::proc::TriggerPtr pSender);

  //!@brief adds a Trigger, which will receive trigger signals from this instance from now on
  void addTrigger(cedar::proc::TriggerPtr trigger);

  //!@brief removes a Trigger, which will no longer receive trigger signals
  void removeTrigger(cedar::proc::TriggerPtr trigger);

  //!@brief a boolean check, if a given step is a listener of this Trigger instance
  bool isListener(cedar::proc::TriggerablePtr step);

  //!@brief a boolean check, if a given Trigger is a listener of this Trigger instance
  bool isListener(cedar::proc::TriggerPtr trigger);

  //!@brief returns a list of listeners
  const std::vector<cedar::proc::TriggerablePtr>& getListeners() const;

  //!@brief returns a list of listening triggers
  const std::vector<cedar::proc::TriggerPtr>& getTriggerListeners() const;

  //!@brief empty implementation of a function that gets called if a trigger connects to this instance
  virtual void notifyConnected(cedar::proc::TriggerPtr trigger);

  //!@brief empty implementation of a function that gets called if a trigger disconnects from this instance
  virtual void notifyDisconnected(cedar::proc::TriggerPtr trigger);

  //!@brief saves a configuration to a ConfigurationNode
  void writeConfiguration(cedar::aux::ConfigurationNode& node);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief removes a listener, which will no longer receive trigger signals
  virtual void removeListener(cedar::proc::TriggerablePtr step);

  //!@brief adds a listener, which will receive trigger signals from this instance from now on
  virtual void addListener(cedar::proc::TriggerablePtr step);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  //!@brief list of listeners
  std::vector<cedar::proc::TriggerablePtr> mListeners;
  //!@brief list of listenings triggers
  std::vector<cedar::proc::TriggerPtr> mTriggers;
private:
  //!@brief find a step in the list of listeners
  std::vector<cedar::proc::TriggerablePtr>::iterator find(cedar::proc::TriggerablePtr triggerable);
  //!@brief find a trigger in the list of trigger listeners
  std::vector<cedar::proc::TriggerPtr>::iterator find(cedar::proc::TriggerPtr triggerableT);
}; // class cedar::proc::Trigger

#endif // CEDAR_PROC_TRIGGER_H

