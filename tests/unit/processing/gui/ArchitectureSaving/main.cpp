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
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/Trigger.h"
#include "cedar/processing/Step.h"

// SYSTEM INCLUDES
#include <QApplication>
#ifndef Q_MOC_RUN
  #include <boost/filesystem.hpp>
#endif
#include <iostream>

void simulateEventLoop()
{
  unsigned int i = 0;
  while (QApplication::hasPendingEvents() && ++i < 1000)
  {
    QApplication::processEvents();
  }
}

int testSaving(cedar::proc::gui::Ide *pIde)
{
  int errors = 0;

  cedar::proc::gui::View *p_view = pIde->getArchitectureView();
  cedar::proc::gui::Scene *p_scene = p_view->getScene();
  cedar::proc::gui::GroupPtr p_gui_network = p_scene->getRootGroup();
  cedar::proc::GroupPtr network = p_gui_network->getGroup();

  // first, add some items to the scene
  cedar::proc::ElementPtr field = p_scene->createElement(network, "cedar.dynamics.NeuralField", QPointF(180, 240));

  cedar::proc::gui::GraphicsBase* p_field_item = p_scene->getGraphicsItemFor(field.get());

  // Select the item, then deselect it again -- this should cover some potential bugs in the PropertyPane.
  // (specifically, this checks that the property pane properly disconnects its slots)
  p_field_item->setSelected(true);
  simulateEventLoop();
  CEDAR_DEBUG_ASSERT(p_field_item->isSelected());

  p_field_item->setSelected(false);
  simulateEventLoop();
  CEDAR_DEBUG_ASSERT(!p_field_item->isSelected());

  p_gui_network->writeJson("test.json");

  boost::filesystem::remove("test.json");

  return errors;
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  unsigned int errors = 0;

  cedar::proc::gui::SettingsSingleton::getInstance()->disableWriting();

  cedar::proc::gui::Ide *p_ide = new cedar::proc::gui::Ide(false, false, true);

  errors += testSaving(p_ide);

  delete p_ide;

  // return
  std::cout << "Done, " << errors << " error(s) occurred." << std::endl;
  return errors;
}
