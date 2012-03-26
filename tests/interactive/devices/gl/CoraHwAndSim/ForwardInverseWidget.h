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

    File:        ForwardInverseWidget.h

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.ruhr-uni-bochum.de
    Date:        2011 04 28

    Description: Widget to control CoRA via forward and reverse kinematic.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_FORWARDINVERSEWIDGET_H
#define CEDAR_FORWARDINVERSEWIDGET_H


// LOCAL INCLUDES

#include "iiClosedFormInverseKinematics.hpp"

// PROJECT INCLUDES

#include "cedar/devices/robot/KinematicChain.h"

// SYSTEM INCLUDES

#include <QTimer>
#include <QGridLayout>
#include <QWidget>


/*!@brief Widget to control CoRA via forward and reverse kinematic.
 */
class ForwardInverseWidget : public QWidget
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  Q_OBJECT;

public:

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------

  /*!@brief Constructor
   *
   *@param kinematicChain the kinematic chain to control
   *@param kinematicChainModel the model that calculates the forward kinematics
   *@param parent QWidget parent
   *@param f QWidget window flags
   */
  ForwardInverseWidget
  (
    const cedar::dev::robot::KinematicChainPtr kinematicChain,
    QWidget *parent = 0, Qt::WindowFlags f = 0
  );

  /*!@brief Constructor
   *
   *@param kinematicChains vector of kinematic chains to control
   *@param kinematicChainModel the model that calculates the forward kinematics
   *@param parent QWidget parent
   *@param f QWidget window flags
   */
  ForwardInverseWidget
  (
    const std::vector<cedar::dev::robot::KinematicChainPtr> &kinematicChains,
    QWidget *parent = 0, Qt::WindowFlags f = 0
  );

  //!@brief Destructor
  ~ForwardInverseWidget();

//--------------------------------------------------------------------------------------------------------------------
// public methods
//--------------------------------------------------------------------------------------------------------------------

public:

//--------------------------------------------------------------------------------------------------------------------
// protected methods
//--------------------------------------------------------------------------------------------------------------------

protected:

  // none yet

//--------------------------------------------------------------------------------------------------------------------
// private methods
//--------------------------------------------------------------------------------------------------------------------

private:

  void initWindow();
  void setActiveColumn(unsigned int c);

private slots:

  void radioButtonForwardClicked();
  void radioButtonInverseClicked();
  void updateJointValue();
  void updateSpinBoxes();


//--------------------------------------------------------------------------------------------------------------------
// members
//--------------------------------------------------------------------------------------------------------------------

public:

  // none yet (hopefully never!)

protected:

  // none yet

private:

  static const int mUpdateInterval = 100;

  std::vector<cedar::dev::robot::KinematicChainPtr> mpKinematicChains;
  ClosedFormInverseKinematics *mpClosedFormInverseKinematics;
  QGridLayout *mpGridLayout;
  QTimer *mpTimer;
  int mDecimals;
  double mSingleStep;


//--------------------------------------------------------------------------------------------------------------------
// parameters
//--------------------------------------------------------------------------------------------------------------------

public:

  // none yet (hopefully never!)

protected:

  // none yet

private:

  // none yet

}; // class ForwardInverseWidget


#endif /* CEDAR_FORWARDINVERSEWIDGET_H */
