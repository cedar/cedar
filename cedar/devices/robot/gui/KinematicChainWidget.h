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

    File:        KinematicChainWidget.h

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.rub.de
    Date:        2011 01 06

    Description: Header for the @em cedar::dev::robot::KinematicChainWidget class.

    Credits:

======================================================================================================================*/


#ifndef KINEMATICCHAINWIDGET_H_
#define KINEMATICCHAINWIDGET_H_

// LOCAL INCLUDES
#include "cedar/devices/lib.h"
#include "cedar/devices/robot/gui/namespace.h"
#include "cedar/devices/robot/KinematicChain.h"
#include "cedar/auxiliaries/ConfigurationInterface.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QtCore/QTimer>
#include <QtGui/QGridLayout>
#include <QtGui/QWidget>

//!@brief A simple widget to access all the joints via GUI
//!@todo I removed the CEDAR_DEV_LIB_EXPORT here, check if this still runs on Windows.
//class CEDAR_DEV_LIB_EXPORT cedar::dev::robot::gui::KinematicChainWidget
class cedar::dev::robot::gui::KinematicChainWidget
:
public QWidget,
public cedar::aux::ConfigurationInterface
{
  //----------------------------------------------------------------------------
  // macros
  //----------------------------------------------------------------------------

  Q_OBJECT;

  //----------------------------------------------------------------------------
  // constructors and destructor
  //----------------------------------------------------------------------------

public:
//!@todo please check if we really need four constructors in this class
  /*!@brief Constructor
   *
   *@param kinematicChain pointer to a kinematic chain
   *@param parent parent parameter of QWidget
   *@param f WindowFlags for QWidget
   */
  KinematicChainWidget(const cedar::dev::robot::KinematicChainPtr &kinematicChain, QWidget *parent = 0, Qt::WindowFlags f = 0);

  /*!@brief Constructor
   *
   *@param kinematicChain pointer to a kinematic chain
   *@param configFileName path of a configuration file
   *@param parent parent parameter of QWidget
   *@param f WindowFlags for QWidget
   */
  KinematicChainWidget(const cedar::dev::robot::KinematicChainPtr &kinematicChain, const std::string& configFileName, QWidget *parent = 0, Qt::WindowFlags f = 0);

  /*!@brief Constructor taking a vector of kinematic chains
   *
   * If a vector of kinematic chains is given, the widgets writes the same
   * values to all of them. It reads the values from the first kinematic chain.
   */
  KinematicChainWidget(const std::vector<cedar::dev::robot::KinematicChainPtr> &kinematicChains, QWidget *parent = 0, Qt::WindowFlags f = 0);

  /*!@brief Constructor taking a vector of kinematic chains
   *
   * If a vector of kinematic chains is given, the widgets writes the same
   * values to all of them. It reads the values from the first kinematic chain.
   */
  KinematicChainWidget(const std::vector<cedar::dev::robot::KinematicChainPtr> &kinematicChains, const std::string& configFileName, QWidget *parent = 0, Qt::WindowFlags f = 0);

  ~KinematicChainWidget();

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
  void setActiveColumn(unsigned int c);

private slots:

  void radioButtonAngleClicked();
  void radioButtonVelocityClicked();
  void radioButtonAccelerationClicked();
  void updateJointValue();
  void updateSpinBoxes();

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------

public:

  // none yet (hopefully never!)

protected:

  // none yet

private:

  static const int mUpdateInterval = 100;

  std::vector<cedar::dev::robot::KinematicChainPtr> mpKinematicChains;
  QGridLayout *mpGridLayout;
  QTimer *mpTimer;
  int mDecimals;
  double mSingleStep;

  //----------------------------------------------------------------------------
  // parameters
  //----------------------------------------------------------------------------

public:

  // none yet

protected:

  // none yet

private:

  // none yet

};

#endif /* KINEMATICCHAINWIDGET_H_ */
