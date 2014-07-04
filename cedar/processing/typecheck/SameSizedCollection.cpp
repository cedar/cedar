/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        SameSizedCollection.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 01 06

    Description: Source file for the class cedar::proc::typecheck::SameSizedCollection.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/SameSizedCollection.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::typecheck::SameSizedCollection::SameSizedCollection(bool allow0D)
:
mAllow0D(allow0D)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::DataSlot::VALIDITY
  cedar::proc::typecheck::SameSizedCollection::check(cedar::proc::ConstDataSlotPtr slot, cedar::aux::ConstDataPtr data)
  const
{
  //!@todo Should this be necessary? Aren't typechecks always performed on input slots?
  auto external_slot = cedar::aux::asserted_pointer_cast<cedar::proc::ConstExternalData>(slot);
  if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
  {
    //!@todo Special case: what if the first items are all 0D? This should still accept. Might be a problem for steps using this check, though.
    if (external_slot->getDataCount() == 0
        || (this->mAllow0D && mat_data->getDimensionality() == 0)
        || external_slot->getData(0)->getData<cv::Mat>().size == mat_data->getData().size)
    {
      return this->validityOk();
    }
  }

  return this->validityBad();
}
