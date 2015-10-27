/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

// SYSTEM INCLUDES
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDropEvent>
#include <QListWidget>
#include <QModelIndex>
#include <QMessageBox>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::gui::RobotCard::RobotCard(const QString& robotName)
{
  // associate robot with this card
  this->mRobot = cedar::dev::RobotManagerSingleton::getInstance()->getRobot(robotName.toStdString());

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

  // header
  auto p_header_layout = new QHBoxLayout();
  p_header_layout->setContentsMargins(contents_margins, contents_margins, contents_margins, contents_margins);
  p_outer_layout->addLayout(p_header_layout, 0);
  mpRobotNameEdit = new QLineEdit();
  p_header_layout->addWidget(mpRobotNameEdit);
  mpRobotNameEdit->setText(robotName);
  auto p_recycle_button = new QPushButton(QIcon(":/cedar/auxiliaries/gui/trashcan.svg"), "");
  p_recycle_button->setFixedWidth(24);
  p_recycle_button->setFixedHeight(24);
  p_recycle_button->setIconSize(QSize(16, 16));
  p_header_layout->addWidget(p_recycle_button);

  // center
  this->mpIcon = new cedar::dev::gui::RobotCardIconHolder(this);
  p_outer_layout->addWidget(this->mpIcon, 1);

  // footer
  auto p_footer_layout = new QHBoxLayout();
  p_footer_layout->setContentsMargins(contents_margins, contents_margins, contents_margins, contents_margins);
  p_outer_layout->addLayout(p_footer_layout, 0);
  mpConfigurationSelector = new QComboBox();
  p_footer_layout->addWidget(mpConfigurationSelector, 1);

  this->mpConnectButton = new QPushButton(QIcon(":/cedar/dev/gui/icons/not_connected.svg"), "");
  QSizePolicy policy = this->mpConnectButton->sizePolicy();
  policy.setVerticalPolicy(QSizePolicy::Expanding);
  this->mpConnectButton->setSizePolicy(policy);
  p_footer_layout->addWidget(this->mpConnectButton, 0);
  QObject::connect(this->mpConnectButton, SIGNAL(clicked()), this, SLOT(connectClicked()));

  this->setLayout(p_outer_layout);

  QObject::connect(this->mpIcon, SIGNAL(robotDropped(const QString&)), this, SLOT(robotDropped(const QString&)));
  QObject::connect(mpConfigurationSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedConfigurationChanged(int)));
  QObject::connect(p_recycle_button, SIGNAL(clicked()), this, SLOT(deleteClicked()));

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

  this->updateConnectionIcon();
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
}

cedar::dev::gui::RobotCard::~RobotCard()
{
  this->mRobotRemovedConnection.disconnect();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::gui::RobotCard::updateConnectionIcon()
{
  cedar::dev::RobotPtr robot = cedar::dev::RobotManagerSingleton::getInstance()->getRobot(this->getRobotName());
  unsigned int open = robot->countOpenChannels();

  if (open > 0 && open == robot->getNumberOfChannels())
  {
    this->mpConnectButton->setIcon(QIcon(":/cedar/dev/gui/icons/connected.svg"));
  }
  else if (open > 0)
  {
    this->mpConnectButton->setIcon(QIcon(":/cedar/dev/gui/icons/partially_connected.svg"));
  }
  else
  {
    this->mpConnectButton->setIcon(QIcon(":/cedar/dev/gui/icons/not_connected.svg"));
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
    unsigned int open = robot->countOpenChannels();

    if (open == 0)
    {
      robot->openChannels();
    }
    else
    {
      robot->closeChannels();
    }

    this->updateConnectionIcon();
  }
  catch (const cedar::aux::ExceptionBase& e)
  {
    this->mpConnectButton->setIcon(QIcon(":/cedar/auxiliaries/gui/error.svg"));
    this->mpConnectButton->setToolTip(QString::fromStdString(e.exceptionInfo()));
  }
  catch (const std::exception& e)
  {
    this->mpConnectButton->setIcon(QIcon(":/cedar/auxiliaries/gui/error.svg"));
    this->mpConnectButton->setToolTip(e.what());
  }

  this->mpConnectButton->setEnabled(true);
}

void cedar::dev::gui::RobotCard::setRobotTemplate(const std::string& templateName)
{
  this->robotDropped(QString::fromStdString(templateName));
  this->mpIcon->setIconFromTemplate(templateName);
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
    this->deleteLater();
  }
}

std::string cedar::dev::gui::RobotCard::getRobotName() const
{
  return this->mpRobotNameEdit->text().toStdString();
}

void cedar::dev::gui::RobotCard::selectedConfigurationChanged(int index)
{
  // index 0 is the selection hint
  QString combo_text = this->mpConfigurationSelector->itemText(index);
  if (index == 0 && combo_text == "-- select to instantiate --")
  {
    return;
  }

  cedar::dev::RobotManagerSingleton::getInstance()->loadRobotTemplateConfiguration(this->getRobotName(), combo_text.toStdString());
}

void cedar::dev::gui::RobotCard::robotDropped(const QString& robotTypeName)
{
  auto robot_template = cedar::dev::RobotManagerSingleton::getInstance()->getTemplate(robotTypeName.toStdString());

  this->mRobotTemplateName = robotTypeName.toStdString();

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
