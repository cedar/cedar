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

    File:        RobotManager.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 02 27

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/gui/RobotManager.h"
#include "cedar/devices/gui/RobotCard.h"
#include "cedar/devices/RobotManager.h"
#include "cedar/devices/Robot.h"
#include "cedar/devices/Component.h"
#include "cedar/devices/Channel.h"
#include "cedar/devices/exceptions.h"
#include "cedar/auxiliaries/gui/ResourceDialog.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <QMenu>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::gui::RobotManager::RobotManager()
:
mpComponentsNode(NULL),
mpChannelsNode(NULL)
{
  this->setupUi(this);

  // construct menu of the load button
  QMenu* load_menu = new QMenu();
  QAction* p_load_config = load_menu->addAction("configuration ...");
  QObject::connect(p_load_config, SIGNAL(triggered()), this, SLOT(loadConfigurationTriggered()));

  QAction* p_load_config_from_resource = load_menu->addAction("configuration from resource ...");
  QObject::connect(p_load_config_from_resource, SIGNAL(triggered()), this, SLOT(loadConfigurationFromResourceTriggered()));

  this->mpLoadButton->setMenu(load_menu);

  QObject::connect(mpCloseButton, SIGNAL(clicked(void)), this, SLOT(closeWindow()));

  QObject::connect(this, SIGNAL(robotNameAdded(QString)), this, SLOT(addRobotName(QString)));

  mRobotAddedConnection = cedar::dev::RobotManagerSingleton::getInstance()->
                                                    connectToRobotNameAddedSignal
                                                    (
                                                      boost::bind
                                                      (
                                                        &cedar::dev::gui::RobotManager::robotAddedSignalTranslator,
                                                        this,
                                                        _1
                                                      )
                                                    );

  QObject::connect(this, SIGNAL(robotConfigurationLoaded(QString)), this, SLOT(updateRobotConfiguration(QString)));

  mRobotConfigurationChangedConnection = cedar::dev::RobotManagerSingleton::getInstance()->
                                           connectToRobotConfigurationChangedSignal
                                           (
                                             boost::bind
                                             (
                                               &cedar::dev::gui::RobotManager::robotConfigurationLoadedSignalTranslator,
                                               this,
                                               _1
                                             )
                                           );

  mRobotRemovedConnection = cedar::dev::RobotManagerSingleton::getInstance()->
                                connectToRobotRemovedSignal
                                (
                                  boost::bind
                                  (
                                    &cedar::dev::gui::RobotManager::robotRemoved,
                                    this,
                                    _1
                                  )
                                );

  QObject::connect(this->mpRobotSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(robotNameSelected(int)));
  QObject::connect
  (
    this->mpRobotTree,
    SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
    this,
    SLOT(partSelected(QTreeWidgetItem*, QTreeWidgetItem*))
  );

  QObject::connect(this->mpRemoveButton, SIGNAL(clicked()), this, SLOT(removeClicked()));

  this->fillSimpleRobotList();
  this->fillExistingRobots();

  this->mpSimpleRobotIconList->viewport()->setAcceptDrops(true);

  mRobotRenamedConnection = cedar::dev::RobotManagerSingleton::getInstance()->connectToRobotNameChangedSignal
  (
    boost::bind
    (
      &cedar::dev::gui::RobotManager::robotRenamed,
      this,
      _1,
      _2
    )
  );
}

cedar::dev::gui::RobotManager::~RobotManager()
{
  mRobotAddedConnection.disconnect();
  mRobotRemovedConnection.disconnect();
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::gui::RobotManager::robotRenamed(const std::string& oldName, const std::string& newName)
{
  //!@todo This should probably be emitted using Qt signals in case it gets called outside the GUI thread

  for (int i = 0; i < this->mpRobotSelector->count(); ++i)
  {
    if (this->mpRobotSelector->itemText(i).toStdString() == oldName)
    {
      // update name and user data
      this->mpRobotSelector->setItemText(i, QString::fromStdString(newName));
      this->mpRobotSelector->setItemData(i, QString::fromStdString(newName));
      return;
    }
  }

  CEDAR_DEBUG_NON_CRITICAL_ASSERT(false && "Could not find previous robot name in selector.");
}

void cedar::dev::gui::RobotManager::fillExistingRobots()
{
  std::vector<std::string> robot_names = cedar::dev::RobotManagerSingleton::getInstance()->getRobotNames();

//  if(robot_names.size() == 0)
//  {
    // if there is no robot yet, add an empty placeholder
    // Always add this robot?

//    return;
//  }

  bool doesDummyExist = false;

  for (auto robot_name_iter = robot_names.begin(); robot_name_iter != robot_names.end(); ++robot_name_iter)
  {
    const std::string& robot_name = *robot_name_iter;
    this->addRobotName(QString::fromStdString(robot_name));

    try
    {
      std::string nameTest = cedar::dev::RobotManagerSingleton::getInstance()->getRobotTemplateName(robot_name);
    }
    catch(cedar::dev::TemplateNotFoundException e)
    {
      //We have a dummy Robot!
      doesDummyExist = true;
    }
  }

  if(!doesDummyExist)
  {
    simpleModeAdd();
  }

}

void cedar::dev::gui::RobotManager::removeClicked()
{
    cedar::dev::RobotManagerSingleton::getInstance()->removeRobot(this->getSelectedRobotName());
}

void cedar::dev::gui::RobotManager::closeWindow()
{
    closeRobotManager();
}

void cedar::dev::gui::RobotManager::robotRemoved(const std::string& robotName)
{
  int index = this->mpRobotSelector->findData(QString::fromStdString(robotName));
  CEDAR_ASSERT(index >= 0);

  this->mpRobotSelector->removeItem(index);
}

void cedar::dev::gui::RobotManager::fillSimpleRobotList()
{
  std::vector<std::string> robot_names = cedar::dev::RobotManagerSingleton::getInstance()->getRobotTemplateNames();

  for (size_t i = 0; i < robot_names.size(); ++i)
  {
    const std::string& robot_name = robot_names.at(i);
    auto p_item = new QListWidgetItem(QString::fromStdString(robot_name));

    auto robot_template = cedar::dev::RobotManagerSingleton::getInstance()->getTemplate(robot_name);

    p_item->setFlags(p_item->flags() | Qt::ItemIsDragEnabled);
    p_item->setIcon(QIcon(QString::fromStdString(robot_template.getIconPath())));
    p_item->setData(Qt::UserRole, QString::fromStdString(robot_name));

    this->mpSimpleRobotIconList->addItem(p_item);
  }
}

void cedar::dev::gui::RobotManager::simpleModeAdd()
{
  std::string new_robot_name = cedar::dev::RobotManagerSingleton::getInstance()->getNewRobotName();
  cedar::dev::RobotManagerSingleton::getInstance()->addRobotName(new_robot_name);
}

void cedar::dev::gui::RobotManager::partSelected(QTreeWidgetItem* pCurrent, QTreeWidgetItem*)
{
  if
  (
    pCurrent == NULL
    || this->mpRobotSelector->currentIndex() == -1
    || this->getSelectedRobotName().empty()
    || !this->getSelectedRobot()
  )
  {
    this->mpPropertyPane->resetContents();
  }
  else
  {
    cedar::aux::ConfigurablePtr to_display;
    if (pCurrent->parent() == this->mpComponentsNode)
    {
      std::string component_name = pCurrent->text(0).toStdString();
      to_display = this->getSelectedRobot()->getComponent(component_name);
    }
    else if (pCurrent->parent() == this->mpChannelsNode)
    {
      std::string channel_name = pCurrent->text(0).toStdString();
      try
      {
        to_display = this->getSelectedRobot()->getChannel(channel_name);
      }
      catch (const cedar::dev::ChannelNotInstantiatedException&)
      {
        // ok -- nothing to do
      }
    }

    if (to_display)
    {
      this->mpPropertyPane->display(to_display);
    }
    else
    {
      this->mpPropertyPane->resetContents();
    }
  }
}

void cedar::dev::gui::RobotManager::addRobotClicked()
{
  QString name
    = QInputDialog::getText
      (
        this,
        "Adding robot",
        "Enter name for the new robot"
      );

  if (!name.isEmpty())
  {
    try
    {
      cedar::dev::RobotManagerSingleton::getInstance()->addRobotName(name.toStdString());
    }
    catch (const cedar::aux::DuplicateNameException& e)
    {
      cedar::aux::LogSingleton::getInstance()->error
      (
        e.exceptionInfo(),
        "void cedar::dev::gui::RobotManager::addRobotClicked()"
      );
      QMessageBox::critical
      (
        this,
        "Name already in use.",
        QString::fromStdString(e.exceptionInfo())
      );
    }
  }
}

void cedar::dev::gui::RobotManager::loadConfiguration(const cedar::aux::Path& configuration)
{
  std::string robot_name = this->getSelectedRobotName();
  try
  {
    cedar::dev::RobotManagerSingleton::getInstance()->loadRobotConfiguration(robot_name, configuration);
  }
  catch (const cedar::aux::ResourceNotFoundException& e)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      e.exceptionInfo(),
      "void cedar::dev::gui::RobotManager::loadConfigurationFromResourceTriggered()"
    );
    QMessageBox::critical
    (
      this,
      "Resource not found.",
      QString::fromStdString(e.exceptionInfo())
    );
  }
  catch (const cedar::dev::ChannelConfigurationNotFoundException& e)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      e.exceptionInfo(),
      "void cedar::dev::gui::RobotManager::loadConfigurationFromResourceTriggered()"
    );
    QMessageBox::critical
    (
      this,
      "Invalid configuration file.",
      QString::fromStdString(e.exceptionInfo())
    );
  }
}

void cedar::dev::gui::RobotManager::loadConfigurationFromResourceTriggered()
{
  std::vector<std::string> extensions;
  extensions.push_back(".json");
  cedar::aux::Path resource = cedar::aux::gui::ResourceDialog::openResource(this, extensions);

  if (!resource.empty())
  {
    this->loadConfiguration(resource);
  }
}

cedar::dev::RobotPtr cedar::dev::gui::RobotManager::getSelectedRobot() const
{
  return cedar::dev::RobotManagerSingleton::getInstance()->getRobot(this->getSelectedRobotName());
}

std::string cedar::dev::gui::RobotManager::getSelectedRobotName() const
{
  //!@todo Proper exception.
  CEDAR_ASSERT (this->mpRobotSelector->currentIndex() != -1);

  return this->mpRobotSelector->itemData(this->mpRobotSelector->currentIndex()).toString().toStdString();
}

void cedar::dev::gui::RobotManager::loadConfigurationTriggered()
{
  QString file = QFileDialog::getOpenFileName(this, "Select a configuration file to load.", "", "*");

  if (!file.isEmpty())
  {
    this->loadConfiguration(file.toStdString());
  }
}

void cedar::dev::gui::RobotManager::robotNameSelected(int nameIndex)
{
  bool something_selected = (nameIndex != -1);
  this->mpLoadButton->setEnabled(something_selected);
  // TODO enable once they are implemented
//  this->mpRenameButton->setEnabled(nothing_selected);
//  this->mpSaveButton->setEnabled(nothing_selected);
  this->mpRemoveButton->setEnabled(something_selected);

  if (nameIndex == -1)
  {
    this->deselectRobot();
  }
  else
  {
    this->selectRobot(this->getSelectedRobotName());
  }
}

void cedar::dev::gui::RobotManager::updateRobotConfiguration(QString addedRobotName)
{
  this->selectRobot(addedRobotName.toStdString());

  //Lets try to send the signal via the non-gui class
  cedar::dev::RobotManagerSingleton::getInstance()->emitConfigurationChanged();

//  emit configurationChanged();
}

void cedar::dev::gui::RobotManager::deselectRobot()
{
  if (this->mpComponentsNode != NULL)
  {
    delete this->mpComponentsNode;
    this->mpComponentsNode = NULL;
  }

  if (this->mpChannelsNode != NULL)
  {
    delete this->mpChannelsNode;
    this->mpChannelsNode = NULL;
  }
}

void cedar::dev::gui::RobotManager::selectRobot(const std::string& robotName)
{
  this->deselectRobot();

  QStringList components;
  components << "components";
  this->mpComponentsNode = new QTreeWidgetItem(components);
  this->mpRobotTree->addTopLevelItem(this->mpComponentsNode);
  this->mpComponentsNode->setExpanded(true);
  this->mpComponentsNode->setFlags(this->mpComponentsNode->flags() & (~Qt::ItemIsSelectable));

  QStringList channels;
  channels << "channels";
  this->mpChannelsNode = new QTreeWidgetItem(channels);
  this->mpRobotTree->addTopLevelItem(this->mpChannelsNode);
  this->mpChannelsNode->setExpanded(true);
  this->mpChannelsNode->setFlags(this->mpChannelsNode->flags() & (~Qt::ItemIsSelectable));

  cedar::dev::RobotPtr robot = cedar::dev::RobotManagerSingleton::getInstance()->getRobot(robotName);

  std::vector<std::string> component_slots = robot->listComponentSlots();
  for (auto slot_iter = component_slots.begin(); slot_iter != component_slots.end(); ++slot_iter)
  {
    const std::string& slot_name = *slot_iter;
    cedar::dev::ConstComponentPtr component = robot->getComponent(slot_name);

    QStringList component_infos;
    component_infos << QString::fromStdString(slot_name);

    if (component)
    {
      component_infos
        << QString::fromStdString(cedar::dev::ComponentManagerSingleton::getInstance()->getTypeId(component));
    }
    else
    {
      component_infos << "(null)";
    }

    QTreeWidgetItem* p_item = new QTreeWidgetItem(component_infos);
    this->mpComponentsNode->addChild(p_item);
  }

  std::vector<std::string> channel_names = robot->listChannels();
  for (auto channel_iter = channel_names.begin(); channel_iter != channel_names.end(); ++channel_iter)
  {
    const std::string& channel_name = *channel_iter;
    cedar::dev::ConstChannelPtr channel;
    try
    {
      channel = robot->getChannel(channel_name);
    }
    catch (const cedar::dev::ChannelNotInstantiatedException&)
    {
      // ok -- do nothing
    }

    QStringList channel_infos;
    channel_infos << QString::fromStdString(channel_name);

    if (channel)
    {
      channel_infos
        << QString::fromStdString(cedar::dev::ChannelManagerSingleton::getInstance()->getTypeId(channel));
    }
    else
    {
      channel_infos << "(null)";
    }

    QTreeWidgetItem* p_item = new QTreeWidgetItem(channel_infos);
    this->mpChannelsNode->addChild(p_item);
  }
}

void cedar::dev::gui::RobotManager::addRobotName(QString addedRobotName)
{
  /* the first parameter is the displayed string, the second is the name used to internally identify the robot.
   * the first parameter can be changed without consequences (e.g., to display useful information), the second should
   * not be changed.
   */
  this->mpRobotSelector->addItem(addedRobotName, addedRobotName);

  // add a card for the simple mode
  auto p_card = new cedar::dev::gui::RobotCard(addedRobotName);
  p_card->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
  QObject::connect(p_card, SIGNAL(addBlankCard(void)), this, SLOT(simpleModeAdd()));
  int index = std::max(0, this->mpSimpleRobotListLayout->count() - 1);
  this->mpSimpleRobotListLayout->insertWidget(index, p_card);

}

void cedar::dev::gui::RobotManager::robotAddedSignalTranslator(const std::string& addedRobotName)
{
  emit robotNameAdded(QString::fromStdString(addedRobotName));
}

void cedar::dev::gui::RobotManager::robotConfigurationLoadedSignalTranslator(const std::string& robotName)
{
  emit robotConfigurationLoaded(QString::fromStdString(robotName));
}
