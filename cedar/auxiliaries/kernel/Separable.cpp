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

    File:        Separable.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 07 07

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "auxiliaries/kernel/Separable.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::kernel::Separable::Separable()
{
}

cedar::aux::kernel::Separable::Separable(unsigned int dimensionality, const std::string& kernelFile)
:
cedar::aux::kernel::Kernel(dimensionality, kernelFile)
{
}

cedar::aux::kernel::Separable::~Separable()
{
#ifdef DEBUG
  std::cout << "> freeing data (Separable)" << std::endl;
#endif
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
const cv::Mat& cedar::aux::kernel::Separable::getKernelPart(unsigned int dimension) const
{
  return mKernelParts.at(dimension);
}