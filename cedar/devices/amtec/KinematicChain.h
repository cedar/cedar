/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        KinematicChain.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.ruhr-uni-bochum.de
    Date:        2011 01 18

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_AMTEC_KINEMATIC_CHAIN_H
#define CEDAR_DEV_AMTEC_KINEMATIC_CHAIN_H

// MAKE AMTEC OPTIONAL
#include "cedar/configuration.h"
#ifdef CEDAR_USE_AMTEC

// CEDAR INCLUDES
#include "cedar/auxiliaries/IntVectorParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/devices/amtec/namespace.h"
#include "cedar/devices/KinematicChain.h"

// SYSTEM INCLUDES

// workaround for amtec drivers needing __LINUX__ define
#ifdef CEDAR_OS_LINUX
  #ifndef __LINUX__
    #define CEDAR_DEV_AMTEC_KINEMATIC_CHAIN_DID__LINUX__DEFINE
    #define __LINUX__
  #endif // __LINUX__
#endif // CEDAR_OS_LINUX

#include "AmtecDeviceDriver/Device/Device.h"

#if defined(__LINUX__) and defined(CEDAR_DEV_AMTEC_KINEMATIC_CHAIN_DID__LINUX__DEFINE)
  #undef __LINUX__
  #undef CEDAR_DEV_AMTEC_KINEMATIC_CHAIN_DID__LINUX__DEFINE
#endif // __LINUX__

#include <QMutex>

/*!@brief KinematicChain implementation for Amtec modules
 */
class cedar::dev::amtec::KinematicChain : public cedar::dev::KinematicChain
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  KinematicChain();

  //!@brief Destructor
  ~KinematicChain();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief check whether the kinematic chain is currently responsive to movement commands
   *
   * @return    state
   */
  bool isMovable() const;

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

  /*!@brief set a joint angle, which should be reached in a given time
   * @param index index of the joint
   * @param value new joint angle
   * @param stepTime time to reach the new joint angle
   * */
  void setJointAngle(unsigned int index, double value, double stepTime);

  /*!brief initialize the amtec device
   * @return flag set to true if initialized successfully
   */
  bool initDevice();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  bool calibrateModule(unsigned int module);
  double getJointAngle(unsigned int index) const;
  double getJointVelocity(unsigned int index) const;
  bool isCalibrated(unsigned int module);
  void readParamsFromConfigFile();
  void setJointAngle(unsigned int index, double value);
  bool setJointVelocity(unsigned int index, double velocity);

  //! Returns the amtec initialization string.
  inline const std::string& getInitString()
  {
    return this->_mInitString->getValue();
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  CDevice* mpDevice;
  int mInit;
  mutable QMutex mCanBusMutex;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! The amtec initialization string.
  cedar::aux::StringParameterPtr _mInitString;
  //! The map of module identifiers
  cedar::aux::IntVectorParameterPtr _mModuleMap;

}; // class cedar::dev::amtec::KinematicChain
#endif // CEDAR_USE_AMTEC
#endif // CEDAR_DEV_ROBOT_AMTEC_KINEMATIC_CHAIN_H
