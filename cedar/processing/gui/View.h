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

    File:        ProcessingView.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 05

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_VIEW_H
#define CEDAR_PROC_VIEW_H

// CEDAR INCLUDES
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/namespace.h"

// SYSTEM INCLUDES
#include <QGraphicsView>
#include <QGraphicsRectItem>


/*!@brief Class that displays a cedar::proc::gui::Scene.
 */
class cedar::proc::gui::View : public QGraphicsView
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  View(QWidget *pParent = NULL);

  //!@brief Destructor
  ~View();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief return the displayed scene
  cedar::proc::gui::Scene* getScene();

  /*!@brief sets the current operation mode in the scene, chosen from cedar::proc::gui::Scene::MODE
   * @param mode the operation mode taken from cedar::proc::gui::Scene::MODE
   * @param param parameter string for the chosen mode
   */
  void setMode(cedar::proc::gui::Scene::MODE mode, const QString& param = "");

  /*!@brief Returns the current zoom level.
   */
  double getZoomLevel() const
  {
    return this->mCurrentZoomLevel;
  }

public slots:
  /*!@brief Changes the current zoom level of the architecture.
   */
  void setZoomLevel(int newLevel);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief handles resize events
  void resizeEvent(QResizeEvent *pEvent);

signals:
  void zoomLevelChanged(double newZoomLevel);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief a pointer to a scene displayed by the view
  cedar::proc::gui::Scene* mpScene;

  //!@brief Variable to keep track of the current zoom level.
  qreal mCurrentZoomLevel;

}; // class ProcessingView

#endif // CEDAR_PROC_VIEW_H
