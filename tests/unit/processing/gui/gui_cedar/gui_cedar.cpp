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

    File:        gui_ProcessingIde.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 12

    Description: 

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/Ide.h"
#include "cedar/processing/gui/View.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/LoopedTrigger.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <vector>
#include <string>
#include <iostream>

void simulateEventLoop()
{
  std::cout << "   Entering event loop ";
  unsigned int i = 0;
  while (QApplication::hasPendingEvents() && ++i < 1000)
  {
    std::cout << ".";
    QApplication::processEvents();
  }
  std::cout << " done." << std::endl;
}

int main(int argc, char** argv)
{
  // needs to be created because we deal with widgets here
  QApplication app(argc, argv);

  // the number of errors encountered in this test
  int errors = 0;

  cedar::proc::gui::Ide* p_ide = new cedar::proc::gui::Ide(false, false, true);
  p_ide->suppressCloseDialog(true);
  {
    simulateEventLoop();

    cedar::proc::gui::View *p_view = p_ide->getArchitectureView();
    cedar::proc::gui::Scene *p_scene = p_view->getScene();
    cedar::proc::gui::GroupPtr p_gui_network = p_scene->getRootGroup();
    cedar::proc::GroupPtr network = p_gui_network->getGroup();

    // first, add some items to the scene
    cedar::proc::ElementPtr field_element = p_scene->createElement(network, "cedar.dynamics.NeuralField", QPointF(180, 240));
    simulateEventLoop();

    cedar::proc::ElementPtr trigger_element = p_scene->createElement(network, "cedar.processing.LoopedTrigger", QPointF(180, 240));
    simulateEventLoop();

    cedar::proc::LoopedTriggerPtr looped_trigger
      = boost::dynamic_pointer_cast<cedar::proc::LoopedTrigger>(trigger_element);
    cedar::proc::TriggerablePtr field_triggerable
      = boost::dynamic_pointer_cast<cedar::proc::Triggerable>(field_element);
    network->connectTrigger(looped_trigger, field_triggerable);
    simulateEventLoop();


    std::cout << "Selecting field" << std::endl;
    cedar::proc::gui::StepItem* p_field_item
      = p_scene->getStepItemFor(dynamic_cast<cedar::proc::Step*>(field_element.get()));
    p_field_item->setSelected(true);
    simulateEventLoop();
    CEDAR_ASSERT(p_scene->selectedItems().size() > 0);
    CEDAR_ASSERT(p_scene->selectedItems().contains(p_field_item));

    std::cout << "Starting trigger ..." << std::endl;
    looped_trigger->start();
    simulateEventLoop();
  }

  std::cout << "Closing ide." << std::endl;
  p_ide->close();
  simulateEventLoop();

  std::cout << "Deleting ide." << std::endl;
  delete p_ide;
  simulateEventLoop();


  std::cout << "Done. There were " << errors << " errors." << std::endl;
  return errors;
}
