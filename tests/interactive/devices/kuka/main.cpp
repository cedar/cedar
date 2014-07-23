/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Maintainer:  Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2012 1 30

 ----- Description: Testing of the interface for the KUKA LBR

 ----- Credits:
 -----------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/devices/kuka/gui/FriStatusWidget.h"
#include "cedar/devices/gui/KinematicChainWidget.h"
#include "cedar/devices/KinematicChain.h"
#include "cedar/devices/gl/KinematicChain.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/systemFunctions.h"

// SYSTEM INCLUDES
#include <vector>
#include <QApplication>
#include <iostream>

using cedar::dev::kuka::gui::FriStatusWidget;

int main(int argc, char **argv)
{
  std::string mode = "0";
  std::string configuration_file = cedar::aux::locateResource("configs/kuka_lwr4.json");
  QApplication a(argc, argv);
  FriStatusWidget* p_fri_status_widget = 0;
  cedar::dev::gui::KinematicChainWidget* p_kinematic_chain_widget = 0;

  // create the hardware interface
  cedar::dev::kuka::KinematicChainPtr p_arm(new cedar::dev::kuka::KinematicChain());
  p_arm->readJson(configuration_file);
  p_fri_status_widget = new FriStatusWidget(p_arm);
  p_fri_status_widget->startTimer(100);
  p_fri_status_widget->show();
  p_kinematic_chain_widget = new cedar::dev::gui::KinematicChainWidget(p_arm);
  p_kinematic_chain_widget->show();

  a.exec();

  delete p_fri_status_widget;
  delete p_kinematic_chain_widget;

  return 0;
}
