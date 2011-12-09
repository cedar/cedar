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

    File:        MatrixPlot.cpp

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
#include "cedar/auxiliaries/gui/HistoryPlot.h"
#include "cedar/auxiliaries/gui/HistoryPlot0D.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/DoubleData.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <iostream>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::HistoryPlot::HistoryPlot(QWidget *pParent)
:
cedar::aux::gui::DataPlotInterface(pParent),
mpCurrentPlotWidget(NULL)
{
  QVBoxLayout *p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);
}

cedar::aux::gui::HistoryPlot::~HistoryPlot()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::HistoryPlot::display(cedar::aux::DataPtr data)
{
  //!@todo implement for matrices and more dimensions
  if (this->mpCurrentPlotWidget)
  {
    delete this->mpCurrentPlotWidget;
    this->mpCurrentPlotWidget = NULL;
  }

  if (this->mData = boost::shared_dynamic_cast<cedar::aux::DoubleData>(data))
  {
    this->mpCurrentPlotWidget = new cedar::aux::gui::HistoryPlot0D(this->mData);
  }
  else
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData, "Don't know how to plot this data.");
  }
  this->layout()->addWidget(this->mpCurrentPlotWidget);
}
