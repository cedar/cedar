/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        KinematicChain.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 08 30

 ----- Description: Header for the @em cedar::dev::robot::KinematicChain class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_H
#define CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_H

// LOCAL INCLUDES
#include "namespace.h"
#include "Component.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <cv.h>
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
  const ReferenceGeometryPtr& getReferenceGeometry() const;
  const unsigned int getNumberOfJoints() const;
  void setReferenceGeometry(const ReferenceGeometryPtr& geometry);

  virtual const double getJointAngle(const unsigned int index) const = 0;
  virtual const std::vector<double> getJointAngles() const = 0;
  virtual const cv::Mat getJointAnglesMatrix() const = 0;

  virtual const double getJointVelocity(const unsigned int index) const;
  virtual const std::vector<double> getJointVelocities() const;
  virtual const cv::Mat getJointVelocitiesMatrix() const;

  virtual const double getJointAcceleration(const unsigned int index) const;
  virtual const std::vector<double> getJointAccelerations() const;
  virtual const cv::Mat getJointAccelerationMatrix() const;

  virtual void setJointAngle(const unsigned int index, const double angle) = 0;
  virtual void setJointAngles(const cv::Mat& angleMatrix) = 0;
  virtual void setJointAngles(const std::vector<double>& angles) = 0;

  virtual void setJointVelocity(const unsigned int index, const double velocity);
  virtual void setJointVelocities(const cv::Mat& velocities);
  virtual void setJointVelocities(const std::vector<double>& velocities);

  virtual void setJointAcceleration(const unsigned int index, const double velocity);
  virtual void setJointAccelerations(const cv::Mat& velocities);
  virtual void setJointAccelerations(const std::vector<double>& velocities);

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
  cedar::dev::robot::ReferenceGeometryPtr mpReferenceGeometry;
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

