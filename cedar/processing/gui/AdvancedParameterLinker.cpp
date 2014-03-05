/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        AdvancedParameterLinker.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 03 04

    Description: Source file for the class cedar::proc::gui::AdvancedParameterLinker.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/AdvancedParameterLinker.h"
#include "cedar/processing/Element.h"
#include "cedar/processing/Group.h"
#include "cedar/auxiliaries/ParameterLink.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::AdvancedParameterLinker::AdvancedParameterLinker()
{
  this->setupUi(this);
  this->fillLinkTypes();

  this->mpAddButton->setEnabled(false);
  this->mpRemoveButton->setEnabled(false);

  QObject::connect(this->mpLinkTree, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));

  QObject::connect(this->mpSourceElementName, SIGNAL(textEdited(const QString&)), this, SLOT(linkInfoChanged()));
  QObject::connect(this->mpSourceParameterName, SIGNAL(textEdited(const QString&)), this, SLOT(linkInfoChanged()));
  QObject::connect(this->mpTargetElementName, SIGNAL(textEdited(const QString&)), this, SLOT(linkInfoChanged()));
  QObject::connect(this->mpTargetParameterName, SIGNAL(textEdited(const QString&)), this, SLOT(linkInfoChanged()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::AdvancedParameterLinker::linkInfoChanged()
{
  std::string new_source_element_name = this->mpSourceElementName->text().toStdString();
  std::string new_source_parameter_name = this->mpSourceParameterName->text().toStdString();
  std::string new_target_element_name = this->mpTargetElementName->text().toStdString();
  std::string new_target_parameter_name = this->mpTargetParameterName->text().toStdString();

  bool changed = false;

  auto& current = this->getCurrentLinkInfo();

  auto group = this->getCurrentGroup();

  try
  {
    cedar::proc::ElementPtr new_source_element;
    if (!new_source_element_name.empty())
    {
      new_source_element = group->getElement(new_source_element_name);
    }
    else
    {
      new_source_element = group->shared_from_this();
    }

    CEDAR_ASSERT(new_source_element);
    current.mSourceElement = new_source_element;
    changed = true;
  }
  catch (cedar::aux::InvalidNameException)
  {
  }

  try
  {
    cedar::proc::ElementPtr new_target_element;
    if (!new_target_element_name.empty())
    {
      new_target_element = group->getElement(new_target_element_name);
    }
    else
    {
      new_target_element = group->shared_from_this();
    }

    CEDAR_ASSERT(new_target_element);
    current.mTargetElement = new_target_element;
    changed = true;
  }
  catch (cedar::aux::InvalidNameException)
  {
  }

  cedar::aux::ParameterPtr new_source_parameter;
  if (current.mSourceElement)
  {
    try
    {
      new_source_parameter = current.mSourceElement->getParameter(new_source_parameter_name);
    }
    catch (cedar::aux::UnknownNameException)
    {
    }
  }

  cedar::aux::ParameterPtr new_target_parameter;
  if (current.mTargetElement)
  {
    try
    {
      new_target_parameter = current.mTargetElement->getParameter(new_target_parameter_name);
    }
    catch (cedar::aux::UnknownNameException)
    {
    }
  }

  if (new_source_parameter && new_target_parameter)
  {
    current.mParameterLink->setLinkedParameters(new_source_parameter, new_target_parameter);
    changed = true;
  }

  if (changed)
  {
    auto selected = this->mpLinkTree->selectedItems();
    CEDAR_DEBUG_ASSERT(selected.size() == 1);
    this->updateLinkItem(selected[0], current);
  }
}

void cedar::proc::gui::AdvancedParameterLinker::itemSelectionChanged()
{
  bool valid_selection = false;
  try
  {
    auto link_info = this->getCurrentLinkInfo();
    this->show(link_info);
    valid_selection = true;
  }
  catch (cedar::aux::NotFoundException)
  {
  }

  this->mpAddButton->setEnabled(valid_selection);
  this->mpRemoveButton->setEnabled(valid_selection);
}

cedar::proc::Group* cedar::proc::gui::AdvancedParameterLinker::getCurrentGroup() const
{
  auto items = this->mpLinkTree->selectedItems();

  if (items.size() != 1)
  {
    CEDAR_THROW(cedar::aux::NotFoundException, "Could not find parameter link info: wrong item selection.");
  }

  auto item = items[0];

  if (item->parent())
  {
    return (cedar::proc::Group*)(item->parent()->data(0, Qt::UserRole).value<void*>());
  }
  else
  {
    return this->mGroup.get();
  }
}

cedar::proc::Group::ParameterLinkInfo& cedar::proc::gui::AdvancedParameterLinker::getCurrentLinkInfo() const
{
  auto items = this->mpLinkTree->selectedItems();

  if (items.size() != 1)
  {
    CEDAR_THROW(cedar::aux::NotFoundException, "Could not find parameter link info: wrong item selection.");
  }

  auto item = items[0];
  auto p_link = (cedar::aux::ParameterLink*)(item->data(0, Qt::UserRole).value<void*>());

  auto group = this->getCurrentGroup();

  for (auto& link : group->getParameterLinks())
  {
    if (link.mParameterLink.get() == p_link)
    {
      return link;
    }
  }
  CEDAR_THROW(cedar::aux::NotFoundException, "Could not find parameter link info.");
}

void cedar::proc::gui::AdvancedParameterLinker::show(const cedar::proc::Group::ParameterLinkInfo& linkInfo)
{
  this->mpSourceElementName->setText(QString::fromStdString(linkInfo.getSourceElementPath()));
  this->mpSourceParameterName->setText(QString::fromStdString(linkInfo.getSourceParameterPath()));
  this->mpTargetElementName->setText(QString::fromStdString(linkInfo.getTargetElementPath()));
  this->mpTargetParameterName->setText(QString::fromStdString(linkInfo.getTargetParameterPath()));

  this->mpLinkParameters->display(linkInfo.mParameterLink);
}

void cedar::proc::gui::AdvancedParameterLinker::setGroup(cedar::proc::GroupPtr group)
{
  this->mGroup = group;

  this->makeGroupItem(this->mpLinkTree->invisibleRootItem(), group);
  this->addGroup(this->mpLinkTree->invisibleRootItem(), this->mGroup);
}

void cedar::proc::gui::AdvancedParameterLinker::addGroup(QTreeWidgetItem* pRoot, cedar::proc::GroupPtr group)
{
  for (const auto& link : group->getParameterLinks())
  {
    auto p_item = new QTreeWidgetItem();
    this->updateLinkItem(p_item, link);

    p_item->setData(0, Qt::UserRole, QVariant::fromValue((void*)link.mParameterLink.get()));

    pRoot->addChild(p_item);
  }

  for (auto name_element_pair : group->getElements())
  {
    auto element = name_element_pair.second;
    if (auto subgroup = boost::dynamic_pointer_cast<cedar::proc::Group>(element))
    {
      auto p_item = new QTreeWidgetItem();
      pRoot->addChild(p_item);
      this->makeGroupItem(p_item, subgroup);
      this->addGroup(p_item, subgroup);
    }
  }
}

void cedar::proc::gui::AdvancedParameterLinker::updateLinkItem
     (
       QTreeWidgetItem* pItem,
       const cedar::proc::Group::ParameterLinkInfo& linkInfo
     )
{
  QString source_id;
  if (linkInfo.mSourceElement != linkInfo.mGroup.lock())
  {
    source_id += QString::fromStdString(linkInfo.getSourceElementPath()) + ".";
  }
  source_id += QString::fromStdString(linkInfo.getSourceParameterPath());
  pItem->setText(0, source_id);

  QString target_id;
  if (linkInfo.mTargetElement != linkInfo.mGroup.lock())
  {
    target_id += QString::fromStdString(linkInfo.getTargetElementPath()) + ".";
  }
  target_id += QString::fromStdString(linkInfo.getTargetParameterPath());
  pItem->setText(1, target_id);
}

void cedar::proc::gui::AdvancedParameterLinker::makeGroupItem(QTreeWidgetItem* pItem, cedar::proc::GroupPtr group)
{
  pItem->setText(0, QString::fromStdString(group->getName()));
  pItem->setFirstColumnSpanned(true);
  pItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)(group.get())));
}

void cedar::proc::gui::AdvancedParameterLinker::fillLinkTypes()
{
  this->mpLinkTypeSelector->clear();

  std::vector <std::string> types;
  cedar::aux::ParameterLinkFactoryManagerSingleton::getInstance()->listTypes(types);
  for (auto entry : types)
  {
    this->mpLinkTypeSelector->addItem(QString::fromStdString(entry));
  }
}
