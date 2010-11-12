/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        ReferenceGeometry.cpp

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 10 25

 ----- Description: Header for the \em ReferenceGeometry class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "ReferenceGeometry.h"

// PROJECT INCLUDES
#include "src/auxiliaries/Namespace.h"

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
  readOrDefaultConfiguration();
}

//! destructor
ReferenceGeometry::~ReferenceGeometry()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

int ReferenceGeometry::readClassSpecificConfiguration()
{
  // read joint information
  std::string joint_path = "joints";
  std::string joint_path_dot = joint_path + ".";

  libconfig::Setting& r_joint_setting = mConfig.lookup(joint_path);
  const unsigned int number_of_joints = static_cast<unsigned int>(r_joint_setting.getLength());

  for (unsigned int i = 0; i < number_of_joints; ++i)
  {
    ReferenceGeometry::JointPtr p_joint(new ReferenceGeometry::Joint());
    //TODO
    //mConfig.lookupValue(joint_path_dot, p_joint.position);

  }

  std::cout << "number of joints: " << r_joint_setting.getLength() << "\n";

  return cedar::aux::ConfigurationInterface::CONFIG_SUCCESS;
}

const ReferenceGeometry::JointPtr& ReferenceGeometry::getJoint(const unsigned int index) const
{
  return _mJoints[index];
}

const ReferenceGeometry::LinkSegmentPtr& ReferenceGeometry::getLinkSegment(const unsigned int index) const
{
  return _mLinkSegments[index];
}

const std::vector<double>& ReferenceGeometry::getBasePosition() const
{
  return _mBasePosition;
}
