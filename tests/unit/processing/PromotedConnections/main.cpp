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

    File:        main.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 19

    Description: 

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/dynamics/fields/NeuralField.h"
#include "cedar/processing/steps/StaticGain.h"
#include "cedar/processing/Network.h"

// SYSTEM INCLUDES
#include <iostream>

int main(int, char**)
{
  // the number of errors encountered in this test
  int errors = 0;
  
  cedar::proc::NetworkPtr network (new cedar::proc::Network());

  cedar::dyn::NeuralFieldPtr field(new cedar::dyn::NeuralField());
  cedar::proc::steps::StaticGainPtr static_gain(new cedar::proc::steps::StaticGain());

  std::string field_output_name = field->getDataSlots(cedar::proc::DataRole::OUTPUT).begin()->second->getName();
  std::string static_gain_input_name = static_gain->getDataSlots(cedar::proc::DataRole::INPUT).begin()->second->getName();
  std::cout << "Field output name determined as: " << field_output_name << std::endl;
  std::cout << "Gain input name determined as: " << static_gain_input_name << std::endl;

  network->add(field, "field");
  network->add(static_gain, "gain");

  network->connectSlots("field." + field_output_name, "gain." + static_gain_input_name);

  cedar::proc::NetworkPtr subnetwork (new cedar::proc::Network());
  network->add(subnetwork, "subnetwork");
  std::list<cedar::proc::ElementPtr> elements;
  elements.push_back(field);
  subnetwork->add(elements);

  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
