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

    File:        Arguments.h

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

#ifndef CEDAR_AUX_COMP_TRIGGER_H
#define CEDAR_AUX_COMP_TRIGGER_H

// LOCAL INCLUDES
#include "auxiliaries/computation/namespace.h"


// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <vector>

/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::aux::comp::Trigger
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
  void trigger();

  virtual void onTrigger(Trigger*);

  void addListener(cedar::aux::comp::ProcessingStepPtr step);

  void addTrigger(cedar::aux::comp::TriggerPtr trigger);

  void removeListener(cedar::aux::comp::ProcessingStepPtr step);

  void removeTrigger(cedar::aux::comp::TriggerPtr trigger);

  const std::vector<cedar::aux::comp::ProcessingStepPtr>& getListeners() const;

  virtual void notifyConnected(cedar::aux::comp::Trigger* trigger);

  virtual void notifyDisconnected(cedar::aux::comp::Trigger* trigger);

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
  std::vector<cedar::aux::comp::ProcessingStepPtr> mListeners;
  std::vector<cedar::aux::comp::TriggerPtr> mTriggers;
private:
  std::vector<cedar::aux::comp::ProcessingStepPtr>::iterator find(cedar::aux::comp::ProcessingStepPtr triggerable);
  std::vector<cedar::aux::comp::TriggerPtr>::iterator find(cedar::aux::comp::TriggerPtr triggerableT);

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::comp::Trigger

#endif // CEDAR_AUX_COMP_TRIGGER_H

