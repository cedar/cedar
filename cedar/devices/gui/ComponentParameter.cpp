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

    File:        ComponentParameter.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 03 05

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/gui/ComponentParameter.h"
#include "cedar/devices/ComponentParameter.h"
#include "cedar/devices/ComponentSlot.h"
#include "cedar/devices/RobotManager.h"
#include "cedar/devices/Robot.h"
#include "cedar/devices/exceptions.h"
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/Factory.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialog>
#include <QTreeWidget>
#include <QDialogButtonBox>


//----------------------------------------------------------------------------------------------------------------------
// associate gui representation class with the underlying parameter class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered = cedar::aux::gui::ParameterFactorySingleton::getInstance()->add
      <
        cedar::dev::ComponentParameter,
        cedar::dev::gui::ComponentParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::gui::ComponentParameter::ComponentParameter()
{
  auto p_layout = new QHBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);

  this->mpComponentPathDisplay = new QLineEdit();
  this->mpComponentPathDisplay->setReadOnly(true);
  p_layout->addWidget(this->mpComponentPathDisplay, 1);

  auto p_button = new QPushButton(QIcon(":/cedar/dev/gui/icons/select_component.svg"), "");
  p_button->setIconSize(QSize(16, 16));
  p_layout->addWidget(p_button, 0);

  this->setLayout(p_layout);

  QObject::connect(p_button, SIGNAL(clicked()), this, SLOT(openComponentSelector()));
  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterChanged()));

  p_button->setMinimumWidth(20);
  p_button->setMaximumWidth(30);
}

cedar::dev::gui::ComponentParameter::~ComponentParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::gui::ComponentParameter::parameterChanged()
{
  this->mParameter = boost::dynamic_pointer_cast<cedar::dev::ComponentParameter>(this->getParameter());
  QObject::connect(this->mParameter.get(), SIGNAL(valueChanged()), this, SLOT(updatePathText()));

  bool blocked = this->mpComponentPathDisplay->blockSignals(true);
  this->updatePathText();
  this->mpComponentPathDisplay->blockSignals(blocked);
}

void cedar::dev::gui::ComponentParameter::openComponentSelector()
{
  auto p_dialog = new QDialog();
  p_dialog->setModal(true);
  auto p_layout = new QVBoxLayout();

  auto p_tree = new QTreeWidget();
  p_layout->addWidget(p_tree, 1);
  p_tree->setColumnCount(1);
  p_tree->setHeaderHidden(true);
  this->fillRobots(p_tree->invisibleRootItem());

  auto p_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  QObject::connect(p_buttons, SIGNAL(accepted()), p_dialog, SLOT(accept()));
  QObject::connect(p_buttons, SIGNAL(rejected()), p_dialog, SLOT(reject()));
  p_layout->addWidget(p_buttons);

  p_dialog->setLayout(p_layout);

  int res = p_dialog->exec();
  if (res == QDialog::Accepted)
  {
    QTreeWidgetItem* p_selected = p_tree->currentItem();

    //TODO proper error handling
    if (p_selected == NULL || p_selected->parent() == NULL || p_selected->parent() == p_tree->invisibleRootItem())
    {
      return;
    }

    std::string robot_name = p_selected->parent()->text(0).toStdString();
    std::string component_name = p_selected->text(0).toStdString();

    cedar::dev::RobotPtr robot = cedar::dev::RobotManagerSingleton::getInstance()->getRobot(robot_name);
    CEDAR_ASSERT(robot);
    cedar::dev::ComponentSlotPtr slot = robot->getComponentSlot(component_name);
    this->selectComponent(slot);
  }
}

void cedar::dev::gui::ComponentParameter::selectComponent(cedar::dev::ComponentSlotPtr slot)
{
  cedar::aux::asserted_pointer_cast<cedar::dev::ComponentParameter>(this->getParameter())->setValue(slot);
}

void cedar::dev::gui::ComponentParameter::updatePathText()
{
  std::string text;
  try
  {
    text = this->mParameter->getStringRepresentation();
  }
  catch (const cedar::dev::NoComponentSelectedException&)
  {
    // nothing to do -- text remains empty.
  }
  this->mpComponentPathDisplay->setText(QString::fromStdString(text));
}

void cedar::dev::gui::ComponentParameter::fillRobots(QTreeWidgetItem* pItem)
{
  auto robot_names = cedar::dev::RobotManagerSingleton::getInstance()->getRobotNames();

  for (auto iter = robot_names.begin(); iter != robot_names.end(); ++iter)
  {
    const std::string& robot_name = *iter;
    cedar::dev::RobotPtr robot = cedar::dev::RobotManagerSingleton::getInstance()->getRobot(robot_name);

    auto p_robot_item = new QTreeWidgetItem();
    p_robot_item->setText(0, QString::fromStdString(robot_name));
    this->fillComponents(p_robot_item, robot);

    pItem->addChild(p_robot_item);
  }
}

void cedar::dev::gui::ComponentParameter::fillComponents(QTreeWidgetItem* pItem, cedar::dev::RobotPtr robot)
{
  auto components = robot->listComponentSlots();

  for (auto iter = components.begin(); iter != components.end(); ++iter)
  {
    const std::string& component_name = *iter;

    auto p_component_item = new QTreeWidgetItem();
    p_component_item->setText(0, QString::fromStdString(component_name));

    pItem->addChild(p_component_item);
  }
}

