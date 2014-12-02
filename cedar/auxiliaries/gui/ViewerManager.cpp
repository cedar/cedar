/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ViewerManager.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2014 10 21

    Description: Source file for the class cedar::aux::gui::ViewerManager.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/ViewerManager.h"
#include "cedar/auxiliaries/gl/GlobalScene.h"

// SYSTEM INCLUDES
#include <boost/make_shared.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::ViewerManager::ViewerManager()
{
}

cedar::aux::gui::ViewerManager::~ViewerManager()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::gui::ViewerPtr cedar::aux::gui::ViewerManager::getNewUnnamedViewer()
{
  auto scene = cedar::aux::gl::GlobalSceneSingleton::getInstance();

  auto viewer = boost::make_shared< cedar::aux::gui::Viewer >( scene );

  // todo: move this to somplace else
  scene->setSceneLimit(2);
  scene->drawFloor(true);

  // sensible default settings:
  viewer->setSceneRadius( scene->getSceneLimit() );

  mViewers.push_back( viewer );

  return viewer;
}


