/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik

 ----- File:        coordinateTransformations.h

 ----- Author:      Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 01

 ----- Description: this file provides screw calculus functions for rigid body geometry

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_MATH_SCREW_CALCULUS_H
#define CEDAR_AUX_MATH_SCREW_CALCULUS_H

// LOCAL INCLUDES (includes from this project)

// PROJECT INCLUDES (includes from other projects)

// SYSTEM INCLUDES (everything else)
#include <cv.h>

namespace cedar
{
  namespace aux
  {
    namespace math
    {

      /**********************************************************************************************************************/
      /*                transformations between different representations                                                   */
      /**********************************************************************************************************************/
 
      /*! wedge operator for axes of a rotation
       * maps an axis vector to the skew symmetric matrix encoding the cross product with the axis vector
       * @param axis    operand, use 3 rows, 1 column
       * @return    result of wedge operator on vector
       */
      template<typename T>
      void wedgeAxis(const cv::Mat& axis, cv::Mat& result);
      /*! wedge operator for axes of a rotation
       * maps an axis vector to the skew symmetric matrix encoding the cross product with the axis vector
       * @param axis    operand, use 3 rows, 1 column
       * @return    result of wedge operator on vector
       */
      template<typename T>
      cv::Mat wedgeAxis(const cv::Mat& axis);

      /*! vee operator for axes of a rotation
       * maps a skew symmetric matrix encoding the cross product with a vector to that vector
       * @param matrix    skew symmetric matrix
       * @param result    axis of the rotation represented by matrix
       */
      template<typename T>
      void veeAxis(const cv::Mat& matrix, cv::Mat& result);
      /*! vee operator for axes of a rotation
       * maps a skew symmetric matrix encoding the cross product with a vector to that vector
       * @param matrix    skew symmetric matrix
       * @return    axis of the rotation represented by matrix
       */
      template<typename T>
      cv::Mat veeAxis(const cv::Mat& matrix);

      /*! wedge operator for rigid transformations
       * maps the twist coordinate vector of a rigid transformation to the matrix representation
       * @param twist    coordinate vector, use 6 rows, 1 column, axis (row 4-6) must be normed
       * @param result    twist in matrix form (4 \times 4)
       */
      template<typename T>
      void wedgeTwist(const cv::Mat& twist, cv::Mat& result);
      /*! wedge operator for twists
       * maps coordinate vector of a twist to the matrix representation
       * @param twist    coordinate vector, use 6 rows, 1 column, axis (row 4-6) must be normed
       * @return    twist in matrix form (4 \times 4)
       */
      template<typename T>
      cv::Mat wedgeTwist(const cv::Mat& twist);

      /*! vee operator for twists
       * maps a twist matrix to the twist coordinates in vector form
       * @param matrix    twist matrix (4 \times 4, upper left 3 \times 3 is skew symmetric)
       * @param result    vector of twist coordinates
       */
      template<typename T>
      void veeTwist(const cv::Mat& matrix, cv::Mat& result);
      /*! vee operator for twists
       * maps a twist matrix to the twist coordinates in vector form
       * @param matrix    twist matrix (4 \times 4, upper left 3 \times 3 is skew symmetric)
       * @return    vector of twist coordinates
       */
      template<typename T>
      cv::Mat veeTwist(const cv::Mat& matrix);

      /*! exponential map for wedges of axes, using Rodriguez' formula
       * maps an axis and angle of rotation to the rotation matrix
       * @param axis    axis of rotation, 3 \times 1
       * @param theta    angle of rotation
       * @param result    rotation matrix, 3 \times 3 orthonormal
       */
      template<typename T>
      void expAxis(const cv::Mat& axis, double theta, cv::Mat& result);
      /*! exponential map for wedges of axes, using Rodriguez' formula
       * maps an axis and angle of rotation to the rotation matrix
       * @param axis    axis of rotation, 3 \times 1
       * @param theta    angle of rotation
       * @return    rotation matrix, 3 \times 3 orthonormal
       */
      template<typename T>
      cv::Mat expAxis(const cv::Mat& axis, double theta);

      /*! inverse of the exponential map for skew symmetric matrices
       * maps a rotation matrix to its axis and angle of rotation, angle is chosen positive
       * @param rotation    rotation matrix, 3 \times 3, orthonormal
       * @param omega    axis of the rotation
       * @param theta    angle of rotation
       * @param optionalThetaChoice gives negative theta and changes the axis accordingly
       */
      template<typename T>
      void logAxis(const cv::Mat& rotation, cv::Mat& omega, double& theta, bool optionalThetaChoice = false);

      /*! exponential map for twists matrices
       * maps a twist matrix corresponding to an axis and support point to the rotation around that axis with the given angle
       * @param xi    twist matrix, 4 \times 4
       * @param theta    angle of rotation
       * @param result    rigid transformation matrix, 4 \times 4
       */
      template<typename T>
      void expTwist(const cv::Mat& xi, double theta, cv::Mat& result);
      /*! exponential map for twists matrices
       * maps a twist matrix corresponding to an axis and support point to the rotation around that axis with the given angle
       * @param xi    twist matrix, 4 \times 4
       * @param theta    angle of rotation
       * @return    rigid transformation matrix, 4 \times 4
       */
      template<typename T>
      cv::Mat expTwist(const cv::Mat& xi, double theta);

      /*! inverse of the exponential map for twist matrices
       * maps a rigid transformation matrix to a twist matrix and angle of rotation, angle is chosen positive
       * @param transformation    rigid transformation matrix, 4 \times 4
       * @param xi    twist matrix
       * @param theta    angle of rotation
       * @param optionalThetaChoice gives negative theta and changes the axis accordingly
       */
      template<typename T>
      void logTwist(const cv::Mat& transformation, cv::Mat& xi, double& theta, bool optionalThetaChoice = false);

      /*! creates adjoint transformation (6x6) of twist space corresponding to a rigid transformation (4x4)
       * @param rigidTransformation    rigid transformation matrix, 4 \times 4
       * @param adjointTransformation    adjoint transformation matrix, 6 \times 6
       */
      template<typename T>
      void rigidToAdjointTransformation(const cv::Mat& rigidTransformation, cv::Mat& adjointTransformation);
      /*! creates adjoint transformation (6x6) of twist space corresponding to a rigid transformation (4x4)
       * @param rigidTransformation    rigid transformation matrix, 4 \times 4
       * @return    adjoint transformation matrix, 6 \times 6
       */
      template<typename T>
      cv::Mat rigidToAdjointTransformation(const cv::Mat& rigidTransformation);

      /*! calculates the rigid transformation (4x4) corresponding to the given adjoint (6x6)
       * @param adjointTransformation    adjoint transformation matrix, 6 \times 6
       * @param rigidTransformation    rigid transformation matrix, 4 \times 4
       */
      template<typename T>
      void adjointToRigidTransformation(const cv::Mat& adjointTransformation, cv::Mat& rigidTransformation);
      /*! calculates the rigid transformation (4x4) corresponding to the given adjoint (6x6)
       * @param adjointTransformation    adjoint transformation matrix, 6 \times 6
       * @return    rigid transformation matrix, 4 \times 4
       */
      template<typename T>
      cv::Mat adjointToRigidTransformation(const cv::Mat& adjointTransformation);

      /*! calculates the inverse of the adjoint of a rigid transformation
       * @param adjointTransformation    adjoint transformation matrix, 6 \times 6
       * @param inverse    inverse of the given adjoint, 6 \times 6
       */
      template<typename T>
      void invertAdjointTransformation(const cv::Mat& adjointTransformation, cv::Mat& inverse);
      /*! calculates the inverse of the adjoint of a rigid transformation
       * @param adjointTransformation    adjoint transformation matrix, 6 \times 6
       * @return    inverse of the given adjoint, 6 \times 6
       */
      template<typename T>
      cv::Mat invertAdjointTransformation(const cv::Mat& adjointTransformation);

      /**********************************************************************************************************************/
      /*                generation of matrix representations from basic information                                         */
      /**********************************************************************************************************************/

      /*! calculates the twist coordinates for a purely rotational motion, given axis and a support point
       * @param supportPoint    point on the line around which is rotated in , 3 \times 1
       * @param axis    direction of the line around which is rotated in , 3 \times 1
       */
      template<typename T>
      cv::Mat twistCoordinates(const cv::Mat& supportPoint, const cv::Mat& axis);


    };
  };
};

#endif  // CEDAR_AUX_MATH_SCREW_CALCULUS_H
