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

void cedar::aux::kernel::Separable::setNumParts(unsigned int numParts)
{
  this->mKernelParts.resize(numParts);
}

cv::Mat cedar::aux::kernel::Separable::convolveWith(const cv::Mat& mat) const
{
  cv::Mat tmp = mat.clone();
  //!@todo Create a convolve function in cedar::aux(::?) and use it here. That method should also handle any dimensionaly.
  //!@todo Parameter of the convolution must be changeable.
  for (unsigned int i = 0; i < this->mKernelParts.size(); ++i)
  {
    const cv::Mat& kernel_part = this->getKernelPart(i);
    cv::filter2D(tmp, tmp, -1, kernel_part);
  }
  return tmp;
}

const cv::Mat& cedar::aux::kernel::Separable::getKernelPart(unsigned int dimension) const
{
  return mKernelParts.at(dimension);
}

void cedar::aux::kernel::Separable::setKernelPart(unsigned int dimension, const cv::Mat& mat)
{
  this->mKernelParts.at(dimension) = mat;
}
