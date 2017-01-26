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

    File:        MatDataPlot.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 08 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/MatDataPlot.h"
#include "cedar/auxiliaries/gui/PlotManager.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/gui/ImagePlot.h"
#include "cedar/auxiliaries/gui/MatrixPlot.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/annotation/ColorSpace.h"
#include "cedar/auxiliaries/annotation/Disparity.h"
#include "cedar/auxiliaries/annotation/Depth.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <QLayout>

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registerPlot()
  {
    using cedar::aux::MatData;
    using cedar::aux::gui::MatDataPlot;

    typedef cedar::aux::gui::PlotDeclarationTemplate<MatData, MatDataPlot> DeclarationType;

    boost::shared_ptr<DeclarationType> declaration(new DeclarationType());
    declaration->declare();
    cedar::aux::gui::PlotManagerSingleton::getInstance()->setDefault<MatData, MatDataPlot>();
    return true;
  }

  bool registered = registerPlot();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::MatDataPlot::MatDataPlot(QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent)
{
  this->initLayout();
}

cedar::aux::gui::MatDataPlot::MatDataPlot(cedar::aux::ConstDataPtr data, const std::string& title, QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent)
{
  this->initLayout();

  this->plot(data, title);
}


void cedar::aux::gui::MatDataPlot::initLayout()
{
  mpCurrentPlotWidget = nullptr;

  QVBoxLayout *p_layout = new QVBoxLayout();
  this->setLayout(p_layout);

  this->setContentsMargins(0, 0, 0, 0);
  p_layout->setContentsMargins(0, 0, 0, 0);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::MatDataPlot::readConfiguration(const cedar::aux::ConfigurationNode& configuration)
{
  if (auto configurable = dynamic_cast<cedar::aux::Configurable*>(this->mpCurrentPlotWidget))
  {
    configurable->readConfiguration(configuration);
  }
}

void cedar::aux::gui::MatDataPlot::writeConfiguration(cedar::aux::ConfigurationNode& configuration) const
{
  if (auto configurable = dynamic_cast<cedar::aux::Configurable*>(this->mpCurrentPlotWidget))
  {
    configurable->writeConfiguration(configuration);
  }
}

bool cedar::aux::gui::MatDataPlot::canAppend(cedar::aux::ConstDataPtr data) const
{
  if (this->mpCurrentPlotWidget == nullptr)
  {
    return false;
  }
  else if
  (
    cedar::aux::gui::MultiPlotInterface *p_multi_plot
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

bool cedar::aux::gui::MatDataPlot::canDetach(cedar::aux::ConstDataPtr data) const
{
  if (this->mpCurrentPlotWidget == nullptr)
  {
    return false;
  }
  else if
  (
    cedar::aux::gui::MultiPlotInterface *p_multi_plot
      = dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(this->mpCurrentPlotWidget)
  )
  {
    return p_multi_plot->canDetach(data);
  }
  else
  {
    return false;
  }
}

void cedar::aux::gui::MatDataPlot::doAppend(cedar::aux::ConstDataPtr data, const std::string& title)
{
  CEDAR_DEBUG_ASSERT(this->mpCurrentPlotWidget != nullptr);
  cedar::aux::gui::MultiPlotInterface *p_multi_plot
    = dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(this->mpCurrentPlotWidget);

  CEDAR_DEBUG_ASSERT(p_multi_plot != nullptr);
  p_multi_plot->append(data, title);
}

void cedar::aux::gui::MatDataPlot::doDetach(cedar::aux::ConstDataPtr data)
{
  CEDAR_DEBUG_ASSERT(this->mpCurrentPlotWidget != nullptr);
  cedar::aux::gui::MultiPlotInterface *p_multi_plot
    = dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(this->mpCurrentPlotWidget);

  CEDAR_DEBUG_ASSERT(p_multi_plot != nullptr);
  p_multi_plot->detach(data);
}

void cedar::aux::gui::MatDataPlot::plot(cedar::aux::ConstDataPtr data, const std::string& title)
{
  this->mData= boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  if (!this->mData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Cannot cast to cedar::aux::MatData in cedar::aux::gui::MatrixPlot::display.");
  }

  if (this->mpCurrentPlotWidget)
  {
    delete this->mpCurrentPlotWidget;
    this->mpCurrentPlotWidget = nullptr;
  }

  // color space-annotated data, disparity-annotated data or depth-annotated data
  if
  (
    this->mData->hasAnnotation<cedar::aux::annotation::ColorSpace>()
    || this->mData->hasAnnotation<cedar::aux::annotation::Disparity>()
    || this->mData->hasAnnotation<cedar::aux::annotation::Depth>()
    || this->mData->getData().channels() > 1
  )
  {
    // data should be plotted as an image
    cedar::aux::gui::ImagePlot* p_plot = new cedar::aux::gui::ImagePlot();
    p_plot->plot(this->mData, title);
    this->mpCurrentPlotWidget = p_plot;
  }

  // no special plot has been opened yet
  if (this->mpCurrentPlotWidget == nullptr)
  {
    // data should be plotted as a matrix
    cedar::aux::gui::MatrixPlot* p_plot = new cedar::aux::gui::MatrixPlot();
    p_plot->plot(this->mData, title);
    this->mpCurrentPlotWidget = p_plot;
  }

  this->layout()->addWidget(this->mpCurrentPlotWidget);
}
