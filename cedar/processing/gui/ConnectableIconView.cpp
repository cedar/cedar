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

    File:        ConnectableIconView.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 08 01

    Description: Source file for the class cedar::proc::gui::ConnectableIconView.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ConnectableIconView.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::ConnectableIconView::ConnectableIconView()
{
}

cedar::proc::gui::ConnectableIconView::~ConnectableIconView()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::ConnectableIconView::setConnectable(cedar::proc::ConstConnectablePtr connectable)
{
  this->mConnectable = connectable;

  this->connectableChanged();
}

void cedar::proc::gui::ConnectableIconView::connectableChanged()
{
  // default implementation is empty
}

cedar::proc::ConstConnectablePtr cedar::proc::gui::ConnectableIconView::getConnectable() const
{
  return this->mConnectable;
}

void cedar::proc::gui::ConnectableIconView::prepareSvgExport()
{
  // empty default implementation
}

void cedar::proc::gui::ConnectableIconView::unprepareSvgExport()
{
  // empty default implementation
}

