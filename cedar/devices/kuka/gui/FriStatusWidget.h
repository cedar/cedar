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

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.ruhr-uni-bochum.de
    Date:        2010 11 23

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KUKA_GUI_STATUS_WIDGET_H
#define CEDAR_DEV_KUKA_GUI_STATUS_WIDGET_H

// MAKE FRI OPTIONAL
#include "cedar/devices/robot/CMakeDefines.h"
#ifdef CEDAR_USE_KUKA_LWR

// LOCAL INCLUDES
#include "namespace.h"

// PROJECT INCLUDES
#include "cedar/devices/kuka/KukaInterface.h"
#ifdef DEBUG
#include "cedar/devices/debug/kuka/gui/ui_FriStatusWidget.h"
#else
#include "cedar/devices/release/kuka/gui/ui_FriStatusWidget.h"
#endif
#include "cedar/auxiliaries/gui/BaseWidget.h"

// SYSTEM INCLUDES
#include <Qt>
#include <QObject>

/*!@brief Widget that displays informations about the status of the KUKA-FRI
 *
 * This includes the status, the connection quality, the sample time and if the robot is powered
 */
class cedar::dev::kuka::gui::FriStatusWidget : public cedar::aux::gui::BaseWidget,
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
   *
   * @param pKukaIn pointer to an instance of KukaInterface, where this widget gets the data
   * @param parent parent widget
   */
  FriStatusWidget(cedar::dev::kuka::KukaInterfacePtr &pKukaIn, QWidget *parent=0);

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
  cedar::dev::kuka::KukaInterfacePtr mpKukaIn; //!<this is an external reference

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::kuka::gui::FriStatusWidget

#endif // CEDAR_USE_KUKA_FRI
#endif // CEDAR_DEV_KUKA_GUI_STATUS_WIDGET_H

