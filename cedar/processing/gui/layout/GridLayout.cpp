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

    File:        GridLayout.cpp

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2022 08 16

    Description: Source file for the class cedar::proc::gui::layout::GridLayout.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/gui/layout/GridLayout.h"
#include "cedar/processing/gui/Group.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::layout::GridLayout::GridLayout()
{
}

cedar::proc::gui::layout::GridLayout::~GridLayout()
{
}

void cedar::proc::gui::layout::GridLayout::arrange(cedar::proc::gui::Group* group)
{
  std::map<std::string, cedar::proc::ElementPtr> elements = group->getGroup()->getElements();
  int numElements = elements.size();
  if(numElements <= 0)
  {
    return;
  }
  int width = ceil(sqrt(numElements));
  int i = 0;
  for (auto iter = elements.begin(); iter != elements.end(); ++iter)
  {
    cedar::proc::gui::Element* guiElement = group->getScene()->getGraphicsItemFor(iter->second);
    guiElement->setPos((i % width) * 200, (i / width) * 110);
    i++;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
