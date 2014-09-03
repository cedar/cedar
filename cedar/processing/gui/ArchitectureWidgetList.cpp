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

    File:        ArchitectureWidgetList.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 09 03

    Description: Source file for the class cedar::proc::gui::ArchitectureWidgetList.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ArchitectureWidgetList.h"
#include "cedar/processing/gui/Group.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::ArchitectureWidgetList::ArchitectureWidgetList(QWidget* pParent, cedar::proc::gui::GroupPtr group)
:
QDialog(pParent),
mGroup(group)
{
  this->setupUi(this);

  QObject::connect(this, SIGNAL(accepted()), this, SLOT(dialogAccepted()));

  QObject::connect(this->mpAddButton, SIGNAL(clicked()), this, SLOT(addRowClicked()));
  QObject::connect(this->mpRemoveButton, SIGNAL(clicked()), this, SLOT(removeRowClicked()));

  const auto& plots = this->mGroup->getArchitectureWidgets();
  for (const auto& name_path_pair : plots)
  {
    const auto& name = name_path_pair.first;
    const auto& path = name_path_pair.second;

    this->appendRow(name, path);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::ArchitectureWidgetList::appendRow(const std::string& name, const cedar::aux::Path& path)
{
  int row = this->mpTable->rowCount();
  this->mpTable->setRowCount(row + 1);

  auto name_item = new QTableWidgetItem(QString::fromStdString(name));
  auto path_item = new QTableWidgetItem(QString::fromStdString(path.toString(false)));

  this->mpTable->setItem(row, 0, name_item);
  this->mpTable->setItem(row, 1, path_item);
}


void cedar::proc::gui::ArchitectureWidgetList::addRowClicked()
{
  this->appendRow("", "");
}

void cedar::proc::gui::ArchitectureWidgetList::removeRowClicked()
{
  std::set<int> rows_to_delete;
  auto selected_items = this->mpTable->selectedItems();

  for (auto item : selected_items)
  {
    rows_to_delete.insert(item->row());
  }

  // delete in reverse so that we don't need to care about row indices moving around as a consequence of deleting things
  for (auto row_it = rows_to_delete.rbegin(); row_it != rows_to_delete.rend(); ++row_it)
  {
    int row = *row_it;
    this->mpTable->removeRow(row);
  }
}

void cedar::proc::gui::ArchitectureWidgetList::dialogAccepted()
{
  std::map<std::string, cedar::aux::Path> new_widgets;

  for (int row = 0; row < this->mpTable->rowCount(); ++row)
  {
    std::string name = this->mpTable->item(row, 0)->text().toStdString();
    cedar::aux::Path path = this->mpTable->item(row, 1)->text().toStdString();
    if (!name.empty())
    {
      new_widgets[name] = path;
    }
  }

  this->mGroup->setArchitectureWidgets(new_widgets);
}
