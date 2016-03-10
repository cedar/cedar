/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
    Date:        2010 11 23

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KUKA_KINEMATIC_CHAIN_H
#define CEDAR_DEV_KUKA_KINEMATIC_CHAIN_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_KUKA_LWR

// CEDAR INCLUDES
#include "cedar/devices/kuka/namespace.h"
#include "cedar/devices/KinematicChain.h"
#include "cedar/auxiliaries/IntParameter.h"
#include "cedar/auxiliaries/StringParameter.h"

// SYSTEM INCLUDES
#include <fri/friremote.h>
#include <QReadWriteLock>
#include <vector>


/*!@brief kinematic chain interface for the KUKA LBR
 *
 * This class wraps the KUKA Fast Research Interface (FRI)
 */
class cedar::dev::kuka::KinematicChain :  public cedar::dev::KinematicChain
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Standard constructor
   */
  KinematicChain();

  /*!the destructor */
  virtual ~KinematicChain();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief check whether the kinematic chain is currently responsive to movement commands
   *
   * @return    state
   */
  bool isMovable() const;

  /*! @brief returns angle for a specified joint
   *
   *  @param index  index of the joint, since the KUKA LBR has seven of them, it must be in the interval [0,6]
   *  @return joint angle for the given index
   */
  virtual double getJointAngle(unsigned int index) const;

  /*! @brief returns all joint angles
   *
   *  @return a vector filled with the joint angles
   *  \throws std::out_of_range if index is out of range
   */
  virtual void setJointAngle(unsigned int index, double angle);




  void sendSimulatedAngles(cv::Mat mat);
  cv::Mat retrieveSimulatedAngles();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief This method initializes the object.
   *
   * This method is called from all constructors of the class.
   * @param commandMode establish command mode if true
   */
  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  //!@brief copies data from the FRI to member variables for access from outside the loop thread
  void copyFromFRI();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief true, if the object has been initialized
  bool mIsInit;
  //!@brief locker for read/write protection
  mutable QReadWriteLock mLock;
  //!@brief last commanded joint position
  std::vector<double> mCommandedJointPosition;
  //!@brief last measured joint Position
  std::vector<double> mMeasuredJointPosition;
  //!@brief the FRI channel
  cedar::dev::kuka::FRIChannelPtr mFRIChannel;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet
};
#endif // CEDAR_USE_KUKA_FRI
#endif /* CEDAR_DEV_KUKA_KINEMATIC_CHAIN_H */
