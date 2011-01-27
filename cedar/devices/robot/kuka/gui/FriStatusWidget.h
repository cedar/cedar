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

    File:        FriStatusWidget.h

    Maintainer:  Guido Knips
    Email:       guido.knips@ini.rub.de
    Date:        2011 01 25

    Description: Widget for showing FRI information

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_KUKA_GUI_STATUS_WIDGET_H
#define CEDAR_DEV_ROBOT_KUKA_GUI_STATUS_WIDGET_H

// LOCAL INCLUDES
#include "namespace.h"

// PROJECT INCLUDES
#include "devices/robot/kuka/KukaInterface.h"
#include "cedar/devices/robot/kuka/gui/ui_FriStatusWidget.h"
#include "auxiliaries/gui/BaseWidget.h"

// SYSTEM INCLUDES
#include <Qt>
#include <QObject>

/*!@brief Widget that displays informations about the status of the KUKA-FRI
 *
 * This includes the status, the connection quality, the sample time and if the robot is powered
 */
class cedar::dev::robot::kuka::gui::FriStatusWidget : public cedar::aux::gui::BaseWidget,
                                                      private Ui_FriStatusWidget
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
  /*!@brief The constructor.

   * @param pKukaIn pointer to an instance of KukaInterface, where this widget gets the data
   */
  FriStatusWidget(cedar::dev::robot::kuka::KukaInterface *pKukaIn, QWidget *parent=0);

  //!@brief Destructor
  ~FriStatusWidget();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief refreshes the displayed data*/
  void timerEvent(QTimerEvent* pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief initializes the class
   */
  void init();
  /*!@brief updates the displayed information
   */
  void updateInformation();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  bool mIsInit; //!<true, if object has been initialized
  cedar::dev::robot::kuka::KukaInterface *mpKukaIn; //!<this is an external reference

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::robot::kuka::gui::FriStatusWidget

#endif // CEDAR_DEV_ROBOT_KUKA_GUI_STATUS_WIDGET_H

