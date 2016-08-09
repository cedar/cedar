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
QWidget(parent),
mpScene(new cedar::aux::gl::Scene)
{
  this->setupUi(this);

  // initialize scene
  mpScene->setSceneLimit(1);
  mpScene->drawFloor(true);

  // initialize widget containing object manipulation controls
  mpSceneWidget = new cedar::aux::gui::SceneWidget(mpScene, this);
  mpSceneWidget->setObjectName(QString::fromUtf8("mpSceneWidget"));
  horizontalLayout->addWidget(mpSceneWidget);

  // create an initial viewer widget
  mpView = new cedar::aux::gui::Viewer(mpScene);
  mpView->show();
  mpView->setSceneRadius(mpScene->getSceneLimit());
  mpView->camera()->setPosition(qglviewer::Vec(0.0, 0.0, 4.0));
  mpView->camera()->lookAt(qglviewer::Vec(0.0, 0.0, 0.0));
  mpView->startTimer(25);
  mpView->setObjectName(QString::fromUtf8("mpView"));
  horizontalLayout->addWidget(mpView);
}
