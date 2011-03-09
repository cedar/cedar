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

    File:        KukaCommunicator.h

    Maintainer:  Guido Knips
    Email:       guido.knips@ini.rub.de
    Date:        2011 01 28

    Description: loop-threaded communication class for steady communication with the kuka LBR

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_KUKA_KUKA_COMMUNICATOR_H
#define CEDAR_DEV_ROBOT_KUKA_KUKA_COMMUNICATOR_H

// LOCAL INCLUDES
#include "namespace.h"

// PROJECT INCLUDES
#include "auxiliaries/LoopedThread.h"

// SYSTEM INCLUDES
#include <fri/friremote.h>
#include <QReadWriteLock>
#include <vector>
#include <string>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::dev::robot::kuka::KukaCommunicator : public cedar::aux::LoopedThread
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The constructor.
   *
   * @param configFileName Name of the coniguration file tha should be used
   * In the configuration file, the parameter names are the ones for loopedThread as well as "ServerPort" (the port on the LBR
   * side, 0 means default port) and "RemoteHost" (the remote host to contact to, "NULL" means auto detection)
   */
  KukaCommunicator(const std::string& configFileName);


  //!@brief Destructor
  virtual ~KukaCommunicator();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief get the measured angle of a specific joint
   *
   * @param index index to specify the joint
   * @return angle of the specified joint in rad
   */
  double getJointAngle(unsigned index);
  /*!@brief get a vector of all joint angles
   *
   * @return std::vector with all measured joint angles in rad
   */
  const std::vector<double> getJointAngles();
  /*!@brief set a single joint position
   *
   * @param index index to specify the joint
   * @param value value for the new joint angle in rad
   */
  void setJointAngle(unsigned index, double value);
  /*!@brief set all joint angles
   *
   * @param values std::vector with the new angles in rad
   */
  void setJointAngles(const std::vector<double>& values);

  /*Wrapping of some FRI-Functions that are needed for ensuring connection quality*/

  /*! @brief returns the state of the Interface.

   * this can be FRI_STATE_OFF, FRI_STATE_MON and FRI_STATE_CMD
   * Commands can only be send if the state is FRI_STATE_CMD, which represents the command mode
   * @return current state of the interface
   */
  FRI_STATE getFriState();
  /*! @brief returns the quality of the connection.
   * this can range from FRI_QUALITY_UNACCEPTABLE to FRI_QUALITY_PERFECT
   * if the Quality is worse (means: less) than FRI_QUALITY_GOOD, command mode switches to monitor mode automatically
   * @return current Quality of the connection
   */
  FRI_QUALITY getFriQuality();
  /*! @brief does Data exchange with the KUKA-LBR
   * normally not necessary, the functions setJointAngle() and setJointAngles() do this by themselves.
   * the get-Functions don't, though
   */
  float getSampleTime();
  /*! @brief check if the robot is powered
   *
   * This method does not call doDataExchange itself
   * this especially means the dead man switch is in the right position and the robot is in command mode
   * @return true, if power is on
   */
  bool isPowerOn();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief every step is used to do communication between FRI and KUKA-RC
  void step(double time);
  //!@brief is called by every constructor
  void init();
  //!@brief copies data from the FRI to member variables for acces from outside the loop thread
  void copyFromFRI();
  //!@brief copies data to the FRI member variables
  void copyToFRI();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  //true, if the object has ben initialized
  bool mIsInit;
  //KUKA Vendor-Interface, wrapped by this class
  friRemote *mpFriRemote;
  //locker for read/write protection
  QReadWriteLock mLock;
  //last commanded joint position
  std::vector<double> mCommandedJointPosition;
  //last measured joint Position
  std::vector<double> mMeasuredJointPosition;
  //Copy of the FRI state
  FRI_STATE mFriState;
  //Copy of the current FRI quality
  FRI_QUALITY mFriQuality;
  //Sample Time of the FRI connection
  float mSampleTime;
  //last known status if power is on on the Kuka RC
  bool mPowerOn;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet
protected:
  // none yet
private:
  //!IP Address of the remote host
  std::string _mRemoteHost;
  //!local server port
  int _mServerPort;

}; // class cedar::dev::robot::kuka::KukaCommunicator

#endif // CEDAR_DEV_ROBOT_KUKA_KUKA_COMMUNICATOR_H

