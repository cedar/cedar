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

// CEDAR INCLUDES
#include "cedar/auxiliaries/kernel/Separable.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::kernel::Separable::Separable()
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
}

cedar::aux::kernel::Separable::Separable(unsigned int dimensionality)
:
cedar::aux::kernel::Kernel(dimensionality)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  this->mKernelParts.resize(dimensionality);
}

cedar::aux::kernel::Separable::~Separable()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

const cv::Mat& cedar::aux::kernel::Separable::getKernelPart(unsigned int dimension) const
{
  return this->mKernelParts.at(dimension);
}

void cedar::aux::kernel::Separable::setKernelPart(unsigned int dimension, const cv::Mat& mat)
{
  this->mKernelParts.at(dimension) = mat;
}
