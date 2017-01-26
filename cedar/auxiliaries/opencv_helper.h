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

    File:        tools.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2014 11 26

    Description: A helper to work around some of opencvs changes over different versions

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_OPENCV_HELPER_H
#define CEDAR_AUX_OPENCV_HELPER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// opencv incldues
#include <opencv2/opencv.hpp>

// in opencv 3, CV_VERSION_MAJOR means something different than in opencv 2...
#ifdef CV_VERSION_EPOCH
  #define CEDAR_OPENCV_MAJOR_VERSION CV_VERSION_EPOCH
#else
  #define CEDAR_OPENCV_MAJOR_VERSION CV_VERSION_MAJOR
#endif


#if CEDAR_OPENCV_MAJOR_VERSION >= 3
#define CEDAR_OPENCV_CONSTANT(CONSTANT_NAME) cv::CONSTANT_NAME
#else
#define CEDAR_OPENCV_CONSTANT(CONSTANT_NAME) CV_ ## CONSTANT_NAME
#endif

#endif // CEDAR_AUX_OPENCV_HELPER_H
