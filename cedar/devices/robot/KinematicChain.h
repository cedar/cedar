/*------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        KinematicChain.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 08 30

 ----- Description: Header for the @em cedar::dev::robot::KinematicChain class.

 ----- Credits:
 -----------------------------------------------------------------------------*/

#ifndef CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_H
#define CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_H

// LOCAL INCLUDES
#include "namespace.h"
#include "Component.h"
#include "cedar/auxiliaries/Thread.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <cv.h>
#include <vector>
#include <string>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::dev::robot::KinematicChain : public cedar::dev::robot::Component, public cedar::aux::Thread
{
  //----------------------------------------------------------------------------
  // macros
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  // constructors and destructor
  //----------------------------------------------------------------------------
public:
  //!@brief constructor
  KinematicChain();
  //!@brief constructor
  KinematicChain(unsigned numberOfJoints, unsigned stepSize = 1);
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
  void setReferenceGeometry(const ReferenceGeometryPtr geometry);

  /*!@brief get current state of a single joint angle
   *
   * @return    joint angle value
   */
  virtual double getJointAngle(unsigned int index) const = 0;

  /*!@brief get current state of all joint angles
   *
   * @param index    specifies the joint
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
   * @return    joint velocity value
   */
  virtual double getJointVelocity(unsigned int index) const;

  /*!@brief get current state of all joint velocities
   *
   * @param index    specifies the joint
   * @return    vector of joint velocities
   */
  virtual std::vector<double> getJointVelocities() const;

  /*!@brief get current state of all joint velocities
   *
   * @param index    specifies the joint
   * @return    vector of joint velocities
   */
  virtual cv::Mat getJointVelocitiesMatrix() const;

  /*!@brief get current state of a single joint acceleration
   *
   * @return    joint acceleration value
   */
  virtual double getJointAcceleration(unsigned int index) const;

  /*!@brief get current state of all joint accelerations
   *
   * @param index    specifies the joint
   * @return    vector of joint accelerations
   */
  virtual std::vector<double> getJointAccelerations() const;

  /*!@brief get current state of all joint accelerations
   *
   * @return    vector of joint accelerations
   */
  virtual cv::Mat getJointAccelerationMatrix() const;

  /*!@brief set current state of a single joint angle
   *
   * @param index    specifies the joint
   * @param angle    new joint angle value
   */
  virtual void setJointAngle(unsigned int index, double angle);

  /*!@brief set current state of all joint angles
   *
   * @param angleMatrix    vector of new joint angle values
   */
  virtual void setJointAngles(const cv::Mat& angleMatrix);

  /*!@brief set current state of all joint angles
   *
   * @param angles    vector of new joint angle values
   */
  virtual void setJointAngles(const std::vector<double>& angles);

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

  virtual void setJointAngleOnDevice(unsigned int index, double angle) = 0;


  //----------------------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------------------
private:
  void step(unsigned long time);

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
  cedar::dev::robot::ReferenceGeometryPtr mpReferenceGeometry;
  unsigned int mNumberOfJoints;
private:
  std::vector<double> mJointAngles;
  std::vector<double> mJointVelocities;
  std::vector<double> mJointAccelerations;
  std::vector<ActionType> mJointWorkingModes;

}; // class cedar::dev::robot::KinematicChain

#endif // CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_H

