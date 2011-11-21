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

    File:        ReferenceGeometry.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 10 25

    Description: Encapsulates values that describe the physical properties of a robot.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "cedar/devices/robot/ReferenceGeometry.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>
#include <iostream>
#include <libconfig.h++>

using namespace cedar::dev::robot;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
ReferenceGeometry::ReferenceGeometry(const std::string& configFileName)
:
cedar::aux::ConfigurationInterface(configFileName)
{
  // initialize the configuration reading process
  init();
  // read configuration file
  readOrDefaultConfiguration();
}

//! destructor
ReferenceGeometry::~ReferenceGeometry()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void ReferenceGeometry::init()
{
  //! \todo this will be moved to configurationInterface, when this is done, remove here
  addParameter(&_mName, "Name", "<name>");

  // add parameters for joint information
  const std::string joint_path = "joints";

  libconfig::Setting& r_joint_setting = mConfig.lookup(joint_path);
  const unsigned int number_of_joints = static_cast<unsigned int>(r_joint_setting.getLength());

  for (unsigned int i = 0; i < number_of_joints; ++i)
  {
    ReferenceGeometry::JointPtr p_joint(new ReferenceGeometry::Joint());
    _mJoints.push_back(p_joint);

    std::string parameter_path = joint_path + ".[" + cedar::aux::toString<unsigned int>(i) + "].";

    addParameter(&(p_joint->position), parameter_path + "position", 0.0);
    addParameter(&(p_joint->axis), parameter_path + "axis", 0.0);
    addParameter(&(p_joint->angleLimits.min), parameter_path + "angleLimits.[0]", 0.0);
    addParameter(&(p_joint->angleLimits.max), parameter_path + "angleLimits.[1]", 0.0);
    addParameter(&(p_joint->velocityLimits.min), parameter_path + "velocityLimits.[0]", 0.0);
    addParameter(&(p_joint->velocityLimits.max), parameter_path + "velocityLimits.[1]", 0.0);
  }
  
  // add parameter for end effector information
  ReferenceGeometry::EndEffectorPtr p_end_effector(new ReferenceGeometry::EndEffector());
  _mpEndEffector = p_end_effector;
  addParameter(&(_mpEndEffector->position), "endEffector.position", 0.0);
  addParameter(&(_mpEndEffector->orientation), "endEffector.orientation", 0.0);

  // add parameters for link segment information
  const std::string link_segment_path = "links";

  libconfig::Setting& r_link_segment_setting = mConfig.lookup(link_segment_path);
  const unsigned int number_of_link_segments = static_cast<unsigned int>(r_link_segment_setting.getLength());

  for (unsigned int i = 0; i < number_of_link_segments; ++i)
  {
    ReferenceGeometry::LinkSegmentPtr p_link_segment(new ReferenceGeometry::LinkSegment());
    _mLinkSegments.push_back(p_link_segment);

    std::string parameter_path = link_segment_path + ".[" + cedar::aux::toString<unsigned int>(i) + "].";

    addParameter(&(p_link_segment->centerOfMassPosition), parameter_path + "centerOfMassPosition", 0.0);
    addParameter(&(p_link_segment->orientation), parameter_path + "orientation", 0.0);
    addParameter(&(p_link_segment->inertiaMoments), parameter_path + "inertiaMoments", 0.0);
  }
}

unsigned int ReferenceGeometry::getNumberOfJoints() const
{
  return _mJoints.size();
}

const ReferenceGeometry::JointPtr& ReferenceGeometry::getJoint(const unsigned int index) const
{
  return _mJoints[index];
}

const ReferenceGeometry::EndEffectorPtr& ReferenceGeometry::getEndEffector() const
{
  return _mpEndEffector;
}

const ReferenceGeometry::LinkSegmentPtr& ReferenceGeometry::getLinkSegment(const unsigned int index) const
{
  return _mLinkSegments[index];
}
