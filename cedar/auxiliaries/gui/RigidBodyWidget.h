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

    File:        RigidBodyWidget.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 02 01

    Description: Header for the @em cedar::aux::gui::RigidBodyWidget class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_RIGID_BODY_WIDGET_H
#define CEDAR_AUX_GUI_RIGID_BODY_WIDGET_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/RigidBody.h"
#include "cedar/auxiliaries/ConfigurationInterface.h"

// SYSTEM INCLUDES
#include <QGridLayout>
#include <QDoubleSpinBox>
#include <QWidget>

//!@brief A simple widget to monitor and change the state of a rigid body
//!@todo I removed the CEDAR_DEV_LIB_EXPORT here, check if this still runs on Windows.
//class CEDAR_DEV_LIB_EXPORT cedar::dev::robot::gui::RigidBodyWidget
class cedar::aux::gui::RigidBodyWidget
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
  /*!@brief Constructor without configuration
   *
   *@param rigidBody pointer to a RigidBody chain
   *@param parent parent parameter of QWidget
   */
  RigidBodyWidget(const cedar::aux::RigidBodyPtr rigidBody, QWidget* parent = 0);

  /*!@brief Constructor with configuration
   *
   *@param rigidBody pointer to a RigidBody
   *@param configFileName path of a configuration file
   *@param parent parent parameter of QWidget
   */
  RigidBodyWidget(const cedar::aux::RigidBodyPtr rigidBody, const std::string& configFileName, QWidget* parent = 0);

  ~RigidBodyWidget();

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

  /*!@brief set the pointer to the RigidBody
   *
   * @param    pRigidBody pointer to the new RigidBody
   */
  void setRigidBody(cedar::aux::RigidBodyPtr pRigidBody);

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

public slots:

  //!@brief read data from the rigid body and display them
  void update();

private slots:

  void rotateXPos();
  void rotateXNeg();
  void rotateYPos();
  void rotateYNeg();
  void rotateZPos();
  void rotateZNeg();
  void positionChanged(double);

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
protected:
  // none yet

private:
  static const int mUpdateInterval = 100;
  cedar::aux::RigidBodyPtr mpRigidBody;
  QGridLayout *mpGridLayout;
  QDoubleSpinBox *mpPositionXSpinBox;
  QDoubleSpinBox *mpPositionYSpinBox;
  QDoubleSpinBox *mpPositionZSpinBox;
  int mDecimals;
  int mRotationInterval;
  double mSinglePositionStep;
  double mSingleRotationStep;
  double mXMin;
  double mXMax;
  double mYMin;
  double mYMax;
  double mZMin;
  double mZMax;
};

#endif /* CEDAR_AUX_GUI_RIGID_BODY_WIDGET_H */
