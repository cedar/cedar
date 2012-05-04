/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Log.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 04 13

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Log.h"
#include "cedar/auxiliaries/logFilter/All.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <QHeaderView>
#include <QLabel>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::Log::Log(QWidget* pParent)
:
QTabWidget(pParent)
{
  this->mpDefaultPane = this->addPane("all");

  this->addPane(cedar::aux::LOG_LEVEL_ERROR, "error", ":/cedar/auxiliaries/gui/error.svg");
  this->addPane(cedar::aux::LOG_LEVEL_WARNING, "warning", ":/cedar/auxiliaries/gui/warning.svg");
  this->addPane(cedar::aux::LOG_LEVEL_MESSAGE, "message", ":/cedar/auxiliaries/gui/message.svg");

#ifdef DEBUG
  this->addPane(cedar::aux::LOG_LEVEL_DEBUG, "debug", ":/cedar/auxiliaries/gui/debug.svg");
  this->addPane(cedar::aux::LOG_LEVEL_MEM_DEBUG, "memory", ":/cedar/auxiliaries/gui/memory.svg");
#endif // DEBUG

  QObject::connect
  (
    this,
    SIGNAL(messageReceived(int, QString, QString)),
    this,
    SLOT(printMessage(int, QString, QString))
  );
}

cedar::aux::gui::Log::~Log()
{
  this->uninstallHandlers();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::Log::installHandlers(bool removeMessages)
{
  this->mLogger = cedar::aux::gui::Log::LogInterfacePtr(new cedar::aux::gui::Log::LogInterface(this));
  cedar::aux::LogFilterPtr filter(new cedar::aux::logFilter::All());
  filter->setRemovesMessages(removeMessages);

  cedar::aux::LogSingleton::getInstance()->addLogger
  (
    this->mLogger,
    filter
  );
}

void cedar::aux::gui::Log::uninstallHandlers()
{
  if (this->mLogger)
  {
    cedar::aux::LogSingleton::getInstance()->removeLogger
    (
      this->mLogger
    );

    this->mLogger.reset();
  }
}

void cedar::aux::gui::Log::addPane(cedar::aux::LOG_LEVEL level, const std::string& title, const std::string& icon)
{
  QTableWidget* p_pane = this->addPane(title, icon);
  mpPanes[level] = p_pane;
  mIcons[level] = icon;
}

QTableWidget* cedar::aux::gui::Log::addPane(const std::string& title, const std::string& iconPath)
{
  QTableWidget* widget = new QTableWidget();
  if (iconPath.empty())
  {
    this->addTab(widget, QString::fromStdString(title));
  }
  else
  {
    QString icon_path = QString::fromStdString(iconPath);
    QIcon icon(icon_path);
    this->addTab(widget, icon, QString::fromStdString(title));
  }

  QStringList columns;
  widget->setColumnCount(2);
  columns << "title" << "message";
  widget->setHorizontalHeaderLabels(columns);
  widget->setWordWrap(true);

  widget->horizontalHeader()->setStretchLastSection(true);
  widget->verticalHeader()->setVisible(false);

  return widget;
}

void cedar::aux::gui::Log::postMessage
(
  QTableWidget* pTable,
  const QString& message,
  const QString& title,
  const QString& icon
)
{
  Qt::ItemFlags item_flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  int row = pTable->rowCount();
  pTable->insertRow(row);

  QTableWidgetItem *p_title_item = NULL;
  if (icon.isEmpty())
  {
    p_title_item = new QTableWidgetItem(title);
  }
  else
  {
    p_title_item = new QTableWidgetItem(QIcon(icon), title);
  }

  QLabel* p_message_item = new QLabel(message);
  p_message_item->setToolTip("<p>" + message + "</p>");

  p_title_item->setFlags(item_flags);

  pTable->setItem(row, 0, p_title_item);
  pTable->setCellWidget(row, 1, p_message_item);
  pTable->resizeRowToContents(row);
}

void cedar::aux::gui::Log::message
     (
       cedar::aux::LOG_LEVEL level,
       const std::string& message,
       const std::string& title
     )
{
  emit messageReceived(static_cast<int>(level), QString::fromStdString(title), QString::fromStdString(message));
}

void cedar::aux::gui::Log::printMessage(int type, QString title, QString message)
{
  cedar::aux::LOG_LEVEL level = static_cast<cedar::aux::LOG_LEVEL>(type);
  QTableWidget* p_level_pane = NULL;

  QString icon;

  std::map<cedar::aux::LOG_LEVEL, QTableWidget*>::iterator iter = this->mpPanes.find(level);
  if (iter != this->mpPanes.end())
  {
    p_level_pane = iter->second;
    CEDAR_DEBUG_ASSERT(mIcons.find(level) != mIcons.end());

    icon = QString::fromStdString(mIcons[level]);
  }

  switch (level)
  {
    case cedar::aux::LOG_LEVEL_MEM_DEBUG:
#ifdef DEBUG
      CEDAR_DEBUG_ASSERT(p_level_pane != NULL);
      this->postMessage(p_level_pane, message, title, icon);
#endif // DEBUG
      break;

    default:
      if(p_level_pane != NULL)
      {
        this->postMessage(p_level_pane, message, title, icon);
      }
      this->postMessage(this->mpDefaultPane, message, title, icon);
  }
}
