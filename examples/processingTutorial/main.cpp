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

    File:        main.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 12 09

    Description:

    Credits:

======================================================================================================================*/

#include <cedar/processing/Group.h>
#include <cedar/processing/sources/GaussInput.h>
#include "SimpleSummation.h" // header for the class we have written above

int main(int, char**) // we don't use the arguments here
{
  // create a group
  cedar::proc::GroupPtr group(new cedar::proc::Group());

  boost::shared_ptr<SimpleSummation> sum(new SimpleSummation());
  // This adds the sum step to the group under the name "sum".
  group->add(sum, "sum");

  // Gauss inputs
  cedar::proc::sources::GaussInputPtr gauss1(new cedar::proc::sources::GaussInput());
  group->add(gauss1, "gauss1");
  cedar::proc::sources::GaussInputPtr gauss2(new cedar::proc::sources::GaussInput());
  group->add(gauss2, "gauss2");

  // connect the first gauss to the sum
  group->connectSlots("gauss1.Gauss input", "sum.operand1");
  // connect the second gauss to the sum
  group->connectSlots("gauss2.Gauss input", "sum.operand2");

  return 0;
}

