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
    Date:        2011 05 27

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_COMP_MULTI_TRIGGER_H
#define CEDAR_AUX_COMP_MULTI_TRIGGER_H

// LOCAL INCLUDES
#include "auxiliaries/computation/namespace.h"
#include "auxiliaries/computation/Trigger.h"


// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <vector>
#include <map>

/*!@brief A trigger that merges multiple trigger signals into one.
 *
 * This class maintains a list of incoming triggers. Everytime one of the incoming triggers receives a trigger signal,
 * it is marked as triggered. Once all incoming triggers are marked thusly, the MultiTrigger sends a trigger signal to
 * all its listeners.
 */
class cedar::aux::comp::MultiTrigger : public Trigger
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  MultiTrigger();

  //!@brief Destructor
  virtual ~MultiTrigger();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void onTrigger(Trigger* sender);

  void notifyConnected(cedar::aux::comp::Trigger* trigger);

  void notifyDisconnected(cedar::aux::comp::Trigger* trigger);

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
  void checkCondition();
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:

private:
  std::map<Trigger*, bool> mIncoming;

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

