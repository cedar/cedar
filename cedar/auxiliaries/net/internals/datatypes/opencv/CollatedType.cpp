/*=============================================================================

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

===============================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        CollatedType.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011 05:14:51 PM CEST

    Description: openCV specialization for CollatedType.
                 This tells us how to acces an openCV-type.
                 This file holds explicit instantiations.

    Credits:

=============================================================================*/

// LOCAL INCLUDES
#include "../CollatedType.h"

// PROJECT INCLUDES
#include <opencv/cv.h>

// SYSTEM INCLUDES



//////////// explicit instatiation
template struct cedar::aux::net::detail::CollatedType<cv::Mat>;
template struct cedar::aux::net::detail::CollatedType< cv::Mat_<float> >;


