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

cedar::proc::typecheck::SameSizedCollection::SameSizedCollection(bool allow0D, bool allow1DTranspositions)
:
mAllow0D(allow0D),
mAllow1DTranspositions(allow1DTranspositions)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::DataSlot::VALIDITY
  cedar::proc::typecheck::SameSizedCollection::check(cedar::proc::ConstDataSlotPtr slot, cedar::aux::ConstDataPtr data, std::string& info)
  const
{
  //!@todo Should this be necessary? Aren't typechecks always performed on input slots?
  auto external_slot = cedar::aux::asserted_pointer_cast<cedar::proc::ConstExternalData>(slot);
  if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
  {
    if (mat_data->isEmpty())
    {
      info = "Got an empty matrix, but cannot handle empty matrices.";
      return this->validityBad();
    }

    // if 0d matrices are allowed and the data is 0d, accept it
    if (this->mAllow0D && mat_data->getDimensionality() == 0)
    {
      return this->validityOk();
    }

    // find data for comparing the size
    cedar::aux::ConstMatDataPtr data_to_compare;

    for (size_t i = 0; i < external_slot->getDataCount(); ++i)
    {
      auto slot_mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(external_slot->getData(i));
      if (slot_mat_data && slot_mat_data != data)
      {
        // If 0d data is not allowed, we just take the first mat data we can find (if it is 0d, all others must be as well).
        // Otherwise, we look for the first non-0d mat data.
        if (!this->mAllow0D || slot_mat_data->getDimensionality() > 0)
        {
          data_to_compare = slot_mat_data;
          break;
        }
      }
    }

    // If we didn't find data to compare to, this is the first valid data for the slot, accept it.
    // Otherwise, only accept if the sizes match
    if (!data_to_compare || data_to_compare->getData().size == mat_data->getData().size)
    {
      return this->validityOk();
    }
    // If sizes don't match but 1D transpositions are allowed, check if both data objects are 1D and have the same length
    else if (this->mAllow1DTranspositions && data_to_compare->getDimensionality() == 1 && mat_data->getDimensionality() == 1)
    {
      if (cedar::aux::math::get1DMatrixSize(data_to_compare->getData()) && cedar::aux::math::get1DMatrixSize(mat_data->getData()))
      {
        return this->validityOk();
      }
    }
  }

  // if none of the above returned valid, well, tough luck.
  info = "Not all matrices are of same size.";
  return this->validityBad();
}
