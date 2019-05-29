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

    File:        RobotCard.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 03 04

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/gui/RobotCard.h"
#include "cedar/devices/RobotManager.h"
#include "cedar/devices/Robot.h"
#include "cedar/devices/exceptions.h"
#include "cedar/devices/Component.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDropEvent>
#include <QListWidget>
#include <QModelIndex>
#include <QMessageBox>
#include <QMimeData>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::gui::RobotCard::RobotCard(const QString& robotName)
{
  // associate robot with this card
  this->mrRobot = cedar::dev::RobotManagerSingleton::getInstance()->getRobot(robotName.toStdString());

  this->mRobotRemovedConnection
    = cedar::dev::RobotManagerSingleton::getInstance()->connectToRobotRemovedSignal
      (
        boost::bind
        (
          &cedar::dev::gui::RobotCard::robotRemoved,
          this,
          _1
        )
      );



  // build user interface
  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  this->setFixedWidth(200);
  this->setFrameShape(QFrame::StyledPanel);

  const int contents_margins = 2;

  auto p_outer_layout = new QVBoxLayout();
  p_outer_layout->setContentsMargins(contents_margins, contents_margins, contents_margins, contents_margins);

  // delete button
  auto p_del_layout = new QVBoxLayout();

  mpDeleteButton = new QPushButton(QIcon(":/cedar/auxiliaries/gui/trashcan.svg"), "");
  mpDeleteButton->setFixedWidth(24);
  mpDeleteButton->setFixedHeight(24);
  mpDeleteButton->setIconSize(QSize(16, 16));
  mpDeleteButton->setToolTip("Remove this robot");

  p_del_layout->addWidget(mpDeleteButton);
  p_del_layout->setAlignment(mpDeleteButton, Qt::AlignRight);
  p_outer_layout->addLayout(p_del_layout, 1);

  // robot icon
  this->mpIcon = new cedar::dev::gui::RobotCardIconHolder(this);
  p_outer_layout->addWidget(this->mpIcon, 1);

  // connect button
  auto p_connect_layout = new QHBoxLayout();
  p_connect_layout->setContentsMargins(contents_margins, contents_margins, contents_margins, contents_margins);
  this->mpConnectButton = new QPushButton(QIcon(":/cedar/dev/gui/icons/not_connected.svg"), "");
  mpConnectButton->setToolTip("Click to connect the selected instance");
  mpConnectButton->setIconSize(QSize(64, 64));
  mpConnectButton->setFixedWidth(64);
  mpConnectButton->setFixedHeight(64);
  p_connect_layout->addWidget(this->mpConnectButton, 0);

  QObject::connect(this->mpConnectButton, SIGNAL(clicked()), this, SLOT(connectClicked()));
  p_outer_layout->addLayout(p_connect_layout, 0);

  // connection configure box
  auto p_connect_config_layout = new QVBoxLayout();
  p_connect_config_layout->setContentsMargins(contents_margins, contents_margins, contents_margins, contents_margins);
  this->mpAutomaticConnectBox = new QCheckBox();
  this->mpAutomaticConnectBox->setText(QString::fromStdString("connect on play?"));
  this->mpAutomaticConnectBox->setToolTip(QString::fromStdString("Disconnection has to be handled manually"));
  p_connect_config_layout->addWidget(mpAutomaticConnectBox, 1);

  mpConfigurationSelector = new QComboBox();
  p_connect_config_layout->addWidget(mpConfigurationSelector, 1);
  p_outer_layout->addLayout(p_connect_config_layout, 0);

  // name layout
  auto p_name_layout = new QHBoxLayout();
  mpRobotNameEdit = new QLineEdit();
  p_name_layout->addWidget(mpRobotNameEdit);
  mpRobotNameEdit->setText(robotName);
  mCurrentName = robotName.toStdString();
  p_outer_layout->addLayout(p_name_layout, 0);

  this->setLayout(p_outer_layout);

  // prevent any action on an empty placeholder card
  mpDeleteButton->setEnabled(false);
  mpConfigurationSelector->setEnabled(false);
  mpRobotNameEdit->setEnabled(false);
  mpConnectButton->setEnabled(false);

  // fade out everything out of circle
  this->setStyleSheet("QFrame { \
       background: rgba(156, 156, 156, 255) }");

  QObject::connect(this->mpIcon, SIGNAL(robotDropped(const QString&)), this, SLOT(robotDropped(const QString&)));
  QObject::connect(mpConfigurationSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedConfigurationChanged(int)));
  QObject::connect(mpDeleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
  QObject::connect(this->mpRobotNameEdit, SIGNAL(editingFinished(void)), this, SLOT(robotNameEditValueChanged(void)));
  QObject::connect(this->mpAutomaticConnectBox, SIGNAL(stateChanged(int)), this, SLOT(automaticConnectClicked(int)));

  try
  {
    this->setRobotTemplate
      (
        cedar::dev::RobotManagerSingleton::getInstance()->getRobotTemplateName(robotName.toStdString())
      );
  }
  catch (const cedar::dev::TemplateNotFoundException&)
  {
    // ok -- nothing to do here
  }

  // if the currently selected index is the selection hint or there is no possible selection, gray out the connect button
  if(mpConfigurationSelector->currentIndex() ==0 || mpConfigurationSelector->count()==0)
  {
    mpConnectButton->setDisabled(true);
  }else
  { // if not, set the correct icon
    this->updateConnectionIcon();
  }

  robotNameEditValueChanged();
}

cedar::dev::gui::RobotCardIconHolder::RobotCardIconHolder(cedar::dev::gui::RobotCard* pCard)
:
mpCard(pCard)
{
  CEDAR_ASSERT(this->mpCard != NULL);
  this->setAcceptDrops(true);
  this->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  this->setBackgroundRole(QPalette::Light);
  this->setMinimumHeight(200);
  QIcon icon(":/cedar/dev/gui/icons/drop_robot.png");
  this->setPixmap(icon.pixmap(QSize(190, 190)));
}

cedar::dev::gui::RobotCard::~RobotCard()
{
  this->mRobotRemovedConnection.disconnect();

  for(auto connection: mRobotDisconnectConnections)
  {
    connection.disconnect();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::gui::RobotCard::updateConnectionIcon()
{
  this->mpConnectButton->setEnabled(true);
  cedar::dev::RobotPtr robot = cedar::dev::RobotManagerSingleton::getInstance()->getRobot(this->getRobotName());


  if (robot->areAllComponentsCommunicating())
  {
    this->mpConnectButton->setIcon(QIcon(":/cedar/dev/gui/icons/connected.svg"));
    this->mpConnectButton->setToolTip("Fully connected");
    this->mpConnectButton->setStyleSheet(
       "QPushButton { \
           background: rgb(222, 10, 244);\
           color: rgb(233, 10, 255);\
       }");

    this->mpConfigurationSelector->setEnabled(false);
  }
  else if (robot->areSomeComponentsCommunicating())
  {
    this->mpConnectButton->setIcon(QIcon(":/cedar/dev/gui/icons/partially_connected.svg"));
    this->mpConnectButton->setToolTip("Partially connected");
    this->mpConnectButton->setStyleSheet(
       "QPushButton { \
                background: qlineargradient(x1:0, y1:0, x2:1, y2:1,\
                stop:0 white, stop: 0.4 rgba(10, 10, 10, 40), stop:1 rgb(233, 10, 255, 255));\
       }");

    this->mpConfigurationSelector->setEnabled(false);
  }
  else
  {
    this->mpConnectButton->setIcon(QIcon(":/cedar/dev/gui/icons/not_connected.svg"));
    this->mpConnectButton->setToolTip("Not connected");
    this->mpConnectButton->setStyleSheet(""); // reset yellow background to default
    this->mpConfigurationSelector->setEnabled(true);
  }
}

void cedar::dev::gui::RobotCard::connectClicked()
{
  this->mpConnectButton->setEnabled(false);
  this->mpConnectButton->setIcon(QIcon(":/cedar/dev/gui/icons/connecting.svg"));
  this->mpConnectButton->setToolTip("");

  unsigned int count;

  count = 0;
  while (QApplication::hasPendingEvents() && ++count < 1000)
  {
    QApplication::processEvents();
  }

  try
  {
    cedar::dev::RobotPtr robot = cedar::dev::RobotManagerSingleton::getInstance()->getRobot(this->getRobotName());
    if (!robot->areSomeComponentsCommunicating())
    {
      robot->startCommunicationOfComponents(true); // suppress user interaction!... why?
    }
    else
    {
      robot->stopCommunicationOfComponents();
    }

    this->updateConnectionIcon(); // Somehow a connection to the ConnectHook blocks everything. Could be the result of the boost signals. So update of the Icon will still be updated here as well.
  }
  catch (const cedar::aux::ExceptionBase& e)
  {
    this->mpConnectButton->setIcon(QIcon(":/cedar/auxiliaries/gui/error.svg"));
    this->mpConnectButton->setToolTip(QString::fromStdString(e.exceptionInfo()));
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Connecting failed: " + e.exceptionInfo(),
      CEDAR_CURRENT_FUNCTION_NAME
    );
  }
  catch (const std::exception& e)
  {
    this->mpConnectButton->setIcon(QIcon(":/cedar/auxiliaries/gui/error.svg"));
    this->mpConnectButton->setToolTip(e.what());
    cedar::aux::LogSingleton::getInstance()->error
    (
      e.what(),
      CEDAR_CURRENT_FUNCTION_NAME
    );
  }

  this->mpConnectButton->setEnabled(true);
}

//TODO: Function Naming seems off here. This robot Dropped should call setRobotTemplate and not the other way round!
void cedar::dev::gui::RobotCard::setRobotTemplate(const std::string& templateName)
{
  this->robotDropped(QString::fromStdString(templateName));
  this->mpIcon->setIconFromTemplate(templateName);
  this->mpAutomaticConnectBox->setChecked(cedar::dev::RobotManagerSingleton::getInstance()->isAutomaticallyConnecting(this->getRobotName()));

}

void cedar::dev::gui::RobotCard::deleteClicked()
{
  QMessageBox::StandardButton res = QMessageBox::question
            (
              this,
              "Delete robot?",
              QString::fromStdString("Are you sure you want to delete the robot \"" + this->getRobotName() + "\"?"),
              QMessageBox::Yes | QMessageBox::No
            );

  if (res == QMessageBox::Yes)
  {
    cedar::dev::RobotManagerSingleton::getInstance()->removeRobot(this->getRobotName());
  }
}

void cedar::dev::gui::RobotCard::robotRemoved(const std::string& robotName)
{
  if (robotName == this->getRobotName())
  {
    cedar::dev::RobotManagerSingleton::getInstance()->setRobotTemplateConfigurationName(robotName, "");
    this->deleteLater();
  }
}

std::string cedar::dev::gui::RobotCard::getRobotName() const
{
  return this->mpRobotNameEdit->text().toStdString();
}

void cedar::dev::gui::RobotCard::selectedConfigurationChanged(int index)
{

  // remove former visualisation
  cedar::aux::gl::GlobalSceneSingleton::getInstance()->deleteObjectVisualization(this->getRobotName());

  // index 0 is the selection hint
  QString combo_text = this->mpConfigurationSelector->itemText(index);
  if (index == 0 && combo_text == "-- select to instantiate --")
  {
    mpConnectButton->setDisabled(true);    
    return;
  }

  mpConnectButton->setDisabled(false);
  cedar::dev::RobotManagerSingleton::getInstance()->loadRobotTemplateConfiguration(this->getRobotName(), combo_text.toStdString());




  for(auto connection: mRobotDisconnectConnections)
  {
    connection.disconnect();
  }

  auto componentSlotNames = this->mrRobot->getComponentSlotNames();
  for(auto slotName:componentSlotNames)
  {
    auto component = this->mrRobot->getComponent(slotName);
    auto connectionToDisconnect = component->registerDisconnectedHook
            (
                    boost::bind
                            (
                                    &cedar::dev::gui::RobotCard::updateConnectionIcon,
                                    this
                            )
            );
    this->mRobotDisconnectConnections.push_back(connectionToDisconnect);
  }


}

void cedar::dev::gui::RobotCard::robotDropped(const QString& robotTypeName)
{
  auto robot_template = cedar::dev::RobotManagerSingleton::getInstance()->getTemplate(robotTypeName.toStdString());
  this->mRobotTemplateName = robotTypeName.toStdString();

  if(this->mpConfigurationSelector->count() == 0)
  {
    // if a robot has been dropped into an empty card, create a new blank card before filling this one
    addBlankCard();
  }

  bool blocked = this->mpConfigurationSelector->blockSignals(true);
  this->mpConfigurationSelector->clear();
  this->mpConfigurationSelector->addItem("-- select to instantiate --");

  auto configuration_names = robot_template.getConfigurationNames();

  std::string loaded_configuration;
  try
  {
    loaded_configuration
      = cedar::dev::RobotManagerSingleton::getInstance()->getRobotTemplateConfiguration(this->getRobotName());
  }
  catch(const cedar::dev::NoTemplateConfigurationLoadedException& e)
  {
    // nothing loaded, nothing to do
  }

  int selected = 0;
  for (size_t i = 0; i < configuration_names.size(); ++i)
  {
    const std::string configuration = configuration_names.at(i);
    this->mpConfigurationSelector->addItem(QString::fromStdString(configuration));
    if (configuration == loaded_configuration)
    {
      selected = this->mpConfigurationSelector->count() - 1;
    }
  }

  this->mpConfigurationSelector->setCurrentIndex(selected);
  this->mpConfigurationSelector->blockSignals(blocked);

  // a new placeholder card has already bene placed, so allow for actions here
  mpDeleteButton->setEnabled(true);
  mpConfigurationSelector->setEnabled(true);
  mpRobotNameEdit->setEnabled(true);

  this->setStyleSheet("");
}

std::string cedar::dev::gui::RobotCardIconHolder::getRobotName() const
{
  return this->mpCard->getRobotName();
}

void cedar::dev::gui::RobotCardIconHolder::setIconFromTemplate(const std::string& templateName)
{
  auto robot_template = cedar::dev::RobotManagerSingleton::getInstance()->getTemplate(templateName);
  QIcon icon(QString::fromStdString(robot_template.getIconPath()));
  this->setPixmap(icon.pixmap(QSize(200, 200)));
}

void cedar::dev::gui::RobotCardIconHolder::setRobotFromTemplate(const std::string& templateName)
{
  cedar::dev::RobotManagerSingleton::getInstance()->setRobotTemplateName(this->getRobotName(), templateName);

  this->setIconFromTemplate(templateName);
}

void cedar::dev::gui::RobotCardIconHolder::dragEnterEvent(QDragEnterEvent* pEvent)
{
  QListWidgetItem *p_item = this->itemFromMime(pEvent);
  if (p_item != NULL)
  {
    pEvent->acceptProposedAction();
  }
}

void cedar::dev::gui::RobotCardIconHolder::dropEvent(QDropEvent* pEvent)
{
  QListWidgetItem *p_item = this->itemFromMime(pEvent);

  if (p_item != NULL)
  {
    pEvent->acceptProposedAction();
    QString robot_name = p_item->data(Qt::UserRole).toString();

    this->setRobotFromTemplate(robot_name.toStdString());

    emit robotDropped(robot_name);
  }
}

QListWidgetItem* cedar::dev::gui::RobotCardIconHolder::itemFromMime(QDropEvent* pEvent)
{
  auto p_source = dynamic_cast<QListWidget*>(pEvent->source());

  if (!p_source)
  {
    return NULL;
  }

  const QMimeData *p_mime_data = pEvent->mimeData();

  if (!p_mime_data)
  {
    return NULL;
  }

  QByteArray item_data = pEvent->mimeData()->data("application/x-qabstractitemmodeldatalist");
  QDataStream stream(&item_data, QIODevice::ReadOnly);

  int r, c;
  QMap<int, QVariant> v;
  stream >> r >> c >> v;

  return p_source->item(r);
}

void cedar::dev::gui::RobotCard::robotNameEditValueChanged()
{
  const std::string robot_name = mpRobotNameEdit->text().toStdString();

  if(robot_name == mCurrentName)
  {
    return;
  }

  cedar::aux::LogSingleton::getInstance()->message
  (
    "\""+mCurrentName+"\" was renamed to \""+robot_name+"\"",
    "cedar::dev::gui::RobotCard::robotNameEditValueChanged()"
  );

  cedar::dev::RobotManagerSingleton::getInstance()->renameRobot(mCurrentName, robot_name);
  mCurrentName = robot_name;
}

void cedar::dev::gui::RobotCard::automaticConnectClicked(int state)
{
  //Checkboxes can either be 0,1 or 2, where 1 is a partial check
  cedar::dev::RobotManagerSingleton::getInstance()->setAutomaticallyConnect(this->getRobotName(),state > 0);
}