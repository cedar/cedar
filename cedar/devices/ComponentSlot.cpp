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

    File:        ComponentSlot.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 23

    Description: Slot for components of a robot (e.g., a kinematic chain).

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/ComponentSlot.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

  cedar::dev::ComponentPtr cedar::dev::ComponentSlot::getComponent() const
  {
    if (!mComponent)
    {
      std::string component_type_id = _mComponentTypeIds[_mChannelType->getValue()];
      mComponent = FactorySingleton::getInstance()->allocate(component_type_id);
    }

    return mComponent;
  }
