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

    File:        KinematicChainModel.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 08 30

    Description: Header for the @em cedar::dev::robot::KinematicChainModel class.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_MODEL_H
#define CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_MODEL_H

// LOCAL INCLUDES
#include "devices/robot/namespace.h"
#include "ReferenceGeometry.h"
#include "KinematicChain.h"

// PROJECT INCLUDES
#include "auxiliaries/Object.h"

// SYSTEM INCLUDES
#include <string>
#include <opencv2/opencv.hpp>
#include <QReadWriteLock>
#include <QObject>

/*! \brief Model of the kinematics of a serial chain, or open chain manipulator
 *
 * A KinematicChainModel is constructed with a pointer to a KinematicChain, which needs to provide a valid reference
 * geometry object. From this information about the geometry of the chain in the reference configuration and the current
 * joint angles, this class calculates the rigid transformations to the joint frames and other mappings.The model must
 * be updated, e.g. by starting its timer
 */
class cedar::dev::robot::KinematicChainModel : public cedar::aux::Object
{
private:

  Q_OBJECT;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  KinematicChainModel(cedar::dev::robot::KinematicChainPtr& rpKinematicChain);
  //!@brief destructor
  virtual ~KinematicChainModel();
  
  //--------------------------------------------------------------------------------------------------------------------
  // Qt events
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief implementation of handling routine for Qt timer events
   *
   * @param    event
   */
  void timerEvent(QTimerEvent* event);

  //--------------------------------------------------------------------------------------------------------------------
  // Qt slots
  //--------------------------------------------------------------------------------------------------------------------
public slots:

//!@brief updates the model to the current configuration of the kinematic chain
  void update();

//--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief gives the number of joints
   *
   * @return    number of joints
   */
  unsigned int getNumberOfJoints();
  
  /*!@brief transformation matrix between base frame and the specified joint frame
   *
   * @param index    index of the joint to which the transformation is given
   * @return    rigid transformation matrix, 4 \f$\times\f$ 4
   */
  cv::Mat getJointTransformation(const unsigned int index);
  
  /*!@brief calculates cartesian Jacobian of a point/vector
   *
   * @param point    relevant point for which the Jacobian is calculated
   * @param jointIndex    index of the joint frame the point is fixed to, joints after that will not move the point
   * @param result    Jacobian of the given point, in base coordinates, 3 \f$\times\f$ N matrix, where N = number of joints
   * @param coordinateFrame    specifies in which coordinate frame the point is represented
   */
  void calculateCartesianJacobian(
                                   const cv::Mat& point,
                                   const unsigned int jointIndex,
                                   cv::Mat& result,
                                   const unsigned int coordinateFrame
                                 );
  
  /*!@brief calculates Cartesian Jacobian of a point/vector
   * slightly slower than calculateJacobian()
   *
   * @param point    relevant point for which the Jacobian is calculated
   * @param jointIndex    index of the joint frame the point is fixed to, joints after that will not move the point
   * @param coordinateFrame    specifies in which coordinate frame the point is represented
   * @return    Jacobian of the given point, in base coordinates, 3 \f$\times\f$ N matrix, where N = number of joints
   */
  cv::Mat calculateCartesianJacobian(
                                      const cv::Mat& point,
                                      const unsigned int jointIndex,
                                      const unsigned int coordinateFrame
                                    );

  //! \todo explain coordinate system enum and give default
  /*!@brief calculates the temporal derivative of the Cartesian Jacobian of a point/vector
   *
   * @param point    relevant point for which the Jacobian is calculated
   * @param jointIndex    index of the joint frame the point is fixed to, joints after that will not move the point
   * @param result    Jacobian of the given point, in base coordinates, 3 \f$\times\f$ N matrix, where N = number of joints
   * @param coordinateFrame    specifies in which coordinate frame the point is represented
   */
  void calculateCartesianJacobianTemporalDerivative(
                                                     const cv::Mat& point,
                                                     const unsigned int jointIndex,
                                                     cv::Mat& result,
                                                     const unsigned int coordinateFrame
                                                   );

  /*!@brief calculates the temporal derivative of the Cartesian Jacobian of a point/vector given in homogeneous
   * coordinates of the relevant joint frame
   *
   * @param point    relevant point for which the Jacobian is calculated
   * @param jointIndex    index of the joint frame the point is fixed to, joints after that will not move the point
   * @param coordinateFrame    specifies in which coordinate frame the point is represented
   * @return    Jacobian of the given point, in base coordinates, 3 \f$\times\f$ N matrix, where N = number of joints
   */
  cv::Mat calculateCartesianJacobianTemporalDerivative(
                                                        const cv::Mat& point,
                                                        const unsigned int jointIndex,
                                                        const unsigned int coordinateFrame
                                                      );

  /*!@brief calculates cartesian velocity of a point
   *
   * @param point    point for which the velocity is calculated
   * @param jointIndex    index of the joint frame the point is fixed to, joints after that will not move the point
   * @param coordinateFrame    specifies in which coordinate frame the point is represented
   * @return    velocity of the given point, in world coordinates (homogeneous) 4 \f$\times\f$ 1 matrix
   */
  cv::Mat calculateVelocity(
                             const cv::Mat& point,
                             const unsigned int jointIndex,
                             const unsigned int coordinateFrame
                           );

  /*!@brief gives the spatial Jacobian in the current configuration
   * 
   * @param index    index of the joint up to which rotations are considered
   * @return    spatial Jacobian, 6 \f$\times\f$ N matrix, where N = number of joints
   */
  cv::Mat calculateSpatialJacobian(unsigned int index);

  /*!@brief gives the temporal derivative of the spatial Jacobian in the current configuration
   *
   * @param index    index of the joint up to which rotations are considered
   * @return    derivative of the spatial Jacobian, 6 \f$\times\f$ N matrix, where N = number of joints
   */
  cv::Mat calculateSpatialJacobianTemporalDerivative(unsigned int index);

  /*!@brief calculates cartesian acceleration of a point
   *
   * @param point    point for which the acceleration is calculated
   * @param jointIndex    index of the joint frame the point is fixed to, joints after that will not move the point
   * @param coordinateFrame    specifies in which coordinate frame the point is represented
   * @return    acceleration of the given point, in world coordinates (homogeneous) 4 \f$\times\f$ 1 matrix
   */
  cv::Mat calculateAcceleration(
                             const cv::Mat& point,
                             const unsigned int jointIndex,
                             const unsigned int coordinateFrame
                           );

  /*!@brief gives the end-effector position in the current configuration
   * 
   * @return    end effector position in homogeneous coordinates, 4 \f$\times\f$ 1 matrix
   */
  cv::Mat calculateEndEffectorPosition();

  /*!@brief gives the transformation from base frame to the end-effector frame in the current configuration
   * 
   * @return    rigid transformation matrix from base to end-effector frame, 4 \f$\times\f$ 4 matrix
   */
  cv::Mat calculateEndEffectorTransformation();

  /*!@brief gives the cartesian end-effector Jacobian in the current configuration
   * 
   * @return    end effector Jacobian, 3 \f$\times\f$ N matrix, where N = number of joints
   */
  cv::Mat calculateEndEffectorJacobian();

  /*!@brief gives the cartesian end-effector velocity
   *
   * @return    end effector velocity, 4 \f$\times\f$ 1 matrix (homogeneous coordinates)
   */
  cv::Mat calculateEndEffectorVelocity();

  /*!@brief gives the cartesian end-effector acceleration
   *
   * @return    end effector acceleration, 4 \f$\times\f$ 1 matrix (homogeneous coordinates)
   */
  cv::Mat calculateEndEffectorAcceleration();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief initialization
  void init();
  
  //!@brief calculates the transformations to the joint frames for the given joint angle vector
  void calculateTransformations();
  
  /*!@brief gives the temporal derivative of a joint twist in the current configuration
   *
   * @param index    index of the joint twist
   * @return    derivative of the joint twist, 6 \f$\times\f$ 1 matrix
   */
  cv::Mat calculateTwistTemporalDerivative(unsigned int index);

  //--------------------------------------------------------------------------------------------------------------------
  // public members
  //--------------------------------------------------------------------------------------------------------------------
public:
  //TODO: these are needed in more situations and should be a global cedar thing, check where to put them
  enum {
         WORLD_COORDINATES,
         BASE_COORDINATES,
         LOCAL_COORDINATES
       };
  
  //--------------------------------------------------------------------------------------------------------------------
  // protected members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief pointer to the kinematic chain that is being modelled
  cedar::dev::robot::KinematicChainPtr mpKinematicChain;

  //--------------------------------------------------------------------------------------------------------------------
  // private members
  //--------------------------------------------------------------------------------------------------------------------
private:
  // locks
  QReadWriteLock mTransformationsLock;
  
  //! twist coordinates for the transformations induced by rotating the joints (assuming reference configurations)
  std::vector<cv::Mat> mReferenceJointTwists;
  //! transformations to the joint frames (assuming reference configurations)
  std::vector<cv::Mat> mReferenceJointTransformations;
  //! transformations to the end-effector frame (assuming reference configurations)
  cv::Mat mReferenceEndEffectorTransformation;
  
  //! exponentials of joint twists with specified joint angle
  std::vector<cv::Mat> mTwistExponentials;
  //! transformation matrices between joints, generated by exponential map of joint twists
  std::vector<cv::Mat> mProductsOfExponentials;
  //! transformation matrices to the joint frames in the current configuration
  std::vector<cv::Mat> mJointTransformations;
  //! transformation matrix to the end-effecor frame in the current configuration
  cv::Mat mEndEffectorTransformation;
  //! twist coordinates for the transformations induced by rotating the joints in the curent configuration
  std::vector<cv::Mat> mJointTwists;
}; // class cedar::dev::robot::KinematicChainModel

#endif // CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_MODEL_H

