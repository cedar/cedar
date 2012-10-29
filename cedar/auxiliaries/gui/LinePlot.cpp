/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        LinePlot.cpp

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
#include "cedar/auxiliaries/gui/LinePlot.h"
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
#include <QThread>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

std::vector<QColor> cedar::aux::gui::LinePlot::mLineColors;
std::vector<Qt::PenStyle> cedar::aux::gui::LinePlot::mLineStyles;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::LinePlot::LinePlot(QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpLock(new QReadWriteLock())
{
  this->init();
}

cedar::aux::gui::LinePlot::LinePlot(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpLock(new QReadWriteLock())
{
  this->init();
  this->plot(matData, title);
}

cedar::aux::gui::LinePlot::~LinePlot()
{
  if (mpLock)
  {
    delete mpLock;
  }

  if (this->mpWorkerThread)
  {
    this->mpWorkerThread->quit();
    this->mpWorkerThread->wait();
    delete this->mpWorkerThread;
    this->mpWorkerThread = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::math::Limits<double> cedar::aux::gui::LinePlot::getXLimits() const
{
  cedar::aux::math::Limits<double> limits;

  QwtScaleDiv* p_interval = this->mpPlot->axisScaleDiv(QwtPlot::xBottom);
  limits.setLower(p_interval->lowerBound());
  limits.setUpper(p_interval->upperBound());
  return limits;
}

void cedar::aux::gui::LinePlot::applyStyle(size_t lineId, QwtPlotCurve *pCurve)
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

bool cedar::aux::gui::LinePlot::canAppend(cedar::aux::ConstDataPtr data) const
{
  cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data);
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
    || cedar::aux::math::get1DMatrixSize(matrix) != this->mPlotSeriesVector.at(0)->mXValues.size())
  {
    return false;
  }

  return true;
}

void cedar::aux::gui::LinePlot::doAppend(cedar::aux::ConstDataPtr data, const std::string& title)
{
  PlotSeriesPtr plot_series(new PlotSeries());

  size_t line_id = mPlotSeriesVector.size();

  mpLock->lockForWrite();
  mPlotSeriesVector.push_back(plot_series);

  plot_series->mMatData = boost::shared_dynamic_cast<cedar::aux::ConstMatData>(data);


  if (!plot_series->mMatData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Could not cast to cedar::aux::MatData in cedar::aux::gui::LinePlot::plot.");
  }


  plot_series->mpCurve = new QwtPlotCurve(title.c_str());
  applyStyle(line_id, plot_series->mpCurve);

  data->lockForRead();
  const cv::Mat& mat = plot_series->mMatData->getData();

  //!@todo This throws an exception when null data (or data of other dimensionality than 1) is passed.
  size_t num = cedar::aux::math::get1DMatrixSize(mat);
  data->unlock();

  // skip if the matrix is empty
  if (num == 0)
  {
    return;
  }

  plot_series->buildArrays(num);

  //!@todo write a macro that does this check (see HistoryPlot0D.cpp)
#if (QWT_VERSION >> 16) == 5
  plot_series->mpCurve->setData(&plot_series->mXValues.at(0), &plot_series->mYValues.at(0), num);
#elif (QWT_VERSION >> 16) == 6
  plot_series->mpCurve->setRawSamples(&plot_series->mXValues.at(0), &plot_series->mYValues.at(0), num);
#else
#error unsupported qwt version
#endif
  plot_series->mpCurve->attach(this->mpPlot);

  mpLock->unlock();

  this->startTimer(30); //!@todo make the refresh time configurable.
}

void cedar::aux::gui::LinePlot::attachMarker(QwtPlotMarker *pMarker)
{
  pMarker->attach(this->mpPlot);
}

void cedar::aux::gui::LinePlot::clearMarkers()
{
  this->mpPlot->detachItems(QwtPlotMarker::Rtti_PlotMarker, true);
}

void cedar::aux::gui::LinePlot::plot(cedar::aux::ConstDataPtr data, const std::string& title)
{
  mpLock->lockForWrite();
  mPlotSeriesVector.clear();
  mpLock->unlock();

  this->append(data, title);

  this->startTimer(30); //!@todo make the refresh time configurable.
}

void cedar::aux::gui::LinePlot::init()
{
  QPalette palette = this->palette();
  palette.setColor(QPalette::Window, Qt::white);
  this->setPalette(palette);

  QVBoxLayout *p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);

  mpPlot = new QwtPlot(this);
  this->layout()->addWidget(mpPlot);

  mpWorkerThread = new QThread();

  mConversionWorker = cedar::aux::gui::detail::LinePlotWorkerPtr(new cedar::aux::gui::detail::LinePlotWorker(this));
  mConversionWorker->moveToThread(mpWorkerThread);

  //!@todo Add possibility to change priority
  this->mpWorkerThread->start(QThread::LowPriority);

  QObject::connect(mConversionWorker.get(), SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
  QObject::connect(this, SIGNAL(convert()), mConversionWorker.get(), SLOT(convert()));
  QObject::connect(mConversionWorker.get(), SIGNAL(done()), this, SLOT(conversionDone()));
}

void cedar::aux::gui::LinePlot::contextMenuEvent(QContextMenuEvent *pEvent)
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

void cedar::aux::gui::LinePlot::showLegend(bool show)
{
  if (show)
  {
    // show legend
    QwtLegend *p_legend = this->mpPlot->legend();
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

void cedar::aux::gui::LinePlot::PlotSeries::buildArrays(unsigned int new_size)
{
  CEDAR_DEBUG_ASSERT(this->mXValues.size() == this->mYValues.size());

  unsigned int old_size = this->mXValues.size();

  this->mXValues.resize(new_size);
  this->mYValues.resize(new_size);

  for (unsigned int i = old_size; i < new_size; ++i)
  {
    this->mXValues.at(i) = static_cast<double>(i);
  }
}

//!@cond SKIPPED_DOCUMENTATION
void cedar::aux::gui::detail::LinePlotWorker::convert()
{
  QWriteLocker plot_locker(this->mpPlot->mpLock);
  for (size_t i = 0; i < this->mpPlot->mPlotSeriesVector.size(); ++i)
  {
    cedar::aux::gui::LinePlot::PlotSeriesPtr series = this->mpPlot->mPlotSeriesVector.at(i);

    QReadLocker locker(&series->mMatData->getLock());
    const cv::Mat& mat = series->mMatData->getData();
    if (cedar::aux::math::getDimensionalityOf(mat) != 1) // plot is no longer capable of displaying the data
    {
      emit dataChanged();
      return;
    }
    CEDAR_DEBUG_ASSERT(series->mXValues.size() == series->mYValues.size());

    // Check if the size of the matrix has changed
    unsigned int size = cedar::aux::math::get1DMatrixSize(mat);

    // skip if the array is empty
    if (size == 0)
    {
      return;
    }

    if (series->mXValues.size() != size)
    {
      series->buildArrays(size);
    }

    for (size_t i = 0; i < series->mXValues.size(); ++i)
    {
      series->mYValues.at(i) = cedar::aux::math::getMatrixEntry<double>(mat, i);
    }
  }

  plot_locker.unlock();

  emit done();
}
//!@endcond

void cedar::aux::gui::LinePlot::conversionDone()
{
  QReadLocker locker(this->mpLock);
  for (size_t i = 0; i < this->mPlotSeriesVector.size(); ++i)
  {
    PlotSeriesPtr series = this->mPlotSeriesVector.at(i);

    // choose the right function depending on the qwt version
    //!@todo write a macro that does this check (see HistoryPlot0D.cpp)
    #if (QWT_VERSION >> 16) == 5
      series->mpCurve->setData
      (
        &series->mXValues.at(0),
        &series->mYValues.at(0),
        static_cast<int>(series->mXValues.size())
      );
    #elif (QWT_VERSION >> 16) == 6
      series->mpCurve->setRawSamples
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
}

void cedar::aux::gui::LinePlot::timerEvent(QTimerEvent * /* pEvent */)
{
  if (!this->isVisible())
  {
    return;
  }

  emit convert();
}
