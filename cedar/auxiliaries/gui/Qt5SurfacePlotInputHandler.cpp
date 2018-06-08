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

    File:        Qt5SurfacePlot.cpp

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

#include "cedar/configuration.h"

#ifdef CEDAR_USE_QT5_PLOTS
#define NOMINMAX // prevents MSVC conflicts

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Qt5SurfacePlotInputHandler.h"
#include "cedar/auxiliaries/gui/Qt5SurfacePlot.h"


// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::gui::Qt5SurfacePlotInputHandler::Qt5SurfacePlotInputHandler(cedar::aux::gui::Qt5SurfacePlot *plot,
                                                                        QObject *parent)
        :
        QtDataVisualization::Q3DInputHandler(parent),
        m_plot(plot)
{
//  connect(graph, &QtDataVisualization::QAbstract3DGraph::selectedElementChanged, this,
//            &Qt5SurfacePlotInputHandler::handleElementSelected);

}

//void cedar::aux::gui::Qt5SurfacePlotInputHandler::handleElementSelected(QtDataVisualization::QAbstract3DGraph::ElementType type)
//{
//    switch (type) {
//    case QtDataVisualization::QAbstract3DGraph::ElementAxisXLabel:
//        std::cout<<"Hit Axis X!"<<std::endl;
//        break;
//    case QtDataVisualization::QAbstract3DGraph::ElementAxisYLabel:
//        std::cout<<"Hit Axis X!"<<std::endl;
//        break;
//    case QtDataVisualization::QAbstract3DGraph::ElementAxisZLabel:
//       std::cout<<"Hit Axis X!"<<std::endl;
//        break;
//    default:
//        std::cout<<"Hit Something unspecified!"<<std::endl;
//        break;
//    }
//}

void cedar::aux::gui::Qt5SurfacePlotInputHandler::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
{
  if (Qt::LeftButton == event->button())
  {
    // Map Functionality from Right Button to Left Button!
    QMouseEvent *switchedEvent = new QMouseEvent(QEvent::MouseButtonPress, event->localPos(), Qt::RightButton,
                                                 Qt::RightButton, event->modifiers());
    QtDataVisualization::Q3DInputHandler::mousePressEvent(switchedEvent, mousePos);
  }
  else if (Qt::MiddleButton == event->button())
  {
    // Map Functionality from Left Button to Middle Button!
    QMouseEvent *switchedEvent = new QMouseEvent(QEvent::MouseButtonPress, event->localPos(), Qt::LeftButton,
                                                 Qt::LeftButton, event->modifiers());
    QtDataVisualization::Q3DInputHandler::mousePressEvent(switchedEvent, mousePos);
  }  else if (Qt::RightButton == event->button())
  {
    // Call the ContextMenu with the Right Button
    callContextMenu(event->globalPos());
  } else
  {
    // Default Button Behavior
    QtDataVisualization::Q3DInputHandler::mousePressEvent(event, mousePos);
  }
}

void cedar::aux::gui::Qt5SurfacePlotInputHandler::wheelEvent(QWheelEvent *event)
{
  QtDataVisualization::Q3DInputHandler::wheelEvent(event);
  auto curFont = m_plot->m_graph->activeTheme()->font();
  auto rotatedDegrees = event->angleDelta().y();
  int factor = rotatedDegrees > 0 ? -2:2;
  curFont.setPointSize(std::max(curFont.pointSize() + factor,100));
//  std::cout << "CurrentSize : "<< curFont.pointSize() + factor << std::endl;
  m_plot->m_graph->activeTheme()->setFont(curFont);
}


void cedar::aux::gui::Qt5SurfacePlotInputHandler::callContextMenu(QPoint menuPos)
{
  QMenu menu(m_plot);

  QAction *p_reset_perspective = menu.addAction("reset perspective");

  QMenu *p_perspectives = menu.addMenu("set perspective");
  for (size_t i = 0; i < m_plot->mPerspectives.size(); ++i)
  {
    QAction *p_action = p_perspectives->addAction(m_plot->mPerspectives.at(i).getName().c_str());
    p_action->setData(QVariant(static_cast<uint>(i)));
  }

  menu.addSeparator();

  QAction *p_show_grid = menu.addAction("show grid");
  p_show_grid->setCheckable(true);
  p_show_grid->setChecked(m_plot->mShowGridLines);

  menu.addSeparator();
  QAction *p_font_bold = menu.addAction("bold font");
  p_font_bold->setCheckable(true);
  p_font_bold->setChecked(m_plot->mIsBold);


  QAction *p_action = menu.exec(menuPos);

  if (p_action != NULL)
  {
    if (p_action == p_reset_perspective)
    {
      m_plot->resetPerspective();
    }
    else if (p_action->parentWidget() == p_perspectives)
    {
      size_t index = static_cast<size_t>(p_action->data().toUInt());
      m_plot->resetPerspective(index);
    }
    else if (p_action == p_show_grid)
    {
      m_plot->showGrid(!m_plot->mShowGridLines);
    }
    else if (p_action == p_font_bold)
    {
      m_plot->setBold(!m_plot->mIsBold);
    }
  }
}
#endif // CEDAR_USE_QT5_PLOTS


