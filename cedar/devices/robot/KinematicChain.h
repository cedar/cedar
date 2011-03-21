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

    File:        KinematicChain.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 08 30

    Description: Chain of joints (e.g., a robotic arm).

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_H
#define CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_H

// LOCAL INCLUDES
#include "devices/robot/namespace.h"
#include "devices/robot/Component.h"
#include "auxiliaries/LoopedThread.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <cv.h>
#include <vector>
#include <string>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::dev::robot::KinematicChain : public cedar::dev::robot::Component, public cedar::aux::LoopedThread
{
  //----------------------------------------------------------------------------
  // macros
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  // constructors and destructor
  //----------------------------------------------------------------------------
public:
  //!@brief constructor
  KinematicChain(const cedar::dev::robot::ReferenceGeometryPtr& rpReferenceGeometry);
  //!@brief constructor
  KinematicChain(const std::string& configFileName);
  //!@brief destructor
  virtual ~KinematicChain() = 0;

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------
public:
  /*!@brief get reference geometry for this kinematic chain
   *
   * @return    reference geometry
   */
  const ReferenceGeometryPtr& getReferenceGeometry() const;

  /*!@brief get number of joints in this kinematic chain
   *
   * @return    number of joints
   */
  unsigned int getNumberOfJoints() const;

  /*!@brief set a new reference geometry
   *
   * @param geometry    new reference geometry
   */
  void setReferenceGeometry(const ReferenceGeometryPtr& geometry);

  /*!@brief get current state of a single joint angle
   *
   * @param index    specifies the joint
   * @return    joint angle value
   */
  virtual double getJointAngle(unsigned int index) const = 0;

  /*!@brief get current state of all joint angles
   *
   * @return    vector of joint angles
   */
  virtual std::vector<double> getJointAngles() const = 0;

  /*!@brief get current state of all joint angles
   *
   * @return    vector of joint angles
   */
  virtual cv::Mat getJointAnglesMatrix() const = 0;

  /*!@brief get current state of a single joint velocity
   *
   * @param index    specifies the joint
   * @return    joint velocity value
   */
  virtual double getJointVelocity(unsigned int index) const;

  /*!@brief get current state of all joint velocities
   *
   * @return    vector of joint velocities
   */
  virtual std::vector<double> getJointVelocities() const;

  /*!@brief get current state of all joint velocities
   *
   * @return    vector of joint velocities
   */
  virtual cv::Mat getJointVelocitiesMatrix() const;

  /*!@brief get current state of a single joint acceleration
   *
   * @param index    specifies the joint
   * @return    joint acceleration value
   */
  virtual double getJointAcceleration(unsigned int index) const;

  /*!@brief get current state of all joint accelerations
   *
   * @return    vector of joint accelerations
   */
  virtual std::vector<double> getJointAccelerations() const;

  /*!@brief get current state of all joint accelerations
   *
   * @return    vector of joint accelerations
   */
  virtual cv::Mat getJointAccelerationsMatrix() const;

  /*!@brief set current state of a single joint angle
   *
   * @param index    specifies the joint
   * @param angle    new joint angle value
   */
  virtual void setJointAngle(unsigned int index, double angle) = 0;

  /*!@brief set current state of all joint angles
   *
   * @param angleMatrix    vector of new joint angle values
   */
  virtual void setJointAngles(const cv::Mat& angleMatrix) = 0;

  /*!@brief set current state of all joint angles
   *
   * @param angles    vector of new joint angle values
   */
  virtual void setJointAngles(const std::vector<double>& angles) = 0;

  /*!@brief set current state of a single joint velocity
   *
   * @param index    specifies the joint
   * @param velocity    new joint velocity value
   */
  virtual void setJointVelocity(unsigned int index, double velocity);

  /*!@brief set current state of all joint velocities
   *
   * @param velocities    vector of new joint velocity values
   */
  virtual void setJointVelocities(const cv::Mat& velocities);

  /*!@brief set current state of all joint velocities
   *
   * @param velocities    vector of new joint velocity values
   */
  virtual void setJointVelocities(const std::vector<double>& velocities);

  /*!@brief set current state of a single joint acceleration
   *
   * @param index    specifies the joint
   * @param acceleration    new joint acceleration value
   */
  virtual void setJointAcceleration(unsigned int index, double acceleration);

  /*!@brief set current state of all joint velocities
   *
   * @param accelerations    vector of new joint velocity values
   */
  virtual void setJointAccelerations(const cv::Mat& accelerations);

  /*!@brief set current state of all joint velocities
   *
   * @param accelerations    vector of new joint velocity values
   */
  virtual void setJointAccelerations(const std::vector<double>& accelerations);

  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------
protected:

  //----------------------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------------------
private:
  void step(double time);

  //----------------------------------------------------------------------------
  // parameters
  //----------------------------------------------------------------------------
public:
  // none yet
protected:
  // none yet
private:
  enum ActionType { ANGLE, VELOCITY, ACCELERATION };

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  //!@brief geometry in reference configuration
  cedar::dev::robot::ReferenceGeometryPtr mpReferenceGeometry;
private:
  std::vector<double> mJointVelocities;
  std::vector<double> mJointAccelerations;
  std::vector<ActionType> mJointWorkingModes;

}; // class cedar::dev::robot::KinematicChain

#endif // CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_H

