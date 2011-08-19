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

    File:        NetReader.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Thu 18 Aug 2011 11:39:32 AM CEST

    Description: This file only has explicit template initializations.
                 Please refer to NetReader.h

    Credits:

=============================================================================*/

// LOCAL INCLUDES
#include "NetReader.h"
#include "internals/transport/simple/SimpleNetReader.h"
#include "internals/transport/collated/CollatedNetReader.h"
#include "internals/datatypes/opencv/cvMatHelper.h"

// PROJECT INCLUDES
#include <opencv/cv.h>

// SYSTEM INCLUDES


using namespace cedar::aux::net::detail;

namespace cedar {
  namespace aux {
    namespace net {


// explizite instantiierung:
template class NetReader<int>;
template class NetReader<float>;
template class NetReader<double>;
template class NetReader<cv::Mat>;

} } } // end namespaces

