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

 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Maintainer:  Guido Knips
 ----- Email:       guido.knips@ini.rub.de
 ----- Date:        2011 1 11

 ----- Description: Testing of the interface for the KUKA LBR

 ----- Credits:
 -----------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "devices/robot/kuka/gui/FriStatusWidget.h"
#include "devices/robot/gui/KinematicChainWidget.h"

// SYSTEM INCLUDES
#include <vector>
#include <QApplication>
#include <iostream>

using namespace std;
using namespace cedar::dev::robot::kuka;
using cedar::dev::robot::kuka::gui::FriStatusWidget;
using cedar::dev::robot::KinematicChainPtr;

int main(int argc, char **argv)
{
  QApplication a(argc, argv);

  KukaInterfacePtr kukain(new KukaInterface("test_arm.conf", false));

  FriStatusWidget * p_fri_status_widget = new FriStatusWidget(kukain);
  p_fri_status_widget->startTimer(100);
  p_fri_status_widget->show();

  KinematicChainWidget * p_kinematic_chain_widget = new KinematicChainWidget(kukain);
  p_kinematic_chain_widget->show();

  a.exec();

  delete p_fri_status_widget;
  delete p_kinematic_chain_widget;

  return 0;
}
