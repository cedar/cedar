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

    File:        ProjectionMapping.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2011 11 30

    Description: Unit test for the cedar::proc::ProjectionMapping class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/ProjectionMapping.h"
#include "cedar/auxiliaries/LogFile.h"

// SYSTEM INCLUDES

int main()
{
  // create log file
  cedar::aux::LogFile log_file("ProjectionMapping.log");
  log_file.addTimeStamp();
  log_file << "\n";

  // count the number of errors
  unsigned int number_of_errors = 0;

  // create a projection mapping
  cedar::proc::ProjectionMapping mapping;

  // initialize the mapping with five elements
  mapping.initialize(2);

  // check that it got initialized with five elements
  if (mapping.getNumberOfMappings() != 2)
  {
    log_file << "The mapping was not initialized with the correct number of mappings.\n";
    ++number_of_errors;
  }

  // check that all five mappings are initialized to drop the input dimension
  bool initialized_to_drop = true;
  for (unsigned int i = 0; i < mapping.getNumberOfMappings(); ++i)
  {
    if (!mapping.isDropped(i))
    {
      initialized_to_drop = false;
    }
  }
  if (!initialized_to_drop)
  {
    log_file << "Not all mappings were initialized to \"drop\" the input dimension\n";
    ++number_of_errors;
  }

  // check that the initial validity of the mapping is VALIDITY_ERROR
  if (mapping.getValidity() != cedar::proc::ProjectionMapping::VALIDITY_ERROR)
  {
    log_file << "The initial validity of the mapping is not VALIDITY_ERROR.\n";
    ++number_of_errors;
  }

  // check that changing a mapping works
  mapping.changeMapping(0, 0);

  if (mapping.lookUp(0) != 0)
  {
    log_file << "Failed to change a mapping.\n";
    ++number_of_errors;
  }

  // check that set and get for the output dimensionality work
  mapping.setOutputDimensionality(2);
  if (mapping.getOutputDimensionality() != 2)
  {
    log_file << "Mapping returned incorrect output dimensionality.\n";
    ++number_of_errors;
  }

  // check that the validity of the mapping remains VALIDITY_ERROR
  if (mapping.getValidity() != cedar::proc::ProjectionMapping::VALIDITY_ERROR)
  {
    log_file << "For an invalid mapping, the validity is not updated to VALIDITY_ERROR.\n";
    ++number_of_errors;
  }

  // check that changing another mapping works
  mapping.changeMapping(1, 1);
  if (mapping.lookUp(1) != 1)
  {
    log_file << "Failed to change a mapping.\n";
    ++number_of_errors;
  }

  // check that the validity of the mapping is updated to VALIDITY_VALID
  if (mapping.getValidity() != cedar::proc::ProjectionMapping::VALIDITY_VALID)
  {
    log_file << "The for a valid mapping, the validity of the mapping is not updated to VALIDITY_VALID.\n";
    ++number_of_errors;
  }

  // check that dropping a dimension works
  mapping.drop(0);
  if (!mapping.isDropped(0))
  {
    log_file << "Dropping a dimension failed.\n";
    ++number_of_errors;
  }

  log_file << "Done. There were " << number_of_errors << " errors.\n";

  return number_of_errors;
}
