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

    File:        SceneControl.cpp

    Maintainer:  Nico Kuerschner
    Email:       nico.kuerschner@ini.rub.de
    Date:        2016 08 08

    Description: control a simulator scene (object manipulation and view widgets)


======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/SceneControl.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::SceneControl::SceneControl(QWidget* parent)
:
QWidget(parent)
{
  this->setupUi(this);

  // create the scene for the visualization
  cedar::aux::gl::ScenePtr scene(new cedar::aux::gl::Scene);
  scene->setSceneLimit(2);
  scene->drawFloor(true);

  // widget containing object manipulation controls
  mpSceneWidget = new cedar::aux::gui::SceneWidget(scene, this);
  mpSceneWidget->setObjectName(QString::fromUtf8("mpSceneWidget"));
  horizontalLayout->addWidget(mpSceneWidget);


  // create an initial viewer widget
  mpView = new cedar::aux::gui::Viewer(scene);
  mpView->show();
  mpView->setSceneRadius(scene->getSceneLimit());
  mpView->startTimer(50);
  mpView->setObjectName(QString::fromUtf8("mpViewerWidget"));

  horizontalLayout->addWidget(mpView);
}
