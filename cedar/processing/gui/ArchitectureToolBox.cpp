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

    File:        ArchitectureToolBox.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 11

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/ArchitectureToolBox.h"
#include "cedar/processing/gui/View.h"
#include "cedar/processing/gui/exceptions.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::ArchitectureToolBox::ArchitectureToolBox(QWidget *pParent)
:
cedar::proc::gui::ToolBox(1, pParent),
mpView(NULL)
{
  this->addItem(":/modeicons/select.svg", "mode.Select", "selection mode");
  QObject::connect(this, SIGNAL(selectionChanged(QString)), this, SLOT(selectionChanged(QString)));
}

cedar::proc::gui::ArchitectureToolBox::~ArchitectureToolBox()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::ArchitectureToolBox::setView(cedar::proc::gui::View *pView)
{
  this->mpView = pView;
}

void cedar::proc::gui::ArchitectureToolBox::selectionChanged(QString data)
{
  QStringList list = data.split(":");
  const QString& mode = list[0];
  QString param = "";

  if (list.size() > 1)
  {
    param = list[1];
  }

  CEDAR_DEBUG_ASSERT(this->mpView != NULL);

  cedar::proc::gui::Scene::MODE mode_val;
  if (mode == "mode.Select")
  {
    mode_val = cedar::proc::gui::Scene::MODE_SELECT;
  }
  else if (mode == "mode.Connect")
  {
    mode_val = cedar::proc::gui::Scene::MODE_CONNECT;
  }
  else
  {
    CEDAR_THROW(cedar::proc::gui::InvalidModeException, "This mode is not known.");
  }

  this->mpView->setMode(mode_val, param);
}
