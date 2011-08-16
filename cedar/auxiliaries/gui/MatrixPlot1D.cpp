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

    File:        MatrixPlot2D.cpp

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

// LOCAL INCLUDES
#include "auxiliaries/gui/MatrixPlot1D.h"
#include "auxiliaries/gui/exceptions.h"
#include "auxiliaries/DataT.h"
#include "auxiliaries/exceptions.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::MatrixPlot1D::MatrixPlot1D(QWidget *pParent)
:
cedar::aux::gui::DataPlotInterface(pParent)
{
  this->init();
}

cedar::aux::gui::MatrixPlot1D::MatrixPlot1D(cedar::aux::DataPtr matData, QWidget *pParent)
:
cedar::aux::gui::DataPlotInterface(pParent)
{
  this->init();
  this->display(matData);
}

cedar::aux::gui::MatrixPlot1D::~MatrixPlot1D()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::aux::gui::MatrixPlot1D::display(cedar::aux::DataPtr data)
{
  this->mMatData = boost::shared_dynamic_cast<cedar::aux::MatData>(data);

  if (!this->mMatData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Could not cast to cedar::aux::MatData in cedar::aux::gui::MatrixPlot1D::display.");
  }
  //!@todo implement
  std::cout << "Sorry -- the matrixplot1d isn't implemented yet. That makes me a sad panda!" << std::endl;
  this->startTimer(30); //!@todo make the refresh time configurable.
}

void cedar::aux::gui::MatrixPlot1D::init()
{
  //TODO
}

void cedar::aux::gui::MatrixPlot1D::timerEvent(QTimerEvent * /* pEvent */)
{
  //TODO
}
