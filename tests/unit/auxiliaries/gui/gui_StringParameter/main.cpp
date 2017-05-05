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
    Date:        2012 07 12

    Description: 

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"

//!@todo Skip this unit test instead of just passing without testing anything
#ifdef CEDAR_COMPILER_MSVC

int main(int, char**)
{
  return 0;
}

#else // CEDAR_COMPILER_MSVC

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/StringParameter.h"
#include "cedar/auxiliaries/StringParameter.h"

#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <vector>
#include <string>
#include <iostream>

int test_string(const std::string& str)
{
  int errors = 0;

  std::cout << "Testing string \"" << str << "\"" << std::endl;

  cedar::aux::ConfigurablePtr conf(new cedar::aux::Configurable());
  cedar::aux::StringParameterPtr string(new cedar::aux::StringParameter(conf.get(), "string", str));

  if (string->getValue() != str)
  {
    std::cout << "Error: did not properly set the default value \"" << str << "\"." << std::endl;
    ++errors;
  }

  std::cout << "creating widget." << std::endl;
  cedar::aux::gui::StringParameter *p_param = new cedar::aux::gui::StringParameter();

  std::cout << "passing parameter to widget." << std::endl;
  p_param->setParameter(string);

  if (string->getValue() != str)
  {
    std::cout << "Error: widget changed value from \"" << str << "\" to \"" << string->getValue() << "\"." << std::endl;
    ++errors;
  }

  // find the QLineEdit child
  const QObjectList& children = p_param->children();
  for (int i = 0; i < children.size(); ++i)
  {
    // assume that the first found QLineEdit is mpEdit
    if (const auto* line_edit = dynamic_cast<QLineEdit*>(children.at(i)))
    {
      if (line_edit->text().toStdString() != str)
      {
        std::cout << "Error: widget shows the wrong value: \""
             << line_edit->text().toStdString() << "\" instead of \"" << str << "\"." << std::endl;
        ++errors;
      }
      break;
    }
  }

  std::cout << "deleting widget." << std::endl;
  delete p_param;

  return errors;
}

int main(int argc, char** argv)
{
  // needs to be created because we deal with widgets here
  QApplication app(argc, argv);

  // the number of errors encountered in this test
  int errors = 0;

  errors += test_string("\\n");

  std::cout << "Done. There were " << errors << " errors." << std::endl;
  return errors;
}

#endif // CEDAR_COMPILER_MSVC
