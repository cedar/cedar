/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        SceneControl.h

    Maintainer:  Nico Kuerschner
    Email:       nico.kuerschner@ini.rub.de
    Date:        2016 08 08

    Description: control a simulator scene (object manipulation and view widgets)


======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_SCENE_CONTROL_H
#define CEDAR_AUX_GUI_SCENE_CONTROL_H

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/SceneControl.fwd.h"

// CEDAR INCLUDES
#ifdef CEDAR_USE_QGLVIEWER
#include "cedar/auxiliaries/gui/ui_SceneControl.h"
#else
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidget>
#endif // CEDAR_USE_QGLVIEWER
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/gl/GlobalScene.h"



class cedar::aux::gui::SceneControl : public QWidget
#ifdef CEDAR_USE_QGLVIEWER
        , private Ui_SceneControl
#endif // CEDAR_USE_QGLVIEWER
{
  Q_OBJECT

public:

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------

  /*!@brief constructor with parent widget */
  SceneControl(QWidget* parent=0);
  
  /*!@brief the destructor */
  ~SceneControl(){}

  cedar::aux::gl::ScenePtr getScene() const;

  void showTab();

private:
  cedar::aux::gl::ScenePtr mpScene;
  bool started = false;

}; // class cedar::aux:gui::SceneControl

#endif // CEDAR_AUX_GUI_SCENE_CONTROL_H
