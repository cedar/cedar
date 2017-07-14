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
#include <string>


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
:
mpComponentDialog(NULL),
mpComponentTree(NULL)
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
  QObject::connect(this->mParameter.get(), SIGNAL(reselect(const std::string&, const std::string&)), this, SLOT(reselect(const std::string&, const std::string&)));

  bool blocked = this->mpComponentPathDisplay->blockSignals(true);
  this->updatePathText();
  this->mpComponentPathDisplay->blockSignals(blocked);
}

void cedar::dev::gui::ComponentParameter::componentDoubleClicked()
{
  QTreeWidgetItem* p_selected = this->mpComponentTree->currentItem();

  //TODO proper error handling
  if (p_selected == NULL || p_selected->parent() == NULL || p_selected->parent() == this->mpComponentTree->invisibleRootItem())
  {
    return;
  }

  this->mpComponentDialog->accept();
}

void cedar::dev::gui::ComponentParameter::openComponentSelector()
{
  if (mpComponentDialog != NULL)
  {
    delete this->mpComponentDialog;
  }
  this->mpComponentDialog = new QDialog();
  this->mpComponentDialog->setWindowTitle("Please select a component...");

  this->mpComponentDialog->setModal(true);
  auto p_layout = new QVBoxLayout();

  this->mpComponentTree = new QTreeWidget();
  p_layout->addWidget(this->mpComponentTree, 1);
  this->mpComponentTree->setColumnCount(1);
  this->mpComponentTree->setHeaderHidden(true);
  this->fillRobots(this->mpComponentTree->invisibleRootItem());

  QObject::connect
  (
    this->mpComponentTree,
    SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
    this,
    SLOT(componentDoubleClicked())
  );

  auto p_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  QObject::connect(p_buttons, SIGNAL(accepted()), this->mpComponentDialog, SLOT(accept()));
  QObject::connect(p_buttons, SIGNAL(rejected()), this->mpComponentDialog, SLOT(reject()));
  p_layout->addWidget(p_buttons);

  this->mpComponentDialog->setLayout(p_layout);

  int res = this->mpComponentDialog->exec();
  if (res == QDialog::Accepted)
  {
    QTreeWidgetItem* p_selected = this->mpComponentTree->currentItem();

    //TODO proper error handling
    if (p_selected == NULL || p_selected->parent() == NULL || p_selected->parent() == this->mpComponentTree->invisibleRootItem())
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

void cedar::dev::gui::ComponentParameter::reselect(const std::string& , const std::string& )
{
//As far as I can see is this code only picking up a signal from ComponentParameter and to perform a function on ComponentParameter. No reason to be here at all!
  //Just to be sure that I did not overlook anything I will keep this function and throw an exception!
CEDAR_THROW(cedar::dev::ResourceNotAvailableException,"This Slot is not implemented. Something is connected to cedar::dev::gui::ComponentParameter::reselect! This is a bug and needs to be fixed!")


//  cedar::dev::RobotPtr robot = cedar::dev::RobotManagerSingleton::getInstance()->getRobot(robot_name);
//  CEDAR_ASSERT(robot);
//  cedar::dev::ComponentSlotPtr slot = robot->getComponentSlot(slot_name);
//  this->selectComponent(slot);
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
    text= "";
  }

  this->mpComponentPathDisplay->setText(QString::fromStdString(text));

  if(!text.empty())
  {
    this->mParameter->setRobotName(text.substr(0, text.find(".")));
    this->mParameter->setSlotName(text.substr(text.find(".")+1));
  }
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
    p_robot_item->setFlags(p_robot_item->flags() & (~Qt::ItemIsSelectable));

    if(!robot->listComponentSlots().empty())
    {
      this->fillComponents(p_robot_item, robot);
      pItem->addChild(p_robot_item);
    }
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

