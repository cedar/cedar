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

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        ArchitectureWidget.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 05 08

    Description: Source file for the class cedar::proc::gui::ArchitectureWidget.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ArchitectureWidget.h"
#include "cedar/processing/Connectable.h"
#include "cedar/processing/Group.h"
#include "cedar/auxiliaries/gui/PlotManager.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/gui/PlotInterface.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <QGridLayout>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::ArchitectureWidget::ArchitectureWidget(cedar::proc::GroupPtr group, QWidget* pParent)
:
QWidget(pParent),
mGroup(group)
{
  auto layout = new QGridLayout();
  this->setLayout(layout);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::ArchitectureWidget::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  auto entries_iter = node.find("entries");
  if (entries_iter == node.not_found())
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Could not entries for architecture widget.",
      CEDAR_CURRENT_FUNCTION_NAME
    );
    return;
  }

  auto grid_layout = dynamic_cast<QGridLayout*>(this->layout());

  for (const auto& entry_pair : entries_iter->second)
  {
    const auto& entry = entry_pair.second;

    //!@todo Check that these nodes actually exist
    auto row = entry.get<int>("row");
    auto column = entry.get<int>("column");
    auto type = entry.get<std::string>("type");

    int column_span = 1;
    int row_span = 1;

    auto col_span_iter = entry.find("column span");
    if (col_span_iter != entry.not_found())
    {
      column_span = col_span_iter->second.get_value<int>();
    }

    if (type == "default plot")
    {
      //!@todo Check that these nodes actually exist
      auto data_path = entry.get<std::string>("data");
      //!@todo Move this code to a group function
      std::string data_name, step_path, step_path_rest, role;
      cedar::aux::splitLast(data_path, ".", step_path, data_name);
      cedar::aux::splitLast(step_path, "[", step_path_rest, role);
      if (!role.empty() && role.at(role.size() - 1) == ']')
      {
        role = role.substr(0, role.size() - 1);
      }

      auto connectable = this->mGroup->getElement<cedar::proc::Connectable>(step_path_rest);

      auto role_enum = cedar::proc::DataRole::type().get(role);
      auto data = connectable->getData(role_enum, data_name);

      //!@todo Error handling
      CEDAR_ASSERT(data);

      auto declaration = cedar::aux::gui::PlotManagerSingleton::getInstance()->getDefaultDeclarationFor(data);
      auto plot = declaration->createPlot();
      plot->plot(data, "data");

      grid_layout->addWidget(plot, row, column, row_span, column_span);
    }
    else
    {
      //!@todo error message
    }
  }
}

void cedar::proc::gui::ArchitectureWidget::writeConfiguration(cedar::aux::ConfigurationNode& /* node */) const
{
  //!@todo Also write a method for actually writing out the configuration of the widget
}
