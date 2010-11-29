/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        KinematicChain.cpp

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 11 15

 ----- Description: Header for the @em cedar::aux::KinematicChain class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "KinematicChain.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::dev::robot;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
KinematicChain::KinematicChain()
{
}

//! destructor
KinematicChain::~KinematicChain()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

const ReferenceGeometryPtr& KinematicChain::getReferenceGeometry() const
{
  return mpReferenceGeometry;
}

const unsigned int KinematicChain::getNumberOfJoints() const
{
  return mNumberOfJoints;
}

void KinematicChain::setReferenceGeometry(const ReferenceGeometryPtr& rpGeometry)
{
  mpReferenceGeometry = rpGeometry;
}

const double KinematicChain::getJointVelocity(const unsigned int index) const
{
  // TODO: Implementation (Bjoern Weghenkel)
  return 0.0;
}
const std::vector<double> KinematicChain::getJointVelocities() const
{
  // TODO: Implementation (Bjoern Weghenkel)
  std::vector<double> dummy;
  return dummy;
}
const cv::Mat KinematicChain::getJointVelocitiesMatrix() const
{
  // TODO: Implementation (Bjoern Weghenkel)
  return cv::Mat();
}

const double KinematicChain::getJointAcceleration(const unsigned int index) const
{
  // TODO: Implementation (Bjoern Weghenkel)
  return 0.0;
}

const std::vector<double> KinematicChain::getJointAccelerations() const
{
  // TODO: Implementation (Bjoern Weghenkel)
  std::vector<double> dummy;
  return dummy;
}

const cv::Mat KinematicChain::getJointAccelerationMatrix() const
{
  // TODO: Implementation (Bjoern Weghenkel)
  return cv::Mat();
}

void KinematicChain::setJointVelocity(const unsigned int index, const double velocity)
{
  // TODO: Implementation (Bjoern Weghenkel)
}

void KinematicChain::setJointVelocities(const std::vector<double>& velocities)
{
  // TODO: Implementation (Bjoern Weghenkel)
}

void KinematicChain::setJointVelocities(const cv::Mat& velocities)
{
  // TODO: Implementation (Bjoern Weghenkel)
}

void KinematicChain::setJointAcceleration(const unsigned int index, const double acceleration)
{
  // TODO: Implementation (Bjoern Weghenkel)
}

void KinematicChain::setJointAccelerations(const cv::Mat& accelerations)
{
  // TODO: Implementation (Bjoern Weghenkel)
}

void KinematicChain::setJointAccelerations(const std::vector<double>& accelerations)
{
  // TODO: Implementation (Bjoern Weghenkel)
}
