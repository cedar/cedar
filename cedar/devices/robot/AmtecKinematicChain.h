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

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.ruhr-uni-bochum.de
    Date:        2011 01 18

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_AMTEC_KINEMATIC_CHAIN_H
#define CEDAR_DEV_ROBOT_AMTEC_KINEMATIC_CHAIN_H

// MAKE AMTEC OPTIONAL
#include "CMakeDefines.h"
#ifdef CEDAR_USE_AMTEC

// LOCAL INCLUDES
#include "namespace.h"
#include "ReferenceGeometry.h"
#include "KinematicChain.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include "AmtecDeviceDriver/Device/Device.h"
#include <QMutex>


/*!@brief KinematicChain implementation for Amtec modules
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

  /*!@brief returns the max. acceleration of a joint
   *
   * @param index
   */
  float getMaxAcceleration(unsigned int index);

  /*!@brief sets the max. acceleration of a joint
   *
   * Unfortunately, the Amtec modules seem to ignore this value while in
   * velocity mode.
   *
   * @param index
   * @param maxAcc
   */
void setMaxAcceleration(unsigned int index, float maxAcc);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  bool initDevice();
  bool calibrateModule(unsigned int module);
  double getJointAngle(unsigned int index);
  double getJointVelocity(unsigned int index);
  bool isCalibrated(unsigned int module);
  void readParamsFromConfigFile();
  void setJointAngle(unsigned int index, double value);
  bool setJointVelocity(unsigned int index, double velocity);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  CDevice *mpDevice;
  std::string mInitString;
  int mInit;
  std::vector<int> mModules;
  QMutex mCanBusMutex;

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

#endif // CEDAR_USE_AMTEC
#endif // CEDAR_DEV_ROBOT_AMTEC_KINEMATIC_CHAIN_H
