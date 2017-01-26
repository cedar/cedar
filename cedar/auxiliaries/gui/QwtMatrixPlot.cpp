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

#define NOMINMAX // prevents MSVC conflicts

// CEDAR INCLUDES
#include "cedar/configuration.h"
#include "cedar/auxiliaries/gui/QwtMatrixPlot.h"
#ifdef CEDAR_USE_QWT
  #include "cedar/auxiliaries/gui/QwtLinePlot.h"
  #include "cedar/auxiliaries/gui/HistoryPlot0D.h"
#endif // CEDAR_USE_QWT
#ifdef CEDAR_USE_QWTPLOT3D
  #include "cedar/auxiliaries/gui/QwtSurfacePlot.h"
#endif // CEDAR_USE_QWTPLOT3D
#include "cedar/auxiliaries/gui/ImagePlot.h"
#include "cedar/auxiliaries/gui/MatrixSlicePlot3D.h"
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
#include <limits>

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------
#if defined(CEDAR_USE_QWT) || defined(CEDAR_USE_QWTPLOT3D)
namespace
{
  bool registerPlot()
  {
    using cedar::aux::MatData;
    using cedar::aux::gui::QwtMatrixPlot;

    typedef cedar::aux::gui::PlotDeclarationTemplate<MatData, QwtMatrixPlot> DeclarationType;

    boost::shared_ptr<DeclarationType> declaration(new DeclarationType());
    declaration->declare();

    return true;
  }

  bool registered = registerPlot();
}
#endif // defined(CEDAR_USE_QWT) || defined(CEDAR_USE_QWTPLOT3D)

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::QwtMatrixPlot::QwtMatrixPlot(QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpCurrentPlotWidget(NULL),
mTitle("")
{
  QVBoxLayout *p_layout = new QVBoxLayout();
  this->setLayout(p_layout);

  this->setContentsMargins(0, 0, 0, 0);
  p_layout->setContentsMargins(0, 0, 0, 0);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::QwtMatrixPlot::readConfiguration(const cedar::aux::ConfigurationNode& configuration)
{
  if (auto configurable = dynamic_cast<cedar::aux::Configurable*>(this->mpCurrentPlotWidget))
  {
    configurable->readConfiguration(configuration);
  }
}

void cedar::aux::gui::QwtMatrixPlot::writeConfiguration(cedar::aux::ConfigurationNode& configuration) const
{
  if (auto configurable = dynamic_cast<cedar::aux::Configurable*>(this->mpCurrentPlotWidget))
  {
    configurable->writeConfiguration(configuration);
  }
}

bool cedar::aux::gui::QwtMatrixPlot::canAppend(cedar::aux::ConstDataPtr data) const
{
  if (this->mpCurrentPlotWidget == nullptr)
  {
    return false;
  }
  else if
  (
    auto p_multi_plot = dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(this->mpCurrentPlotWidget)
  )
  {
    return p_multi_plot->canAppend(data);
  }
  else
  {
    return false;
  }
}

void cedar::aux::gui::QwtMatrixPlot::doAppend(cedar::aux::ConstDataPtr data, const std::string& title)
{
  CEDAR_DEBUG_ASSERT(this->mpCurrentPlotWidget != nullptr);
  auto p_multi_plot = dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(this->mpCurrentPlotWidget);

  CEDAR_DEBUG_ASSERT(p_multi_plot != nullptr);
  p_multi_plot->append(data, title);
  mTitle = title;
}

bool cedar::aux::gui::QwtMatrixPlot::canDetach(cedar::aux::ConstDataPtr data) const
{
  if (this->mpCurrentPlotWidget == nullptr)
  {
    return false;
  }
  else if (auto p_multi_plot = dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(this->mpCurrentPlotWidget))
  {
    return p_multi_plot->canDetach(data);
  }
  else
  {
    return false;
  }
}

void cedar::aux::gui::QwtMatrixPlot::doDetach(cedar::aux::ConstDataPtr data)
{
  CEDAR_DEBUG_ASSERT(this->mpCurrentPlotWidget != nullptr);
  auto p_multi_plot = dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(this->mpCurrentPlotWidget);

  CEDAR_DEBUG_ASSERT(p_multi_plot != nullptr);
  p_multi_plot->detach(data);
}

void cedar::aux::gui::QwtMatrixPlot::plot(cedar::aux::ConstDataPtr data, const std::string& title)
{
  this->mData= boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  if (!this->mData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Cannot cast to cedar::aux::MatData in cedar::aux::gui::MatrixPlot::plot.");
  }

  if (this->mpCurrentPlotWidget)
  {
    delete this->mpCurrentPlotWidget;
    this->mpCurrentPlotWidget = NULL;
  }

  const cv::Mat& mat = this->mData->getData();
  unsigned int dims = cedar::aux::math::getDimensionalityOf(mat);
  if (mat.empty())
  {
    dims = UINT_MAX;
  }

  switch (dims)
  {
#ifdef CEDAR_USE_QWT
    case 0:
      this->mpCurrentPlotWidget = new cedar::aux::gui::HistoryPlot0D(this->mData, title);
      connect(this->mpCurrentPlotWidget, SIGNAL(dataChanged()), this, SLOT(processChangedData()));
      break;

    case 1:
      this->mpCurrentPlotWidget = new cedar::aux::gui::QwtLinePlot(this->mData, title);
      connect(this->mpCurrentPlotWidget, SIGNAL(dataChanged()), this, SLOT(processChangedData()));
      break;
#endif // CEDAR_USE_QWT

    case 2:
    {
#ifdef CEDAR_USE_QWTPLOT3D
      this->mpCurrentPlotWidget = new cedar::aux::gui::QwtSurfacePlot(this->mData, title);
#else
      this->mpCurrentPlotWidget = new cedar::aux::gui::ImagePlot(this->mData, title);
#endif // CEDAR_USE_QWTPLOT3D

      connect(this->mpCurrentPlotWidget, SIGNAL(dataChanged()), this, SLOT(processChangedData()));
      break;
    }

    case 3:
    {
      this->mpCurrentPlotWidget = new cedar::aux::gui::MatrixSlicePlot3D(this->mData, title);
      connect(this->mpCurrentPlotWidget, SIGNAL(dataChanged()), this, SLOT(processChangedData()));
      break;
    }

    case UINT_MAX:
    {
      std::string message = "The qwt matrix plot widget can not handle empty matrices.";
      message += "\nPress here to refresh the plot after you have changed the dimensionality.";
      this->mpCurrentPlotWidget = new QPushButton(QString::fromStdString(message));
      connect(this->mpCurrentPlotWidget, SIGNAL(pressed()), this, SLOT(processChangedData()));
      break;
    }

    default:
    {
      std::string message = "The qwt matrix plot widget can not handle a matrix with the given dimensionality (";
      message += cedar::aux::toString(mat.dims);
      message += ").\nPress here to refresh the plot after you have changed the dimensionality.";
      this->mpCurrentPlotWidget = new QPushButton(QString::fromStdString(message));
      connect(this->mpCurrentPlotWidget, SIGNAL(pressed()), this, SLOT(processChangedData()));
    }
  }
  this->layout()->addWidget(this->mpCurrentPlotWidget);
  mTitle = title;
}


void cedar::aux::gui::QwtMatrixPlot::processChangedData()
{
  if (mpCurrentPlotWidget)
  {
    if (auto p_multi = dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(mpCurrentPlotWidget))
    {
      // first, recover data from multiplot
      auto map = p_multi->getDataTitleMap();
      //QWriteLocker map_locker(p_multi->getLock());
      auto iter = map.begin();
      if (iter != map.end())
      {
        this->plot(iter->first, iter->second);
        if (dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(mpCurrentPlotWidget))
        {
          for (auto rest = ++iter; rest != map.end(); ++rest)
          {
            this->append(rest->first, rest->second);
          }
        }
      }
      //map_locker.unlock();
    }
    else
    {
      this->plot(this->mData, mTitle);
    }
  }
}
