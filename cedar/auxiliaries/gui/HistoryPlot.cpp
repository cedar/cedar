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
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/HistoryPlot.h"
#ifdef CEDAR_USE_QWT
  #include "cedar/auxiliaries/gui/HistoryPlot0D.h"
#endif // CEDAR_USE_QWT
#include "cedar/auxiliaries/gui/HistoryPlot1D.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/DoubleData.h"
#include "cedar/auxiliaries/UnitData.h"

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
    // Double data plot ----------------------------------------------------------------------------
    typedef
      cedar::aux::gui::PlotDeclarationTemplate<cedar::aux::DoubleData, cedar::aux::gui::HistoryPlot>
      DoubleDataDeclaration;

    boost::shared_ptr<DoubleDataDeclaration> declaration(new DoubleDataDeclaration());
    declaration->declare();

    // Mat data plot -------------------------------------------------------------------------------
    typedef
      cedar::aux::gui::PlotDeclarationTemplate<cedar::aux::MatData, cedar::aux::gui::HistoryPlot>
      MatDataDeclaration;

    boost::shared_ptr<MatDataDeclaration> decl2(new MatDataDeclaration());
    decl2->declare();

    // Unit data plot ------------------------------------------------------------------------------
    typedef
      cedar::aux::gui::PlotDeclarationTemplate<cedar::aux::UnitData, cedar::aux::gui::HistoryPlot>
      UnitDataDeclaration;

    boost::shared_ptr<UnitDataDeclaration> decl3(new UnitDataDeclaration());
    decl3->declare();

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
  this->mData = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  if (!this->mData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Cannot cast to cedar::aux::MatData in cedar::aux::gui::HistoryPlot::plot.");
  }

  if (this->mpCurrentPlotWidget)
  {
    delete this->mpCurrentPlotWidget;
    this->mpCurrentPlotWidget = NULL;
  }

  if
  (
    boost::dynamic_pointer_cast<cedar::aux::ConstDoubleData>(data)
    || boost::dynamic_pointer_cast<cedar::aux::ConstUnitData>(data)
  )
  {
#ifdef CEDAR_USE_QWT
    this->mpCurrentPlotWidget = new cedar::aux::gui::HistoryPlot0D(this->mData, title);
#else
    CEDAR_THROW
    (
      cedar::aux::gui::InvalidPlotData, "Don't know how to plot this data."
    );
#endif // CEDAR_USE_QWT
  }
  else if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
  {
    switch (mat_data->getDimensionality())
    {
#ifdef CEDAR_USE_QWT
    case 0:
      this->mpCurrentPlotWidget = new cedar::aux::gui::HistoryPlot0D(this->mData, title);
      break;
#endif // CEDAR_USE_QWT
    case 1:
      this->mpCurrentPlotWidget = new cedar::aux::gui::HistoryPlot1D(this->mData, title);
      break;

    default:
      CEDAR_THROW
      (
        cedar::aux::gui::InvalidPlotData,
        "Don't know how to plot MatData with the given dimensionality ("
          + cedar::aux::toString(mat_data->getDimensionality()) + ")."
      );
    }
  }
  else
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData, "Don't know how to plot this data.");
  }
  connect(this->mpCurrentPlotWidget, SIGNAL(dataChanged()), this, SLOT(processChangedData()));
  this->mpCurrentPlotWidget->plot(this->mData, title);
  this->layout()->addWidget(this->mpCurrentPlotWidget);
}

void cedar::aux::gui::HistoryPlot::processChangedData()
{
  this->plot(this->mData, "");
}
