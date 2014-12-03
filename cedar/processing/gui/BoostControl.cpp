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

    File:        BoostControl.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 06 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/BoostControl.h"
#include "cedar/processing/gui/View.h"
#include "cedar/processing/gui/GraphicsBase.h"
#include "cedar/processing/sources/Boost.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/NetworkPath.h"
#include "cedar/processing/Element.h"
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <QHeaderView>
#include <string>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::BoostControl::BoostControl(cedar::proc::gui::View* view)
:
mpView(view)
{
  this->setupUi(this);

  QObject::connect(this, SIGNAL(elementAddedSignal(QString)), this, SLOT(elementAdded(QString)));
  QObject::connect(this, SIGNAL(elementRemovedSignal(QString)), this, SLOT(elementRemoved(QString)));
  QObject::connect(this->mpBoostTree, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(itemActivated(QTreeWidgetItem*, int)));

  this->mpBoostTree->header()->setResizeMode(0, QHeaderView::Stretch);
  this->mpBoostTree->header()->setResizeMode(1, QHeaderView::ResizeToContents);
}

cedar::proc::gui::BoostControl::~BoostControl()
{
  this->disconnectSignals();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::BoostControl::itemActivated(QTreeWidgetItem* pItem, int column)
{
  // only activating the name centers the boost
  if (column == 0 && isBoostItem(pItem))
  {
    auto boost = this->findBoostFor(pItem);
    if (!boost)
    {
      return;
    }

    if (auto graphics_item = this->mpView->getScene()->getGraphicsItemFor(boost.get()))
    {
      this->mpView->centerOn(graphics_item);
    }
  }
}

bool cedar::proc::gui::BoostControl::isBoostItem(QTreeWidgetItem* pItem) const
{
  // there are only group- and boost items, and group items are expanded
  return pItem->isExpanded() == false;
}

cedar::proc::sources::BoostPtr cedar::proc::gui::BoostControl::findBoostFor(QTreeWidgetItem* pItem) const
{
  cedar::proc::NetworkPath path;

  // traverse the parents and add them to the path
  QTreeWidgetItem* parent = pItem->parent();
  while (parent != nullptr && parent != this->mpBoostTree->invisibleRootItem() && !parent->text(0).isEmpty())
  {
    path += parent->text(0).toStdString();
    parent = parent->parent();
  }

  path += pItem->text(0).toStdString();

  return boost::dynamic_pointer_cast<cedar::proc::sources::Boost>(this->mGroup->getElement(path));
}

void cedar::proc::gui::BoostControl::disconnectSignals()
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

void cedar::proc::gui::BoostControl::setGroup(cedar::proc::GroupPtr group)
{
  this->disconnectSignals();

  this->mpBoostTree->clear();
  this->mElementNames.clear();

  this->mGroup = group;

  this->addGroup(this->mGroup);
}

void cedar::proc::gui::BoostControl::addGroup(cedar::proc::GroupPtr group)
{
  for (const auto& name_element_pair : group->getElements())
  {
    this->translateElementAddedSignal(name_element_pair.second);
  }

  this->mElementAddedConnections.push_back
  (
    group->connectToNewElementAddedSignal
    (
      boost::bind(&cedar::proc::gui::BoostControl::translateElementAddedSignal, this, _1)
    )
  );

  this->mElementRemovedConnections.push_back
  (
    group->connectToElementRemovedSignal
    (
      boost::bind(&cedar::proc::gui::BoostControl::translateElementRemovedSignal, this, _1)
    )
  );
}

void cedar::proc::gui::BoostControl::translateElementRemovedSignal(cedar::proc::ConstElementPtr element)
{
  auto iter = this->mElementNames.find(element.get());
  if (iter != this->mElementNames.end())
  {
    QString name = iter->second;
    this->mElementNames.erase(iter);

    emit elementRemovedSignal(name);
  }
}

void cedar::proc::gui::BoostControl::elementRemoved(QString elementName)
{
  auto items = this->mpBoostTree->findItems(elementName, 0);

  QTreeWidgetItemIterator it(this->mpBoostTree);
  while (*it)
  {
    QTreeWidgetItem* p_item = *it;
    if (p_item->data(0, Qt::UserRole).toString() == elementName)
    {
      delete p_item;
      return;
    }
    ++it;
  }
}

void cedar::proc::gui::BoostControl::translateElementAddedSignal(cedar::proc::ElementPtr element)
{
  std::string path = this->mGroup->findPath(element);
  emit elementAddedSignal(QString::fromStdString(path));
}

QTreeWidgetItem* cedar::proc::gui::BoostControl::getGroupItem(const std::string& elementPath)
{
  std::vector<std::string> subpaths;
  cedar::aux::split(elementPath, ".", subpaths);

  CEDAR_ASSERT(!subpaths.empty());

  subpaths.pop_back();

  QTreeWidgetItem* p_last = this->mpBoostTree->invisibleRootItem();
  for (const auto& subpath : subpaths)
  {
    QTreeWidgetItem* p_item = nullptr;
    for (int i = 0; i < p_last->childCount(); ++i)
    {
      auto child = p_last->child(i);
      if (child->text(0).toStdString() == subpath)
      {
        p_item = child;
      }
    }
    if (p_item == nullptr)
    {
      p_item = new QTreeWidgetItem();
      p_item->setText(0, QString::fromStdString(subpath));
      p_last->addChild(p_item);
      p_item->setExpanded(true);
    }
    p_last = p_item;
  }

  return p_last;
}

void cedar::proc::gui::BoostControl::elementAdded(QString elementName)
{
  cedar::proc::ElementPtr element = this->mGroup->getElement(elementName.toStdString());

  if (auto boost = boost::dynamic_pointer_cast<cedar::proc::sources::Boost>(element))
  {
    this->addBoost(boost);
  }
  else if (auto group = boost::dynamic_pointer_cast<cedar::proc::Group>(element))
  {
    std::string path = this->mGroup->findPath(group);

    this->connectRenameSignal(group);

    this->addGroup(group);
  }
}

void cedar::proc::gui::BoostControl::elementNameChanged()
{
  auto name = cedar::aux::asserted_cast<cedar::aux::Parameter*>(QObject::sender());
  auto element = dynamic_cast<cedar::proc::Element*>(name->getOwner());

  auto iter = this->mElementNames.find(element);
  if (iter == this->mElementNames.end())
  {
    return; // can happen if the element is being deleted
  }

  QString old_name = iter->second;
  QString new_name = QString::fromStdString(this->mGroup->findPath(element));

  auto items = this->mpBoostTree->findItems(old_name, 0);

  for (int i = 0; i < items.size(); ++i)
  {
    items.at(i)->setText(0, new_name);
  }

  this->mElementNames[element] = new_name;
}

void cedar::proc::gui::BoostControl::connectRenameSignal(cedar::proc::ElementPtr element)
{
  cedar::aux::ParameterPtr name = element->getParameter("name");
  QObject::connect(name.get(), SIGNAL(valueChanged()), this, SLOT(elementNameChanged()));
}

void cedar::proc::gui::BoostControl::addBoost(cedar::proc::sources::BoostPtr boost)
{
  std::string path = this->mGroup->findPath(boost);
  this->mElementNames[boost.get()] = QString::fromStdString(path);

  this->connectRenameSignal(boost);

  QStringList labels;
  labels << QString::fromStdString(boost->getName());

  auto p_item = new QTreeWidgetItem(labels);
  p_item->setData(0, Qt::UserRole, QString::fromStdString(path));
  p_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  p_item->setToolTip(0, "Double-click to show the boost in the architecture.");

  // this retrieves the group item (or creates it if it does not exist)
  auto group_item = this->getGroupItem(path);
  group_item->addChild(p_item);

  // create checkbox for enabling/disabling boost (by creating a gui::BoolParameter and passing the boost's parameter)
  cedar::aux::ParameterPtr active_parameter = boost->getParameter("active");
  cedar::aux::gui::Parameter* p_enabler
    = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(active_parameter)->allocateRaw();
  this->mpBoostTree->setItemWidget(p_item, 1, p_enabler);
  p_enabler->setParameter(active_parameter);

  // create spinbox for setting boost strength (by creating a gui::DoubleParameter and passing the boost's parameter)
  cedar::aux::ParameterPtr strength_parameter = boost->getParameter("strength");
  cedar::aux::gui::Parameter* p_strength
    = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(strength_parameter)->allocateRaw();
  this->mpBoostTree->setItemWidget(p_item, 2, p_strength);
  p_strength->setParameter(strength_parameter);
}
