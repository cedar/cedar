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

    File:        BaseWidget.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.rub.de
    Date:        2010 01 19

    Description:

    Credits:

======================================================================================================================*/


#ifndef CEDAR_AUX_GUI_BASE_WIDGET_H
#define CEDAR_AUX_GUI_BASE_WIDGET_H

// LOCAL INCLUDES
#include "namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <string>
#include <QSettings>
#include <QKeyEvent>
#include <QWidget>

/*!
 * This class implements various base functions for widgets. This includes reading and writing of the window settings.
 *
 * @remarks To read/write the window settings, call the corresponding functions. To read/write custom settings,
 *          overwrite the readCustomSettings and writeCustomSettings methods.
 */
class cedar::aux::gui::BaseWidget : public QWidget
{
private:
  Q_OBJECT

public:
  // constructors
  /*!
   * @brief   Constructor that takes the widget name as argument.
   *
   *          To read the window settings, call readWindowSettings().
   */
  BaseWidget(const std::string& widgetName = "", QWidget *pParent = NULL);

  // destructor
  virtual ~BaseWidget(void);

protected:

  /*!
   * @brief the widget sends out a signal when it is shown
   */
  virtual void showEvent(QShowEvent*);
  /*!
   * @brief the widget sends out a signal when it is hidden
   */
  virtual void hideEvent(QHideEvent*);

  /*!
   * Overwrite this to read custom window settings.
   */
  virtual void readCustomSettings(QSettings&);

  /*!
   * Overwrite this to write custom window settings.
   */
  virtual void writeCustomSettings(QSettings&);

  /*!
   * @brief Reads the window settings using the QSettings class.
   */
  virtual void readWindowSettings();

  /*!
   * @brief Writes the window settings using the QSettings class.
   */
  virtual void writeWindowSettings();

  /*!
   * @brief Processes key events.
   *
   *        This function handles ctrl+G, which saves the window settings.
   */
  virtual void keyPressEvent(QKeyEvent* pEvent);

signals:
  void visibleChanged(bool);

private:
  //! Unique identifier for each instance. This is used to associate the window settings with the instance.
  std::string mWidgetName;
};

#endif  // OL_BASE_WIDGET_H
