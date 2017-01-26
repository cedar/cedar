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
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/gui/PlotManager.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/gui/PlotInterface.h"
#include "cedar/auxiliaries/gui/MultiPlotInterface.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include <deque>

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

QWidget* cedar::proc::gui::ArchitectureWidget::readLabel(const cedar::aux::ConfigurationNode& entry)
{
  auto label = new QLabel();
  std::string text = readOptional<std::string>(entry, "text", "");
  label->setText(QString::fromStdString(text));
  return label;
}

QWidget* cedar::proc::gui::ArchitectureWidget::readPlot(const cedar::aux::ConfigurationNode& entry, int row, int column)
{
  auto data_i = entry.find("data");
  if (data_i == entry.not_found())
  {
    CEDAR_THROW(cedar::aux::NotFoundException, "Could not read plot: no data specified.");
  }
  const auto& data_node = data_i->second;

  // hidden knowledge: boost ptree represents arrays by nodes with empty keys
  bool has_multiple_data = data_node.size() != 0 && data_node.begin()->first == "";

  std::string first_data_path, first_data_title;

  if (!has_multiple_data)
  {
    this->readDataNode(data_node, first_data_path, first_data_title);
  }
  else
  {
    this->readDataNode(data_node.begin()->second, first_data_path, first_data_title);
  }

  auto data = this->findData(first_data_path);

  if (!data)
  {
    CEDAR_THROW(cedar::aux::NullPointerException, "Could not read plot: data is not set.");
  }

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

      auto label = new QLabel(QString::fromStdString("Could not find plot declaration for \"" + plot_type + "\"."));
      label->setWordWrap(true);
      return label;
    }
  }
  cedar::aux::gui::PlotInterface* plot = declaration->createPlot();
  CEDAR_DEBUG_ASSERT(plot != nullptr);

  plot->plot(data, first_data_title);

  auto cfg_i = entry.find("plot configuration");
  if (cfg_i != entry.not_found())
  {
    plot->readConfiguration(cfg_i->second);
  }

  if (has_multiple_data)
  {
    auto multi_plot = dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(plot);
    if (!multi_plot)
    {
      cedar::aux::LogSingleton::getInstance()->error
      (
        "Cannot add more data to cell " + cedar::aux::toString(row) + ", " + cedar::aux::toString(column) + ": not a multi plot.",
        CEDAR_CURRENT_FUNCTION_NAME
      );
      return plot;
    }
    auto iter = data_node.begin();
    ++iter;

    for (; iter != data_node.end(); ++iter)
    {
      std::string path, title;
      this->readDataNode(iter->second, path, title);
      auto data = this->findData(path);

      if (!data)
      {
        CEDAR_THROW(cedar::aux::NullPointerException, "Could not add data while reading plot: data is not set.");
      }

      if (multi_plot->canAppend(data))
      {
        multi_plot->append(data, title);
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

  return plot;
}

void cedar::proc::gui::ArchitectureWidget::readTemplates(const cedar::aux::ConfigurationNode& templates)
{
  for (const auto& name_template_cfg_pair : templates)
  {
    const auto& name = name_template_cfg_pair.first;
    const auto& template_cfg = name_template_cfg_pair.second;

    if (this->mTemplates.find(name) == this->mTemplates.end())
    {
      this->mTemplates[name] = template_cfg;
    }
    else
    {
      cedar::aux::LogSingleton::getInstance()->error
      (
        "Could not read template: there is already a template named \"" + name + "\".",
        CEDAR_CURRENT_FUNCTION_NAME
      );
    }
  }
}

void cedar::proc::gui::ArchitectureWidget::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  // check if there are entries in this widget; if not, no sense parsing the rest
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

  // read all templates so they are present when later parsing the entries
  auto templates_iter = node.find("templates");
  if (templates_iter != node.not_found())
  {
    this->readTemplates(templates_iter->second);
  }

  // go through all entries
  for (const auto& entry_pair : entries_iter->second)
  {
    const auto& entry = entry_pair.second;
    this->addEntry(entry);
  }

  // read the overall widget configuration (e.g., widget size)
  auto config_iter = node.find("configuration");
  if (config_iter != node.not_found())
  {
    this->readConfig(config_iter->second);
  }
}

void cedar::proc::gui::ArchitectureWidget::readConfig(const cedar::aux::ConfigurationNode& config)
{
  auto default_size_iter = config.find("default size");
  if (default_size_iter != config.not_found())
  {
    const auto& default_size_node = default_size_iter->second;
    auto width_iter = default_size_node.find("width");
    auto height_iter = default_size_node.find("height");


    if (width_iter != default_size_node.not_found() && height_iter != default_size_node.not_found())
    {
      int width = width_iter->second.get_value<int>();
      int height = height_iter->second.get_value<int>();
      this->resize(width, height);
    }
  }
}

void cedar::proc::gui::ArchitectureWidget::addEntry
(
  const cedar::aux::ConfigurationNode& entry,
  int rowOffset, int colOffset,
  const cedar::proc::gui::ArchitectureWidget::BaseCellAttributes& passedAttributes
)
{
  auto grid_layout = cedar::aux::asserted_cast<QGridLayout*>(this->layout());

  auto row = readOptional<int>(entry, "row", 0) + rowOffset;
  auto column = readOptional<int>(entry, "column", 0) + colOffset;
  auto type = readOptional<std::string>(entry, "type", "plot");

  BaseCellAttributes attributes;
  attributes.mRowStretch = readOptional<int>(entry, "row stretch", passedAttributes.mRowStretch);
  attributes.mColumnStretch = readOptional<int>(entry, "column stretch", passedAttributes.mColumnStretch);
  grid_layout->setRowStretch(row, attributes.mRowStretch);
  grid_layout->setColumnStretch(column, attributes.mColumnStretch);

  attributes.mRowSpan = readOptional<int>(entry, "row span", passedAttributes.mRowSpan);
  attributes.mColumnSpan = readOptional<int>(entry, "column span", passedAttributes.mColumnSpan);

  QWidget* widget = nullptr;
  if (type == "plot")
  {
    widget = this->readPlot(entry, row, column);
  }
  else if (type == "label")
  {
    widget = this->readLabel(entry);
  }
  else if (type == "step action")
  {
    widget = this->readStepAction(entry);
  }
  else if (type == "template")
  {
    this->addTemplate(row, column, entry, attributes);
  }
  else
  {
    auto label = new QLabel(QString::fromStdString("Unknown cell type: \"" + type + "\"."));
    label->setWordWrap(true);
    widget = label;
  }

  if (widget != nullptr)
  {
    grid_layout->addWidget(widget, row, column, attributes.mRowSpan, attributes.mColumnSpan);

    auto style = readOptional<std::string>(entry, "style sheet", "");
    if (!style.empty())
    {
      widget->setStyleSheet(QString::fromStdString(style));
    }
  }
}

QWidget* cedar::proc::gui::ArchitectureWidget::readStepAction(const cedar::aux::ConfigurationNode& entry)
{
  // create a button with the given label (or a default if none is given)
  auto label = readOptional<std::string>(entry, "label", "step action");
  auto button = new QPushButton(QString::fromStdString(label));

  // create a parameters-object to hold the settings for this button
  StepActionParametersPtr parameters(new StepActionParameters());
  // add it to the map so the slot can later access it
  this->mStepActionParameters[button] = parameters;

  // read the list of step-actions to be performed by the button
  auto actions_iter = entry.find("actions");
  if (actions_iter != entry.not_found())
  {
    const auto& actions_node = actions_iter->second;
    // the actions list contains a set of actions to be performed; each entry is a pair step name -> action name
    for (const auto& iter : actions_node)
    {
      if (iter.second.size() != 1)
      {
        cedar::aux::LogSingleton::getInstance()->warning("Invalid entry in step action list.", CEDAR_CURRENT_FUNCTION_NAME);
        continue;
      }
      const std::string& step_name = iter.second.begin()->first;
      std::string action_name = iter.second.begin()->second.get_value<std::string>();
      auto pair = std::make_pair(step_name, action_name);
      parameters->mStepActions.push_back(pair);
    }
  }

  // read other parameters
  parameters->mConfirm = readOptional<bool>(entry, "confirm", false);

  // now that the parameters exist, connect the button to the slot
  QObject::connect(button, SIGNAL(clicked()), this, SLOT(triggerStepAction()));

  return button;
}

void cedar::proc::gui::ArchitectureWidget::triggerStepAction()
{
  auto sender = dynamic_cast<QPushButton*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(sender != nullptr);
  auto parameters_iter = this->mStepActionParameters.find(sender);
  CEDAR_DEBUG_ASSERT(parameters_iter != this->mStepActionParameters.end());
  auto parameters = parameters_iter->second;

  // if a confirmation is set to be requested, get it
  if (parameters->mConfirm)
  {
    auto r = QMessageBox::question(this, "Confirm running action", "Are you sure you want to trigger the action \"" + sender->text() + "\"?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (r != QMessageBox::Yes)
    {
      return;
    }
  }

  // call the actions
  for (const auto& step_action_pair : parameters->mStepActions)
  {
    const std::string& step_name = step_action_pair.first;
    const std::string& action_name = step_action_pair.second;
    auto step = this->mGroup->getElement<cedar::proc::Step>(step_name);
    if (!step)
    {
      CEDAR_THROW(cedar::aux::NotFoundException, "Could not find a step with the path \"" + step_name + "\" while trying to run action.");
    }
    if (!step->hasAction(action_name))
    {
      CEDAR_THROW(cedar::aux::NotFoundException, "Step \"" + step_name + "\" has no action called \"" + action_name + "\".");
    }
    else
    {
      step->callAction(action_name);
    }
  }
}

void cedar::proc::gui::ArchitectureWidget::addTemplate
(
  int row, int column,
  const cedar::aux::ConfigurationNode& entry,
  const cedar::proc::gui::ArchitectureWidget::BaseCellAttributes& passedAttributes
)
{
  auto template_name = readOptional<std::string>(entry, "template name", "");
  if (template_name.empty())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Could not read template: template name is empty.",
      CEDAR_CURRENT_FUNCTION_NAME
    );
    return;
  }

  auto iter = this->mTemplates.find(template_name);
  if (iter == this->mTemplates.end())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Could not read template: template \"" + template_name + "\" not found.",
      CEDAR_CURRENT_FUNCTION_NAME
    );
    return;
  }

  auto substitutions_i = entry.find("substitutions");
  std::map<std::string, std::string> substitutions;

  auto conf = iter->second;

  // modify the configuration: apply substitutions
  if (substitutions_i != entry.not_found())
  {
    this->applySubstitutions(conf, substitutions_i->second);
  }

  for (const auto& entry_pair : conf)
  {
    this->addEntry(entry_pair.second, row, column, passedAttributes);
  }
}

void cedar::proc::gui::ArchitectureWidget::applySubstitutions(cedar::aux::ConfigurationNode& target, const cedar::aux::ConfigurationNode& substitutions)
{
  for (const auto& key_value_pair : substitutions)
  {
    std::string key = key_value_pair.first;
    std::string substitution = key_value_pair.second.get_value<std::string>();

    for (auto& name_child_pair : target)
    {
      std::string data = name_child_pair.second.data();
      data = cedar::aux::replace(data, key, substitution);
      name_child_pair.second.data() = data;

      this->applySubstitutions(name_child_pair.second, substitutions);
    }
  }
}

void cedar::proc::gui::ArchitectureWidget::readDataNode
     (
       const cedar::aux::ConfigurationNode& node,
       std::string& dataPath,
       std::string& title
     )
{
  if (node.size() == 0)
  {
    dataPath = node.get_value<std::string>();
    title = dataPath;
  }
  else
  {
    auto data = node.begin();
    title = data->first;
    dataPath = data->second.get_value<std::string>();
  }
}

cedar::aux::ConstDataPtr cedar::proc::gui::ArchitectureWidget::findData(const std::string& path) const
{
  //!@todo Use cedar::proc::GroupPath instead
  std::string data_name, step_path, step_path_rest, role;
  cedar::aux::splitLast(path, ".", step_path, data_name);
  cedar::aux::splitLast(step_path, "[", step_path_rest, role);

  if (!role.empty() && role.at(role.size() - 1) == ']')
  {
    role = role.substr(0, role.size() - 1);
  }

  if (role.empty())
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "When opening architecture widget: No role found in data string \"" + path + "\".",
      CEDAR_CURRENT_FUNCTION_NAME
    );
  }

  if (data_name.empty())
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "When opening architecture widget: No data name found in data string \"" + path + "\".",
      CEDAR_CURRENT_FUNCTION_NAME
    );
  }

  if (step_path.empty())
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "When opening architecture widget: No step name found in data string \"" + path + "\".",
      CEDAR_CURRENT_FUNCTION_NAME
    );
  }

  auto connectable = this->mGroup->getElement<cedar::proc::Connectable>(step_path_rest);
  if (!connectable)
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "When opening architecture widget: No element found with the path \"" + step_path + "\".",
      CEDAR_CURRENT_FUNCTION_NAME
    );
    return cedar::aux::ConstDataPtr();
  }

  auto role_enum = cedar::proc::DataRole::type().get(role);
  return connectable->getData(role_enum, data_name);
}

void cedar::proc::gui::ArchitectureWidget::writeConfiguration(cedar::aux::ConfigurationNode& /* node */) const
{
  //!@todo Also write a method for actually writing out the configuration of the widget
}
