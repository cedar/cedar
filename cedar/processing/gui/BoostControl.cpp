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
#include "cedar/processing/sources/Boost.h"
#include "cedar/processing/Network.h"
#include "cedar/processing/Element.h"
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <QHeaderView>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::BoostControl::BoostControl()
{
  this->setupUi(this);

  QObject::connect(this, SIGNAL(elementAddedSignal(QString)), this, SLOT(elementAdded(QString)));
  QObject::connect(this, SIGNAL(boostRemovedSignal(QString)), this, SLOT(boostRemoved(QString)));

  this->mpBoostTree->header()->setResizeMode(0, QHeaderView::Stretch);
  this->mpBoostTree->header()->setResizeMode(1, QHeaderView::ResizeToContents);
}

cedar::proc::gui::BoostControl::~BoostControl()
{
  if (this->mNetwork)
  {
    this->mElementAddedConnection.disconnect();
    this->mElementRemovedConnection.disconnect();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::BoostControl::setNetwork(cedar::proc::NetworkPtr network)
{
  if (this->mNetwork)
  {
    this->mElementAddedConnection.disconnect();
    this->mElementRemovedConnection.disconnect();
  }

  this->mpBoostTree->clear();
  this->mBoostNames.clear();

  this->mNetwork = network;

  for (auto iter = this->mNetwork->getElements().begin(); iter != this->mNetwork->getElements().end(); ++iter)
  {
    this->translateElementAddedSignal(iter->second);
  }

  this->mElementAddedConnection
    = this->mNetwork->connectToNewElementAddedSignal(boost::bind(&cedar::proc::gui::BoostControl::translateElementAddedSignal, this, _1));
  this->mElementRemovedConnection
    = this->mNetwork->connectToElementRemovedSignal(boost::bind(&cedar::proc::gui::BoostControl::translateElementRemovedSignal, this, _1));
}

void cedar::proc::gui::BoostControl::translateElementRemovedSignal(cedar::proc::ConstElementPtr element)
{
  if (auto boost = boost::dynamic_pointer_cast<cedar::proc::sources::ConstBoost>(element))
  {
    auto iter = this->mBoostNames.find(boost.get());
    if (iter != this->mBoostNames.end())
    {
      this->mBoostNames.erase(iter);
    }

    emit boostRemovedSignal(QString::fromStdString(element->getName()));
  }
}

void cedar::proc::gui::BoostControl::boostRemoved(QString elementName)
{
  auto items = this->mpBoostTree->findItems(elementName, 0);

  for (int i = 0; i < items.size(); ++i)
  {
    delete items.at(i);
  }
}

void cedar::proc::gui::BoostControl::translateElementAddedSignal(cedar::proc::ElementPtr element)
{
  emit elementAddedSignal(QString::fromStdString(element->getName()));
}

void cedar::proc::gui::BoostControl::elementAdded(QString elementName)
{
  cedar::proc::ElementPtr element = this->mNetwork->getElement(elementName.toStdString());

  if (cedar::proc::sources::BoostPtr boost = boost::dynamic_pointer_cast<cedar::proc::sources::Boost>(element))
  {
    this->addBoost(boost);
  }
}

void cedar::proc::gui::BoostControl::boostNameChanged()
{
  auto name = cedar::aux::asserted_cast<cedar::aux::Parameter*>(QObject::sender());
  auto boost = dynamic_cast<cedar::proc::sources::Boost*>(name->getOwner());

  auto iter = this->mBoostNames.find(boost);
  if (iter == this->mBoostNames.end())
  {
    return; // can happen if the boost is just now being deleted
  }

  QString old_name = iter->second;
  QString new_name = QString::fromStdString(boost->getName());

  auto items = this->mpBoostTree->findItems(old_name, 0);

  for (int i = 0; i < items.size(); ++i)
  {
    items.at(i)->setText(0, new_name);
  }

  this->mBoostNames[boost] = new_name;
}

void cedar::proc::gui::BoostControl::addBoost(cedar::proc::sources::BoostPtr boost)
{
  this->mBoostNames[boost.get()] = QString::fromStdString(boost->getName());

  cedar::aux::ParameterPtr name = boost->getParameter("name");
  QObject::connect(name.get(), SIGNAL(valueChanged()), this, SLOT(boostNameChanged()));

  QStringList labels;
  labels << QString::fromStdString(boost->getName());

  auto p_item = new QTreeWidgetItem(labels);
  p_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

  this->mpBoostTree->invisibleRootItem()->addChild(p_item);

  cedar::aux::ParameterPtr active_parameter = boost->getParameter("active");
  cedar::aux::gui::Parameter* p_enabler
    = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(active_parameter)->allocateRaw();
  this->mpBoostTree->setItemWidget(p_item, 1, p_enabler);
  p_enabler->setParameter(active_parameter);

  cedar::aux::ParameterPtr strength_parameter = boost->getParameter("strength");
  cedar::aux::gui::Parameter* p_strength
    = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(strength_parameter)->allocateRaw();
  this->mpBoostTree->setItemWidget(p_item, 2, p_strength);
  p_strength->setParameter(strength_parameter);
}
