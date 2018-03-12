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

    File:        ElementTreeWidget.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 10 21

    Description: Source file for the class cedar::proc::gui::ElementTreeWidget.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ElementTreeWidget.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <QMessageBox>
#include <boost/bind.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::ElementTreeWidget::ElementTreeWidget(QWidget* pParent)
:
QTreeWidget(pParent),
mNameColumn(0),
mNameEditingEnabled(false)
{
  QObject::connect(this, SIGNAL(elementAddedSignal(QString)), this, SLOT(elementAdded(QString)));
  QObject::connect(this, SIGNAL(elementRemovedSignal(QString)), this, SLOT(elementRemoved(QString)));
  QObject::connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(itemChanged(QTreeWidgetItem*, int)));
}

cedar::proc::gui::ElementTreeWidget::~ElementTreeWidget()
{
  this->disconnectSignals();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::ElementTreeWidget::setItemPath(QTreeWidgetItem* pItem, const std::string& newPath)
{
  pItem->setData(this->getNameColumn(), Qt::UserRole, QString::fromStdString(newPath));
}

QTreeWidgetItem* cedar::proc::gui::ElementTreeWidget::findItemWithPath(const cedar::proc::GroupPath& path)
{
  auto searched_path = path.toString();
  QTreeWidgetItemIterator it(this);
  while (*it)
  {
    QTreeWidgetItem* p_item = *it;
    auto item_path = p_item->data(this->getNameColumn(), Qt::UserRole).toString().toStdString();

    if (item_path == path.toString())
    {
      return p_item;
    }
    ++it;
  }

  return nullptr;
}

QTreeWidgetItem* cedar::proc::gui::ElementTreeWidget::findItemForElement(cedar::proc::ConstElementPtr element)
{
  return this->findItemWithPath(element->getFullPath());
}

std::string cedar::proc::gui::ElementTreeWidget::getPathFromItem(QTreeWidgetItem* pItem) const
{
  return pItem->data(this->getNameColumn(), Qt::UserRole).toString().toStdString();
}

void cedar::proc::gui::ElementTreeWidget::itemChanged(QTreeWidgetItem* pItem, int column)
{
  // only react if the change was made in the name column
  if (column != this->mNameColumn)
  {
    return;
  }

  // if we are not allowed to edit the name, return here
  if (!this->mNameEditingEnabled)
  {
    return;
  }

  QString new_name = pItem->text(this->getNameColumn());
  std::string path = this->getPathFromItem(pItem);
  auto element = this->mGroup->getElement(path);
  try
  {
    element->setName(new_name.toStdString());
  }
  catch (cedar::aux::ValidationFailedException& e)
  {
    QMessageBox::critical(this, "Renaming failed", "Could not rename element. " + QString::fromStdString(e.getMessage()), QMessageBox::Ok);
    pItem->setText(this->getNameColumn(), QString::fromStdString(element->getName()));
    return;
  }
  this->setItemPath(pItem, this->mGroup->findPath(element));
  emit elementNameChanged(pItem);
}

void cedar::proc::gui::ElementTreeWidget::setNameEditingEnabled(bool enabled)
{
  this->mNameEditingEnabled = enabled;
}

void cedar::proc::gui::ElementTreeWidget::setFilter(const boost::function<bool(cedar::proc::ConstElementPtr)>& filter)
{
  this->mFilter = filter;
}

void cedar::proc::gui::ElementTreeWidget::setGroup(cedar::proc::GroupPtr group)
{
  this->mGroup = group;

  this->disconnectSignals();

  this->clear();
  this->mElementNames.clear();

  this->addGroup(this->mGroup);
}

void cedar::proc::gui::ElementTreeWidget::addGroup(cedar::proc::GroupPtr group)
{
  for (const auto& name_element_pair : group->getElements())
  {
    this->translateElementAddedSignal(name_element_pair.second);
  }

  this->mElementAddedConnections.push_back
  (
    group->connectToNewElementAddedSignal
    (
      boost::bind(&cedar::proc::gui::ElementTreeWidget::translateElementAddedSignal, this, _1)
    )
  );

  this->mElementRemovedConnections.push_back
  (
    group->connectToElementRemovedSignal
    (
      boost::bind(&cedar::proc::gui::ElementTreeWidget::translateElementRemovedSignal, this, _1)
    )
  );
}

void cedar::proc::gui::ElementTreeWidget::addElement(cedar::proc::ElementPtr element)
{
  std::string path = this->mGroup->findPath(element);
  this->mElementNames[element.get()] = QString::fromStdString(path);

  this->connectRenameSignal(element);

  auto p_item = new QTreeWidgetItem();
  p_item->setText(this->mNameColumn, QString::fromStdString(element->getName()));
  this->setItemPath(p_item, path);

  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  if (this->mNameEditingEnabled)
  {
    flags |= Qt::ItemIsEditable;
  }
  p_item->setFlags(flags);

  // this retrieves the group item (or creates it if it does not exist)
  auto group_item = this->getGroupItem(path);
  group_item->addChild(p_item);

  this->signalElementAdded(p_item, element);
}

void cedar::proc::gui::ElementTreeWidget::disconnectSignals()
{
  for (auto connection : this->mElementAddedConnections)
  {
    connection.disconnect();
  }
  for (auto connection : this->mElementRemovedConnections)
  {
    connection.disconnect();
  }
}

void cedar::proc::gui::ElementTreeWidget::translateElementAddedSignal(cedar::proc::ElementPtr element)
{
  std::string path = this->mGroup->findPath(element);
  emit elementAddedSignal(QString::fromStdString(path));
}

void cedar::proc::gui::ElementTreeWidget::elementAdded(QString elementName)
{
  auto element = this->mGroup->getElement(elementName.toStdString());

  if (auto group = boost::dynamic_pointer_cast<cedar::proc::Group>(element))
  {
    std::string path = this->mGroup->findPath(group);

    this->connectRenameSignal(group);

    this->addGroup(group);
  }
  else
  {
    if (!this->mFilter || this->mFilter(element))
    {
      this->addElement(element);
    }
  }
}

void cedar::proc::gui::ElementTreeWidget::connectRenameSignal(cedar::proc::ElementPtr element)
{
  cedar::aux::ParameterPtr name = element->getParameter("name");
  QObject::connect(name.get(), SIGNAL(valueChanged()), this, SLOT(elementNameChanged()));
}

void cedar::proc::gui::ElementTreeWidget::elementNameChanged()
{
  auto name = cedar::aux::asserted_cast<cedar::aux::Parameter*>(QObject::sender());
  auto element = dynamic_cast<cedar::proc::Element*>(name->getOwner());
  cedar::proc::GroupPath new_path = this->mGroup->findPath(element);

  auto iter = this->mElementNames.find(element);
  if (iter == this->mElementNames.end())
  {
    return; // can happen if the element is being deleted
  }

  QString old_name = iter->second;
  QString new_name = QString::fromStdString(this->mGroup->findPath(element));

  this->mElementNames[element] = new_name;

  auto item = this->findItemWithPath(old_name.toStdString());
  CEDAR_ASSERT(item != nullptr);

  this->setItemPath(item, new_path.toString());
  item->setText(this->mNameColumn, QString::fromStdString(new_path.getElementName()));

  emit elementNameChanged(item);
}

void cedar::proc::gui::ElementTreeWidget::translateElementRemovedSignal(cedar::proc::ConstElementPtr element)
{
  auto iter = this->mElementNames.find(element.get());
  if (iter != this->mElementNames.end())
  {
    QString name = iter->second;
    this->mElementNames.erase(iter);

    emit elementRemovedSignal(name);
  }
}

void cedar::proc::gui::ElementTreeWidget::elementRemoved(QString elementName)
{
  auto items = this->findItems(elementName, 0, this->mNameColumn);

  QTreeWidgetItemIterator it(this);
  while (*it)
  {
    QTreeWidgetItem* p_item = *it;
    QString item_name = p_item->data(this->getNameColumn(), Qt::UserRole).toString();
    if (item_name == elementName)
    {
      auto path = item_name.toStdString();
      delete p_item;
      return;
    }
    ++it;
  }
}

QTreeWidgetItem* cedar::proc::gui::ElementTreeWidget::getGroupItem(const std::string& elementPath)
{
  cedar::proc::GroupPath path(elementPath);

  CEDAR_ASSERT(!path.empty());

  QTreeWidgetItem* p_last = this->invisibleRootItem();
  for (size_t path_index = 0; path.getElementCount() > 0 && path_index < path.getElementCount() - 1; ++path_index)
  {
    std::string component = path[path_index];
    QTreeWidgetItem* p_item = nullptr;

    // find the item that corresponds to the group
    for (int i = 0; i < p_last->childCount(); ++i)
    {
      auto child = p_last->child(i);
      if (child->text(this->mNameColumn).toStdString() == component)
      {
        p_item = child;
        break;
      }
    }

    // if no group item was found, create one
    if (p_item == nullptr)
    {
      auto subpath = path(0, path_index + 1);
      std::string subpath_str = subpath.toString();

      p_item = new QTreeWidgetItem();
      p_item->setText(this->mNameColumn, QString::fromStdString(component));
      p_last->addChild(p_item);
      p_item->setExpanded(true);
      this->setItemPath(p_item, subpath_str);

      auto group_element = this->mGroup->getElement(subpath);
      this->mElementNames[group_element.get()] = QString::fromStdString(subpath_str);
    }
    p_last = p_item;
  }

  return p_last;
}
