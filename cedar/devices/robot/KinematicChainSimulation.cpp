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
 
 ----- Author:      Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 06
 
 ----- Description: implementation for the \em cedar::dev::robot::KinematicChainSimulation class
 
 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "KinematicChainSimulation.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::dev::robot;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//KinematicChainSimulation::KinematicChainSimulation()
//:
//mKinematicChainModel(), //TODO: use reference geometry constructor here
//mKinematicChainVisualization(&mKinematicChainModel),
//mScene(),
//mViewer(&mScene)
//{
//  std::cout << "running KinematicChainSimulation()" << endl;
//  init();
//}

KinematicChainSimulation::KinematicChainSimulation(const cedar::dev::robot::ReferenceGeometryPtr& rpReferenceGeometry)
:
KinematicChain(rpReferenceGeometry)
{
  init();
}

KinematicChainSimulation::~KinematicChainSimulation()
{
  
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

const double KinematicChainSimulation::getJointAngle(const unsigned int index) const
{
  return mJointAngles.at<double>(index, 0);
}

const std::vector<double> KinematicChainSimulation::getJointAngles(void) const
{
  vector<double> angles;
  angles.resize(mNumberOfJoints);
  for ( unsigned int j=0; j<mNumberOfJoints; j++)
  {
    angles[j] = mJointAngles.at<double>(j, 0);
  }
  return angles;
}

const cv::Mat KinematicChainSimulation::getJointAnglesMatrix(void) const
{
  // TODO: data should be locked, I guess - check what how to do that in cedar
  Mat angles;
  angles = mJointAngles.clone();
  return angles;
}

void KinematicChainSimulation::setJointAngle(const unsigned int index, const double angle)
{
  mJointAngles.at<double>(index, 0) = angle;
}

void KinematicChainSimulation::setJointAngles(const cv::Mat& angleMatrix)
{
  // TODO: assert that the passed matrix has the right size
  mJointAngles = angleMatrix;
}

void KinematicChainSimulation::setJointAngles(const std::vector<double>& angles)
{
  // TODO: assert that the passed vector has the right size
  for ( unsigned int j=0; j<mNumberOfJoints; j++)
  {
    mJointAngles.at<double>(j, 0) = angles[j];
  }
}

void KinematicChainSimulation::init(void)
{
  mJointAngles = Mat::zeros(mNumberOfJoints, 1, CV_64FC1);
}
