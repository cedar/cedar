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
#include "auxiliaries/Thread.h"

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
private:
  //!@brief constructor
  KinematicChain() {};  // disabled!
public:
  //!@brief constructor
  KinematicChain(unsigned numberOfJoints, unsigned stepSize = 1);
  //!@brief destructor
  virtual ~KinematicChain() = 0;

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------
public:
  const ReferenceGeometryPtr& getReferenceGeometry() const;
  unsigned int getNumberOfJoints() const;
  void setReferenceGeometry(const ReferenceGeometryPtr& geometry);

  virtual double getJointAngle(unsigned int index) const = 0;
  virtual std::vector<double> getJointAngles() const = 0;
  virtual cv::Mat getJointAnglesMatrix() const = 0;

  virtual double getJointVelocity(unsigned int index) const;
  virtual std::vector<double> getJointVelocities() const;
  virtual cv::Mat getJointVelocitiesMatrix() const;

  virtual double getJointAcceleration(unsigned int index) const;
  virtual std::vector<double> getJointAccelerations() const;
  virtual cv::Mat getJointAccelerationMatrix() const;

  virtual void setJointAngle(unsigned int index, double angle) = 0;
  virtual void setJointAngles(const cv::Mat& angleMatrix) = 0;
  virtual void setJointAngles(const std::vector<double>& angles) = 0;

  virtual void setJointVelocity(unsigned int index, double velocity);
  virtual void setJointVelocities(const cv::Mat& velocities);
  virtual void setJointVelocities(const std::vector<double>& velocities);

  virtual void setJointAcceleration(unsigned int index, double acceleration);
  virtual void setJointAccelerations(const cv::Mat& accelerations);
  virtual void setJointAccelerations(const std::vector<double>& accelerations);

  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------
protected:
  // none yet

  //----------------------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------------------
private:
  void step(unsigned time);

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

