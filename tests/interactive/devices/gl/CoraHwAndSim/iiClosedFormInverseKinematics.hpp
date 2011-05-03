/*--------------------------------------------------------------------------
 ----- Institute:   	Ruhr-Universitaet-Bochum
 Institut fuer Neuroinformatik

 ----- File:			iiClosedFormInverseKinematics.hpp

 ----- Author:			Ioannis Iossifidis
 iossifidis@neuroinformatik.rub.de

 ----- Date:       	10 Σεπ 2009

 ----- Description:

 ----- Copyright:   	(c) Ioannis Iossifidis 10 Σεπ 2009

 ----- Warranties:		Of course, there's NO WARRANTY OF ANY KIND :-)

 ----- Credits:

 ----- Project:		hrOpenChainManipulatorLib

 --------------------------------------------------------------------------*/

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

//------------------------------------------------------------------rad2deg
inline double rad2deg(double rad)
{
    return rad * 180.0 / M_PI;
}

/*---------------------------------------------------------------------------
 TASK_STRUCT includes kartesian taskcoordinates [x,y,z], the griper
 orientation and the elbowangel.
 ----------------------------------------------------------------------------*/

typedef struct  {
	cv::Mat	Pos;
	cv::Mat	eefOrientationAngle; //phi,theta,gamma

	double shoulderGear;
	double trunkAng;
	double redundancyAng;

} TaskCoord;
/*---------------------------------------------------------------------------
 HAND_STRUCT includes the position vector of the wrist, the endeffector
 and the  gripper (open or closed in cm)
 ----------------------------------------------------------------------------*/
typedef struct HAND_STRUCT {

  cv::Mat wristPos;
  cv::Mat eefPos;
  cv::Mat gripPos;
  cv::Mat eefOrientationAngle; //phi,theta,gamma
  cv::Mat wristOrientationAngle; //phi,theta,gamma
} HandCoord;
/*---------------------------------------------------------------------------
 ELBOW_STRUCT includes the elbowangle and the position vector of the elbow
 ----------------------------------------------------------------------------*/
typedef struct ELBOW_STRUCT {
  cv::Mat Pos;			// r_U
	double Angle;			// alpha
} ElbowCoord;

///*---------------------------------------------------------------------------
// HEAD_STRUCT includes the elbowangle and the position vector of the elbow
// ----------------------------------------------------------------------------*/
//typedef struct HEAD_STRUCT {
//	double Phi;			// r_U
//	double Theta;			// alpha
//} HeadCoord;


class ClosedFormInverseKinematics
{
public:
	ClosedFormInverseKinematics();
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
		int TaskCoordinatesToArmGeometry();
		double CalcTrunkAng();
		void InitVariables();
		void InverseKinematics();

//		(HandCoord *Hand,
//			ElbowCoord *Elbow,
//			TaskCoord Task,
//			double eefLength,
//			double* trunkAng);

		TaskCoord  mTaskCoordinates;
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

		cv::Mat mJointAngle;
		void Rx(double rotAngle, cv::Mat* pRotMatX);
		void Ry(double rotAngle, cv::Mat* pRotMatY);
		void Rz(double rotAngle, cv::Mat* pRotMatZ);

};

#endif /* IICLOSEDFORMINVERSEKINEMATICS_HPP_ */
