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

    File:        DataPlotter.cpp

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

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/DataPlotter.h"
#include "cedar/auxiliaries/DoubleData.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/ImageData.h"
#include "cedar/auxiliaries/gui/MatrixPlot.h"
#include "cedar/auxiliaries/gui/ImagePlot.h"
#include "cedar/auxiliaries/gui/HistoryPlot.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::gui::DataPlotter::DataPlotter(QWidget *pParent)
:
cedar::aux::gui::PlotInterface(pParent)
{
  QVBoxLayout *p_layout = new QVBoxLayout();
  this->setLayout(p_layout);
  this->layout()->setContentsMargins(0, 0, 0, 0);
}

cedar::aux::gui::DataPlotter::~DataPlotter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::DataPlotter::plot(cedar::aux::DataPtr data, const std::string& title)
{
  this->mData = data;
  this->mTitle = title;

  //!@todo doesn't work this way -- related to the to-do entry below.
//  PlotWidgetInterface *p_widget = getWidgetFactory().get(data)->allocateRaw();
//  p_widget->plot(data);

  //!@todo find a better solution for this!
  cedar::aux::gui::PlotInterface *p_plot = NULL;
  if (dynamic_cast<cedar::aux::ImageData*>(data.get()))
  {
    p_plot = new cedar::aux::gui::ImagePlot(this);
  }
  else if (dynamic_cast<cedar::aux::MatData*>(data.get()))
  {
    p_plot = new cedar::aux::gui::MatrixPlot(this);
  }
  else if (dynamic_cast<cedar::aux::DoubleData*>(data.get()))
  {
    p_plot = new cedar::aux::gui::HistoryPlot(this);
  }
  else
  {
    CEDAR_THROW(cedar::aux::UnhandledTypeException, "Unhandled data type in cedar::aux::gui::DataPlotter::plot.");
  }
  connect(p_plot, SIGNAL(dataChanged()), this, SLOT(dataChanged()));
  p_plot->plot(data, title);

  // safely remove all children
  QLayoutItem *child;
  while ((child = this->layout()->takeAt(0)) != 0)
  {
    delete child;
  }

  this->layout()->addWidget(p_plot);
}

void cedar::aux::gui::DataPlotter::dataChanged()
{
  this->plot(mData, this->mTitle);
}
