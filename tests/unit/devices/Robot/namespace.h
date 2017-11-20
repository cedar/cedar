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

    File:        namespace.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 11 08

    Description: Namespace file for cedar::tests::unit::dev::Robot.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_TESTS_UNIT_DEV_ROBOT_NAMESPACE_H
#define CEDAR_TESTS_UNIT_DEV_ROBOT_NAMESPACE_H

#include "cedar/defines.h"

namespace cedar
{
  namespace tests
  {
    namespace unit
    {
      namespace dev
      {
        //!@brief Namespace for the unit test of Robot.
        namespace Robot
        {
          class TestRobot;
          class TestComponent;
          CEDAR_GENERATE_POINTER_TYPES(TestComponent);

          class TestChannel;
          CEDAR_GENERATE_POINTER_TYPES(TestChannel);
        }
      }
    }
  }
}

#endif // CEDAR_TESTS_UNIT_DEV_ROBOT_NAMESPACE_H
