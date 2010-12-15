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

    File:        CoraArm.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 14

    Description: implementation for a class visualizing the arm of CoRA

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "CoraArm.h"
#include "devices/robot/gl/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::dev::robot;
using namespace cedar::dev::robot::gl;
//using namespace cedar::aux::gl;
//using namespace std;
//using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

gl::CoraArm::CoraArm(cedar::dev::robot::KinematicChainModelPtr& rpKinematicChainModel)
:
gl::CoraKinematicChain(rpKinematicChainModel)
{

}

gl::CoraArm::~CoraArm()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void gl::CoraArm::drawBase()
{
  KinematicChain::drawBase();
}

void gl::CoraArm::drawSegment(unsigned int index)
{
  KinematicChain::drawSegment(index);
}

void gl::CoraArm::drawEndEffector()
{
  KinematicChain::drawEndEffector();
}
