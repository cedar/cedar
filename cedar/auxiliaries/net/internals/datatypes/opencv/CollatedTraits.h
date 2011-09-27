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

    File:        CollatedTraits.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011 04:43:49 PM CEST

    Description: collated_traits in the opencv specialization

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATEDTRAITS_OPENCV_H
#define CEDAR_COLLATEDTRAITS_OPENCV_H

// LOCAL INCLUDES
// traits specializations:
#include "../../namespace.h"
#include <opencv2/opencv.hpp>

// PROJECT INCLUDES

// SYSTEM INCLUDES


namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {
        template<> struct collated_traits<cv::Mat>;
        template<> struct collated_traits< cv::Mat_<float> >;
        template <typename CVT> class cvMatHelper;
        class cvMatNetHeader; // forward declaration
      }
    }
  }
}


//////////////// OPENCV SPECIALIZATION of our traits class

//!@brief collated_traits implementation for cv::Mat
template<>
struct cedar::aux::net::detail::collated_traits<cv::Mat>
{
  typedef cv::Mat                   data_type;
  typedef cedar::aux::net::detail::cvMatHelper<cv::Mat>    helper_type;
  typedef cedar::aux::net::detail::cvMatNetHeader header_type;
};

//!@brief collated_traits implementation for cv::Mat_<float> 
template<>
struct cedar::aux::net::detail::collated_traits< cv::Mat_<float> >
{
  typedef cv::Mat_<float>           data_type;
  typedef cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >    helper_type;
  typedef cedar::aux::net::detail::cvMatNetHeader header_type;
};

#endif

