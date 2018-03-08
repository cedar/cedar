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
    Date:        2010 11 11

    Description: Math tools

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MATH_TOOLS_H
#define CEDAR_AUX_MATH_TOOLS_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/lib.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/opencv_helper.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <type_traits>
#include <vector>
#include <string>

#if defined CEDAR_COMPILER_MSVC && _MSC_VER < 1600 // MSVC with a version greater than 1600 should come with the stdint header.
  typedef __int8 int8_t;
  typedef unsigned __int8 uint8_t;
  typedef __int16 int16_t;
  typedef unsigned __int16 uint16_t;
  typedef __int32 int32_t;
  typedef unsigned __int32 uint32_t;
  typedef __int64 int64_t;
  typedef unsigned __int64 uint64_t;
#else
  #include <stdint.h>
#endif

// Windows specific undefinition of the max macro
#ifdef CEDAR_OS_WINDOWS
#  ifdef max
#    undef max
#  endif // def max
#  ifdef min
#    undef min
#  endif // def min
#endif // CEDAR_OS_WINDOWS

/*
 *  this file has very generic math functions that do not fall under some other category
 *  more specific functions should be put in specialized files
 *  some precompiler macros are also defined here
 */


/* epsilon surrounding for near zero values */
#ifndef EQN_EPS
#define EQN_EPS 1e-9
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

      /*! Flips a matrix of up to 3 dimensions.
       * @param flipped Matrix to which the result is written. Should be initialized to the proper size (same size as input).
       */
      CEDAR_AUX_LIB_EXPORT void flip(const cv::Mat& toFlip, cv::Mat& flipped, const std::vector<bool>& flippedDimensions);

      //! Flips a matrix of up to 3 dimensions.
      CEDAR_AUX_LIB_EXPORT cv::Mat flip(const cv::Mat& toFlip, const std::vector<bool>& flippedDimensions);

      //! Returns the index of the global maximum of a matrix.
      CEDAR_AUX_LIB_EXPORT unsigned int maxIndex1D(const cv::Mat matrix);

      //! Returns the index of the global minimum of a matrix.
      CEDAR_AUX_LIB_EXPORT unsigned int minIndex1D(const cv::Mat matrix);

      //!@brief a templated round function
      template <typename T> inline T round(T val)
      {
        return std::floor(val + static_cast<T>(0.5));
      }

      //!@brief checks the zero-ness of a floating point value with a given tolerance
      template <typename T>  inline bool isZero(const T& value, const T& precision = static_cast<T>(1e-8))
      {
        if (std::is_floating_point<T>::value)
        {
          return value > -precision && value < precision;
        }
        else
        {
          return value == 0;
        }
      }

      /*!@brief   Limits a number to be in the range [lower, upper].
       *
       * @returns max(lower, min(value, upper))
       */
      template <typename T> inline T saturate(const T& value, const T& lower, const T& upper)
      {
        return std::max(lower, std::min(value, upper));
      }
      
      /*!@brief   Function that checks whether a matrix has the correct type.
       *
       * @returns True, if mat.type() corresponds to the given T (e.g., if mat.type() == CV_32F and T == float),
       *          false otherwise.
       */
      template <typename T>
      inline bool matrixTypeCheck(const cv::Mat&)
      {
        // this should not happen because the actual type-check is implemented in the template specializations.
        CEDAR_THROW(cedar::aux::UnhandledTypeException, "The given matrix type check is not implemented.");
      }

      /*!@brief Template specialization for matrixTypeCheck with double/CV_64F.
       *
       */
      template <>
      inline bool matrixTypeCheck<double>(const cv::Mat& mat)
      {
        return mat.type() == CV_64F;
      }

      /*!@brief Template specialization for matrixTypeCheck with double/CV_64F.
       *
       */
      template <>
      inline bool matrixTypeCheck<float>(const cv::Mat& mat)
      {
        return mat.type() == CV_32F;
      }

      //!@brief Returns a string corresponding to the given matrix's type
	  CEDAR_AUX_LIB_EXPORT std::string matrixTypeToString(const cv::Mat& matrix);

      //! Converts a string representation, e.g., "CV_32F" to the corresponding type value, e.g., CV_32F
      int matrixTypeFromString(const std::string& typeStr);

      //!@brief a helper function to determine the real dimensionality of a cv::Mat (matrix.dims works only for 2+ dims)
      inline unsigned int getDimensionalityOf(const cv::Mat& matrix)
      {
        if (matrix.rows == 1 && matrix.cols == 1)
        {
          return 0;
        }
        else if (matrix.rows == 1 || matrix.cols == 1)
        {
          return 1;
        }
        return matrix.dims;
      }

      //!@brief Determines the size of a 1d matrix.
      inline unsigned int get1DMatrixSize(const cv::Mat& mat)
      {
        CEDAR_DEBUG_ASSERT(mat.rows == 1 || mat.cols == 1);
        if (mat.rows == 1)
        {
          return mat.cols;
        }
        else
        {
          return mat.rows;
        }
      }

      //!@brief set a matrix entry to a given value without having to check if the matrix is of type double or float
      template <typename T>
      inline void assignMatrixEntry(cv::Mat& matrix, std::vector<int> index, T value)
      {
        CEDAR_ASSERT
        (
          matrix.type() == CV_8U
          || matrix.type() == CV_16U
          || matrix.type() == CV_32F
          || matrix.type() == CV_64F
        );

        switch (matrix.type())
        {
          case CV_8U:
            matrix.at<uint8_t>(&index.at(0)) = static_cast<uint8_t>(value);
            break;

          case CV_16U:
            matrix.at<uint16_t>(&index.at(0)) = static_cast<uint16_t>(value);
            break;

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

      /*!@brief Set a 1D or 0D matrix entry to a given value without having to check if the matrix is of type double or
       *        float.
       */
      template <typename T>
      inline void assignMatrixEntry(cv::Mat& matrix, int index, T value)
      {
        CEDAR_ASSERT
        (
          matrix.type() == CV_8U
          || matrix.type() == CV_16U
          || matrix.type() == CV_32F
          || matrix.type() == CV_64F
        );
        CEDAR_ASSERT(cedar::aux::math::getDimensionalityOf(matrix) <= 1);

        switch (matrix.type())
        {
          case CV_8U:
            matrix.at<uint8_t>(index) = static_cast<uint8_t>(value);
            break;

          case CV_16U:
            matrix.at<uint16_t>(index) = static_cast<uint16_t>(value);
            break;

          case CV_32F:
            matrix.at<float>(index) = static_cast<float>(value);
            break;

          case CV_64F:
            matrix.at<double>(index) = static_cast<double>(value);
            break;

          default:
            // this should never happen due to the assert above.
            CEDAR_ASSERT(false);
        }
      }

      /*!@brief Returns the value of a 1D or 0D matrix entry as the specified type.
       */
      template <typename T>
      inline T getMatrixEntry(const cv::Mat& matrix, int index)
      {
        CEDAR_ASSERT
        (
          matrix.type() == CV_8UC1
          || matrix.type() == CV_16UC1
          || matrix.type() == CV_32F
          || matrix.type() == CV_64F
          || matrix.type() == CV_32S
        );
        CEDAR_ASSERT(cedar::aux::math::getDimensionalityOf(matrix) <= 1);

        switch (matrix.type())
        {
          case CV_8UC1:
            return static_cast<T>(matrix.at<uint8_t>(index));
            break;

          case CV_16UC1:
            return static_cast<T>(matrix.at<uint16_t>(index));
            break;

          case CV_32S:
            return static_cast<T>(matrix.at<int>(index));
            break;

          case CV_32F:
            return static_cast<T>(matrix.at<float>(index));
            break;

          case CV_64F:
            return static_cast<T>(matrix.at<double>(index));
            break;

          default:
            // this should never happen due to the assert above.
            CEDAR_ASSERT(false);
        }
      }
      
      /*!@brief Returns the value of a 2D matrix entry as the specified type.
       */
      template <typename T>
      inline T getMatrixEntry(const cv::Mat& matrix, int row, int col)
      {
        CEDAR_ASSERT(cedar::aux::math::getDimensionalityOf(matrix) <= 2);

        switch (matrix.type())
        {
          case CV_8S:
            return static_cast<T>(matrix.at<int8_t>(row, col));
            break;

          case CV_8U:
            return static_cast<T>(matrix.at<uint8_t>(row, col));
            break;

          case CV_16UC1:
            return static_cast<T>(matrix.at<uint16_t>(row, col));
            break;

          case CV_32F:
            return static_cast<T>(matrix.at<float>(row, col));
            break;

          case CV_64F:
            return static_cast<T>(matrix.at<double>(row, col));
            break;

          default:
            // this should never happen due to the assert above.
            CEDAR_ASSERT(false && "Not implemented for this data type.");
        }
      }

      /*!@brief Same functionality as cvReduce for 2D->1D.
       *
       * @param[in] source a 3D matrix source
       * @param[out] destination a 2D matrix destination (must be same type as src)
       * @param[in] dimensionToReduce along which dimension should be reduced?
       * @param[in] reductionOperator reduction operator (again, same choices as cvReduce)
       * @param[in] swapDimensions whether to switch the other two dimensions
       */
      template <typename T>
      void reduceCvMat3D(
                          const cv::Mat& source,
                          cv::Mat& destination,
                          int dimensionToReduce,
                          int reductionOperator = CEDAR_OPENCV_CONSTANT(REDUCE_SUM),
                          bool swapDimensions = false
                        );

      /*!@brief Creates a matrix containing a one-dimensional ramp.
       */
      template <typename T>
      inline cv::Mat ramp(int mat_type, int size, T lower, T upper)
      {
        CEDAR_ASSERT(size > 1);
        cv::Mat result(size, 1, mat_type);
        T difference = upper - lower;
        T step = difference / static_cast<T>(size - 1);
        int index;
        T value;
        for (index = 0, value = lower; index < size; ++index, value += step)
        {
          cedar::aux::math::assignMatrixEntry(result, index, value);
        }
        return result;
      }

      /*! @brief   Compares the sizes of two matrices.
       *
       *           This function also takes into account the special case of 1D matrices.
       *
       *  @returns True, if both sizes are equal, false otherwise.
       *
       *  @remarks Note, that this function also considers an n x 1 matrix and an 1 x n matrix to be of the same size.
       */
      inline bool matrixSizesEqual(const cv::Mat& matrixA, const cv::Mat& matrixB)
      {
        unsigned int dim_a = cedar::aux::math::getDimensionalityOf(matrixA);
        unsigned int dim_b = cedar::aux::math::getDimensionalityOf(matrixB);

        if (dim_a != dim_b)
        {
          return false;
        }

        if (dim_a == 1)
        {
          unsigned int size_a = get1DMatrixSize(matrixA);
          unsigned int size_b = get1DMatrixSize(matrixB);
          return size_a == size_b;
        }

        return matrixA.size == matrixB.size;
      }

      /*! @brief Writes the size of a matrix to a string.
       */
      inline std::string matrixSizeToString(const cv::Mat& matrix)
      {
        std::string res;

        switch (cedar::aux::math::getDimensionalityOf(matrix))
        {
          case 0:
            res = "1x1";
          case 1:
            res = cedar::aux::toString(cedar::aux::math::get1DMatrixSize(matrix)) + "x1";
            break;
          case 2:
            res = cedar::aux::toString(matrix.rows) + "x" + cedar::aux::toString(matrix.cols);
            break;
          default:
            for (int d = 0; d < matrix.dims; ++d)
            {
              if (d > 0)
              {
                res += "x";
              }
              res += cedar::aux::toString(matrix.size[d]);
            }
            break;
        }

        return res;
      }

      /*! @brief This function puts vectors in a canonical orientation (i.e., makes all vectors column vectors).
       */
      inline cv::Mat canonicalColVector(const cv::Mat& mat)
      {
        cv::Mat res = mat;
        if (res.cols == 1)
        {
          res = res.t();
        }
        return res;
      }

      /*! @brief This function puts vectors in a canonical orientation (i.e., makes all vectors row vectors).
       */
      inline cv::Mat canonicalRowVector(const cv::Mat& mat)
      {
        cv::Mat res = mat;
        if (res.rows == 1)
        {
          res = res.t();
        }
        return res;
      }

      /*! Given an activation matrix, this function groups together values above a given threshold into peaks and returns
       *  a list of centers of these peaks.
       *
       *  @param activation The activation matrix.
       *  @param peakCenters Found centers will be written into this vector.
       *  @param threshold Only values above this threshold are considered to be part of a peak.
       */
      CEDAR_AUX_LIB_EXPORT void findPeaks(const cv::Mat& activation, std::vector<cv::Point>& peakCenters, double threshold = 0.1);

    } // namespace math
  } // namespace aux
} // namespace cedar

#endif // CEDAR_AUX_MATH_TOOLS_H
