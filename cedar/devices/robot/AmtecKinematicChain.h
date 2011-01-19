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

    File:        AmtecKinematicChain.h

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.ruhr-uni-bochum.de
    Date:        2011 01 18

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_AMTEC_KINEMATIC_CHAIN_H
#define CEDAR_DEV_ROBOT_AMTEC_KINEMATIC_CHAIN_H

// LOCAL INCLUDES
#include "namespace.h"
#include "ReferenceGeometry.h"
#include "KinematicChain.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include "AmtecDeviceDriver/Device/Device.h"


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::dev::robot::AmtecKinematicChain : public cedar::dev::robot::KinematicChain
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  AmtecKinematicChain(const cedar::dev::robot::ReferenceGeometryPtr& rpReferenceGeometry);
  //!@brief constructor
  AmtecKinematicChain(const std::string& configFileName);

  //!@brief Destructor
  ~AmtecKinematicChain();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  bool init();
  bool calibrateModule(unsigned int module);
  double getJointAngle(unsigned int) const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  CDevice *mpDevice;
  std::vector<int> mModules;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  // none yet

}; // class cedar::dev::robot::AmtecKinematicChain

#endif // CEDAR_DEV_ROBOT_AMTEC_KINEMATIC_CHAIN_H

