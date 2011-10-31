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
#include "processing/namespace.h"
#include "auxiliaries/NamedConfigurable.h"


// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <vector>

/*!@brief A base class for all sorts of Trigger. Trigger provides a generic interface for the trigger concept in cedar.
 * Trigger can have listeners, to which they send a trigger signal.
 *
 */
class cedar::proc::Trigger : public virtual cedar::aux::NamedConfigurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Trigger(const std::string& name = "");

  //!@brief Destructor
  virtual ~Trigger();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief sends trigger signal and arguments to all listeners
  void trigger(cedar::proc::ArgumentsPtr arguments = cedar::proc::ArgumentsPtr());

  //!@brief handles an incoming trigger signal if Trigger instance is listener
  virtual void onTrigger(Trigger*);

  //!@brief adds a listener, which will receive trigger signals from this instance from now on
  void addListener(cedar::proc::StepPtr step);

  //!@brief adds a Trigger, which will receive trigger signals from this instance from now on
  void addTrigger(cedar::proc::TriggerPtr trigger);

  //!@brief removes a listener, which will no longer receive trigger signals
  void removeListener(cedar::proc::StepPtr step);

  //!@brief removes a Trigger, which will no longer receive trigger signals
  void removeTrigger(cedar::proc::TriggerPtr trigger);

  //!@brief a boolean check, if a given step is a listener of this Trigger instance
  bool isListener(cedar::proc::StepPtr step);

  //!@brief a boolean check, if a given Trigger is a listener of this Trigger instance
  bool isListener(cedar::proc::TriggerPtr trigger);

  //!@brief returns a list of listeners
  const std::vector<cedar::proc::StepPtr>& getListeners() const;

  //!@brief returns a list of listening triggers
  const std::vector<cedar::proc::TriggerPtr>& getTriggerListeners() const;


  virtual void notifyConnected(cedar::proc::Trigger* trigger);

  virtual void notifyDisconnected(cedar::proc::Trigger* trigger);

  //!@brief reads a configuration from a ConfigurationNode
  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  //!@brief saves a configuration to a ConfigurationNode
  void saveConfiguration(cedar::aux::ConfigurationNode& node);

  //!@brief sets the TriggerRegistry at which this Trigger instance is stored
  void setRegistry(cedar::proc::TriggerRegistry* pRegistry);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

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
  std::vector<cedar::proc::StepPtr> mListeners;
  //!@brief list of listenings triggers
  std::vector<cedar::proc::TriggerPtr> mTriggers;
private:
  //!@brief find a step in the list of listeners
  std::vector<cedar::proc::StepPtr>::iterator find(cedar::proc::StepPtr triggerable);
  //!@brief find a trigger in the list of trigger listeners
  std::vector<cedar::proc::TriggerPtr>::iterator find(cedar::proc::TriggerPtr triggerableT);

  //!@brief Registry managing the trigger.
  cedar::proc::TriggerRegistry* mRegisteredAt;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::Trigger

#endif // CEDAR_PROC_TRIGGER_H

