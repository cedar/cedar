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

    File:        SameSize.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 01 17

    Description: Source file for the class cedar::proc::typecheck::SameSize.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/SameSize.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::typecheck::SameSize::addSlot(cedar::proc::DataSlotWeakPtr slot)
{
  mSlots.insert(slot);
}

cedar::proc::DataSlot::VALIDITY
  cedar::proc::typecheck::SameSize::check(cedar::proc::ConstDataSlotPtr, cedar::aux::ConstDataPtr data, std::string& info) const
{
  auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);

  if (!mat_data)
  {
    info = "Expected MatData, but got something different.";
    return this->validityBad();
  }

  for (const auto& slot_weak : this->mSlots)
  {
    auto slot = slot_weak.lock();

    if (slot)
    {
      auto slot_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(slot->getData());
      if (slot_data && !cedar::aux::math::matrixSizesEqual(mat_data->getData(), slot_data->getData()))
      {
        info = "The size of this matrix does not match the size of the matrix in slot " + slot->getName() + ".";
        return this->validityBad();
      }
    }
  }

  return this->validityOk();
}
