/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik


 ----- File:        BaseWidget.cpp

 ----- Author:      Christine Goehrke, Tim Dierkes, Oliver Lomp, Marc Hoener, Tobias Kirchhof

 ----- Date:        2010 01 19

 ----- Description:

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "BaseWidget.h"

// PROJECT INCLUDES

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
  emit visibleChanged(true);
}

void cedar::aux::gui::BaseWidget::hideEvent(QHideEvent*)
{
  emit visibleChanged(false);
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
