/*------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        KinematicChain.cpp

 ----- Author:      Mathis Richter
                    Bjoern Weghenkel
 ----- Email:       mathis.richter@ini.rub.de
                    bjoern.weghenkel@ini.ruhr-uni-bochum.de
 ----- Date:        2010 11 25

 ----- Description: Header for the @em cedar::aux::KinematicChain class.

 ----- Credits:
 -----------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "KinematicChain.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace std;
using namespace cedar::dev::robot;

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------

//! constructor
KinematicChain::KinematicChain(unsigned numberOfJoints)
{
  mJointAngles.resize(numberOfJoints);
  mJointVelocities.resize(numberOfJoints);
  mJointAccelerations.resize(numberOfJoints);
  mJointWorkingModes.resize(numberOfJoints);
  return;
}


//! destructor
KinematicChain::~KinematicChain()
{
}


//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

const ReferenceGeometryPtr& KinematicChain::getReferenceGeometry() const
{
  return mpReferenceGeometry;
}


unsigned int KinematicChain::getNumberOfJoints() const
{
  return mNumberOfJoints;
}


void KinematicChain::setReferenceGeometry(const ReferenceGeometryPtr& rpGeometry)
{
  mpReferenceGeometry = rpGeometry;
}


double KinematicChain::getJointVelocity(unsigned int index) const
{
  if(index >= mNumberOfJoints)
    return 0.0;
  return mJointVelocities[index];
}


std::vector<double> KinematicChain::getJointVelocities() const
{
  std::vector<double> dummy(mJointVelocities);
  return dummy;
}


cv::Mat KinematicChain::getJointVelocitiesMatrix() const
{
  cv::Mat dummy(mNumberOfJoints, 1, CV_32F);
  for (unsigned i = 0; i < mNumberOfJoints; i++)
    dummy.at<double>(i,0) = mJointVelocities[i];
  return dummy;
}


double KinematicChain::getJointAcceleration(unsigned int index) const
{
  if(index >= mNumberOfJoints)
    return 0.0;
  return mJointAccelerations[index];
}


std::vector<double> KinematicChain::getJointAccelerations() const
{
  std::vector<double> dummy(mJointAccelerations);
  return dummy;
}


cv::Mat KinematicChain::getJointAccelerationMatrix() const
{
  cv::Mat dummy(mNumberOfJoints, 1, CV_32F);
  for (unsigned i = 0; i < mNumberOfJoints; i++)
    dummy.at<double>(i,0) = mJointAccelerations[i];
  return dummy;
}


void KinematicChain::setJointVelocity(const unsigned int index, const double velocity)
{
  if(index >= mNumberOfJoints)
    return;
  mJointVelocities[index] = velocity;
  mJointWorkingModes[index] = VELOCITY;
}


void KinematicChain::setJointVelocities(const std::vector<double>& velocities)
{
  if(velocities.size() != mNumberOfJoints)
    return;
  mJointVelocities = velocities;
  for(unsigned i = 0; i < mNumberOfJoints; i++)
    mJointWorkingModes[i] = VELOCITY;
}


void KinematicChain::setJointVelocities(const cv::Mat& velocities)
{
  if(velocities.size().height != (int)mNumberOfJoints || velocities.size().width != 1)
    return;
  for(unsigned i = 0; i < mNumberOfJoints; i++) {
    mJointVelocities[i] = velocities.at<double>(i,0);
    mJointVelocities[i] = VELOCITY;
  }
}


void KinematicChain::setJointAcceleration(unsigned int index, double acceleration)
{
  if(index >= mNumberOfJoints)
    return;
  mJointAccelerations[index] = acceleration;
  mJointWorkingModes[index] = ACCELERATION;
}


void KinematicChain::setJointAccelerations(const std::vector<double>& accelerations)
{
  if(accelerations.size() != mNumberOfJoints)
    return;
  mJointAccelerations = accelerations;
  for(unsigned i = 0; i < mNumberOfJoints; i++)
    mJointWorkingModes[i] = ACCELERATION;
}


void KinematicChain::setJointAccelerations(const cv::Mat& accelerations)
{
  if(accelerations.size().height != (int)mNumberOfJoints || accelerations.size().width != 1)
    return;
  for(unsigned i = 0; i < mNumberOfJoints; i++) {
    mJointAccelerations[i] = accelerations.at<double>(i,0);
    mJointAccelerations[i] = ACCELERATION;
  }
}


void KinematicChain::step(unsigned time) {
  // TODO: Implementation (Bjoern Weghenkel)
  cout << "step" << endl;

  for(unsigned i = 0; i < mNumberOfJoints; i++) {

    switch(mJointWorkingModes[i]) {
    case ANGLE:
      // TODO catch joint angles
      setJointAngle( i, mJointAngles[i] );
      break;
    case VELOCITY:
      break;
    case ACCELERATION:
      break;
    default:
      cerr << "Oh oh, something went terribly wrong here!" << endl;
    }

  }

  return;
}
