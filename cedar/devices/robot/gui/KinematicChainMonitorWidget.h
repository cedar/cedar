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

    File:        KinematicChainMonitorWidget.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 02 01

    Description: Header for the @em cedar::dev::robot::KinematicChainMonitorWidget class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_GUI_KINEMATIC_CHAIN_MONITOR_WIDGET_H
#define CEDAR_DEV_ROBOT_GUI_KINEMATIC_CHAIN_MONITOR_WIDGET_H

// CEDAR INCLUDES
#include "cedar/devices/lib.h"
#include "cedar/devices/robot/gui/namespace.h"
#include "cedar/devices/robot/KinematicChain.h"
#include "cedar/auxiliaries/ConfigurationInterface.h"

// SYSTEM INCLUDES
#include <QtCore/QTimer>
#include <QGridLayout>
#include <QWidget>

//!@brief A simple widget to monitor the state of a kinematic chain
class cedar::dev::robot::gui::KinematicChainMonitorWidget
:
public QWidget
{
  //----------------------------------------------------------------------------
  // macros
  //----------------------------------------------------------------------------
  Q_OBJECT

  //----------------------------------------------------------------------------
  // constructors and destructor
  //----------------------------------------------------------------------------

public:
//!@todo please check if we really need four constructors in this class
  /*!@brief Constructor for a single kinematic chain
   *
   *@param kinematicChain pointer to a kinematic chain
   *@param parent parent parameter of QWidget
   *@param f WindowFlags for QWidget
   */
  KinematicChainMonitorWidget
  (
    cedar::dev::robot::KinematicChainPtr kinematicChain,
    QWidget *parent = 0,
    Qt::WindowFlags f = 0
  );

  ~KinematicChainMonitorWidget();

  //--------------------------------------------------------------------------------------------------------------------
  // Qt events
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief implementation of handling routine for Qt timer events
   *
   * @param    event
   */
  void timerEvent(QTimerEvent* event);

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

public:
  /*!@brief sets the number of decimals used to display the values
   * @param decimals number of decimals
   */
  void setDecimals(unsigned int decimals);

  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------

protected:

  // none yet

  //----------------------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------------------

private:

  void initWindow();

private slots:

  void update();

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
protected:
  // none yet

private:
  static const int mUpdateInterval = 100;
  cedar::dev::robot::KinematicChainPtr mpKinematicChain;
  QGridLayout *mpGridLayout;
  unsigned int mDecimals;
};

#endif /* CEDAR_DEV_ROBOT_GUI_KINEMATIC_CHAIN_MONITOR_WIDGET_H */
