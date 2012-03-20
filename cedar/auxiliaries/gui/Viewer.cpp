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

    File:        Viewer.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 10 28

    Description: Simple viewer for visualizing a scene of objects

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/gl/namespace.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::Viewer::Viewer(cedar::aux::gl::ScenePtr pScene)
:
mpScene(pScene)
{
  mpScene->addViewer(this);
}

cedar::aux::gui::Viewer::~Viewer()
{
  mpScene->removeViewer(this);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::Viewer::init()
{
  restoreStateFromFile();
  mpScene->initGl();
}

void cedar::aux::gui::Viewer::initGl(cedar::aux::gl::ObjectVisualizationPtr pVisualization)
{
  pVisualization->initializeGl();
}

void cedar::aux::gui::Viewer::draw()
{
  mpScene->draw();
}

void cedar::aux::gui::Viewer::timerEvent(QTimerEvent*)
{
  if (isVisible())
  {
    update();
  }
}
