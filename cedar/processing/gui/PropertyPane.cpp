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

    File:        PropertyPane.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 03 09

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/PropertyPane.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/Manager.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/namespace.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::PropertyPane::PropertyPane(QWidget *pParent)
:
cedar::aux::gui::PropertyPane(pParent)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

std::string cedar::proc::gui::PropertyPane::getInstanceTypeId(cedar::aux::ConfigurablePtr pConfigurable) const
{
  if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(pConfigurable))
  {
    return cedar::proc::DeclarationRegistrySingleton::getInstance()->getDeclarationOf(step)->getClassId();
  }
  else if (cedar::proc::TriggerPtr trigger = boost::dynamic_pointer_cast<cedar::proc::Trigger>(pConfigurable))
  {
    return cedar::proc::DeclarationRegistrySingleton::getInstance()->getDeclarationOf(trigger)->getClassId();
  }
  else
  {
    return this->cedar::aux::gui::PropertyPane::getInstanceTypeId(pConfigurable);
  }
}
