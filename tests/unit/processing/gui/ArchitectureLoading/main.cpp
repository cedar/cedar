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
#include <iostream>


int verifyArchitecture1(cedar::proc::gui::Ide *pIde)
{
  int errors = 0;

  cedar::proc::gui::View *p_view = pIde->getArchitectureView();
  cedar::proc::gui::Scene *p_scene = p_view->getScene();
  cedar::proc::GroupPtr network = p_scene->getRootGroup()->getGroup();

  cedar::proc::TriggerPtr trigger;
  cedar::proc::gui::TriggerItem *p_trigger = NULL;

  try
  {
    trigger = network->getElement<cedar::proc::Trigger>("trigger");
    p_trigger = p_scene->getTriggerItemFor(trigger.get());
    if (!p_trigger)
    {
      std::cout << "Could not get trigger item for trigger." << std::endl;
      ++errors;
    }
  }
  catch (cedar::aux::ExceptionBase& e)
  {
    std::cout << "ERROR: Exception. " << e.exceptionInfo() << std::endl;
    ++errors;
  }

  cedar::proc::StepPtr field;
  cedar::proc::gui::StepItem *p_field = NULL;

  try
  {
    field = network->getElement<cedar::proc::Step>("field");
    p_field = p_scene->getStepItemFor(field.get());
    if (!p_field)
    {
      std::cout << "Could not get step item for field." << std::endl;
      ++errors;
    }
  }
  catch (cedar::aux::ExceptionBase& e)
  {
    std::cout << "ERROR: Exception. " << e.exceptionInfo() << std::endl;
    ++errors;
  }

  cedar::proc::StepPtr gauss;
  cedar::proc::gui::StepItem *p_gauss = NULL;

  try
  {
    gauss = network->getElement<cedar::proc::Step>("gaussInput");
    p_gauss = p_scene->getStepItemFor(gauss.get());
    if (!p_gauss)
    {
      std::cout << "Could not get step item for gaussInput." << std::endl;
      ++errors;
    }
  }
  catch (cedar::aux::ExceptionBase& e)
  {
    std::cout << "ERROR: Exception. " << e.exceptionInfo() << std::endl;
    ++errors;
  }

  if (!p_trigger->hasGuiConnectionTo(p_field))
  {
    std::cout << "ERROR: trigger is not connected to field!" << std::endl;
    ++errors;
  }

  if (!p_gauss->hasGuiConnection("Gauss input", p_field, "input"))
  {
    std::cout << "ERROR: trigger is not connected to field!" << std::endl;
    ++errors;
  }

  return errors;
}

int testArchitecture1(cedar::proc::gui::Ide *pIde)
{
  int errors = 0;

  try
  {
    pIde->loadFile(QString::fromStdString(cedar::aux::Path("test://unit/processing/gui/ArchitectureLoading/architecture-1.json").absolute().toString(false)));
  }
  catch (cedar::aux::ExceptionBase& e)
  {
    std::cout << "ERROR: unhandled exception: " << e.exceptionInfo() << std::endl;
    ++errors;
  }
  errors += verifyArchitecture1(pIde);

  return errors;
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  unsigned int errors = 0;

  cedar::proc::gui::SettingsSingleton::getInstance()->disableWriting();

  cedar::proc::gui::Ide *p_ide = new cedar::proc::gui::Ide(false, false, true);

  std::cout << "Testing architecture-1.json" << std::endl;
  errors += testArchitecture1(p_ide);

  std::cout << "Testing re-loading of architecture-1.json" << std::endl;
  errors += testArchitecture1(p_ide);

  delete p_ide;

  // return
  std::cout << "Done, " << errors << " error(s) occurred." << std::endl;
  return errors;
}
