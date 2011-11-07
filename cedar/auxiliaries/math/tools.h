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

    File:        tools.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 11

    Description: Math tools

    Credits:

======================================================================================================================*/


#ifndef CEDAR_AUX_MATH_TOOLS_H
#define CEDAR_AUX_MATH_TOOLS_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/math/namespace.h"
#include "cedar/auxiliaries/lib.h"
#include "cedar/auxiliaries/assert.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>

/*
 *  this file has very generic math functions that do not fall under some other category
 *  more specific functions should be put in specialized files
 *  some precompiler macros are also defined here
 */

/* epsilon surrounding for near zero values */
#ifndef EQN_EPS
#define EQN_EPS 1e-9
#endif

#ifndef IsZero
#define IsZero(x) ((x) > -EQN_EPS && (x) < EQN_EPS)
#endif
namespace cedar
{
  namespace aux
  {
    namespace math
    {
      //! normalizes an angle to the representative interval (-pi, pi]
      CEDAR_AUX_LIB_EXPORT double normalizeAngle(double value);
      //! returns the value of the global maximum of a matrix
      CEDAR_AUX_LIB_EXPORT double max(const cv::Mat matrix);
      //! returns the value of the global minimum of a matrix
      CEDAR_AUX_LIB_EXPORT double min(const cv::Mat matrix);
      //! writes the matrix into the shell properly organized by columns and rows
      //!\todo move write(cv::Mat) to aux::utilities
      //!\todo rework (template for copy & paste code)
      //!\todo add log file capability
      CEDAR_AUX_LIB_EXPORT void write(cv::Mat matrix);

      template <typename T>
      T round(T val)
      {
        return std::floor(val + static_cast<T>(0.5));
      }

      inline unsigned int getDimensionalityOf(cv::Mat matrix)
      {
        if (matrix.rows == 1 || matrix.cols == 1)
        {
          return 1;
        }
        return matrix.dims;
      }

      inline void assignMatrixEntry(cv::Mat& matrix, std::vector<int> index, double value)
      {
        CEDAR_ASSERT(matrix.type() == CV_32F || matrix.type() == CV_64F);

        switch (matrix.type())
        {
          case CV_32F:
            matrix.at<float>(&index.at(0)) = static_cast<float>(value);
            break;

          case CV_64F:
            matrix.at<double>(&index.at(0)) = static_cast<double>(value);
            break;

          default:
            // this should never happen due to the assert above.
            CEDAR_ASSERT(false);
        }
      }

      /*!\brief Same functionality as cvReduce for 2D->1D.
       *
       * \param[in] src a 3D matrix source
       * \param[out] dst a 2D matrix destination (CV_64FC1)
       * \param[in] dim on which dimension should be reduced?
       * \param[in] op reduction operator (again, same choices as cvReduce)
       *
       */
      template <typename T>
      void reduceCvMat3D(const cv::Mat& source, cv::Mat& destination, int dimensionToReduce, int reductionOperator = CV_REDUCE_SUM);
    }
  }
}
#endif // CEDAR_AUX_MATH_TOOLS_H
