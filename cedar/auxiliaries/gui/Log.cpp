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
#include "cedar/auxiliaries/gui/Settings.h"
#include "cedar/auxiliaries/logFilter/All.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/Settings.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <QHeaderView>
#include <QLabel>
#include <QScrollBar>
#include <QMenu>
#include <iostream>

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
  if (cedar::aux::SettingsSingleton::getInstance()->getMemoryDebugOutput())
  {
    this->addPane(cedar::aux::LOG_LEVEL_MEM_DEBUG, "memory", ":/cedar/auxiliaries/gui/memory.svg");
  }
#endif // DEBUG

  QObject::connect
  (
    this,
    SIGNAL(messageReceived(int, QString, QString)),
    this,
    SLOT(printMessage(int, QString, QString))
  );
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(showContextMenu(const QPoint&)));

  this->updateAllMessageCounts();

  this->startTimer(3000);
}

cedar::aux::gui::Log::~Log()
{
  this->uninstallHandlers();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::Log::scrollBarRangeChanged(int, int max)
{
  auto p_scroll_bar = cedar::aux::asserted_cast<QScrollBar*>(QObject::sender());

  bool scroll_down = false;

  auto max_iter = mMaxScrollBarRange.find(p_scroll_bar);
  // either there is no record of a maximum, or the scroll bar was at its previous maximum
  if (max_iter == mMaxScrollBarRange.end() || p_scroll_bar->value() == max_iter->second)
  {
    scroll_down = true;
  }

  if (scroll_down)
  {
    p_scroll_bar->setValue(max);
  }

  mMaxScrollBarRange[p_scroll_bar] = max;
}

void cedar::aux::gui::Log::timerEvent(QTimerEvent*)
{
  for (auto pane_iter = this->mpPanes.begin(); pane_iter != this->mpPanes.end(); ++pane_iter)
  {
    QTableWidget* p_table = pane_iter->second;
    this->updatePaneCurrentness(p_table);
  }

  this->updatePaneCurrentness(this->mpDefaultPane);
}

void cedar::aux::gui::Log::outdateAllMessages()
{
  for (auto pane_iter = this->mpPanes.begin(); pane_iter != this->mpPanes.end(); ++pane_iter)
  {
    QTableWidget* p_table = pane_iter->second;
    this->outdateAllMessages(p_table);
  }

  this->outdateAllMessages(this->mpDefaultPane);
}

void cedar::aux::gui::Log::outdateAllMessages(QTableWidget* pPane)
{
  int threshold = 200;

  for (int i = 0; i < pPane->rowCount(); ++i)
  {
    this->setMessageCurrentness(pPane, i, threshold);
  }
}

void cedar::aux::gui::Log::updatePaneCurrentness(QTableWidget* pPane)
{
  int threshold_high = 240;
  int threshold_low = 200;
  int delta_high = -5;
  int delta_low = -1;

  for (int i = 0; i < pPane->rowCount(); ++i)
  {
    int currentness = this->getMessageCurrentness(pPane, i);

    if (currentness > threshold_high)
    {
      this->setMessageCurrentness(pPane, i, currentness + delta_high);
    }
    else if (currentness > threshold_low)
    {
      this->setMessageCurrentness(pPane, i, currentness + delta_low);
    }
  }
}

int cedar::aux::gui::Log::getMessageCurrentness(QTableWidget* pPane, int message)
{
  auto item = pPane->item(message, 0);

  const QBrush& background = item->background();
  return background.color().red();
}

void cedar::aux::gui::Log::setMessageCurrentness(QTableWidget* pPane, int message, int currentness)
{
  for (int i = 0; i < pPane->columnCount(); ++i)
  {
    QColor background(currentness, currentness, currentness);
    if (auto item = pPane->item(message, i))
    {
      item->setBackground(QBrush(background));
    }
    else if (auto widget = pPane->cellWidget(message, i))
    {
      widget->setStyleSheet("background-color: 255, 0, 0");
    }
  }
}

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

  CEDAR_DEBUG_ASSERT(widget->verticalScrollBar() != NULL);
  QObject::connect(widget->verticalScrollBar(), SIGNAL(rangeChanged(int, int)), this, SLOT(scrollBarRangeChanged(int, int)));

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

  p_title_item->setBackground(QBrush(Qt::white));

  QLabel* p_message_item = new QLabel(message);
  p_message_item->setWordWrap(true);
  p_message_item->setToolTip("<p>" + message + "</p>");

  p_title_item->setFlags(item_flags);

  pTable->setItem(row, 0, p_title_item);
  pTable->setCellWidget(row, 1, p_message_item);
  pTable->resizeRowToContents(row);

  // check for maximum amount of log messages
  unsigned int max_messages = cedar::aux::gui::SettingsSingleton::getInstance()->getMaximumNumberOfLogEntries();
  while (static_cast<unsigned int>(pTable->rowCount()) > max_messages)
  {
    pTable->removeRow(0);
  }
  for (auto pane_iter = this->mpPanes.begin(); pane_iter != this->mpPanes.end(); ++pane_iter)
  {
    if (pane_iter->second == pTable)
    {
      this->updateMessageCount(pane_iter->first, pane_iter->second);
      break;
    }
  }
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

void cedar::aux::gui::Log::showContextMenu(const QPoint& point)
{
  if (point.isNull())
  {
    return;
  }

  QMenu menu(this);
  QAction* p_delete_here = menu.addAction(tr("Delete messages in this tab"));
  QAction* p_delete_all = menu.addAction(tr("Delete all messages"));

  QAction* a = menu.exec(this->mapToGlobal(point));
  if (a == p_delete_here)
  {
    QTableWidget* p_current_table = dynamic_cast<QTableWidget*>(this->currentWidget());
    p_current_table->setRowCount(0);
    for (auto pane_iter = this->mpPanes.begin(); pane_iter != this->mpPanes.end(); ++pane_iter)
    {
      if (pane_iter->second == p_current_table)
      {
        this->updateMessageCount(pane_iter->first, pane_iter->second);
        break;
      }
    }
  }
  else if (a == p_delete_all)
  {
    for (int i = 0; i < this->count(); ++i)
    {
      QTableWidget* p_current_table = dynamic_cast<QTableWidget*>(this->widget(i));
      p_current_table->setRowCount(0);
    }
    this->updateAllMessageCounts();
  }
  else if (a != NULL)
  {
    std::cout << "Unmatched action in cedar::aux::gui::Log::contextMenuEvent." << std::endl;
  }
}

QString cedar::aux::gui::Log::logLevelToString(cedar::aux::LOG_LEVEL level) const
{
  switch (level)
  {
    case cedar::aux::LOG_LEVEL_ERROR:
      return "error";
    case cedar::aux::LOG_LEVEL_WARNING:
      return "warning";
    case cedar::aux::LOG_LEVEL_MESSAGE:
      return "message";
    case cedar::aux::LOG_LEVEL_DEBUG:
      return "debug";
    case cedar::aux::LOG_LEVEL_MEM_DEBUG:
      return "memory";
    default:
      return "all";
  }
}

void cedar::aux::gui::Log::updateAllMessageCounts()
{
  for (auto pane_iter = this->mpPanes.begin(); pane_iter != this->mpPanes.end(); ++pane_iter)
  {
    this->updateMessageCount(pane_iter->first, pane_iter->second);
  }
}

void cedar::aux::gui::Log::updateMessageCount(cedar::aux::LOG_LEVEL level, QTableWidget* pPane)
{
  auto index = this->indexOf(pPane);
  this->setTabText(index, this->logLevelToString(level) + " (" + QString("%1").arg(pPane->rowCount()) + ")");
}
