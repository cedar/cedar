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

    File:        PlotDockWidget.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 14

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_PlOT_DOCK_WIDGET_H
#define CEDAR_PROC_GUI_PlOT_DOCK_WIDGET_H

#include "cedar/configuration.h"


// FORWARD DECLARATIONS
#include "cedar/processing/gui/PlotDockWidget.fwd.h"

// SYSTEM INCLUDES
#include <QMenu>
#include <QDockWidget>
#include <QGraphicsScene>
#include <QPainter>
#include <QPaintEvent>
#include <QLabel>
#include <QPushButton>

/*!@brief
 *
 *
 */
class cedar::proc::gui::PlotDockWidget : public QDockWidget
{
Q_OBJECT
public:

  PlotDockWidget(const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0);
  QLabel* getTitleLabel();
  void setTitleLabelText(const QString & title);

  QPushButton* getJumpButton();
  QPushButton* getDeleteButton();

protected:
  virtual void paintEvent(QPaintEvent *event);

private:
  QLabel* mTitleLabel;
  QPushButton* mJumpButton;
  QPushButton* mDeleteButton;

private:


}; // class cedar::proc::gui::PlotDockWidget


#endif // CEDAR_PROC_GUI_PlOT_DOCK_WIDGET_H
