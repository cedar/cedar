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

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MATH_SCREW_CALCULUS_H
#define CEDAR_AUX_MATH_SCREW_CALCULUS_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"

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
      
      /*! wedge operator for axes of a rotation
       * maps an axis vector to the skew symmetric matrix encoding the cross product with the axis vector
       * @param rAxis    operand, use 3 rows, 1 column
       * @param rResult    result of wedge operator on vector
       */
      template<typename T>
      void wedgeAxis(const cv::Mat& rAxis, cv::Mat& rResult);

      /*!@brief A non-templated verion of cedar::aux::math::wedgeAxis<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      inline void wedgeAxis(const cv::Mat& rAxis, cv::Mat& rResult)
      {
        CEDAR_ASSERT(rAxis.type() == rResult.type());
        switch (rAxis.type())
        {
          case CV_32F:
            cedar::aux::math::wedgeAxis<float>(rAxis, rResult);
            break;

          case CV_64F:
            cedar::aux::math::wedgeAxis<double>(rAxis, rResult);
            break;

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! wedge operator for axes of a rotation
       * maps an axis vector to the skew symmetric matrix encoding the cross product with the axis vector
       * @param rAxis    operand, use 3 rows, 1 column
       * @return    result of wedge operator on vector
       */
      template<typename T>
      cv::Mat wedgeAxis(const cv::Mat& rAxis);

      /*!@brief A non-templated verion of cedar::aux::math::wedgeAxis<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      inline cv::Mat wedgeAxis(const cv::Mat& rAxis)
      {
        switch (rAxis.type())
        {
          case CV_32F:
            return cedar::aux::math::wedgeAxis<float>(rAxis);

          case CV_64F:
            return cedar::aux::math::wedgeAxis<double>(rAxis);

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! vee operator for axes of a rotation
       * maps a skew symmetric matrix encoding the cross product with a vector to that vector
       * @param rMatrix    skew symmetric matrix
       * @param rResult    axis of the rotation represented by matrix
       */
      template<typename T>
      void veeAxis(const cv::Mat& rMatrix, cv::Mat& rResult);

      /*!@brief A non-templated verion of cedar::aux::math::veeAxis<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      inline void veeAxis(const cv::Mat& rMatrix, cv::Mat& rResult)
      {
        CEDAR_DEBUG_ASSERT(rMatrix.type() == rResult.type());
        switch (rMatrix.type())
        {
          case CV_32F:
            cedar::aux::math::veeAxis<float>(rMatrix, rResult);
            break;

          case CV_64F:
            cedar::aux::math::veeAxis<double>(rMatrix, rResult);
            break;

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! vee operator for axes of a rotation
       * maps a skew symmetric matrix encoding the cross product with a vector to that vector
       * @param rMatrix    skew symmetric matrix
       * @return    axis of the rotation represented by matrix
       */
      template<typename T>
      cv::Mat veeAxis(const cv::Mat& rMatrix);

      /*!@brief A non-templated verion of cedar::aux::math::veeAxis<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      cv::Mat veeAxis(const cv::Mat& rMatrix)
      {
        switch (rMatrix.type())
        {
          case CV_32F:
            return cedar::aux::math::veeAxis<float>(rMatrix);

          case CV_64F:
            return cedar::aux::math::veeAxis<double>(rMatrix);

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! wedge operator for rigid transformations
       * maps the twist coordinate vector of a rigid transformation to the matrix representation
       * @param rTwist    coordinate vector, use 6 rows, 1 column, axis (row 4-6) must be normed
       * @param rResult    twist in matrix form \f$(4 \times 4)\f$
       */
      template<typename T>
      void wedgeTwist(const cv::Mat& rTwist, cv::Mat& rResult);

      /*!@brief A non-templated verion of cedar::aux::math::wedgeTwist<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      void wedgeTwist(const cv::Mat& rTwist, cv::Mat& rResult)
      {
        CEDAR_DEBUG_ASSERT(rTwist.type() == rResult.type());

        switch (rTwist.type())
        {
          case CV_32F:
            cedar::aux::math::wedgeTwist<float>(rTwist, rResult);
            break;

          case CV_64F:
            cedar::aux::math::wedgeTwist<double>(rTwist, rResult);
            break;

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! wedge operator for twists
       * maps coordinate vector of a twist to the matrix representation
       * @param rTwist    coordinate vector, use 6 rows, 1 column, axis (row 4-6) must be normed
       * @return    twist in matrix form \f$(4 \times 4)\f$
       */
      template<typename T>
      cv::Mat wedgeTwist(const cv::Mat& rTwist);

      /*!@brief A non-templated verion of cedar::aux::math::veeAxis<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      cv::Mat wedgeTwist(const cv::Mat& rTwist)
      {
        switch (rTwist.type())
        {
          case CV_32F:
            return cedar::aux::math::wedgeTwist<float>(rTwist);

          case CV_64F:
            return cedar::aux::math::wedgeTwist<double>(rTwist);

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! vee operator for twists
       * maps a twist matrix to the twist coordinates in vector form
       * @param rMatrix    twist matrix \f$(4 \times 4)\f$, upper left \f$3 \times 3\f$ is skew symmetric)
       * @param rResult    vector of twist coordinates
       */
      template<typename T>
      void veeTwist(const cv::Mat& rMatrix, cv::Mat& rResult);

      /*!@brief A non-templated verion of cedar::aux::math::veeTwist<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      void veeTwist(const cv::Mat& rMatrix, cv::Mat& rResult)
      {
        CEDAR_DEBUG_ASSERT(rMatrix.type() == rResult.type());

        switch (rMatrix.type())
        {
          case CV_32F:
            cedar::aux::math::veeTwist<float>(rMatrix, rResult);
            break;

          case CV_64F:
            cedar::aux::math::veeTwist<double>(rMatrix, rResult);
            break;

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! vee operator for twists
       * maps a twist matrix to the twist coordinates in vector form
       * @param rMatrix    twist matrix (\f$4 \times 4)\f$, upper left \f$3 \times 3\f$ is skew symmetric)
       * @return    vector of twist coordinates
       */
      template<typename T>
      cv::Mat veeTwist(const cv::Mat& rMatrix);

      /*!@brief A non-templated verion of cedar::aux::math::veeTwist<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      cv::Mat veeTwist(const cv::Mat& rMatrix)
      {
        switch (rMatrix.type())
        {
          case CV_32F:
            return cedar::aux::math::veeTwist<float>(rMatrix);

          case CV_64F:
            return cedar::aux::math::veeTwist<double>(rMatrix);

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! exponential map for wedges of axes, using Rodriguez' formula
       * maps an axis and angle of rotation to the rotation matrix
       * @param rAxis    axis of rotation, \f$3 \times 1\f$
       * @param theta    angle of rotation
       * @param rResult    rotation matrix, \f$3 \times 3\f$ orthonormal
       */
      template<typename T>
      void expAxis(const cv::Mat& rAxis, double theta, cv::Mat& rResult);

      /*!@brief A non-templated verion of cedar::aux::math::expAxis<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      void expAxis(const cv::Mat& rAxis, double theta, cv::Mat& rResult)
      {
        CEDAR_DEBUG_ASSERT(rAxis.type() == rResult.type());

        switch (rAxis.type())
        {
          case CV_32F:
            cedar::aux::math::expAxis<float>(rAxis, theta, rResult);
            break;

          case CV_64F:
            cedar::aux::math::expAxis<double>(rAxis, theta, rResult);
            break;

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! exponential map for wedges of axes, using Rodriguez' formula
       * maps an axis and angle of rotation to the rotation matrix
       * @param rAxis    axis of rotation, \f$3 \times 1\f$
       * @param theta    angle of rotation
       * @return    rotation matrix, \f$3 \times 3\f$ orthonormal
       */
      template<typename T>
      cv::Mat expAxis(const cv::Mat& rAxis, double theta);

      /*!@brief A non-templated verion of cedar::aux::math::veeTwist<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      cv::Mat expAxis(const cv::Mat& rMatrix, double theta)
      {
        switch (rMatrix.type())
        {
          case CV_32F:
            return cedar::aux::math::expAxis<float>(rMatrix, theta);

          case CV_64F:
            return cedar::aux::math::expAxis<double>(rMatrix, theta);

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! inverse of the exponential map for skew symmetric matrices
       * maps a rotation matrix to its axis and angle of rotation, angle is chosen positive
       * @param rRotation    rotation matrix, \f$3 \times 3\f$, orthonormal
       * @param rOmega    axis of the rotation
       * @param rTheta    angle of rotation
       * @param optionalThetaChoice gives negative theta and changes the axis accordingly
       */
      template<typename T>
      void logAxis(const cv::Mat& rRotation, cv::Mat& rOmega, double& rTheta, bool optionalThetaChoice = false);

      /*!@brief A non-templated verion of cedar::aux::math::logAxis<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      void logAxis(const cv::Mat& rRotation, cv::Mat& rOmega, double& rTheta, bool optionalThetaChoice = false)
      {
        CEDAR_DEBUG_ASSERT(rRotation.type() == rOmega.type());

        switch (rRotation.type())
        {
          case CV_32F:
            cedar::aux::math::logAxis<float>(rRotation, rOmega, rTheta, optionalThetaChoice);
            break;

          case CV_64F:
            cedar::aux::math::logAxis<double>(rRotation, rOmega, rTheta, optionalThetaChoice);
            break;

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! exponential map for twists matrices
       * maps a twist matrix corresponding to an axis and support point to the rotation around that axis with the given angle
       * @param rXi    twist matrix, \f$4 \times 4\f$
       * @param theta    angle of rotation
       * @param rResult    rigid transformation matrix, \f$4 \times 4\f$
       */
      template<typename T>
      void expTwist(const cv::Mat& rXi, double theta, cv::Mat& rResult);

      /*!@brief A non-templated verion of cedar::aux::math::expTwist<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      void expTwist(const cv::Mat& rXi, double theta, cv::Mat& rResult)
      {
        CEDAR_DEBUG_ASSERT(rXi.type() == rResult.type());

        switch (rXi.type())
        {
          case CV_32F:
            cedar::aux::math::expTwist<float>(rXi, theta, rResult);
            break;

          case CV_64F:
            cedar::aux::math::expTwist<double>(rXi, theta, rResult);
            break;

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! exponential map for twists matrices
       * maps a twist matrix corresponding to an axis and support point to the rotation around that axis with the given angle
       * @param rXi    twist matrix, \f$4 \times 4\f$
       * @param theta    angle of rotation
       * @return    rigid transformation matrix, \f$4 \times 4\f$
       */
      template<typename T>
      cv::Mat expTwist(const cv::Mat& rXi, double theta);

      /*!@brief A non-templated verion of cedar::aux::math::expTwist<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      cv::Mat expTwist(const cv::Mat& rXi, double theta)
      {
        switch (rXi.type())
        {
          case CV_32F:
            return cedar::aux::math::expTwist<float>(rXi, theta);

          case CV_64F:
            return cedar::aux::math::expTwist<double>(rXi, theta);

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! inverse of the exponential map for twist matrices
       * maps a rigid transformation matrix to a twist matrix and angle of rotation, angle is chosen positive
       * @param rTransformation    rigid transformation matrix, \f$4 \times 4\f$
       * @param rXi    twist matrix
       * @param rTheta    angle of rotation
       * @param optionalThetaChoice gives negative theta and changes the axis accordingly
       */
      template<typename T>
      void logTwist(const cv::Mat& rTransformation, cv::Mat& rXi, double& rTheta, bool optionalThetaChoice = false);

      /*!@brief A non-templated verion of cedar::aux::math::logTwist<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      void logTwist(const cv::Mat& rTransformation, cv::Mat& rXi, double& rTheta, bool optionalThetaChoice = false)
      {
        CEDAR_DEBUG_ASSERT(rTransformation.type() == rXi.type());

        switch (rXi.type())
        {
          case CV_32F:
            cedar::aux::math::logTwist<float>(rTransformation, rXi, rTheta, optionalThetaChoice);
            break;

          case CV_64F:
            cedar::aux::math::logTwist<double>(rTransformation, rXi, rTheta, optionalThetaChoice);
            break;

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! creates adjoint transformation \f$(6 \times 6)\f$ of twist space corresponding to a rigid transformation (4x4)
       * @param rRigidTransformation    rigid transformation matrix, \f$4 \times 4\f$
       * @param rAdjointTransformation    adjoint transformation matrix, \f$6 \times 6\f$
       */
      template<typename T>
      void rigidToAdjointTransformation(const cv::Mat& rRigidTransformation, cv::Mat& rAdjointTransformation);

      /*!@brief A non-templated verion of cedar::aux::math::rigidToAdjointTransformation<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      void rigidToAdjointTransformation(const cv::Mat& rRigidTransformation, cv::Mat& rAdjointTransformation)
      {
        CEDAR_DEBUG_ASSERT(rRigidTransformation.type() == rAdjointTransformation.type());

        switch (rRigidTransformation.type())
        {
          case CV_32F:
            cedar::aux::math::rigidToAdjointTransformation<float>(rRigidTransformation, rAdjointTransformation);
            break;

          case CV_64F:
            cedar::aux::math::rigidToAdjointTransformation<double>(rRigidTransformation, rAdjointTransformation);
            break;

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! creates adjoint transformation \f$(6 \times 6)\f$ of twist space corresponding to a rigid transformation (4x4)
       * @param rRigidTransformation    rigid transformation matrix, \f$4 \times 4\f$
       * @return    adjoint transformation matrix, \f$6 \times 6\f$
       */
      template<typename T>
      cv::Mat rigidToAdjointTransformation(const cv::Mat& rRigidTransformation);

      /*!@brief A non-templated verion of cedar::aux::math::rigidToAdjointTransformation<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      cv::Mat rigidToAdjointTransformation(const cv::Mat& rRigidTransformation)
      {
        switch (rRigidTransformation.type())
        {
          case CV_32F:
            return cedar::aux::math::rigidToAdjointTransformation<float>(rRigidTransformation);

          case CV_64F:
            return cedar::aux::math::rigidToAdjointTransformation<double>(rRigidTransformation);

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! calculates the rigid transformation \f$(4x4)\f$ corresponding to the given adjoint \f$(6 \times 6)\f$
       * @param rAdjointTransformation    adjoint transformation matrix, \f$6 \times 6\f$
       * @param rRigidTransformation    rigid transformation matrix, \f$4 \times 4\f$
       */
      template<typename T>
      void adjointToRigidTransformation(const cv::Mat& rAdjointTransformation, cv::Mat& rRigidTransformation);

      /*!@brief A non-templated verion of cedar::aux::math::adjointToRigidTransformation<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      void adjointToRigidTransformation(const cv::Mat& rAdjointTransformation, cv::Mat& rRigidTransformation)
      {
        CEDAR_DEBUG_ASSERT(rAdjointTransformation.type() == rRigidTransformation.type());

        switch (rRigidTransformation.type())
        {
          case CV_32F:
            cedar::aux::math::adjointToRigidTransformation<float>(rAdjointTransformation, rRigidTransformation);
            break;

          case CV_64F:
            cedar::aux::math::adjointToRigidTransformation<double>(rAdjointTransformation, rRigidTransformation);
            break;

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! calculates the rigid transformation \f$(4 \times 4)\f$ corresponding to the given adjoint \f$(6 \times 6)\f$
       * @param rAdjointTransformation    adjoint transformation matrix, \f$6 \times 6\f$
       * @return    rigid transformation matrix, \f$4 \times 4\f$
       */
      template<typename T>
      cv::Mat adjointToRigidTransformation(const cv::Mat& rAdjointTransformation);

      /*!@brief A non-templated verion of cedar::aux::math::adjointToRigidTransformation<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      cv::Mat adjointToRigidTransformation(const cv::Mat& rRigidTransformation)
      {
        switch (rRigidTransformation.type())
        {
          case CV_32F:
            return cedar::aux::math::adjointToRigidTransformation<float>(rRigidTransformation);

          case CV_64F:
            return cedar::aux::math::adjointToRigidTransformation<double>(rRigidTransformation);

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! calculates the inverse of the adjoint of a rigid transformation
       * @param rAdjointTransformation    adjoint transformation matrix, \f$6 \times 6\f$
       * @param rInverse    inverse of the given adjoint, \f$6 \times 6\f$
       */
      template<typename T>
      void invertAdjointTransformation(const cv::Mat& rAdjointTransformation, cv::Mat& rInverse);

      /*!@brief A non-templated verion of cedar::aux::math::invertAdjointTransformation<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      void invertAdjointTransformation(const cv::Mat& rAdjointTransformation, cv::Mat& rInverse)
      {
        CEDAR_DEBUG_ASSERT(rAdjointTransformation.type() == rInverse.type());

        switch (rAdjointTransformation.type())
        {
          case CV_32F:
            cedar::aux::math::invertAdjointTransformation<float>(rAdjointTransformation, rInverse);
            break;

          case CV_64F:
            cedar::aux::math::invertAdjointTransformation<double>(rAdjointTransformation, rInverse);
            break;

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      /*! calculates the inverse of the adjoint of a rigid transformation
       * @param rAdjointTransformation    adjoint transformation matrix, \f$6 \times 6\f$
       * @return    inverse of the given adjoint, \f$6 \times 6\f$
       */
      template<typename T>
      cv::Mat invertAdjointTransformation(const cv::Mat& rAdjointTransformation);

      /*!@brief A non-templated verion of cedar::aux::math::invertAdjointTransformation<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      cv::Mat invertAdjointTransformation(const cv::Mat& rAdjointTransformation)
      {
        switch (rAdjointTransformation.type())
        {
          case CV_32F:
            return cedar::aux::math::invertAdjointTransformation<float>(rAdjointTransformation);

          case CV_64F:
            return cedar::aux::math::invertAdjointTransformation<double>(rAdjointTransformation);

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }

      //----------------------------------------------------------------------------------------------------------------
      // generation of matrix representations from basic information
      //----------------------------------------------------------------------------------------------------------------

      /*! calculates the twist coordinates for a purely rotational motion, given axis and a support point
       * @param rSupportPoint    point on the line around which is rotated in , \f$3 \times 1\f$
       * @param rAxis    direction of the line around which is rotated in , \f$3 \times 1\f$
       */
      template<typename T>
      cv::Mat twistCoordinates(const cv::Mat& rSupportPoint, const cv::Mat& rAxis);

      /*!@brief A non-templated verion of cedar::aux::math::twistCoordinates<T>.
       *
       * @remarks Computationally, it is slightly more efficient to use the templated version.
       */
      cv::Mat twistCoordinates(const cv::Mat& rSupportPoint, const cv::Mat& rAxis)
      {
        CEDAR_ASSERT(rSupportPoint.type() == rAxis.type());

        switch (rSupportPoint.type())
        {
          case CV_32F:
            return cedar::aux::math::twistCoordinates<float>(rSupportPoint, rAxis);

          case CV_64F:
            return cedar::aux::math::twistCoordinates<double>(rSupportPoint, rAxis);

          default:
            CEDAR_THROW(cedar::aux::UnhandledTypeException, "This function can only be called with CV_32F or CV_64F matrices.");
        }
      }
    }
  }
}

#endif  // CEDAR_AUX_MATH_SCREW_CALCULUS_H
