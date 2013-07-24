/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2013 07 15

    Description: Namespace file for cedar::testing::dev.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_TESTING_DEV_NAMESPACE_H
#define CEDAR_TESTING_DEV_NAMESPACE_H

// CEDAR INCLUDES
#include "cedar/testing/lib.h"

// SYSTEM INCLUDES

namespace cedar
{
  namespace testing
  {
    /*!@brief Namespace that encapsulates functionality shared across dev unit/interactive tests.
     *
     *        This is an internal namespace of cedar. Functionality in here should not be used outside of unit tests!
     */
    namespace dev
    {
      // grabber class only for unit-test
      CEDAR_DECLARE_TESTING_CLASS(TestGrabber);
      CEDAR_DECLARE_TESTING_CLASS(TestChannel);
    }
  }
}

#endif // CEDAR_TESTING_DEV_NAMESPACE_H
