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

    File:        Log.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 04 13

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_LOG_H
#define CEDAR_AUX_GUI_LOG_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/LogInterface.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/Log.fwd.h"

// SYSTEM INCLUDES
#include <QTabWidget>
#include <QTableWidget>
#include <QGraphicsSceneContextMenuEvent>
#include <map>


/*!@brief A default log widget.
 */
class cedar::aux::gui::Log : public QTabWidget
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  class LogInterface : public cedar::aux::LogInterface
  {
    public:
      LogInterface(Log* pLog)
      :
      mpLog(pLog)
      {
      }

      void message
      (
        cedar::aux::LOG_LEVEL level,
        const std::string& message,
        const std::string& title
      )
      {
        this->mpLog->message(level, message, title);
      }

    private:
      Log* mpLog;
  };

  CEDAR_GENERATE_POINTER_TYPES(LogInterface);

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Log(QWidget *pParent = NULL);

  //!@brief Destructor
  ~Log();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief log a message with a given log level
  void message
  (
    cedar::aux::LOG_LEVEL level,
    const std::string& message,
    const std::string& title
  );

  /*!@brief Installs the handlers that redirect log messages to this widget.
   *
   *        Before calling this method, the log will not display anything. Also, remember to uninstall them when the
   *        log's parent is destroyed!
   */
  void installHandlers(bool removeMessages = true);

  /*!@brief Removes the handlers that redirect log messages to this widget.
   */
  void uninstallHandlers();

  //! Marks all log messages as outdated.
  void outdateAllMessages();

public slots:
  //! Opens the context menu.
  void showContextMenu(const QPoint& point);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Called periodically. Responsible for fading out older messages.
  void timerEvent(QTimerEvent* pEvent);

signals:
  //!@brief signals reception of a signal
  void messageReceived(int type, QString title, QString message);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private slots:
  void scrollBarRangeChanged(int min, int max);
private:
  void addPane(cedar::aux::LOG_LEVEL level, const std::string& title, const std::string& icon = "");

  QTableWidget* addPane(const std::string& title, const std::string& icon = "");

  void postMessage
  (
    QTableWidget* pTable,
    const QString& message,
    const QString& title,
    const QString& icon = ""
  );

  //! Returns an (arbitrary) number representing a message's currentness (i.e., how old it is).
  int getMessageCurrentness(QTableWidget* pPane, int message);

  //! Sets an (arbitrary) number representing a message's currentness (i.e., how old it is).
  void setMessageCurrentness(QTableWidget* pPane, int message, int currentness);

  //! Updates the currentness of all items in the pane.
  void updatePaneCurrentness(QTableWidget* pPane);

  //! Outdates all messages in the given pane.
  void outdateAllMessages(QTableWidget* pPane);

  QString logLevelToString(cedar::aux::LOG_LEVEL level) const;

  void updateAllMessageCounts();

  void updateMessageCount(cedar::aux::LOG_LEVEL level, QTableWidget* pPane);

private slots:
  void printMessage(int type, QString title, QString message);


  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  QTableWidget* mpDefaultPane;

  LogInterfacePtr mLogger;

  std::map<cedar::aux::LOG_LEVEL, QTableWidget*> mpPanes;
  std::map<cedar::aux::LOG_LEVEL, std::string> mIcons;
  std::map<QScrollBar*, int> mMaxScrollBarRange;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::gui::Log

#endif // CEDAR_AUX_GUI_LOG_H

