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

    File:        sigmoids.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 11

    Description: Sigmoid functions

    Credits:

======================================================================================================================*/
#ifndef CEDAR_AUX_MATH_SIGMOIDS_H
#define CEDAR_AUX_MATH_SIGMOIDS_H

// LOCAL INCLUDES
#include "auxiliaries/math/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <math.h>
#include <vector>
#include <cv.h>

/*
 *  different sigmoid functions for different data formats
 *  default is the exponential sigmoid, just called sigmoid()
 *  other kinds of sigmoids have more specific names
 */
namespace cedar
{
  namespace aux
  {
    namespace math
    {
      //! an exponential-based sigmoid for a single double value
      double sigmoid(const double x, const double beta, const double threshold = 0);

      //! an abs-based sigmoid for a single double value
      double sigmoidAbs(const double x, const double beta, const double threshold = 0);

      //! an exponential-based sigmoid for a cv::Mat
      template<typename T>
      cv::Mat sigmoid(const cv::Mat& mat, const double beta, const double threshold = 0);

      //! an exponential-based sigmoid for a cv::Mat, that takes the result as an argument and does not allocate new memory
        template<typename T>
        void sigmoid(const cv::Mat& mat, cv::Mat& result, const double beta, const double threshold = 0);

      //! an abs-based sigmoid for a cv::Mat
      template<typename T>
      cv::Mat sigmoidAbs(const cv::Mat& mat, const double beta, const double threshold = 0);

      //! an abs-based sigmoid for a cv::Mat, that takes the result as an argument and does not allocat new memory
      template<typename T>
      void sigmoidAbs(const cv::Mat& mat, cv::Mat& result, const double beta, const double threshold = 0);

      //! a sigmoid for a vector of doubles
      std::vector<double> sigmoid(const std::vector<double>& x, const double beta, const double threshold = 0);
  
      //! a sigmoid that rises smoothly in an interval and is exactly zero resp. one outside that interval
      double sigmoidInterval(const double value, const double t1, const double t2, const bool decreasing = false);
    };
  };
};
#endif  // CEDAR_AUX_MATH_SIG_NAMESPACE_H
