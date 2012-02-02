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
#include <QtGui/QGridLayout>
#include <QtGui/QWidget>

//!@brief A simple widget to monitor the state of a kinematic chain
//!@todo I removed the CEDAR_DEV_LIB_EXPORT here, check if this still runs on Windows.
//class CEDAR_DEV_LIB_EXPORT cedar::dev::robot::gui::KinematicChainMonitorWidget
class cedar::dev::robot::gui::KinematicChainMonitorWidget
:
public QWidget,
public cedar::aux::ConfigurationInterface
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
  /*!@brief Constructor for a single kinematic chain, without configuration
   *
   *@param kinematicChain pointer to a kinematic chain
   *@param parent parent parameter of QWidget
   *@param f WindowFlags for QWidget
   */
  KinematicChainMonitorWidget(const cedar::dev::robot::KinematicChainPtr &kinematicChain, QWidget *parent = 0, Qt::WindowFlags f = 0);

  /*!@brief Constructor for a single kinematic chain, with configuration
   *
   *@param kinematicChain pointer to a kinematic chain
   *@param configFileName path of a configuration file
   *@param parent parent parameter of QWidget
   *@param f WindowFlags for QWidget
   */
  KinematicChainMonitorWidget(const cedar::dev::robot::KinematicChainPtr &kinematicChain, const std::string& configFileName, QWidget *parent = 0, Qt::WindowFlags f = 0);

  /*!@brief Constructor taking a vector of kinematic chains, without configuration
   *
   * If a vector of kinematic chains is given, the widgetreads the values from
   * the first kinematic chain.
   *
   *@param kinematicChains vector of kinematic chain interfaces
   *@param parent parent parameter of QWidget
   *@param f WindowFlags for QWidget
   */
  KinematicChainMonitorWidget(const std::vector<cedar::dev::robot::KinematicChainPtr> &kinematicChains, QWidget *parent = 0, Qt::WindowFlags f = 0);

  /*!@brief Constructor taking a vector of kinematic chains, with configuration
   *
   * If a vector of kinematic chains is given, the widgetreads the values from
   * the first kinematic chain.
   *
   *@param kinematicChains vector of kinematic chain interfaces
   *@param configFileName path of a configuration file
   *@param parent parent parameter of QWidget
   *@param f WindowFlags for QWidget
   */
  KinematicChainMonitorWidget(const std::vector<cedar::dev::robot::KinematicChainPtr> &kinematicChains, const std::string& configFileName, QWidget *parent = 0, Qt::WindowFlags f = 0);

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
// TODO: add init function
private slots:

  void update();

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
protected:
  // none yet

private:
  static const int mUpdateInterval = 100;
  std::vector<cedar::dev::robot::KinematicChainPtr> mpKinematicChains;
  QGridLayout *mpGridLayout;
  int mDecimals;
};

#endif /* CEDAR_DEV_ROBOT_GUI_KINEMATIC_CHAIN_MONITOR_WIDGET_H */
