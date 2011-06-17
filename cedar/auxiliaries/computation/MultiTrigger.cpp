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

    File:        MultiTrigger.cpp


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

// LOCAL INCLUDES
#include "auxiliaries/computation/MultiTrigger.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <algorithm>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::comp::MultiTrigger::MultiTrigger()
{
}

cedar::aux::comp::MultiTrigger::~MultiTrigger()
{
}



//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::aux::comp::MultiTrigger::onTrigger(Trigger* sender)
{
  std::map<Trigger*, bool>::iterator iter = this->mIncoming.find(sender);

  if (iter != this->mIncoming.end())
  {
    iter->second = true;
    this->checkCondition();
  }
}

void cedar::aux::comp::MultiTrigger::checkCondition()
{
  // Check whether all incoming triggers have triggered the multitrigger
  for (std::map<Trigger*, bool>::iterator iter = this->mIncoming.begin(); iter != this->mIncoming.end(); ++iter)
  {
    // if one is false, return and do nothing
    if (!iter->second)
    {
      return;
    }
  }

  // if we got here, all triggers have triggere
  this->trigger();

  // reset all triggers
  for (std::map<Trigger*, bool>::iterator iter = this->mIncoming.begin(); iter != this->mIncoming.end(); ++iter)
  {
    iter->second = false;
  }
}

void cedar::aux::comp::MultiTrigger::notifyConnected(cedar::aux::comp::Trigger* trigger)
{
  mIncoming[trigger] = false;
}

void cedar::aux::comp::MultiTrigger::notifyDisconnected(cedar::aux::comp::Trigger* trigger)
{
  std::map<Trigger*, bool>::iterator iter = this->mIncoming.find(trigger);
  if (iter != this->mIncoming.end())
  {
    this->mIncoming.erase(iter);
  }
}
