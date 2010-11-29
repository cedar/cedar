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
#include "ReferenceGeometry.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace std;
using namespace cedar::dev::robot;

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------

//! constructor
KinematicChain::KinematicChain(unsigned numberOfJoints, unsigned stepSize) : Thread(stepSize)
{
  mNumberOfJoints = numberOfJoints;
  mJointAngles.resize(mNumberOfJoints);
  mJointVelocities.resize(mNumberOfJoints);
  mJointAccelerations.resize(mNumberOfJoints);
  mJointWorkingModes.resize(mNumberOfJoints);
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


void KinematicChain::setJointVelocity(unsigned index, double velocity)
{

  if(index >= mNumberOfJoints)
    return;

  velocity = max<double>( velocity, mpReferenceGeometry->getJoint(index)->velocityLimits.min );
  velocity = min<double>( velocity, mpReferenceGeometry->getJoint(index)->velocityLimits.max );

  mJointVelocities[index] = velocity;
  mJointWorkingModes[index] = VELOCITY;

  return;
}


void KinematicChain::setJointVelocities(const std::vector<double>& velocities)
{

  if(velocities.size() != mNumberOfJoints)
    return;

  for(unsigned i = 0; i < mNumberOfJoints; i++) {

    double velocity = velocities[i];
    velocity = max<double>( velocity, mpReferenceGeometry->getJoint(i)->velocityLimits.min );
    velocity = min<double>( velocity, mpReferenceGeometry->getJoint(i)->velocityLimits.max );

    mJointVelocities[i] = velocity;
    mJointWorkingModes[i] = VELOCITY;

  }

  return;
}


void KinematicChain::setJointVelocities(const cv::Mat& velocities)
{

  if(velocities.size().height != (int)mNumberOfJoints || velocities.size().width != 1)
    return;

  for(unsigned i = 0; i < mNumberOfJoints; i++) {

    double velocity = velocities.at<double>(i,0);

    velocity = max<double>( velocity, mpReferenceGeometry->getJoint(i)->velocityLimits.min );
    velocity = min<double>( velocity, mpReferenceGeometry->getJoint(i)->velocityLimits.max );

    mJointVelocities[i] = velocity;
    mJointVelocities[i] = VELOCITY;
  }

  return;
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

  double currentAngle = 0.0;
  double newAngle = 0.0;
  double velocity = 0.0;

  // update joint angle depending on working mode
  for(unsigned i = 0; i < mNumberOfJoints; i++) {

    // get current joint angle from the device
    currentAngle = getJointAngle(i);

    // update the angle according to working mode
    switch(mJointWorkingModes[i]) {

    case ANGLE:

      // calculate velocity
      newAngle = mJointAngles[i];
      velocity = ( newAngle - currentAngle ) * ( 1000.0 / (double) time );

      // consider limits
      newAngle = max<double>( newAngle, mpReferenceGeometry->getJoint(i)->angleLimits.min );
      newAngle = min<double>( newAngle, mpReferenceGeometry->getJoint(i)->angleLimits.max );

      // apply new values
      setJointAngle( i, newAngle );
      mJointVelocities[i] = velocity;

      break;

    case VELOCITY:

      // calculate new angle
      newAngle = currentAngle + velocity * ( (double) time / 1000.0 );

      // consider angle limits
      newAngle = max<double>( newAngle, mpReferenceGeometry->getJoint(i)->angleLimits.min );
      newAngle = min<double>( newAngle, mpReferenceGeometry->getJoint(i)->angleLimits.max );

      // set new joint angle
      setJointAngle( i, newAngle );
      mJointAngles[i] = newAngle;

      break;

    case ACCELERATION:

      velocity = mJointVelocities[i] + mJointAccelerations[i] * ( (double) time / 1000.0 );

      // consider velocity limits
      velocity = max( velocity, mpReferenceGeometry->getJoint(i)->velocityLimits.min );
      velocity = min( velocity, mpReferenceGeometry->getJoint(i)->velocityLimits.max );

      newAngle = currentAngle + velocity * ( (double) time / 1000.0 );

      // consider angle limits
      newAngle = max<double>( newAngle, mpReferenceGeometry->getJoint(i)->angleLimits.min );
      newAngle = min<double>( newAngle, mpReferenceGeometry->getJoint(i)->angleLimits.max );

      // set new values
      setJointAngle( i, newAngle );
      mJointAngles[i] = newAngle;
      mJointVelocities[i] = velocity;

      break;

    default:
      cerr << "Oh oh, something went terribly wrong here!" << endl;
    }

  }

  return;
}
