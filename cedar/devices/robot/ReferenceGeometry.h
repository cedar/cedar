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

    File:        ReferenceGeometry.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 08 30

    Description: Encapsulates values that describe the physical properties of a robot.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_ROBOT_REFERENCE_GEOMETRY_H
#define CEDAR_DEV_ROBOT_REFERENCE_GEOMETRY_H

// CEDAR INCLUDES
#include "cedar/devices/robot/namespace.h"
#include "cedar/auxiliaries/ConfigurationInterface.h"
#include "cedar/auxiliaries/math/Limits.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>
#include <boost/smart_ptr.hpp>
#include <opencv2/opencv.hpp>

/*!@brief Encapsulates values that describe the physical properties of a kinematic chain.
 *
 * Describes the position, axis and angle/velocity limits of all joints of a kinematic chain, as well as all properties
 * of all link segments relevant for computing trajectories in a dynamical model.
 */
class cedar::dev::robot::ReferenceGeometry : public cedar::aux::ConfigurationInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // structs
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Describes the hardware properties of a joint.
  struct Joint
  {
    //! position of a joint in 3D space
    std::vector<double> position;
    //! axis the joint moves in, in 3D space
    std::vector<double> axis;
    //! minimum and maximum angular values
    cedar::aux::math::Limits<double> angleLimits;
    //! minimum and maximum velocity values
    cedar::aux::math::Limits<double> velocityLimits;
  };

  //!@brief Describes the hardware properties of an end-effector.
  struct EndEffector
  {
    //! position of a tool in 3D space
    std::vector<double> position;
    //! orientation of a tool in 3D space
    std::vector<double> orientation;
  };
  
  //!@brief Describes the hardware properties of a link segment.
  struct LinkSegment
  {
    //! position of center of mass in 3D space
    std::vector<double> centerOfMassPosition;
    //! orientation of center of mass in 3D space
    std::vector<double> orientation;
    //! inertia moments in 3D space
    std::vector<double> inertiaMoments;
  };

public:
  //! smart pointer definition for the Joint struct
  typedef boost::shared_ptr<cedar::dev::robot::ReferenceGeometry::Joint> JointPtr;
  //! smart pointer definition for the EndEffector struct
  typedef boost::shared_ptr<cedar::dev::robot::ReferenceGeometry::EndEffector> EndEffectorPtr;
  //! smart pointer definition for the LinkSegment struct
  typedef boost::shared_ptr<cedar::dev::robot::ReferenceGeometry::LinkSegment> LinkSegmentPtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor
  ReferenceGeometry(const std::string& configFileName);

  //!@brief Destructor
  virtual ~ReferenceGeometry();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Returns the number of joints in the robot
   *
   * @return    number of joints
   */
  unsigned int getNumberOfJoints() const;

  /*!@brief Returns a pointer to a specific joint.
   *
   * @return    pointer to joint struct
   * @param index    index of the joint
   */
  const cedar::dev::robot::ReferenceGeometry::JointPtr getJoint(unsigned int index) const;

  /*!@brief Returns a pointer to the end-effector transformation
   *
   * @return    pointer to endEffector struct
   */
  const cedar::dev::robot::ReferenceGeometry::EndEffectorPtr getEndEffector() const;
  
  /*!@brief Returns a pointer to a specific link segment.
   *
   * @return    pointer to link segment struct
   * @param index    index of the link segment
   */
  const cedar::dev::robot::ReferenceGeometry::LinkSegmentPtr getLinkSegment(unsigned int index) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Initializes the configuration interface.
   *
   * Since the @em cedar::dev::robot::ReferenceGeometry has a more complex configuration, parts of which the
   * @em cedar::aux::ConfigurationInterface does not support out of the box, the reading process must be prepared.
   */
  void init();

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! vector of all joints
  std::vector<cedar::dev::robot::ReferenceGeometry::JointPtr> _mJoints;
  //! end effector
  cedar::dev::robot::ReferenceGeometry::EndEffectorPtr _mpEndEffector;
  //! vector of all link segments
  std::vector<cedar::dev::robot::ReferenceGeometry::LinkSegmentPtr> _mLinkSegments;
}; // class cedar::dev:robot::ReferenceGeometry

#endif // CEDAR_DEV_ROBOT_REFERENCE_GEOMETRY_H

