/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/HistoryPlot.h"
#include "cedar/auxiliaries/gui/HistoryPlot0D.h"
#include "cedar/auxiliaries/gui/HistoryPlot1D.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/DoubleData.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registerPlot()
  {
    typedef
      cedar::aux::gui::PlotDeclarationTemplate<cedar::aux::DoubleData, cedar::aux::gui::HistoryPlot>
      DeclarationType;

    boost::shared_ptr<DeclarationType> declaration(new DeclarationType());
    declaration->declare();

    typedef
      cedar::aux::gui::PlotDeclarationTemplate<cedar::aux::MatData, cedar::aux::gui::HistoryPlot>
      DeclarationType2;
    boost::shared_ptr<DeclarationType2> decl2(new DeclarationType2());
    decl2->declare();

    return true;
  }

  bool registered = registerPlot();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::gui::HistoryPlot::HistoryPlot(QWidget *pParent)
:
cedar::aux::gui::PlotInterface(pParent),
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
void cedar::aux::gui::HistoryPlot::plot(cedar::aux::ConstDataPtr data, const std::string& title)
{
  //!@todo implement for matrices and more dimensions
  if (this->mpCurrentPlotWidget)
  {
    delete this->mpCurrentPlotWidget;
    this->mpCurrentPlotWidget = NULL;
  }

  this->mData = data;
  if (boost::shared_dynamic_cast<cedar::aux::ConstDoubleData>(data))
  {
    this->mpCurrentPlotWidget = new cedar::aux::gui::HistoryPlot0D();
  }
  else if ((cedar::aux::ConstMatDataPtr mat_data = boost::shared_dynamic_cast<cedar::aux::ConstMatData>(data)))
  {
    switch (mat_data->getDimensionality())
    {
      case 1:
        this->mpCurrentPlotWidget = new cedar::aux::gui::HistoryPlot1D();
        break;

      default:
        CEDAR_THROW
        (
          cedar::aux::gui::InvalidPlotData,
          "Don't know how to plot MatData with the given dimensionality ("
            + cedar::aux::toString(mat_data->getDimensionality()) + "."
        );
    }
  }
  else
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData, "Don't know how to plot this data.");
  }
  this->mpCurrentPlotWidget->plot(this->mData, title);
  this->layout()->addWidget(this->mpCurrentPlotWidget);
}
