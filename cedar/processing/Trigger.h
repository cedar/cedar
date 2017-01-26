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

// CEDAR INCLUDES
#include "cedar/processing/Element.h"
#include "cedar/processing/Triggerable.h"
#include "cedar/auxiliaries/LockableMember.h"
#include "cedar/auxiliaries/GraphTemplate.h"
#include "cedar/auxiliaries/boostSignalsHelper.h"

// FORWARD DECLARATIONS
#include "cedar/processing/sources/GroupSource.fwd.h"
#include "cedar/processing/sinks/GroupSink.fwd.h"
#include "cedar/auxiliaries/GraphTemplate.fwd.h"
#include "cedar/processing/Trigger.fwd.h"
#include "cedar/processing/Step.fwd.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>
#ifndef Q_MOC_RUN
  #include <boost/enable_shared_from_this.hpp>
#endif
#include <vector>
#include <map>
#include <set>

/*!@brief A base class for all sorts of Trigger. Trigger provides a generic interface for the trigger concept in cedar.
 * Trigger can have listeners, to which they send a trigger signal.
 *
 */
class cedar::proc::Trigger : public cedar::proc::Element,
                             public cedar::proc::Triggerable
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::proc::Group;
  friend class cedar::proc::TriggerConnection;
  friend class cedar::proc::Triggerable;
  friend class cedar::proc::Step;

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
  void onTrigger(cedar::proc::ArgumentsPtr args, cedar::proc::TriggerPtr pSender);

  //!@brief a boolean check, if a given triggerable is a listener of this Trigger instance
  bool isListener(cedar::proc::TriggerablePtr triggerable) const;

  //!@brief returns a list of listeners
  const std::vector<cedar::proc::TriggerablePtr>& getListeners() const;

  //!@brief saves a configuration to a ConfigurationNode
  void writeConfiguration(cedar::aux::ConfigurationNode& node);

  //!@brief returns the owner of this trigger (used mainly for processing done triggers)
  cedar::proc::Triggerable* getOwner() const
  {
    return this->mpOwner;
  }

  /*! Returns a copy of the triggering order associated with this trigger.
   *
   *  The uint represents the depth; all triggerables with the same depths can be executed in parallel.
   */
  std::map<unsigned int, std::set<cedar::proc::TriggerablePtr>> getTriggeringOrder() const;

  /*! Checks whether this trigger can be connected to the given Triggerable. The default implementation returns true.
   *
   * @param target This is the triggerable that might be connected.
   * @param reason If false is returned, this should contain a reason why.
   */
  virtual bool canTrigger(cedar::proc::TriggerablePtr target, std::string& reason) const;

  //! Convenience overload of canTrigger(Triggerable, string)
  bool canTrigger(cedar::proc::TriggerablePtr target) const;

  //! Checks if this trigger can be connected to the given triggerable. If not, an exception is thrown.
  void checkIfCanBeConnectedTo(cedar::proc::TriggerablePtr) const;

  /*! Same as checkIfCanBeConnectedTo, but instead of throwing, returns a bool.
   *
   * @return True if the this trigger can be connected to @em target.
   */
  bool testIfCanBeConnectedTo(cedar::proc::TriggerablePtr target) const;

  virtual bool canConnectTo(cedar::proc::ConstTriggerablePtr target) const;

  //! Returns the number of triggerables directly listening to this trigger.
  size_t getTriggerCount() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief removes a listener, which will no longer receive trigger signals
  virtual void removeListener(cedar::proc::TriggerablePtr triggerable);

  //!@brief removes a listener, which will no longer receive trigger signals
  virtual void removeListener(cedar::proc::Triggerable* triggerable);

  //!@brief adds a listener, which will receive trigger signals from this instance from now on
  virtual void addListener(cedar::proc::TriggerablePtr triggerable);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Find a triggerable in the list of listeners.
  std::vector<cedar::proc::TriggerablePtr>::iterator find(cedar::proc::TriggerablePtr triggerable);

  //!@brief Find a triggerable in the list of listeners.
  std::vector<cedar::proc::TriggerablePtr>::iterator find(cedar::proc::Triggerable* triggerable);

  //!@brief Find a triggerable in the list of listeners (const-version).
  std::vector<cedar::proc::TriggerablePtr>::const_iterator find(cedar::proc::TriggerablePtr triggerable) const;

  /*!@brief Updates the order of processing of the subsequent triggerables
   *
   * @todo Describe this properly.
   */
  void updateTriggeringOrder(std::set<cedar::proc::Trigger*>& visited, bool recurseUp = true, bool recurseDown = true);

  //! Updates the triggering order of the source recursively, going upwards the triggering chains.
  void updateTriggeringOrderRecurseUpSource(cedar::proc::sources::GroupSource* source, std::set<cedar::proc::Trigger*>& visited);

  void setOwner(cedar::proc::Triggerable* owner)
  {
    this->mpOwner = owner;
  }

  //! Builds a graph of the trigger structure following this trigger.
  void buildTriggerGraph(cedar::aux::GraphTemplate<cedar::proc::TriggerablePtr>& graph);

  //! Part of the exploration done while building the trigger graph.
  void explore
  (
    cedar::proc::TriggerablePtr source,
    cedar::proc::TriggerPtr trigger,
    cedar::aux::GraphTemplate<cedar::proc::TriggerablePtr>& graph,
    std::vector<cedar::proc::TriggerablePtr>& toExplore,
    bool sourceIsTrigger,
    std::set<cedar::proc::TriggerablePtr>& explored
  );

  /*! Part of the exploration done while building the trigger graph. This function explores the triggers by following a
   *  group sink.
   */
  void exploreSink
  (
    cedar::proc::TriggerablePtr source,
    cedar::aux::GraphTemplate<cedar::proc::TriggerablePtr>::NodePtr sourceNode,
    cedar::proc::sinks::GroupSinkPtr startSink,
    cedar::aux::GraphTemplate<cedar::proc::TriggerablePtr>& graph,
    std::vector<cedar::proc::TriggerablePtr>& to_explore
  );

  /*! Part of the exploration done while building the trigger graph. This function explores the triggers by following a
   *  connection that goes into a group.
   */
  void exploreGroupTarget
  (
    cedar::proc::TriggerablePtr source,
    cedar::proc::GroupPtr listener_group,
    cedar::aux::GraphTemplate<cedar::proc::TriggerablePtr>::NodePtr sourceNode,
    cedar::aux::GraphTemplate<cedar::proc::TriggerablePtr>& graph,
    std::vector<cedar::proc::TriggerablePtr>& to_explore
  );

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! Triggerable owning this trigger
  cedar::proc::Triggerable* mpOwner;

  //! List of listeners.
  cedar::aux::LockableMember<std::vector<cedar::proc::TriggerablePtr> > mListeners;

  //! List of the triggerables following this one
  cedar::aux::LockableMember< std::map<unsigned int, std::set<cedar::proc::TriggerablePtr> > > mTriggeringOrder;

private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // boost signals
  //--------------------------------------------------------------------------------------------------------------------
public:
  CEDAR_DECLARE_SIGNAL(TriggerCountChanged, void(size_t));
}; // class cedar::proc::Trigger

#endif // CEDAR_PROC_TRIGGER_H

