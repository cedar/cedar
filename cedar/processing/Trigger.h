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
#include "auxiliaries/Base.h"


// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <vector>

/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::Trigger : public virtual cedar::aux::Base
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Trigger();

  //!@brief Destructor
  virtual ~Trigger();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void trigger(cedar::proc::ArgumentsPtr arguments = cedar::proc::ArgumentsPtr());

  virtual void onTrigger(Trigger*);

  void addListener(cedar::proc::StepPtr step);

  void addTrigger(cedar::proc::TriggerPtr trigger);

  void removeListener(cedar::proc::StepPtr step);

  void removeTrigger(cedar::proc::TriggerPtr trigger);

  const std::vector<cedar::proc::StepPtr>& getListeners() const;

  virtual void notifyConnected(cedar::proc::Trigger* trigger);

  virtual void notifyDisconnected(cedar::proc::Trigger* trigger);

  virtual void readConfiguration(const cedar::aux::ConfigurationNode& node);

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
  std::vector<cedar::proc::StepPtr> mListeners;
  std::vector<cedar::proc::TriggerPtr> mTriggers;
private:
  std::vector<cedar::proc::StepPtr>::iterator find(cedar::proc::StepPtr triggerable);
  std::vector<cedar::proc::TriggerPtr>::iterator find(cedar::proc::TriggerPtr triggerableT);

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

