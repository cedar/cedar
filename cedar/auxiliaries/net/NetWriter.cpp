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

    File:        NetWriter.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Thu 18 Aug 2011 11:39:36 AM CEST

    Description: This file only has explicit template initializations.
                 Please refer to NetWriter.h

    Credits:

=============================================================================*/

// LOCAL INCLUDES
#include "NetWriter.h"
#include "internals/transport/simple/SimpleNetWriter.h"
#include "internals/transport/collated/CollatedNetWriter.h"
#include "internals/datatypes/opencv/cvMatHelper.h"

// PROJECT INCLUDES
#include <opencv2/opencv.hpp>

// SYSTEM INCLUDES


namespace cedar {
  namespace aux {
    namespace net {

// explicit instatiation:
template class NetWriter<int>;
template class NetWriter<float>;
template class NetWriter<double>;
template class NetWriter<cv::Mat>;

} } } // end namespaces


