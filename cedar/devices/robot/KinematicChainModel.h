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

 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        KinematicChainModel.h

 ----- Maintainer:  Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 03
 
 ----- Description: Header for the @em cedar::dev::robot::KinematicChainModel class.
                    Calculates the geometric transformations occurring in a serial chain depending upon joint angles

 ----- Credits:     Mathematics from Murray, Lee, Sastry: A mathematical introduction to robotic manipulation
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_MODEL_H
#define CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_MODEL_H

// LOCAL INCLUDES
#include "namespace.h"
#include "ReferenceGeometry.h"
#include "KinematicChain.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/math/screwCalculus.h"

// SYSTEM INCLUDES
#include <string>
#include <cv.h>
#include <QReadWriteLock>
#include <QObject>

/*! \brief Calculates the geometric transformations occurring in a serial chain depending upon joint angles
 * 
 * A KinematicChainModel is initialized with an instance of ReferenceGeometry, containing all geometric information 
 * about the kinematic chain in reference configuration, i.e. \theta = 0. For a configuration given by a specific joint 
 * angle vector \theta, first call calculateTransformations( .. ). Then the transformations and jacobians can be 
 * accessed using the appropriate functions.
 */
class cedar::dev::robot::KinematicChainModel : public QObject
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
  //TODO: represent type of cv::Mat being used, offer it in constructor
  
  //--------------------------------------------------------------------------------------------------------------------
  // Qt events
  //--------------------------------------------------------------------------------------------------------------------
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
   * @return    rigid transformation matrix, 4 \times 4
   */
  cv::Mat getJointTransformation(const unsigned int index);
  
  /*!@brief calculates cartesian Jacobian of a point/vector given in homogeneous coordinates of the relevant joint frame
   *
   * @param point    relevant point for which the Jacobian is calculated
   * @param jointIndex    index of the joint frame the point is fixed to, joints after that will not move the point
   * @param result    Jacobian of the given point, in base coordinates, 3 \times N matrix, where N = number of joints
   * @param coordinateFrame    specifies in which coordinate frame the point is represented
   */
  void calculateJacobian(
                         const cv::Mat& point,
                         const unsigned int jointIndex,
                         cv::Mat& result,
                         const unsigned int coordinateFrame
                        );
  
  /*!@brief calculates cartesian Jacobian of a point/vector given in homogeneous coordinates of the relevant joint frame
   * slightly slower than the function above that uses references //TODO: doxygen reference to that function
   *
   * @param point    relevant point for which the Jacobian is calculated
   * @param jointIndex    index of the joint frame the point is fixed to, joints after that will not move the point
   * @param coordinateFrame    specifies in which coordinate frame the point is represented
   * @return    Jacobian of the given point, in base coordinates, 3 \times N matrix, where N = number of joints
   */
  cv::Mat calculateJacobian(
                            const cv::Mat& point,
                            const unsigned int jointIndex,
                            const unsigned int coordinateFrame
                           );
  
  /*!@brief gives the spatial Jacobian in the current configuration
   * 
   * @return    spatial Jacobian, 6 \times N matrix, where N = number of joints
   */
  cv::Mat calculateSpatialJacobian();
  
  /*!@brief gives the end-effector position in the current configuration
   * 
   * @return    end effector position in homogeneous coordinates, 4 \times 1 matrix
   */
  cv::Mat calculateEndEffectorPosition();

  /*!@brief gives the transformation from base frame to the end-effector frame in the current configuration
   * 
   * @return    rigid transformation matrix from base to end-effector frame, 4 \times 4 matrix
   */
  cv::Mat calculateEndEffectorTransformation();

  /*!@brief gives the cartesian end-effector Jacobian in the current configuration
   * 
   * @return    end effector Jacobian, 3 \times N matrix, where N = number of joints
   */
  cv::Mat calculateEndEffectorJacobian();
  
  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief initialization of simple example
//  void initExample();
  //TODO: remove when working properly

  //!@brief initialization
  void init();
  
  //!@brief calculates the transformations to the joint frames for the given joint angle vector
  void calculateTransformations();
  
  //--------------------------------------------------------------------------------------------------------------------
  // public members
  //--------------------------------------------------------------------------------------------------------------------
public:
  //TODO: these are needed in more situations and should be a global cedar thing, check where to put them
  enum {
        BASE_COORDINATES, 
        LOCAL_COORDINATES 
       };
  
  //--------------------------------------------------------------------------------------------------------------------
  // protected members
  //--------------------------------------------------------------------------------------------------------------------
protected:
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

