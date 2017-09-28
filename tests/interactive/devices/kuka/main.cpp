/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/devices/Robot.h"
#include "cedar/devices/kuka/gui/FriStatusWidget.h"
#include "cedar/devices/kuka/FRIChannel.h"
#include "cedar/devices/kuka/KinematicChain.h"
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

  auto robot = boost::make_shared< cedar::dev::Robot >();
  robot->readJson("resource://robots/caren/fri_configuration.json");

  QApplication a(argc, argv);

  FriStatusWidget* p_fri_status_widget;
  cedar::dev::gui::KinematicChainWidget* p_kinematic_chain_widget;

  // create the hardware interface
  auto p_arm = robot->getComponent< cedar::dev::kuka::KinematicChain >("arm");

  cedar::dev::kuka::FRIChannelPtr fri_channel
    = boost::static_pointer_cast<cedar::dev::kuka::FRIChannel>(p_arm->getChannel());
  p_fri_status_widget = new FriStatusWidget(fri_channel);
  p_fri_status_widget->startTimer(100);
  p_fri_status_widget->show();
  p_kinematic_chain_widget = new cedar::dev::gui::KinematicChainWidget(p_arm);
  p_kinematic_chain_widget->show();

  p_arm->startCommunication();

  a.exec();

  delete p_fri_status_widget;
  delete p_kinematic_chain_widget;

  return 0;
}
