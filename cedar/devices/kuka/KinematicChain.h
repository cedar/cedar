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

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean.stephane.jokeit@ini.ruhr-uni-bochum.de
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
#include "cedar/devices/KinematicChain.h"
#include "cedar/devices/Component.h"

// SYSTEM INCLUDES


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
  virtual bool isReadyForCommands() const override;
  virtual bool isReadyForMeasurements() const override;
  CEDAR_DECLARE_DEPRECATED( bool isMovable() const );


  void prepareSendingJointAngles(cv::Mat mat);
  void prepareSendingJointTorques(cv::Mat mat);
  cv::Mat prepareRetrievingJointAngles();
  cv::Mat prepareRetrievingJointTorques();
  cv::Mat prepareRetrievingExternalJointTorques();

  void prepareSendingNoop(const cedar::dev::Component::ComponentDataType& type);
  void prepareSendingNotReadyForCommand(const cedar::dev::Component::ComponentDataType& type);

  void exchangeData();
  void postStart();



  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:

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
