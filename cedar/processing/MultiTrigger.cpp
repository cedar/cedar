/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

// CEDAR INCLUDES
#include "cedar/processing/MultiTrigger.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"

// SYSTEM INCLUDES
#include <algorithm>


//----------------------------------------------------------------------------------------------------------------------
// register the trigger class
//----------------------------------------------------------------------------------------------------------------------

// for now, we don't need to use the multi-trigger as it is not useful (and just leads to confusion)
//namespace
//{
//  bool declare()
//  {
//    using cedar::proc::ElementDeclarationPtr;
//    using cedar::proc::ElementDeclarationTemplate;
//
//    ElementDeclarationPtr multi_trigger_declaration
//    (
//      new ElementDeclarationTemplate<cedar::proc::MultiTrigger>
//      (
//        "Triggers",
//        "cedar.processing.MultiTrigger"
//      )
//    );
//    multi_trigger_declaration->setIconPath(":/triggers/multi_trigger.svg");
//
//    cedar::aux::Singleton<cedar::proc::DeclarationRegistry>::getInstance()->declareClass(multi_trigger_declaration);
//
//    return true;
//  }
//
//  bool declared = declare();
//}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::MultiTrigger::MultiTrigger()
{
}

cedar::proc::MultiTrigger::~MultiTrigger()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::MultiTrigger::onTrigger(cedar::proc::ArgumentsPtr, cedar::proc::TriggerPtr pSender)
{
  std::map<cedar::proc::TriggerPtr, bool>::iterator iter = this->mIncoming.find(pSender);

  if (iter != this->mIncoming.end())
  {
#ifdef DEBUG_TRIGGERING
    std::cout << "Multitrigger::onTrigger(" << pSender.get() << ")" << std::endl;
#endif
    iter->second = true;
    this->checkCondition();
  }
}

void cedar::proc::MultiTrigger::checkCondition()
{
  // Check whether all incoming triggers have triggered the multitrigger
  for (std::map<cedar::proc::TriggerPtr, bool>::iterator iter = this->mIncoming.begin(); iter != this->mIncoming.end(); ++iter)
  {
    // if one is false, return and do nothing
    if (!iter->second)
    {
#ifdef DEBUG_TRIGGERING
      std::cout << "Multitrigger::checkCondition(): false: " << iter->first.get() << std::endl;
#endif
      return;
    }
  }

  // if we got here, all triggers have triggered
  this->trigger();

  // reset all triggers
  for (std::map<cedar::proc::TriggerPtr, bool>::iterator iter = this->mIncoming.begin(); iter != this->mIncoming.end(); ++iter)
  {
    iter->second = false;
  }
}

void cedar::proc::MultiTrigger::notifyConnected(cedar::proc::TriggerPtr trigger)
{
  mIncoming[trigger] = false;
}

void cedar::proc::MultiTrigger::notifyDisconnected(cedar::proc::TriggerPtr trigger)
{
  std::map<cedar::proc::TriggerPtr, bool>::iterator iter = this->mIncoming.find(trigger);
  if (iter != this->mIncoming.end())
  {
    this->mIncoming.erase(iter);
  }
}
