/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        KinematicChainSimulation.h
 
 ----- Maintainer:  Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 06
 
 ----- Description: Header for the \em cedar::dev::robot::KinematicChainSimulation class.
 
 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_SIMULATION_H
#define CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_SIMULATION_H

// LOCAL INCLUDES
#include "namespace.h"
#include "ReferenceGeometry.h"
#include "KinematicChain.h"
#include "KinematicChainModel.h"
#include "KinematicChainVisualization.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gl/Viewer.h"

// SYSTEM INCLUDES
#include <cv.h>

/*!@brief simulation of a kinematic chain
 *
 * This class provides a simulation of a kinematic chain determined by the specified reference geometry.
 * The current configuration state of the kinematic chain is represented by a vector of joint angles
 */
class cedar::dev::robot::KinematicChainSimulation : public cedar::dev::robot::KinematicChain
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
//  KinematicChainSimulation();
  //!@brief constructor
  KinematicChainSimulation(cedar::dev::robot::ReferenceGeometry* pReferenceGeometry);
  //!@brief destructor
  ~KinematicChainSimulation();
  
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief get current state of a single joint angle
   *
   * @return    joint angle value
   */
  double getJointAngle(unsigned int index) const;
  
  /*!@brief get current state of all joint angles
   *
   * @param index    specifies the joint
   * @return    vector of joint angles
   */
  std::vector<double> getJointAngles(void) const;
  
  /*!@brief get current state of all joint angles
   *
   * @return    vector of joint angles
   */
  cv::Mat getJointAnglesMatrix(void) const;
  
  /*!@brief set current state of a single joint angle
   *
   * @param index    specifies the joint
   * @param angle    new joint angle value
   */
  void setJointAngle(unsigned int index, const double angle);
  
  /*!@brief set current state of all joint angles
   *
   * @param angleMatrix    vector of new joint angle values
   */
  void setJointAngles(const cv::Mat& angleMatrix);
  
  /*!@brief set current state of all joint angles
   *
   * @param angles    vector of new joint angle values
   */
  void setJointAngles(const std::vector<double>& angles);
  
  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void init(void);
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! model of the kinematics, used for calculating transformations to the joint and end-effector frames
  KinematicChainModel mKinematicChainModel;
//  KinematicChainModelPtr mpKinematicChainModel;
  KinematicChainVisualization mKinematicChainVisualization;
  cedar::aux::gl::Scene mScene;
  cedar::aux::gl::Viewer mViewer;
  
  //TODO: use smart pointers
  //TODO: provide access functions to scene and viewer
  
  //! vector of joint angles, in radians
  cv::Mat mJointAngles;
  
}; // class cedar::dev::robot::KinematicChainSimulation

#endif // CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_SIMULATION_H
