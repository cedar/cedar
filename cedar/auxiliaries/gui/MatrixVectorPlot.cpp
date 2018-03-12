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

    File:        MatrixVectorPlot.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 07 11

    Description:

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"

#ifdef CEDAR_USE_QWT

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/MatrixVectorPlot.h"
#include "cedar/auxiliaries/gui/MatrixPlot.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/math/Limits.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <qwt_legend.h>
#include <qwt_scale_div.h>
#include <QContextMenuEvent>
#include <QVBoxLayout>
#include <QPalette>
#include <QMenu>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registerPlot()
  {
    typedef cedar::aux::gui::PlotDeclarationTemplate
            <
              cedar::aux::MatData,
              cedar::aux::gui::MatrixVectorPlot
            > DeclarationType;
    boost::shared_ptr<DeclarationType> declaration(new DeclarationType());
    declaration->declare();

    return true;
  }

  bool registered = registerPlot();
}

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

std::vector<QColor> cedar::aux::gui::MatrixVectorPlot::mLineColors;
std::vector<Qt::PenStyle> cedar::aux::gui::MatrixVectorPlot::mLineStyles;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::MatrixVectorPlot::MatrixVectorPlot(QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpLock(new QReadWriteLock())
{
  this->init();
}

cedar::aux::gui::MatrixVectorPlot::MatrixVectorPlot(cedar::aux::DataPtr matData, const std::string& title, QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpLock(new QReadWriteLock())
{
  this->init();
  this->plot(matData, title);
}

cedar::aux::gui::MatrixVectorPlot::~MatrixVectorPlot()
{
  this->mPlotSeriesVector.clear();
  
  if (mpLock)
  {
    delete mpLock;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::math::Limits<double> cedar::aux::gui::MatrixVectorPlot::getXLimits() const
{
  cedar::aux::math::Limits<double> limits;

#if QWT_VERSION >= 0x060100
  const QwtScaleDiv* p_interval = &this->mpPlot->axisScaleDiv(QwtPlot::xBottom);
#else // QWT_VERSION >= 0x060100
  QwtScaleDiv* p_interval = this->mpPlot->axisScaleDiv(QwtPlot::xBottom);
#endif // QWT_VERSION >= 0x060100
  limits.setLower(p_interval->lowerBound());
  limits.setUpper(p_interval->upperBound());
  return limits;
}

void cedar::aux::gui::MatrixVectorPlot::applyStyle(size_t lineId, QwtPlotCurve *pCurve)
{
  // initialize vectors, if this has not happened, yet
  if (mLineColors.empty() || mLineStyles.empty())
  {
    mLineColors.clear();
    mLineStyles.clear();

    mLineColors.push_back(QColor(0, 121, 177));
    mLineColors.push_back(QColor(48, 162, 26));
    mLineColors.push_back(QColor(224, 49, 37));
    mLineColors.push_back(QColor(217, 220, 0));
    mLineColors.push_back(QColor(118, 0, 177));
    mLineColors.push_back(QColor(46, 221, 190));
    mLineColors.push_back(QColor(245, 156, 16));

    mLineStyles.push_back(Qt::SolidLine);
    mLineStyles.push_back(Qt::DashLine);
    mLineStyles.push_back(Qt::DotLine);
    mLineStyles.push_back(Qt::DashDotLine);
    mLineStyles.push_back(Qt::DashDotDotLine);
    mLineStyles.push_back(Qt::CustomDashLine);
  }

  const size_t color_count = mLineColors.size();
  const size_t style_count = mLineStyles.size();
  const size_t max_line_id = mLineStyles.size() * mLineStyles.size();

  size_t line_id = lineId % max_line_id;
  size_t color_id = line_id % color_count;
  size_t style_id = (line_id / color_count) % style_count;

  // get old pen
  QPen pen = pCurve->pen();

  // modify accordingly
  pen.setColor(mLineColors.at(color_id));
  pen.setStyle(mLineStyles.at(style_id));
  pen.setWidthF(2);

  // pass pen back to curve
  pCurve->setPen(pen);
}

bool cedar::aux::gui::MatrixVectorPlot::canAppend(cedar::aux::ConstDataPtr data) const
{
  cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  if (!mat_data)
  {
    return false;
  }
  const cv::Mat& matrix = mat_data->getData();

  if (cedar::aux::math::getDimensionalityOf(matrix) != 1)
  {
    return false;
  }

  if
  (
    this->mPlotSeriesVector.size() == 0
    || cedar::aux::math::get1DMatrixSize(matrix) != 2)
  {
    return false;
  }

  return true;
}

void cedar::aux::gui::MatrixVectorPlot::doAppend(cedar::aux::ConstDataPtr data, const std::string& title)
{
  PlotSeriesPtr plot_series(new PlotSeries());

  size_t line_id = mPlotSeriesVector.size();

  QWriteLocker locker(mpLock);
  mPlotSeriesVector.push_back(plot_series);

  plot_series->mMatData = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);


  if (!plot_series->mMatData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Could not cast to cedar::aux::MatData in cedar::aux::gui::MatrixVectorPlot::plot.");
  }

  plot_series->mpCurve = new QwtPlotCurve(title.c_str());
  applyStyle(line_id, plot_series->mpCurve);

  QReadLocker data_locker(&data->getLock());
  const cv::Mat& mat = plot_series->mMatData->getData();
  size_t num = cedar::aux::math::get1DMatrixSize(mat);
  data_locker.unlock();

  // skip if the matrix is empty
  if (num == 0)
  {
    return;
  }

  this->buildArrays(plot_series, num);

#if (QWT_VERSION >= 0x060000)
  plot_series->mpCurve->setRawSamples(&plot_series->mXValues.at(0), &plot_series->mYValues.at(0), num);
#elif (QWT_VERSION >= 0x050000)
  plot_series->mpCurve->setData(&plot_series->mXValues.at(0), &plot_series->mYValues.at(0), num);
#else
#error unsupported qwt version
#endif
  plot_series->mpCurve->attach(this->mpPlot);

  locker.unlock();

  this->startTimer(30);
}

bool cedar::aux::gui::MatrixVectorPlot::canDetach(cedar::aux::ConstDataPtr data) const
{
  if(this->mpPlot != nullptr && this->mPlotSeriesVector.size() > 1)
  {
    for(auto plot_series : this->mPlotSeriesVector)
    {
      if(boost::dynamic_pointer_cast<cedar::aux::ConstData>(plot_series->mMatData) == data)
      {
        return true;
      }
    }
  }

  return false;
}

void cedar::aux::gui::MatrixVectorPlot::doDetach(cedar::aux::ConstDataPtr data)
{
  mpLock->lockForWrite();
  auto new_end = std::remove_if(mPlotSeriesVector.begin(), mPlotSeriesVector.end(), [&](PlotSeriesPtr plot_series){
    return (boost::dynamic_pointer_cast<cedar::aux::ConstData>(plot_series->mMatData) == data);
  });
  mPlotSeriesVector.erase(new_end, mPlotSeriesVector.end());
  mpLock->unlock();
}

void cedar::aux::gui::MatrixVectorPlot::attachMarker(QwtPlotMarker *pMarker)
{
  pMarker->attach(this->mpPlot);
}

void cedar::aux::gui::MatrixVectorPlot::clearMarkers()
{
  this->mpPlot->detachItems(QwtPlotMarker::Rtti_PlotMarker, true);
}

void cedar::aux::gui::MatrixVectorPlot::plot(cedar::aux::ConstDataPtr data, const std::string& title)
{
  QWriteLocker locker(this->mpLock);
  mPlotSeriesVector.clear();
  locker.unlock();

  this->append(data, title);

  this->startTimer(30);
}

void cedar::aux::gui::MatrixVectorPlot::init()
{
  QPalette palette = this->palette();
  palette.setColor(QPalette::Window, Qt::white);
  this->setPalette(palette);

  QVBoxLayout *p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);

  mpPlot = new QwtPlot(this);
  this->layout()->addWidget(mpPlot);
}

void cedar::aux::gui::MatrixVectorPlot::contextMenuEvent(QContextMenuEvent *pEvent)
{
  QMenu menu(this);
  QAction *p_antialiasing = menu.addAction("antialiasing");
  p_antialiasing->setCheckable(true);

  bool combined = true;
  for (size_t i = 0; i < this->mPlotSeriesVector.size(); ++i)
  {
    PlotSeriesPtr series = this->mPlotSeriesVector.at(i);
    combined &= series->mpCurve->testRenderHint(QwtPlotItem::RenderAntialiased);
  }
  p_antialiasing->setChecked(combined);

  QAction *p_legend = menu.addAction("legend");
  p_legend->setCheckable(true);
  QObject::connect(p_legend, SIGNAL(toggled(bool)), this, SLOT(showLegend(bool)));
  p_legend->setChecked(this->mpPlot->legend() != NULL);

  QAction *p_action = menu.exec(pEvent->globalPos());
  if (p_action == NULL)
  {
    return;
  }

  if (p_action == p_antialiasing)
  {
    for (size_t i = 0; i < this->mPlotSeriesVector.size(); ++i)
    {
      PlotSeriesPtr series = this->mPlotSeriesVector.at(i);
      series->mpCurve->setRenderHint(QwtPlotItem::RenderAntialiased, p_action->isChecked());
    }
  }
}

void cedar::aux::gui::MatrixVectorPlot::showLegend(bool show)
{
  if (show)
  {
    // show legend
    QwtLegend* p_legend = cedar::aux::asserted_cast<QwtLegend*>(this->mpPlot->legend());
    if (p_legend == NULL)
    {
      p_legend = new QwtLegend();
      p_legend->setFrameStyle(QFrame::Box);
      p_legend->setFrameShadow(QFrame::Sunken);
      this->mpPlot->insertLegend(p_legend, QwtPlot::BottomLegend);
    }
  }
  else
  {
    // remove legend
    delete this->mpPlot->legend();

    // the following takes care of properly laying out everything again
    QSize size = this->size();
    this->resize(QSize(0, 0));
    this->resize(size);
  }
}

void cedar::aux::gui::MatrixVectorPlot::buildArrays(PlotSeriesPtr series, unsigned int new_size)
{
  CEDAR_DEBUG_ASSERT(series->mXValues.size() == series->mYValues.size());

  unsigned int old_size = series->mXValues.size();

  series->mXValues.resize(new_size);
  series->mYValues.resize(new_size);

  for (unsigned int i = old_size; i < new_size; ++i)
  {
    series->mXValues.at(i) = static_cast<double>(i);
  }
}

void cedar::aux::gui::MatrixVectorPlot::timerEvent(QTimerEvent * /* pEvent */)
{
  if (!this->isVisible())
  {
    return;
  }

  this->mpLock->lockForRead();
  for (size_t i = 0; i < this->mPlotSeriesVector.size(); ++i)
  {
    PlotSeriesPtr series = this->mPlotSeriesVector.at(i);

    series->mMatData->lockForRead();
    const cv::Mat& mat = series->mMatData->getData();
    if (cedar::aux::math::getDimensionalityOf(mat) != 1) // plot is no longer capable of displaying the data
    {
      series->mMatData->unlock();
      emit dataChanged();
      return;
    }
    CEDAR_DEBUG_ASSERT(series->mXValues.size() == series->mYValues.size());

    // Check if the size of the matrix has changed
    unsigned int size = cedar::aux::math::get1DMatrixSize(mat);

    // skip if the array is empty
    if (size != 2)
    {
      series->mMatData->unlock();
      return;
    }

    if (series->mXValues.size() != 2)
    {
      series->mXValues.resize(2);
    }
    if (series->mYValues.size() != 2)
    {
      series->mYValues.resize(2);
    }

    series->mXValues.at(0) = 0.0;
    series->mXValues.at(1) = cedar::aux::math::getMatrixEntry<double>(mat, 0);
    series->mYValues.at(0) = 0.0;
    series->mYValues.at(1) = cedar::aux::math::getMatrixEntry<double>(mat, 1);
    series->mMatData->unlock();

    for (unsigned int i = 0; i < 2; ++i)
    {
      const QwtScaleDiv* p_lower;
      const QwtScaleDiv* p_upper;
      if (i == 0)
      {
#if QWT_VERSION >= 0x060100
        p_lower = &this->mpPlot->axisScaleDiv(QwtPlot::xBottom);
        p_upper = &this->mpPlot->axisScaleDiv(QwtPlot::xTop);
#else // QWT_VERSION >= 0x060100
        p_lower = this->mpPlot->axisScaleDiv(QwtPlot::xBottom);
        p_upper = this->mpPlot->axisScaleDiv(QwtPlot::xTop);
#endif // QWT_VERSION >= 0x060100
      }
      else
      {
#if QWT_VERSION >= 0x060100
        p_lower = &this->mpPlot->axisScaleDiv(QwtPlot::yLeft);
        p_upper = &this->mpPlot->axisScaleDiv(QwtPlot::yRight);
#else // QWT_VERSION >= 0x060100
        p_lower = this->mpPlot->axisScaleDiv(QwtPlot::yLeft);
        p_upper = this->mpPlot->axisScaleDiv(QwtPlot::yRight);
#endif // QWT_VERSION >= 0x060100
      }
      double entry = std::abs(cedar::aux::math::getMatrixEntry<double>(mat, i));
      if (p_upper->upperBound() < entry || p_lower->lowerBound() > -1.0 * entry)
      {
        if (i == 0)
        {
          this->mpPlot->setAxisScale(QwtPlot::xBottom, -1.0 * entry, entry);
          this->mpPlot->setAxisScale(QwtPlot::xTop, -1.0 * entry, entry);
        }
        else
        {
          this->mpPlot->setAxisScale(QwtPlot::yLeft, -1.0 * entry, entry);
          this->mpPlot->setAxisScale(QwtPlot::yRight, -1.0 * entry, entry);
        }
      }
    }

    // choose the right function depending on the qwt version
#if (QWT_VERSION >= 0x060000)
    series->mpCurve->setRawSamples
    (
      &series->mXValues.at(0),
      &series->mYValues.at(0),
      static_cast<int>(series->mXValues.size())
    );
#elif (QWT_VERSION >= 0x050000)
    series->mpCurve->setData
    (
      &series->mXValues.at(0),
      &series->mYValues.at(0),
      static_cast<int>(series->mXValues.size())
    );
#else
#error unsupported qwt version
#endif
  }

  this->mpPlot->replot();
  mpLock->unlock();
}

#endif // CEDAR_USE_QWT
