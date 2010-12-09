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

