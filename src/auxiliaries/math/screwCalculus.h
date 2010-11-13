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
 
      /*! wedge operator for rotations
       * maps a vector to the skew symmetric matrix encoding the cross product with this vector
       * @param vector    operand, use 3 rows, 1 column
       * @return    result of wedge operator on vector
       */
      template<typename T>
      void wedgeRotation(const cv::Mat& vector, cv::Mat& result);
      /*! wedge operator for rotations
       * maps a vector to the skew symmetric matrix encoding the cross product with this vector
       * @param vector    operand, use 3 rows, 1 column
       * @return    result of wedge operator on vector
       */
      template<typename T>
      cv::Mat wedgeRotation(const cv::Mat& vector);

      /*! vee operator for rotations
       * maps a skew symmetric matrix encoding the cross product with a vector to that vector
       * @param matrix    skew symmetric matrix
       * @param result    result of vee operator on matrix
       */
      template<typename T>
      void veeRotation(const cv::Mat& matrix, cv::Mat& result);
      /*! vee operator for rotations
       * maps a skew symmetric matrix encoding the cross product with a vector to that vector
       * @param matrix    skew symmetric matrix
       * @param result    result of vee operator on matrix
       */
      template<typename T>
      cv::Mat veeRotation(const cv::Mat& matrix);

//      //! wedge operator \f[\wedge: \mathbf R^6 \to se(3)\f] for rigid transformations
//      //! from twist coordinates to a rigid transformation matrix in homogeneous coordinates
//      void wedgeRigidTransformation(cv::Mat& twist, cv::Mat& result);
//      //! wedge operator \f[\wedge: \mathbf R^6 \to se(3)\f] for rigid transformations
//      //! from twist coordinates to a rigid transformation matrix in homogeneous coordinates
//      cv::Mat wedgeRigidTransformation(cv::Mat& twist);
//
//      //! vee operator \f[\vee: se(3) \to \mathbf R^6\f] for rigid transformations
//      //! from a rigid transformation matrix in homogeneous coordinates to twist coordinates
//      void veeRigidTransformation(cv::Mat& matrix, cv::Mat& result);
//      //! vee operator \f[\vee: se(3) \to \mathbf R^6\f] for rigid transformations
//      //! from a rigid transformation matrix in homogeneous coordinates to twist coordinates
//      cv::Mat veeRigidTransformation(cv::Mat& matrix);
//
//      //! calculates the exponential of a skew symmetric matrix, given as vector, using Rodriguez' formula
//      //! omega must be normed
//      void expRotation(cv::Mat& omega, double theta, cv::Mat& result);
//      //! calculates the exponential of a skew symmetric matrix, given as vector, using Rodriguez' formula
//      //! omega must be normed
//      cv::Mat expRotation(cv::Mat& omega, double theta);
//
//      //! inverse of the exponential map for rotations
//      //! calculates exponential coordinates for a rotation matrix \f[\in \mathbf R^{3 \times 3}\f]
//      //! which solution is chosen?
//      void logRotation(cv::Mat& rotation, cv::Mat& omega, double& theta, bool optionalThetaChoice = false);
//
//      //! calculates the exponential of a twist given in twist coordinates
//      void expRigidTransformation(cv::Mat& xi, double theta, cv::Mat& result);
//      //! calculates the exponential of a twist given in twist coordinates
//      cv::Mat expRigidTransformation(cv::Mat& xi, double theta);
//
//      //! calculates twist coordinates for a rigid transformation matrix \f[\in se(3) \f]
//      void logRansformation(cv::Mat& transformation, cv::Mat& xi, double& theta, bool optionalThetaChoice = false);
//
//      //! creates adjoint transformation (6x6) of twist space corresponding to a rigid transformation (4x4)
//      void rigidToAdjointTransformation(cv::Mat& rigidTransformation, cv::Mat& adjointTransformation);
//      //! creates adjoint transformation (6x6) of twist space corresponding to a rigid transformation (4x4)
//      cv::Mat rigidToAdjointTransformation(cv::Mat& rigidTransformation);
//
//      //! calculates the rigid transformation (4x4) corresponding to the given adjoint (6x6)
//      void adjointToRigidTransformation(cv::Mat& adjointTransformation, cv::Mat& rigidTransformation);
//      //! calculates the rigid transformation (4x4) corresponding to the given adjoint (6x6)
//      cv::Mat adjointToRigidTransformation(cv::Mat& adjointTransformation);
//
//      //! inverts an adjoint transformation
//      void invertAdjointTransformation(cv::Mat& adjointTransformation, cv::Mat& inverse);
//      //! inverts an adjoint transformation
//      cv::Mat invertAdjointTransformation(cv::Mat& adjointTransformation);
//
//      /**********************************************************************************************************************/
//      /*                generation of matrix representations from basic information                                         */
//      /**********************************************************************************************************************/
//
//      //! creates the generalized inertia matrix (6x6) for a link segment, given mass and main axes moments of inertia
//      cv::Mat createGeneralizedInertiaMatrix(double mass, double Ix, double Iy, double Iz);
//
//      //! creates the generalized inertia matrix (6x6) for a homogeneous block mass link segment
//      cv::Mat createGeneralizedInertiaMatrixBlockMass(double mass, double length, double width, double height);
//
//      //! returns the twist coordinates for a rigid motion, given axis and a support point
//      cv::Mat twistCoordinates(cv::Mat& supportPoint, cv::Mat& axis);

/*
//! concatenates a position and orientation to a rigid transformation
void rigidFromPositionOrientation(CvMAT& position, CvMAT& orientation, CvMAT& rigidTransformation);
*/

    };
  };
};

#endif  // CEDAR_AUX_MATH_SCREW_CALCULUS_H
