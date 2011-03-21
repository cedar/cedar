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

    File:        KTeamPositionControllerWidget.h

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: Graphical User Interface for the KTeam controller.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_MOBILE_GUI_KTEAM_POSITION_CONTROLLER_WIDGET_H
#define CEDAR_DEV_ROBOT_MOBILE_GUI_KTEAM_POSITION_CONTROLLER_WIDGET_H

// LOCAL INCLUDES

// PROJECT INCLUDES

#include "devices/robot/mobile/KTeamPositionController.h"
#include "devices/robot/mobile/MobileRobotModel.h"
#include "cedar/devices/robot/mobile/gui/ui_KTeamPositionControllerWidget.h"
#include "devices/robot/mobile/gui/namespace.h"
#include "auxiliaries/gui/BaseWidget.h"

// SYSTEM INCLUDES

#include <Qt>

/*!@brief Graphical User Interface for the KTeam controller.
 *
 * Type the desired position into the boxes. The current position of the robot is displayed in the relevant boxes.
 */
class cedar::dev::robot::mobile::gui::KTeamPositionControllerWidget
: public cedar::aux::gui::BaseWidget, private Ui_KTeamPositionControllerWidget
{

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

private:

  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------

public:

  //!@brief Constructs the GUI.
  //!@param peController Pointer to the controller used to control the robot.
  //!@param peModel Pointer to the model of the controlled robot.
  KTeamPositionControllerWidget(cedar::dev::robot::mobile::KTeamPositionController *peController,
                      cedar::dev::robot::mobile::MobileRobotModel *peModel,
                      QWidget *parent = 0);

  //!@brief Destructs the GUI.
  virtual ~KTeamPositionControllerWidget();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------

public slots:

  /*!@brief Sets the target-position of the robot.
   */
  void start();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------

protected:

  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------

private:

  /*!@brief The timer-event.
   */
  void timerEvent(QTimerEvent *event);

  /*!@brief Updates the displayed position.
   */
  void update();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------

public:

  // none yet (hopefully never!)

protected:

  // none yet

private:

  //!@brief Pointer to the robot control.
  KTeamPositionController *mpeController;

  //!@brief Pointer to the robot's model.
  MobileRobotModel *mpeModel;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------

public:

  // none yet (hopefully never!)

protected:

  // none yet

private:

  // none yet

}; // class cedar::dev::robot::mobile::gui::KTeamPositionControllerWidget

#endif // CEDAR_DEV_ROBOT_MOBILE_GUI_KTEAM_POSITION_CONTROLLER_WIDGET_H
