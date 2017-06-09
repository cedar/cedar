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

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 17

    Description: This unit test tests cedar::proc::gui::Groups in a non-gui context.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/Ide.h"
#include "cedar/processing/exceptions.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <QMainWindow>
#ifndef Q_MOC_RUN
  #include <boost/filesystem.hpp>
#endif
#include <iostream>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  unsigned int errors = 0;

  cedar::proc::gui::Ide* p_window = new cedar::proc::gui::Ide(false, false, true);

  cedar::proc::gui::Group* p_network
    = new cedar::proc::gui::Group(p_window, p_window->getArchitectureView()->getScene());

  try
  {
    p_network->readJson(cedar::aux::Path("test://unit/processing/gui/gui_Group/test_arch.json").absolute());
  }
  catch (cedar::proc::ArchitectureLoadingException& exception)
  {
    ++errors;
    std::cout << "Architecture failed to load. Exceptions:" << std::endl;

    for (size_t i = 0; i < exception.getMessages().size(); ++i)
    {
      std::cout << i << ": " << exception.getMessages()[i] << std::endl;
    }
  }

  p_network->writeJson("save_test.json");

  if (boost::filesystem::exists("save_test.json"))
  {
    boost::filesystem::remove("save_test.json");
  }
  else
  {
    std::cout << "ERROR: Could not find written file." << std::endl;
    ++errors;
  }

  // return
  std::cout << "Done, " << errors << " error(s) occurred." << std::endl;
  return errors;
}
