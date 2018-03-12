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

    File:        GroupParameterDesigner.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 02 27

    Description: Source file for the class cedar::proc::gui::GroupParameterDesigner.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/GroupParameterDesigner.h"
#include "cedar/processing/Group.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/ParameterDeclaration.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::GroupParameterDesigner::GroupParameterDesigner(cedar::proc::GroupPtr group)
:
mGroup(group)
{
  this->setupUi(this);

  this->fillParameterTypeBox();

  QObject::connect(this->mpAddParameterButton, SIGNAL(clicked()), this, SLOT(addClicked()));
  QObject::connect(this->mpDeleteSelected, SIGNAL(clicked()), this, SLOT(deleteClicked()));
  QObject::connect(this->mpParameterDisplay, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(itemChanged(QTreeWidgetItem*, int)));

  QObject::connect(this, SIGNAL(customParameterAdded(QVariant)), this, SLOT(customParameterAddedSlot(QVariant)));
  QObject::connect(this, SIGNAL(customParameterRemoved(QVariant)), this, SLOT(customParameterRemovedSlot(QVariant)));

  group->connectToCustomParameterAddedSignal
  (
    boost::bind(&cedar::proc::gui::GroupParameterDesigner::translateCustomParameterAdded, this, _1)
  );

  group->connectToCustomParameterRemovedSignal
  (
    boost::bind(&cedar::proc::gui::GroupParameterDesigner::translateCustomParameterRemoved, this, _1)
  );

  for (auto parameter : this->mGroup->getCustomParameters())
  {
    this->addParameterToList(parameter);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::GroupParameterDesigner::itemChanged(QTreeWidgetItem* item, int column)
{
  CEDAR_ASSERT(column == 0);
  std::string name = item->text(column).toStdString();

  auto parameter = this->parameterFromItem(item);
  parameter->setName(name);
}

void cedar::proc::gui::GroupParameterDesigner::translateCustomParameterAdded(cedar::aux::ParameterPtr parameter)
{
  QVariant pointer = QVariant::fromValue((void*)parameter.get());
  emit customParameterAdded(pointer);
}

void cedar::proc::gui::GroupParameterDesigner::translateCustomParameterRemoved(cedar::aux::ParameterPtr parameter)
{
  QVariant pointer = QVariant::fromValue((void*)parameter.get());
  emit customParameterRemoved(pointer);
}

void cedar::proc::gui::GroupParameterDesigner::customParameterAddedSlot(QVariant pointer)
{
  auto parameter = (cedar::aux::Parameter*)pointer.value<void*>();
  this->addParameterToList(cedar::aux::ParameterPtr(parameter));
}

void cedar::proc::gui::GroupParameterDesigner::customParameterRemovedSlot(QVariant pointer)
{
  auto parameter = cedar::aux::ParameterPtr((cedar::aux::Parameter*)pointer.value<void*>());

  std::vector<QTreeWidgetItem*> to_delete;
  auto root = this->mpParameterDisplay->invisibleRootItem();
  for (int i = 0; i < root->childCount(); ++i)
  {
    auto item = root->child(i);
    if (this->parameterFromItem(item) == parameter)
    {
      to_delete.push_back(item);
    }
  }

  for (auto item : to_delete)
  {
    delete item;
  }
}

void cedar::proc::gui::GroupParameterDesigner::addParameterToList(cedar::aux::ParameterPtr parameter)
{
  QStringList strs;
  strs << QString::fromStdString(parameter->getName());

  std::string type_id = cedar::aux::ParameterDeclarationManagerSingleton::getInstance()->getTypeId(parameter);
  strs << QString::fromStdString(type_id);
  auto p_item = new QTreeWidgetItem(strs);
  p_item->setFlags(p_item->flags() | Qt::ItemIsEditable);
  p_item->setData(0, Qt::UserRole, QVariant::fromValue((void*)parameter.get()));
  this->mpParameterDisplay->invisibleRootItem()->addChild(p_item);
}

void cedar::proc::gui::GroupParameterDesigner::fillParameterTypeBox()
{
  for (auto category : cedar::aux::ParameterDeclarationManagerSingleton::getInstance()->listCategories())
  {
    //!@todo category separator
    for (auto declaration : cedar::aux::ParameterDeclarationManagerSingleton::getInstance()->getCategoryEntries(category))
    {
      std::string class_name = declaration->getClassName();
      this->mpTypeSelector->addItem(QString::fromStdString(class_name));
    }
  }
}

void cedar::proc::gui::GroupParameterDesigner::addClicked()
{
  int index = this->mpTypeSelector->currentIndex();

  // check if anything is selected at all
  if (index < 0)
  {
    return;
  }

  // get the class name
  std::string class_name = this->mpTypeSelector->itemText(index).toStdString();
  this->addParameter(class_name);
}

void cedar::proc::gui::GroupParameterDesigner::deleteClicked()
{
  std::vector<cedar::aux::ParameterPtr> to_remove;

  for (auto item : this->mpParameterDisplay->selectedItems())
  {
    auto parameter = this->parameterFromItem(item);
    to_remove.push_back(parameter);
  }

  for (auto parameter : to_remove)
  {
    this->mGroup->removeCustomParameter(parameter->getName());
  }
}

cedar::aux::ParameterPtr cedar::proc::gui::GroupParameterDesigner::parameterFromItem(QTreeWidgetItem* pItem) const
{
  return cedar::aux::ParameterPtr((cedar::aux::Parameter*)(pItem->data(0, Qt::UserRole).value<void*>()));
}

void cedar::proc::gui::GroupParameterDesigner::addParameter(const std::string& type)
{
  this->mGroup->addCustomParameter(type, "new parameter");
}
