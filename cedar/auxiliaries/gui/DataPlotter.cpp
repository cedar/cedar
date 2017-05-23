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
#include "cedar/auxiliaries/gui/PlotManager.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::gui::DataPlotter::DataPlotter(QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpCurrentPlot(NULL)
{
  QVBoxLayout *p_layout = new QVBoxLayout();
  this->setLayout(p_layout);

  this->setContentsMargins(0, 0, 0, 0);
  this->layout()->setContentsMargins(0, 0, 0, 0);
}

cedar::aux::gui::DataPlotter::~DataPlotter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::aux::gui::DataPlotter::canAppend(cedar::aux::ConstDataPtr data) const
{
  if (this->mpCurrentPlot != NULL)
  {
    if (cedar::aux::gui::MultiPlotInterface* multi_plotter
          = dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(this->mpCurrentPlot))
    {
      return multi_plotter->canAppend(data);
    }
  }
  return false;
}

void cedar::aux::gui::DataPlotter::doAppend(cedar::aux::ConstDataPtr data, const std::string& title)
{
  if (this->mpCurrentPlot == NULL)
  {
    this->plot(data, title);
  }
  else
  {
    cedar::aux::gui::MultiPlotInterface* multi_plotter
      = dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(this->mpCurrentPlot);

    CEDAR_ASSERT(multi_plotter);
    multi_plotter->append(data, title);
  }
}

void cedar::aux::gui::DataPlotter::plot(cedar::aux::ConstDataPtr data, const std::string& title)
{
  this->mData = data;
  this->mTitle = title;

  // get the declaration corresponding to the data to be plotted
  cedar::aux::gui::ConstPlotDeclarationPtr declaration
    = cedar::aux::gui::PlotManagerSingleton::getInstance()->getDefaultDeclarationFor(data);

  // remove current plot
  if (this->mpCurrentPlot != NULL)
  {
    delete this->mpCurrentPlot;
    this->mpCurrentPlot = NULL;
  }

  // create the plot
  this->mpCurrentPlot = declaration->createPlot();
  QObject::connect(this->mpCurrentPlot, SIGNAL(dataChanged()), this, SLOT(dataChanged()));
  this->mpCurrentPlot->plot(data, title);

  // add the plot to the layout
  this->layout()->addWidget(this->mpCurrentPlot);
}

void cedar::aux::gui::DataPlotter::dataChanged()
{
  this->plot(mData, this->mTitle);
}