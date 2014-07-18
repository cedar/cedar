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
#include "cedar/auxiliaries/gui/MultiPlotInterface.h"
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

//! A small helper function for reading a value from a tree. Returns a default value if the read value cannot be found.
template <typename T>
T cedar::proc::gui::ArchitectureWidget::readOptional
  (
    const cedar::aux::ConfigurationNode& node,
    const std::string& key,
    const T& defaultValue
  )
{
  auto iter = node.find(key);
  if (iter != node.not_found())
  {
    return iter->second.get_value<T>();
  }
  else
  {
    return defaultValue;
  }
}

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

    auto row = readOptional<int>(entry, "row", 0);
    auto column = readOptional<int>(entry, "column", 0);
    auto type = readOptional<std::string>(entry, "type", "plot");

    grid_layout->setRowStretch(row, 1);
    grid_layout->setColumnStretch(column, 1);

    int column_span = readOptional<int>(entry, "column span", 1);
    int row_span = readOptional<int>(entry, "row span", 1);

    auto data_i = entry.find("data");
    //!@todo Exception
    CEDAR_ASSERT(data_i != entry.not_found());
    const auto& data_node = data_i->second;

    std::string first_data_path;

    if (data_node.size() == 0)
    {
      first_data_path = data_node.get_value<std::string>();
    }
    else
    {
      CEDAR_ASSERT(data_node.size() > 0);
      first_data_path = data_node.begin()->second.get_value<std::string>();
    }

    auto data = this->findData(first_data_path);

    //!@todo Exception
    CEDAR_ASSERT(data);

    cedar::aux::gui::PlotInterface* plot = nullptr;
    if (type == "plot")
    {
      auto plot_type = readOptional<std::string>(entry, "plot type", "default");

      cedar::aux::gui::ConstPlotDeclarationPtr declaration;
      if (plot_type == "default")
      {
        declaration = cedar::aux::gui::PlotManagerSingleton::getInstance()->getDefaultDeclarationFor(data);
      }
      else
      {
        try
        {
          declaration = cedar::aux::gui::PlotManagerSingleton::getInstance()->getDeclaration(plot_type);
        }
        catch (cedar::aux::NotFoundException& e)
        {
          cedar::aux::LogSingleton::getInstance()->error
          (
            "Cannot open plot: " + e.getMessage(),
            CEDAR_CURRENT_FUNCTION_NAME
          );
          return;
        }
      }
      plot = declaration->createPlot();
    }
    else
    {
      cedar::aux::LogSingleton::getInstance()->error
      (
        "Don't know how to process cell tpye \"" + type + "\".",
        CEDAR_CURRENT_FUNCTION_NAME
      );
      return;
    }
    plot->plot(data, first_data_path);

    grid_layout->addWidget(plot, row, column, row_span, column_span);

    if (data_node.size() > 0)
    {
      auto multi_plot = dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(plot);
      if (!multi_plot)
      {
        cedar::aux::LogSingleton::getInstance()->error
        (
          "Cannot add more data: not a multi plot.",
          CEDAR_CURRENT_FUNCTION_NAME
        );
        return;
      }
      auto iter = data_node.begin();
      ++iter;

      for (; iter != data_node.end(); ++iter)
      {
        auto path = iter->second.get_value<std::string>();
        auto data = this->findData(path);

        //!@todo Exception
        CEDAR_ASSERT(data);

        if (multi_plot->canAppend(data))
        {
          multi_plot->append(data, path);
        }
        else
        {
          cedar::aux::LogSingleton::getInstance()->error
          (
            "Cannot add data \"" + path + "\": multiplot refuses.",
            CEDAR_CURRENT_FUNCTION_NAME
          );
        }
      }
    }
    else
    {
      //!@todo error message
    }
  }
}

cedar::aux::ConstDataPtr cedar::proc::gui::ArchitectureWidget::findData(const std::string& path) const
{
  //!@todo Move this code to a group function
  std::string data_name, step_path, step_path_rest, role;
  cedar::aux::splitLast(path, ".", step_path, data_name);
  cedar::aux::splitLast(step_path, "[", step_path_rest, role);
  if (!role.empty() && role.at(role.size() - 1) == ']')
  {
    role = role.substr(0, role.size() - 1);
  }

  auto connectable = this->mGroup->getElement<cedar::proc::Connectable>(step_path_rest);

  auto role_enum = cedar::proc::DataRole::type().get(role);
  return connectable->getData(role_enum, data_name);
}

void cedar::proc::gui::ArchitectureWidget::writeConfiguration(cedar::aux::ConfigurationNode& /* node */) const
{
  //!@todo Also write a method for actually writing out the configuration of the widget
}
