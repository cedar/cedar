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

    File:        DoubleParameter.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 02 28

    Description: This file is responsible for declaring DoubleParameter so that it can be instantiated at run-time.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/ParameterDeclaration.h"

namespace
{
  bool registerParameter()
  {
    typedef cedar::aux::ParameterDeclaration<cedar::aux::DoubleParameterPtr> Declaration;
    CEDAR_GENERATE_POINTER_TYPES(Declaration);

    DeclarationPtr declaration(new Declaration("numeric", "cedar.aux.DoubleParameter"));
    declaration->declare();

    return true;
  }

  bool registered = registerParameter();
}
