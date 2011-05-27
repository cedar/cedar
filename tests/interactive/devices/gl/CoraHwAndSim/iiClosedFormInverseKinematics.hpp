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

    File:        iiClosedFormInverseKinematics.hpp

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.ruhr-uni-bochum.de
    Date:        2011 05 13

    Description: Inverse kinematics for the Cora arm.

    Credits: Ioannis Iossifidis

======================================================================================================================*/

#ifndef IICLOSEDFORMINVERSEKINEMATICS_HPP_
#define IICLOSEDFORMINVERSEKINEMATICS_HPP_

//#include <cvmat.hpp>
//#include "hrOpenChainManipulator.h"
//#include "hrOcmCalculator.h"

#include "cv.h"
//#include "math.h"


inline double deg2rad(double deg)
{
  return deg * M_PI / 180.0;
}

inline double rad2deg(double rad)
{
  return rad * 180.0 / M_PI;
}

/*!@brief Struct to hold the task coordinates
 */
typedef struct  {
  //!@brief x, y, z
	cv::Mat	Pos;
  //!@brief phi, theta, gamma
	cv::Mat	eefOrientationAngle;

  //!@brief shoulder angle
	double shoulderGear;
  //!@brief trunk angle
	double trunkAng;
  //!@brief Redundancy angle of the elbow
	double redundancyAng;
} TaskCoord;

/*!@brief Struct to hold the position and orientation of the end-effector
 */
typedef struct HAND_STRUCT {
  //!@brief wrist position
  cv::Mat wristPos;
  //!@brief end-effector position
  cv::Mat eefPos;
  //!@brief gripper position
  cv::Mat gripPos;
  //!@brief phi, theta, gamma
  cv::Mat eefOrientationAngle; //phi,theta,gamma
  //!@brief wrist orientation
  cv::Mat wristOrientationAngle;
} HandCoord;

/*!@brief Struct to hold the position and orientation of the elbow
 */
typedef struct ELBOW_STRUCT {
  //!@brief elbow position
  cv::Mat Pos;			// r_U
  //!@brief alpha
	double Angle;			// alpha
} ElbowCoord;

///*---------------------------------------------------------------------------
// HEAD_STRUCT includes the elbowangle and the position vector of the elbow
// ----------------------------------------------------------------------------*/
//typedef struct HEAD_STRUCT {
//	double Phi;			// r_U
//	double Theta;			// alpha
//} HeadCoord;


/*!@brief Inverse kinematics for Cora arm
 */
class ClosedFormInverseKinematics
{
public:

  /*!@brief Default constructor
   */
	ClosedFormInverseKinematics();

  /*!@brief Destructor
   */
	virtual ~ClosedFormInverseKinematics();

	/*!------------------------------------------------------------------------
		 KIN_task2Vars():

		 Compute the following variables from the given task space coordinates.
		 The task coordinates give us the endeffectorposition, the endeffector-
		 orintation and the elbowangle.
		 We compute the wristposition, the elbowposition and the gripperpostion
		 under the constrain to be parallel to the ground.
		 Consider that the zero angle direction of the gripper is the x-axis
		 direction.
		 --------------------------------------------------------------------------*/

  /*!@brief
   */
  int TaskCoordinatesToArmGeometry();

  /*!@brief Calculate trunk angle
   */
  double CalcTrunkAng();

  /*!@brief Initializes all variables
   */
  void InitVariables();

  /*!@brief Calculate joint angles from task coordinates
   */
  void InverseKinematics();

//		(HandCoord *Hand,
//			ElbowCoord *Elbow,
//			TaskCoord Task,
//			double eefLength,
//			double* trunkAng);

  //!@brief The task coorinates, e.g. EEF position
  TaskCoord  mTaskCoordinates;
  //!@brief Calculated joint angles
  cv::Mat mJointAngle;

private:
  HandCoord  mCalculatedHandState;
  ElbowCoord mCalculatedElbowState;
  cv::Mat	   mRobotsLimbs;//Length/Coordinates of robots limbs in initial configuration
  cv::Mat	   mTrunk,mShoulder,mUperArm,mForeArm,mEef;

  cv::Mat mP_u;// p_u in the paper
  cv::Mat mP_f;// p_f in the paper
  cv::Mat mP_h;// p_h in the paper
  cv::Mat mP_g;// p_g in the paper
  cv::Mat mP_W;// p_W in the paper
  cv::Mat mP_T;// p_T in the paper

  void Rx(double rotAngle, cv::Mat* pRotMatX);
  void Ry(double rotAngle, cv::Mat* pRotMatY);
  void Rz(double rotAngle, cv::Mat* pRotMatZ);

  /*!@brief Prints a given matrix
   *
   * Useful for debugging.
   */
  void printMatrix(cv::Mat& mat, const char* name);
};

#endif /* IICLOSEDFORMINVERSEKINEMATICS_HPP_ */
