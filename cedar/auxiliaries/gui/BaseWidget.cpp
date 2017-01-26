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

    File:        BaseWidget.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.rub.de
    Date:        2010 01 19

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/BaseWidget.h"

// SYSTEM INCLUDES
#include <QDesktopWidget>
#include <iostream>

// constructors
cedar::aux::gui::BaseWidget::BaseWidget(const std::string& widgetName, QWidget* pParent)
:
QWidget(pParent),
mWidgetName (widgetName)
{
}

// destructor
cedar::aux::gui::BaseWidget::~BaseWidget()
{
}

void cedar::aux::gui::BaseWidget::showEvent(QShowEvent*)
{
  emit visibilityChanged(true);
}

void cedar::aux::gui::BaseWidget::hideEvent(QHideEvent*)
{
  emit visibilityChanged(false);
}

void cedar::aux::gui::BaseWidget::readCustomSettings(QSettings&)
{
}

void cedar::aux::gui::BaseWidget::writeCustomSettings(QSettings&)
{
}

void cedar::aux::gui::BaseWidget::readWindowSettings()
{
  // set the widget title
  QString name = QString::fromStdString(this->mWidgetName);
  setWindowTitle(name);

  // Create the settings object
  QSettings settings("INI", QString::fromStdString(this->mWidgetName));

  // Read the position and size of the window.
  QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("size", QSize(400, 400)).toSize();

  // check that the stored window position fits to the current screen layout
  QDesktopWidget widget;
  QRect screen_size;
  screen_size = widget.screenGeometry(widget.screenNumber(pos));
  if (!screen_size.contains(pos.x(), pos.y()))
  {
    pos.setX(10);
    pos.setY(10);
  }
  if ((size.width() > screen_size.width()) || (size.height() > screen_size.height()))
  {
    size.setWidth(screen_size.width());
    size.setHeight(screen_size.height());
  }

  // Apply size and position
  this->resize(size);
  this->move(pos);

  // Read custom settings.
  this->readCustomSettings(settings);
}

void cedar::aux::gui::BaseWidget::writeWindowSettings()
{
  // Create the settings object
  QSettings settings("INI", QString::fromStdString(this->mWidgetName));

  // Write the position and size
  settings.setValue("pos", this->pos());
  settings.setValue("size", this->size());

  // Write custom settings.
  this->writeCustomSettings(settings);
}

void cedar::aux::gui::BaseWidget::keyPressEvent(QKeyEvent* pEvent)
{
  switch (pEvent->key())
  {
    case Qt::Key_G:
      if (pEvent->modifiers() == Qt::ControlModifier)
      {
        std::cout << "Writing window settings." << std::endl;
        writeWindowSettings();
      }
      break;
    default:
      this->QWidget::keyPressEvent(pEvent);
  }
}
