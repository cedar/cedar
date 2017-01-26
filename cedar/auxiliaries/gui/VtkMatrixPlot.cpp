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

    File:        VtkMatrixPlot.cpp

    Maintainer:  Kai Kuchenbecker
    Email:       Kai.Kuchenbecker@ini.ruhr-uni-bochum.de,
    Date:        2013 05 05

    Description:

    Credits:

======================================================================================================================*/
#define NOMINMAX // prevents MSVC conflicts

// CEDAR INCLUDES
#include "cedar/configuration.h"

#ifdef CEDAR_USE_VTK

#include "cedar/auxiliaries/gui/VtkMatrixPlot.h"
#include "cedar/auxiliaries/gui/VtkLinePlot.h"
#include "cedar/auxiliaries/gui/VtkSurfacePlot.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/gui/PlotManager.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <iostream>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registerPlot()
  {
    using cedar::aux::MatData;
    using cedar::aux::gui::VtkMatrixPlot;

    typedef cedar::aux::gui::PlotDeclarationTemplate<MatData, VtkMatrixPlot> DeclarationType;

    boost::shared_ptr<DeclarationType> declaration(new DeclarationType());
    declaration->declare();

    return true;
  }

  bool registered = registerPlot();
}

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::VtkMatrixPlot::VtkMatrixPlot(QWidget* pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpCurrentPlotWidget(nullptr)
{
  QVBoxLayout* p_layout = new QVBoxLayout();
  this->setLayout(p_layout);

  this->setContentsMargins(0, 0, 0, 0);
  p_layout->setContentsMargins(0, 0, 0, 0);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::aux::gui::VtkMatrixPlot::canAppend(cedar::aux::ConstDataPtr data) const
{
  if (this->mpCurrentPlotWidget == nullptr)
  {
    return false;
  }
  else if
  (
    cedar::aux::gui::MultiPlotInterface* p_multi_plot
      = dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(this->mpCurrentPlotWidget)
  )
  {
    return p_multi_plot->canAppend(data);
  }
  else
  {
    return false;
  }
}

void cedar::aux::gui::VtkMatrixPlot::doAppend(cedar::aux::ConstDataPtr data, const std::string& title)
{
  CEDAR_DEBUG_ASSERT(this->mpCurrentPlotWidget != nullptr);
  cedar::aux::gui::MultiPlotInterface* p_multi_plot
    = dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(this->mpCurrentPlotWidget);

  CEDAR_DEBUG_ASSERT(p_multi_plot != nullptr);
  p_multi_plot->append(data, title);
}

bool cedar::aux::gui::VtkMatrixPlot::canDetach(cedar::aux::ConstDataPtr data) const
{
  if (this->mpCurrentPlotWidget == nullptr)
  {
    return false;
  }
  else if
  (
    cedar::aux::gui::MultiPlotInterface* p_multi_plot
      = dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(this->mpCurrentPlotWidget)
  )
  {
    return p_multi_plot->canAppend(data);
  }
  else
  {
    return false;
  }
}

void cedar::aux::gui::VtkMatrixPlot::doDetach(cedar::aux::ConstDataPtr data)
{
  CEDAR_DEBUG_ASSERT(this->mpCurrentPlotWidget != nullptr);
  cedar::aux::gui::MultiPlotInterface* p_multi_plot
    = dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(this->mpCurrentPlotWidget);

  CEDAR_DEBUG_ASSERT(p_multi_plot != nullptr);
  p_multi_plot->detach(data);
}

void cedar::aux::gui::VtkMatrixPlot::plot(cedar::aux::ConstDataPtr data, const std::string& title)
{
  this->mData= boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  if (!this->mData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Cannot cast to cedar::aux::MatData in cedar::aux::gui::VtkMatrixPlot::display.");
  }

  if (this->mpCurrentPlotWidget)
  {
    delete this->mpCurrentPlotWidget;
    this->mpCurrentPlotWidget = nullptr;
  }

  const cv::Mat& mat = this->mData->getData();
  unsigned int dims = cedar::aux::math::getDimensionalityOf(mat);

  switch (dims)
  {
    case 1:
      this->mpCurrentPlotWidget = new cedar::aux::gui::VtkLinePlot(this->mData, title);
      connect(this->mpCurrentPlotWidget, SIGNAL(dataChanged()), this, SLOT(processChangedData()));
      break;

    case 2:
      this->mpCurrentPlotWidget = new cedar::aux::gui::VtkSurfacePlot(this->mData, title);
      connect(this->mpCurrentPlotWidget, SIGNAL(dataChanged()), this, SLOT(processChangedData()));
      break;

    default:
    {
      std::string message = "The VTK matrix plot widget can not handle a matrix with the given dimensionality (";
      message += cedar::aux::toString(mat.dims);
      message += ")\nPress here to refresh the plot after you have changed the dimensionality.";
      this->mpCurrentPlotWidget = new QPushButton(QString::fromStdString(message));
      connect(this->mpCurrentPlotWidget, SIGNAL(pressed()), this, SLOT(processChangedData()));
    }
  }
  this->layout()->addWidget(this->mpCurrentPlotWidget);
}

void cedar::aux::gui::VtkMatrixPlot::processChangedData()
{
  this->plot(this->mData, "");
}

#endif // CEDAR_USE_VTK
