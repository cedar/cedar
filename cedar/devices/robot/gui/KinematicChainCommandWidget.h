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

    File:        KinematicChainCommandWidget.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 02 01

    Description: Header for the @em cedar::dev::robot::KinematicChainCommandWidget class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_GUI_KINEMATIC_CHAIN_COMMAND_WIDGET_H
#define CEDAR_DEV_ROBOT_GUI_KINEMATIC_CHAIN_COMMAND_WIDGET_H

// CEDAR INCLUDES
#include "cedar/devices/lib.h"
#include "cedar/devices/robot/gui/namespace.h"
#include "cedar/devices/robot/KinematicChain.h"
#include "cedar/auxiliaries/ConfigurationInterface.h"

// SYSTEM INCLUDES
#include <QCheckBox>
#include <QGridLayout>
#include <QComboBox>
#include <QWidget>
#include <QDoubleSpinBox>

//!@brief A simple widget to command single joints
class cedar::dev::robot::gui::KinematicChainCommandWidget
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
  /*!@brief Constructor for a single kinematic chain, without configuration
   *
   *@param kinematicChain pointer to a kinematic chain
   *@param parent parent parameter of QWidget
   *@param f WindowFlags for QWidget
   */
  KinematicChainCommandWidget
  (
    cedar::dev::robot::KinematicChainPtr kinematicChain,
    QWidget* parent = 0,
    Qt::WindowFlags f = 0
  );

  ~KinematicChainCommandWidget();

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
  /*!@brief sets the number of decimals used in the command boxes
   * @param decimals number of decimals
   */
  void setDecimals(unsigned int decimals);

  /*!@brief sets the size of a single step in the command boxes
   * @param singleStep size of the step
   */
  void setSingleStep(double singleStep);

public slots:
  //!@brief updates the window parameters with values from the kinematic chain
  void update();

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

  void changeWorkingMode(int index);
  void commandJoints();
  void stopMovement();
  void setKeepSendingState(int state);

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
protected:
  // none yet

private:
  static const int mUpdateInterval = 100;
  cedar::dev::robot::KinematicChainPtr mpKinematicChain;
  std::vector<QDoubleSpinBox*> mCommandBoxes;
  QGridLayout *mpGridLayout;
  QComboBox* mpModeBox;
  QCheckBox* mpKeepMovingBox;
  int mTimerId;
};

#endif /* CEDAR_DEV_ROBOT_GUI_KINEMATIC_CHAIN_COMMAND_WIDGET_H */
