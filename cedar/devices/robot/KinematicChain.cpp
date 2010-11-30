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
#include "ReferenceGeometry.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::dev::robot;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
KinematicChain::KinematicChain(const cedar::dev::robot::ReferenceGeometryPtr& rpReferenceGeometry)
:
mpReferenceGeometry(rpReferenceGeometry)
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
  return mpReferenceGeometry->numberOfJoints();
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
