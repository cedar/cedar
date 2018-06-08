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

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 01 31

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/sources/Noise.h"

// SYSTEM INCLUDES
#include <QApplication>

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

int main()
{
  while (true)
  {
    // this causes a memory leak somewhere within the connections established in cedar::proc::Connectable::declareData.
    // When all the code in there is commented out, this program no longer leaks. Some of it could probably be fixed
    // by using scoped_connections there to actually disconnect the connections created there, however, there seems to
    // be an issue with the way boost deallocates memory referenced in signals (see
    // https://svn.boost.org/trac/boost/ticket/8533)
    cedar::proc::sources::Noise noise_test;
  }

  return 0;
}
