/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        ReferenceGeometry.cpp

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 10 25

 ----- Description: Header for the @em ReferenceGeometry class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "ReferenceGeometry.h"

// PROJECT INCLUDES
//#include "cedar/auxiliaries/namespace.h" // is this needed?

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

  // TODO: remove when ready
  testOutput();
}

//! destructor
ReferenceGeometry::~ReferenceGeometry()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

// TODO: remove when ready
void ReferenceGeometry::testOutput() const
{
  std::cout << "name: " << _mName.c_str() << "\n";

  std::cout << "number of joints: " << numberOfJoints() << "\n";

  std::cout << "base position: [";
  for (unsigned int j = 0; j < _mBasePosition.size(); ++j)
  {
    std::cout << _mBasePosition[j] << " ";
  }
  std::cout << "]\n";

  for (unsigned int i = 0; i < _mJoints.size(); ++i)
  {
    ReferenceGeometry::JointPtr p_joint = _mJoints[i];

    std::cout << "joint " << i << " position" << ": [";
    for (unsigned int j = 0; j < p_joint->position.size(); ++j)
    {
      std::cout << p_joint->position[j] << " ";
    }
    std::cout << "]\n";

    std::cout << "joint " << i << " axis" << ": [";
    for (unsigned int j = 0; j < p_joint->axis.size(); ++j)
    {
      std::cout << p_joint->axis[j] << " ";
    }
    std::cout << "]\n";

    std::cout << "joint " << i << " angle limits" << ": [";
    std::cout << p_joint->angleLimits.min << " " << p_joint->angleLimits.max << "]\n";

    std::cout << "joint " << i << " velocity limits" << ": [";
    std::cout << p_joint->velocityLimits.min << " " << p_joint->velocityLimits.max << "]\n";
  }

  std::cout << "end effector position: [";
  for (unsigned int j = 0; j < _mEndEffector->position.size(); ++j)
  {
    std::cout << _mEndEffector->position[j] << " ";
  }
  std::cout << "]\n";
  std::cout << "end effector orientation: [";
  for (unsigned int j = 0; j < _mEndEffector->orientation.size(); ++j)
  {
    std::cout << _mEndEffector->orientation[j] << " ";
  }
  std::cout << "]\n";
  
  for (unsigned int i = 0; i < _mLinkSegments.size(); ++i)
  {
    ReferenceGeometry::LinkSegmentPtr p_link_segment = _mLinkSegments[i];

    std::cout << "link " << i << " center of mass position" << ": [";
    for (unsigned int j = 0; j < p_link_segment->centerOfMassPosition.size(); ++j)
    {
      std::cout << p_link_segment->centerOfMassPosition[j] << " ";
    }
    std::cout << "]\n";

    std::cout << "link " << i << " center of mass direction" << ": [";
    for (unsigned int j = 0; j < p_link_segment->centerOfMassOrientation.size(); ++j)
    {
      std::cout << p_link_segment->centerOfMassOrientation[j] << " ";
    }
    std::cout << "]\n";

    std::cout << "link " << i << " inertia moments" << ": [";
    for (unsigned int j = 0; j < p_link_segment->inertiaMoments.size(); ++j)
    {
      std::cout << p_link_segment->inertiaMoments[j] << " ";
    }
    std::cout << "]\n";
  }
}

void ReferenceGeometry::init()
{
  // add parameter name
  addParameter(&_mName, "name", "<name>");

  // add parameter for base position
  addParameter(&_mBasePosition, "base.position", 0.0);

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

  std::cout << "trying to add end effector\n";
  
  // add parameter for end effector information
  ReferenceGeometry::EndEffectorPtr p_end_effector(new ReferenceGeometry::EndEffector());
  _mEndEffector = p_end_effector;
  addParameter(&(_mEndEffector->position), "endEffector.position", 0.0);
  addParameter(&(_mEndEffector->orientation), "endEffector.orientation", 0.0);

  std::cout << "added end effector\n";
  
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
    addParameter(&(p_link_segment->centerOfMassOrientation), parameter_path + "centerOfMassOrientation", 0.0);
    addParameter(&(p_link_segment->inertiaMoments), parameter_path + "inertiaMoments", 0.0);
  }
}

const unsigned int ReferenceGeometry::numberOfJoints() const
{
  return _mJoints.size();
}

const ReferenceGeometry::JointPtr& ReferenceGeometry::getJoint(const unsigned int index) const
{
  return _mJoints[index];
}

const ReferenceGeometry::EndEffectorPtr& ReferenceGeometry::getEndEffector() const
{
  return _mEndEffector;
}

const ReferenceGeometry::LinkSegmentPtr& ReferenceGeometry::getLinkSegment(const unsigned int index) const
{
  return _mLinkSegments[index];
}

const std::vector<double>& ReferenceGeometry::getBasePosition() const
{
  return _mBasePosition;
}
