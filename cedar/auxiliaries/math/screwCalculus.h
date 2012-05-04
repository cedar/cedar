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

    File:        screwCalculus.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 11

    Description: This file provides screw calculus functions for rigid body geometry.

    Credits:     These functions are implementations of concepts taken from the book "A mathematical introduction to
                   robotic manipulation", by R. Murray, S. Sastry and Z. Lee

======================================================================================================================*/

/*!@file  screwCalculus.h
 *
 * @brief Functions for the screw-theoretical approach to rigid transformations
 */

#ifndef CEDAR_AUX_MATH_SCREW_CALCULUS_H
#define CEDAR_AUX_MATH_SCREW_CALCULUS_H

// CEDAR INCLUDES

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>

namespace cedar
{
  namespace aux
  {
    namespace math
    {

      //----------------------------------------------------------------------------------------------------------------
      // transformations between different representations
      //----------------------------------------------------------------------------------------------------------------
      
      /*! @brief wedge-operator for axes of a rotation.
       * maps an axis vector to the skew symmetric matrix encoding the cross
       * product with the axis vector
       * inverse of the vee-operator
       * @param[in] rAxis operand, 3x1
       * @param[out] rResult result of wedge operator on rAxis, 3x3
       */
      template<typename T>
      void wedgeAxis(const cv::Mat& rAxis, cv::Mat& rResult);
      /*! @brief wedge-operator for axes of a rotation
       * Maps an axis vector to the skew symmetric matrix encoding the cross
       * product with the axis vector. Convenience function giving the result as
       * a return value. This is slightly slower than the standard version.
       * @param[in] rAxis operand, 3x1
       * @return result of wedge operator on rAxis, 3x3
       */
      template<typename T>
      cv::Mat wedgeAxis(const cv::Mat& rAxis);

      /*! @brief vee operator for axes of a rotation
       * Maps a skew symmetric matrix encoding the cross product with a vector
       * to that vector.
       * @param[in] rMatrix skew symmetric matrix, 3x3
       * @param[out] rResult axis of the rotation represented by matrix, 3x1
       */
      template<typename T>
      void veeAxis(const cv::Mat& rMatrix, cv::Mat& rResult);
      /*! @brief vee operator for axes of a rotation
       * Maps a skew symmetric matrix encoding the cross product with a vector
       * to that vector. Convenience function giving the result as a return
       * value. This is slightly slower than the standard version.
       * @param[in] rMatrix skew symmetric matrix, 3x3
       * @return axis of the rotation represented by matrix, 3x1
       */
      template<typename T>
      cv::Mat veeAxis(const cv::Mat& rMatrix);

      /*! @brief wedge operator for twists
       * Transforms the coordinate vector of a twist to the matrix form.
       * @param[in] rTwist twist coordinate vector, axis (row 4-6) must be
       * normed, \f$(6 \times 1)\f$
       * @param[out] rResult twist in matrix form, \f$(4 \times 4)\f$
       */
      template<typename T>
      void wedgeTwist(const cv::Mat& rTwist, cv::Mat& rResult);
      /*! @brief wedge operator for twists
       * Transforms the coordinate vector of a twist to the matrix form.
       * Convenience function giving the result as a return value. This is
       * slightly slower than the standard version.
       * @param[in] rTwist twist coordinate vector, axis (row 4-6) must be normed, 6x1
       * @return twist in matrix form, \f$(4 \times 4)\f$
       */
      template<typename T>
      cv::Mat wedgeTwist(const cv::Mat& rTwist);

      /*! @brief vee operator for twists
       * Transforms a twist in matrix form to the twist coordinate vector.
       * @param[in] rMatrix twist in matrix form, \f$(4 \times 4)\f$, upper left
       *   \f$3 \times 3\f$ is skew symmetric
       * @param[out] rResult twist coordinate vector, \f$(6 \times 1)\f$
       */
      template<typename T>
      void veeTwist(const cv::Mat& rMatrix, cv::Mat& rResult);
      /*! @brief vee operator for twists
       * Transforms a twist in matrix form to the twist coordinate vector.
       * Convenience function giving the result as a return value. This is
       * slightly slower than the standard version.
       * @param[in] rMatrix twist in matrix form, \f$(4 \times 4)\f$, upper left
       *   \f$3 \times 3\f$ is skew symmetric
       * @return twist coordinate vector, \f$(6 \times 1)\f$
       */
      template<typename T>
      cv::Mat veeTwist(const cv::Mat& rMatrix);

      /*! @brief exponential map so(3) -> SO(3), using Rodriguez' formula
       * Transforms a normed axis and an angle of rotation to the corresponding
       * rotation matrix.
       * @param[in] rAxis normed axis of rotation, \f$3 \times 1\f$
       * @param[in] theta angle of rotation
       * @param[out] rResult rotation matrix, \f$3 \times 3\f$
       */
      template<typename T>
      void expAxis(const cv::Mat& rAxis, double theta, cv::Mat& rResult);
      /*! @brief exponential map exp: so(3) -> SO(3), using Rodriguez' formula
       * Transforms a normed axis and an angle of rotation to the corresponding
       * rotation matrix. Convenience function giving the result as a return
       * value. This is slightly slower than the standard version.
       * @param[in] rAxis normed axis of rotation, \f$3 \times 1\f$
       * @param[in] theta angle of rotation
       * @return rotation matrix, \f$3 \times 3\f$
       */
      template<typename T>
      cv::Mat expAxis(const cv::Mat& rAxis, double theta);

      /*! @brief inverse of exp: so(3) -> SO(3)
       * Transforms a rotation matrix to its axis and angle of rotation, by
       * default the angle is positive.
       * @param[in] rRotation rotation matrix, \f$3 \times 3\f$
       * @param[out] rOmega axis of the rotation, \f$3 \times 1\f$
       * @param[out] rTheta angle of rotation
       * @param[in] negativeTheta set to TRUE to get negative theta and the
       *   corresponding axis
       */
      template<typename T>
      void logAxis(const cv::Mat& rRotation, cv::Mat& rOmega, double& rTheta, bool negativeTheta = false);

      /*! @brief exponential map exp: se(3) -> SE(3)
       * Transforms a twist matrix (corresponding to a normed axis and support
       * point) to the rotation around that axis with the given angle.
       * @param[in] rXi twist matrix, \f$4 \times 4\f$
       * @param[in] theta angle of rotation
       * @param[out] rResult rigid transformation matrix, \f$4 \times 4\f$
       */
      template<typename T>
      void expTwist(const cv::Mat& rXi, double theta, cv::Mat& rResult);
      /*! @brief exponential map exp: se(3) -> SE(3)
       * Transforms a twist matrix (corresponding to a normed axis and support
       * point) to the rotation around that axis with the given angle.
       * Convenience function giving the result as a return value. This is
       * slightly slower than the standard version.
       * @param[in] rXi twist matrix, \f$4 \times 4\f$
       * @param[in] theta angle of rotation
       * @return rigid transformation matrix, \f$4 \times 4\f$
       */
      template<typename T>
      cv::Mat expTwist(const cv::Mat& rXi, double theta);

      /*! @brief inverse of exp: se(3) -> SE(3)
       * Transforms a rigid transformation matrix to a twist matrix and angle of
       * rotation, by default the angle is positive.
       * @param[in] rTransformation    rigid transformation matrix,
       *   \f$4 \times 4\f$
       * @param[out] rXi twist matrix, \f$4 \times 4\f$
       * @param[out] rTheta angle of rotation
       * @param[in] negativeTheta set to TRUE to get negative theta and the
       *   corresponding axis
       */
      template<typename T>
      void logTwist(const cv::Mat& rTransformation, cv::Mat& rXi, double& rTheta, bool negativeTheta = false);

      /*! @brief creates the adjoint transformation corresponding to a rigid
       * transformation.
       * @param[in] rRigidTransformation rigid transformation matrix,
       *   \f$4 \times 4\f$
       * @param[out] rAdjointTransformation adjoint transformation matrix,
       *   \f$6 \times 6\f$
       */
      template<typename T>
      void rigidToAdjointTransformation(const cv::Mat& rRigidTransformation, cv::Mat& rAdjointTransformation);
      /*! @brief creates the adjoint transformation corresponding to a rigid
       * transformation. Convenience function giving the result as a return
       * value. This is slightly slower than the standard version.
       * @param[in] rRigidTransformation rigid transformation matrix,
       *   \f$4 \times 4\f$
       * @return adjoint transformation matrix, \f$6 \times 6\f$
       */
      template<typename T>
      cv::Mat rigidToAdjointTransformation(const cv::Mat& rRigidTransformation);

      /*! @brief calculates the rigid from the given adjoint transformation
       * @param[in] rAdjointTransformation adjoint transformation matrix,
       *   \f$6 \times 6\f$
       * @param[out] rRigidTransformation rigid transformation matrix,
       *   \f$4 \times 4\f$
       */
      template<typename T>
      void adjointToRigidTransformation(const cv::Mat& rAdjointTransformation, cv::Mat& rRigidTransformation);
      /*! @brief calculates the rigid from the given adjoint transformation
       * Convenience function giving the result as a return value. This is
       * slightly slower than the standard version.
       * @param[in] rAdjointTransformation adjoint transformation matrix,
       *   \f$6 \times 6\f$
       * @return rigid transformation matrix, \f$4 \times 4\f$
       */
      template<typename T>
      cv::Mat adjointToRigidTransformation(const cv::Mat& rAdjointTransformation);

      /*! @brief Calculates the inverse of the adjoint of a rigid transformation.
       * @param[in] rAdjointTransformation adjoint transformation matrix,
       *   \f$6 \times 6\f$
       * @param[out] rInverse inverse of the given adjoint, \f$6 \times 6\f$
       */
      template<typename T>
      void invertAdjointTransformation(const cv::Mat& rAdjointTransformation, cv::Mat& rInverse);
      /*! @brief Calculates the inverse of the adjoint of a rigid transformation.
       * Convenience function giving the result as a return value. This is
       * slightly slower than the standard version.
       * @param[in] rAdjointTransformation adjoint transformation matrix,
       *   \f$6 \times 6\f$
       * @return inverse of the given adjoint, \f$6 \times 6\f$
       */
      template<typename T>
      cv::Mat invertAdjointTransformation(const cv::Mat& rAdjointTransformation);

      //----------------------------------------------------------------------------------------------------------------
      // generation of matrix representations from basic information
      //----------------------------------------------------------------------------------------------------------------

      /*! @brief calculates the twist coordinates for a purely rotational motion
       * @param[in] rSupportPoint point on the line around which is rotated,
       *   \f$3 \times 1\f$
       * @param[in] rAxis direction of the line around which is rotated in
       *   \f$3 \times 1\f$
       * @return twist coordinates of the rotation around the specified axis
       */
      template<typename T>
      cv::Mat twistCoordinates(const cv::Mat& rSupportPoint, const cv::Mat& rAxis);
    }
  }
}

#endif  // CEDAR_AUX_MATH_SCREW_CALCULUS_H
