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
 
 ----- File:        SimulatedKinematicChain.h
 
 ----- Maintainer:  Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 06
 
 ----- Description: Header for the \em cedar::dev::robot::SimulatedKinematicChain class.
 
 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_DEV_ROBOT_SIMULATED_KINEMATIC_CHAIN_H
#define CEDAR_DEV_ROBOT_SIMULATED_KINEMATIC_CHAIN_H

// LOCAL INCLUDES
#include "namespace.h"
#include "ReferenceGeometry.h"
#include "KinematicChain.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <cv.h>

/*!@brief simulation of a kinematic chain
 *
 * This class provides a simulation of a kinematic chain determined by the specified reference geometry.
 * The current configuration state of the kinematic chain is represented by a vector of joint angles
 */
class cedar::dev::robot::SimulatedKinematicChain : public cedar::dev::robot::KinematicChain
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  SimulatedKinematicChain(const cedar::dev::robot::ReferenceGeometryPtr& rpReferenceGeometry);
  //!@brief constructor
  SimulatedKinematicChain(const std::string& configFileName);
  //!@brief destructor
  ~SimulatedKinematicChain();
  
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
   * @return    vector of joint angles
   */
  cv::Mat getJointAnglesMatrix() const;

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
  using cedar::dev::robot::KinematicChain::setJointAngles;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void init();
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! vector of joint angles, in radians
  cv::Mat mJointAngles;
  
}; // class cedar::dev::robot::SimulatedKinematicChain

#endif // CEDAR_DEV_ROBOT_SIMULATED_KINEMATIC_CHAIN_H
