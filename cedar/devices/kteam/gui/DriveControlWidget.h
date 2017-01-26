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

    File:        DriveControlWidget.h

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: Graphical User Interface for controlling the KTeam robots.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KTEAM_GUI_DRIVE_CONTROL_WIDGET_H
#define CEDAR_DEV_KTEAM_GUI_DRIVE_CONTROL_WIDGET_H

// CEDAR INCLUDES
#include "cedar/devices/kteam/namespace.h"
#include "cedar/devices/kteam/gui/ui_DriveControlWidget.h"
#include "cedar/devices/kteam/gui/namespace.h"
#include "cedar/auxiliaries/gui/BaseWidget.h"

// SYSTEM INCLUDES
#include <Qt>

/*!@brief Graphical User Interface for controlling the E-Puck.
 *
 * Type the desired forward velocity and turning rate into the boxes. The wheel speed of the 2 differential-drive-wheels
 * is displayed as well as the encoder-values. The E-Puck starts driving with the specified velocity or changes its
 * velocity after pressing the 'Set Velocity'-button. Stop the E-Puck with 'Stop' and reset speed and encoder-values
 * with 'Reset'.
 */
class cedar::dev::kteam::gui::DriveControlWidget
:
public cedar::aux::gui::BaseWidget, private Ui_DriveControlWidget
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
  //!@brief Constructs the drive control widget.
  //!@param drive Pointer to the drive that shall be controlled.
  //!@param parent Pointer to parent widget
  DriveControlWidget(cedar::dev::kteam::DrivePtr drive, QWidget *parent = 0);

  //!@brief Closes the control.
  virtual ~DriveControlWidget();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public slots:

  /*!@brief Sets the velocity of the robot.
   */
  void drive();

  /*!@brief Stops the robot.
   */
  void stop();

  /*!@brief Stops the robot and resets the encoder-values.
   */
  void reset();

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
   * @param event Pointer to event
   */
  void timerEvent(QTimerEvent *event);

  /*!@brief Updates the displayed wheel-speeds and encoder-values and is called by timerEvent.
   */
  void update();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief Pointer to the controlled robot.
  cedar::dev::kteam::DrivePtr mDrive;
}; // class cedar::dev::robot::kteam::gui::DriveControlWidget
#endif // CEDAR_DEV_KTEAM_GUI_DRIVE_CONTROL_WIDGET_H
