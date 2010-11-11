/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        KinematicChain.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 08 30

 ----- Description: Header for the \em cedar::dev::robot::KinematicChain class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_H
#define CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_H

// LOCAL INCLUDES
#include "Namespace.h"
#include "Component.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <cv>
#include <vector>
#include <string>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::dev::robot::KinematicChain : public cedar::dev::robot::Component
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  KinematicChain();
  //!@brief destructor
  virtual ~KinematicChain() = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  std::vector<double> getReferenceGeometry(void) const;
  unsigned int getNumberOfJoints(void) const;

  virtual double getJointAngle(const unsigned int index) const = 0;
  virtual std::vector<double> getJointAngles(void) const = 0;
  virtual cv::Mat getJointAnglesMatrix(void) const = 0;

  virtual double getJointVelocity(const unsigned int index) const;
  virtual std::vector<double> getJointVelocities(void) const;
  virtual cv::Mat getJointVelocitiesMatrix(void) const;

  virtual double getJointAcceleration(const unsigned int index) const;
  virtual std::vector<double> getJointAccelerations(void) const;
  virtual cv::Mat getJointAccelerationMatrix(void) const;

  virtual void setJointAngle(const unsigned int index, const double angle) = 0;
  virtual void setJointAngles(const cv::Mat angleMatrix) = 0;
  virtual void setJointAngles(const std::vector<double> angles) = 0;

  virtual void setJointVelocity(const unsigned int index, const double velocity);
  virtual void setJointVelocities(const cv::Mat velocities);
  virtual void setJointVelocities(const std::vector<double> velocities);

  virtual void setJointAcceleration(const unsigned int index, const double velocity);
  virtual void setJointAccelerations(const cv::Mat velocities);
  virtual void setJointAccelerations(const std::vector<double> velocities);

  void setReferenceGeometry(const ReferenceGeometryPtr& geometry);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  cedar::dev::robot::ReferenceGeometryPtr mReferenceGeometry;
  unsigned int mNumberOfJoints;

private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet
protected:
  // none yet
private:
  // none yet

}; // class cedar::dev::robot::KinematicChain

#endif // CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_H

