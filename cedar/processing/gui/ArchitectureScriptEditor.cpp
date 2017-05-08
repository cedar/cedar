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

    File:        ArchitectureScriptEditor.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 11 21

    Description: Source file for the class cedar::proc::gui::ArchitectureScriptEditor.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ArchitectureScriptEditor.h"
#include "cedar/processing/CppScriptDeclaration.h"
#include "cedar/processing/CppScript.h"

// SYSTEM INCLUDES
#include <set>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

const int cedar::proc::gui::ArchitectureScriptEditor::M_NAME_COL = 0;
const int cedar::proc::gui::ArchitectureScriptEditor::M_CTRL_COL = 1;
const int cedar::proc::gui::ArchitectureScriptEditor::M_STATUS_COL = 2;
const int cedar::proc::gui::ArchitectureScriptEditor::M_TYPE_COL = 3;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::ArchitectureScriptEditor::ArchitectureScriptEditor(cedar::proc::gui::GroupPtr group, QWidget* pParent)
:
QWidget(pParent),
mGroup(group)
{
  this->setupUi(this);

  this->fillTypeComboBox();

  QObject::connect(this->mpAddButton, SIGNAL(clicked()), this, SLOT(addClicked()));
  QObject::connect(this->mpRemoveButton, SIGNAL(clicked()), this, SLOT(removeClicked()));

  this->mConnectionScriptAdded = this->getGroup()->getGroup()->connectToScriptAddedSignal
  (
    boost::bind(&cedar::proc::gui::ArchitectureScriptEditor::translateScriptAddedSignal, this, _1)
  );
  QObject::connect(this, SIGNAL(scriptAddedInGroup(QString)), this, SLOT(addScriptToList(const QString&)));


  this->mConnectionScriptRemoved = this->getGroup()->getGroup()->connectToScriptRemovedSignal
  (
    boost::bind(&cedar::proc::gui::ArchitectureScriptEditor::translateScriptRemovedSignal, this, _1)
  );
  QObject::connect(this, SIGNAL(scriptRemovedFromGroup(QString)), this, SLOT(removeScriptFromList(const QString&)));

  QObject::connect(this->mpScriptList, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelected()));

  this->refreshScriptList();
}

cedar::proc::gui::detail::ScriptControlWidget::ScriptControlWidget(cedar::proc::CppScriptPtr script)
:
mScript(script)
{
  auto layout = new QHBoxLayout();
  this->setLayout(layout);
  layout->setContentsMargins(1, 1, 1, 1);

  this->mpStartButton = new QPushButton(QIcon(":/cedar/auxiliaries/gui/start.svg"), "");
  QObject::connect(this->mpStartButton, SIGNAL(clicked()), this, SLOT(runScript()));

  this->mpStopButton = new QPushButton(QIcon(":/cedar/auxiliaries/gui/stop.svg"), "");
  QObject::connect(this->mpStopButton, SIGNAL(clicked()), this, SLOT(requestScriptStop()));

  layout->addStretch(1);
  layout->addWidget(this->mpStartButton, 0);
  layout->addWidget(this->mpStopButton, 0);
  layout->addStretch(1);

  QObject::connect(this->mScript.get(), SIGNAL(scriptStarted()), this, SLOT(scriptStarted()));
  QObject::connect(this->mScript.get(), SIGNAL(scriptStopped()), this, SLOT(scriptStopped()));

  this->updateScriptRunningState();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::detail::ScriptControlWidget::requestScriptStop()
{
  this->mpStopButton->setEnabled(false);
  this->mScript->requestStop();
}

void cedar::proc::gui::detail::ScriptControlWidget::updateScriptRunningState()
{
  this->updateScriptRunningState(this->mScript->isRunning());
}

void cedar::proc::gui::detail::ScriptControlWidget::updateScriptRunningState(bool running)
{
  this->mpStartButton->setEnabled(!running);
  this->mpStopButton->setEnabled(running);
}

void cedar::proc::gui::detail::ScriptControlWidget::runScript()
{
  this->mScript->callRun();
}

void cedar::proc::gui::detail::ScriptControlWidget::scriptStarted()
{
  this->updateScriptRunningState(true);
}

void cedar::proc::gui::detail::ScriptControlWidget::scriptStopped()
{
  this->updateScriptRunningState(false);
}


void cedar::proc::gui::ArchitectureScriptEditor::scriptStatusChanged(QString newStatus)
{
  auto script = dynamic_cast<cedar::proc::CppScript*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(script);

  for (int row = 0; row < this->mpScriptList->rowCount(); ++row)
  {
    if (this->mpScriptList->item(row, M_NAME_COL)->text() == QString::fromStdString(script->getName()))
    {
      this->mpScriptList->item(row, M_STATUS_COL)->setText(newStatus);
    }
  }
}

void cedar::proc::gui::ArchitectureScriptEditor::scriptNameChanged()
{
  auto name_parameter = dynamic_cast<cedar::aux::StringParameter*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(name_parameter);
  auto iter = this->mUsedParameterNames.find(name_parameter);
  CEDAR_DEBUG_ASSERT(iter != this->mUsedParameterNames.end());
  std::string old_name = iter->second;
  std::string new_name = name_parameter->getValue();
  auto items = this->mpScriptList->findItems(QString::fromStdString(old_name), Qt::MatchExactly);

  for (auto item : items)
  {
    if (item->column() == M_NAME_COL)
    {
      item->setText(QString::fromStdString(new_name));
    }
  }

  this->mUsedParameterNames[name_parameter] = new_name;
}

void cedar::proc::gui::ArchitectureScriptEditor::refreshScriptList()
{
  this->mpScriptList->setRowCount(0);

  for (auto script : this->getGroup()->getGroup()->getOrderedScripts())
  {
    this->addScriptToList(QString::fromStdString(script->getName()));
  }
}

void cedar::proc::gui::ArchitectureScriptEditor::itemSelected()
{
  auto selected_ranges = this->mpScriptList->selectedRanges();
  if (selected_ranges.count() == 1 && selected_ranges[0].rowCount() == 1)
  {
    int row = selected_ranges[0].topRow();
    auto item = this->mpScriptList->item(row, M_NAME_COL);
    this->showItemProperties(item);
  }
}

void cedar::proc::gui::ArchitectureScriptEditor::showItemProperties(QTableWidgetItem* pItem)
{
  auto script = this->getScriptFromItem(pItem);
  this->mpConfigurationEditor->display(script);
}

cedar::proc::CppScriptPtr cedar::proc::gui::ArchitectureScriptEditor::getScriptFromItem(QTableWidgetItem* pItem) const
{
  auto name = pItem->text().toStdString();
  return this->getGroup()->getGroup()->getScript(name);
}

void cedar::proc::gui::ArchitectureScriptEditor::translateScriptAddedSignal(const std::string& scriptName)
{
  emit scriptAddedInGroup(QString::fromStdString(scriptName));
}

void cedar::proc::gui::ArchitectureScriptEditor::translateScriptRemovedSignal(const std::string& scriptName)
{
  emit scriptRemovedFromGroup(QString::fromStdString(scriptName));
}

void cedar::proc::gui::ArchitectureScriptEditor::removeScriptFromList(const QString& scriptName)
{
  auto items = this->mpScriptList->findItems(scriptName, Qt::MatchExactly);
  for (auto item : items)
  {
    if (item->column() == M_NAME_COL)
    {
      this->mpScriptList->removeRow(item->row());
    }
  }
}

void cedar::proc::gui::ArchitectureScriptEditor::addScriptToList(const QString& scriptName)
{
  auto script = this->getGroup()->getGroup()->getScript(scriptName.toStdString());

  int row = this->mpScriptList->rowCount();
  this->mpScriptList->setRowCount(row + 1);

  this->mpScriptList->setItem(row, M_NAME_COL, new QTableWidgetItem(QString::fromStdString(script->getName())));
  this->mpScriptList->setCellWidget(row, M_CTRL_COL, new cedar::proc::gui::detail::ScriptControlWidget(script));
  this->mpScriptList->setItem(row, M_STATUS_COL, new QTableWidgetItem(QString::fromStdString(script->getStatus())));
  this->mpScriptList->setItem(row, M_TYPE_COL, new QTableWidgetItem(QString::fromStdString(script->getType())));

  auto name_parameter = script->getParameter("name");
  this->mUsedParameterNames[name_parameter.get()] = script->getName();
  QObject::connect(name_parameter.get(), SIGNAL(valueChanged()), this, SLOT(scriptNameChanged()));
  QObject::connect(script.get(), SIGNAL(statusChanged(QString)), this, SLOT(scriptStatusChanged(QString)));
}

void cedar::proc::gui::ArchitectureScriptEditor::fillTypeComboBox()
{
  auto manager = cedar::proc::CppScriptDeclarationManagerSingleton::getInstance();

  bool selected = false;
  for (const auto& category : manager->listCategories())
  {
    this->mpTypeSelector->addItem(QString::fromStdString(category));
    this->setComboBoxItemEnabled(this->mpTypeSelector->count() - 1, false);
    this->mpTypeSelector->insertSeparator(this->mpTypeSelector->count());

    for (const auto& entry : manager->getCategoryEntries(category))
    {
      QString full_name = QString::fromStdString(entry->getClassName());
      QString displayed_name = QString::fromStdString(entry->getClassNameWithoutNamespace());

      this->mpTypeSelector->addItem(displayed_name, full_name);

      if (!selected)
      {
        selected = true;
        this->mpTypeSelector->setCurrentIndex(this->mpTypeSelector->count() - 1);
      }
    }
  }
}

void cedar::proc::gui::ArchitectureScriptEditor::setComboBoxItemEnabled(int index, bool enabled)
{
  int flags_role = Qt::UserRole - 1;
  int flags = this->mpTypeSelector->itemData(index, flags_role).toInt();

  if (enabled)
  {
    flags |= Qt::ItemIsEditable;
  }
  else
  {
    flags &= ~Qt::ItemIsEditable;
  }

  this->mpTypeSelector->setItemData(index, QVariant(flags), flags_role);
}

void cedar::proc::gui::ArchitectureScriptEditor::addClicked()
{
  int index = this->mpTypeSelector->currentIndex();
  if (index == -1)
  {
    return; // nothing selected
  }

  auto data = this->mpTypeSelector->itemData(index);
  if (!data.isValid())
  {
    return;
  }

  this->getGroup()->getGroup()->createScript(data.toString().toStdString());
}

void cedar::proc::gui::ArchitectureScriptEditor::removeClicked()
{
  auto selected = this->mpScriptList->selectedItems();
  std::set<std::string> scripts_to_remove;
  for (auto item : selected)
  {
    if (item->column() == M_NAME_COL)
    {
      scripts_to_remove.insert(item->text().toStdString());
    }
  }

  if (!scripts_to_remove.empty())
  {
    this->mpConfigurationEditor->clear();
    for (const auto& script_name : scripts_to_remove)
    {
      this->getGroup()->getGroup()->removeScript(script_name);
    }
  }
}

cedar::proc::gui::GroupPtr cedar::proc::gui::ArchitectureScriptEditor::getGroup() const
{
  return this->mGroup.lock();
}
