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

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        main.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 24

    Description:

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/processing/gui/IdeApplication.h"
#include "cedar/processing/gui/Ide.h"
#include "cedar/processing/gui/View.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/TriggerItem.h"
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/Trigger.h"
#include "cedar/processing/Step.h"

// SYSTEM INCLUDES
#include <QGraphicsSceneMouseEvent>
#include <iostream>


void simulateEventLoop()
{
  unsigned int i = 0;
  while (QApplication::hasPendingEvents() && ++i < 1000)
  {
    std::cout << "Simulating event ..." << std::endl;
    QApplication::processEvents();
  }
}


int testArchitecture1(cedar::proc::gui::Ide *pIde)
{
  int errors = 0;

  cedar::proc::gui::View *p_view = pIde->getArchitectureView();
  cedar::proc::gui::Scene *p_scene = p_view->getScene();
  cedar::proc::gui::GroupPtr p_gui_network = p_scene->getRootGroup();
  cedar::proc::GroupPtr network = p_gui_network->getGroup();

  // first, add some items to the scene
  cedar::proc::ElementPtr field = p_scene->createElement(network, "cedar.dynamics.NeuralField", QPointF(180, 240));
  simulateEventLoop();
  /* cedar::proc::gui::StepItem* p_field_item = */p_scene->getStepItemFor(dynamic_cast<cedar::proc::Step*>(field.get()));

  cedar::proc::ElementPtr gauss = p_scene->createElement(network, "cedar.processing.sources.GaussInput", QPointF(-80, 240));
  simulateEventLoop();
  /*cedar::proc::gui::StepItem* p_gauss_item = */p_scene->getStepItemFor(dynamic_cast<cedar::proc::Step*>(gauss.get()));

  //!@todo Test connection creation here -- doesn't work so far.
  // create a connection
  /*
  cedar::proc::gui::DataSlotItem* p_gauss_slot
    = p_gauss_item->getSlotItem(cedar::proc::DataRole::OUTPUT, "Gauss input");
  cedar::proc::gui::DataSlotItem* p_field_slot
    = p_field_item->getSlotItem(cedar::proc::DataRole::INPUT, "input");

  if (!p_gauss_item->hasGuiConnection("Gauss input", p_field_item, "input"))
  {
    std::cout << "ERROR: no gui connection between gauss and field." << std::endl;
    ++errors;
  }
  */

  return errors;
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  unsigned int errors = 0;

  cedar::proc::gui::Ide *p_ide = new cedar::proc::gui::Ide(false, false, true);

  simulateEventLoop();

  std::cout << "Testing architecture creation." << std::endl;
  errors += testArchitecture1(p_ide);

  delete p_ide;

  // return
  std::cout << "Done, " << errors << " error(s) occurred." << std::endl;
  return errors;
}
