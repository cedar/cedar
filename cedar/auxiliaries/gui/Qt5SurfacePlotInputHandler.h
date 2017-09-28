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

    File:        Qt5SurfacePlot.h

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

#ifndef CEDAR_AUX_GUI_QT5_SURFACE_PLOT_INPUT_HANDLER_H
#define CEDAR_AUX_GUI_QT5_SURFACE_PLOT_INPUT_HANDLER_H

#include "cedar/configuration.h"

#ifdef CEDAR_USE_QT5_PLOTS
// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/Qt5SurfacePlotInputHandler.fwd.h"
#include "cedar/auxiliaries/gui/Qt5SurfacePlot.fwd.h"

// SYSTEM INCLUDES
#include <QtDataVisualization/Q3DInputHandler>
#include <QtDataVisualization/QAbstract3DGraph>
#include <QMenu>


/*!@brief
 *
 *
 */
class cedar::aux::gui::Qt5SurfacePlotInputHandler : public QtDataVisualization::Q3DInputHandler
{
Q_OBJECT
public:
  explicit Qt5SurfacePlotInputHandler(cedar::aux::gui::Qt5SurfacePlot *plot, QObject *parent = 0);

private:
//  void handleElementSelected(QtDataVisualization::QAbstract3DGraph::ElementType type);
  void callContextMenu(QPoint menuPos);

//  virtual void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
  virtual void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);
  virtual void wheelEvent(QWheelEvent *event);

private:
  cedar::aux::gui::Qt5SurfacePlot *m_plot;

}; // class cedar::aux::gui::Qt5SurfacePlotInputHandler

#endif // CEDAR_USE_QT5_PLOTS
#endif // CEDAR_AUX_GUI_QT5_SURFACE_PLOT_INPUT_HANDLER_H
