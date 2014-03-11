/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        QwtLinePlot.cpp

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

#ifdef CEDAR_USE_QWT

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/QwtLinePlot.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/math/Limits.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/annotation/DiscreteMetric.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/numeric/conversion/bounds.hpp>
#endif
#include <qwt_legend.h>
#include <qwt_scale_div.h>
#include <qwt_symbol.h>
#include <QContextMenuEvent>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPalette>
#include <QMenu>
#include <QThread>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <iostream>
#include <limits.h>

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

std::vector<QColor> cedar::aux::gui::QwtLinePlot::mLineColors;
std::vector<Qt::PenStyle> cedar::aux::gui::QwtLinePlot::mLineStyles;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::QwtLinePlot::QwtLinePlot(QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpLock(new QReadWriteLock())
{
  this->init();
}

cedar::aux::gui::QwtLinePlot::QwtLinePlot(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpLock(new QReadWriteLock())
{
  this->init();
  this->plot(matData, title);
}

cedar::aux::gui::QwtLinePlot::~QwtLinePlot()
{
  if (this->mpWorkerThread)
  {
    this->mpWorkerThread->quit();
    this->mpWorkerThread->wait();
    delete this->mpWorkerThread;
    this->mpWorkerThread = NULL;
  }

  this->mPlotSeriesVector.clear();

  if (mpLock)
  {
    delete mpLock;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::math::Limits<double> cedar::aux::gui::QwtLinePlot::getXLimits() const
{
  cedar::aux::math::Limits<double> limits;

#if QWT_VERSION >= 0x060100
  const QwtScaleDiv* p_interval = &this->mpPlot->axisScaleDiv(QwtPlot::xBottom);
#else
  QwtScaleDiv* p_interval = this->mpPlot->axisScaleDiv(QwtPlot::xBottom);
#endif
  limits.setLower(p_interval->lowerBound());
  limits.setUpper(p_interval->upperBound());
  return limits;
}

cedar::aux::math::Limits<double> cedar::aux::gui::QwtLinePlot::getYLimits() const
{
  cedar::aux::math::Limits<double> limits;

#if QWT_VERSION >= 0x060100
  const QwtScaleDiv* p_interval = &this->mpPlot->axisScaleDiv(QwtPlot::yLeft);
#else
  QwtScaleDiv* p_interval = this->mpPlot->axisScaleDiv(QwtPlot::yLeft);
#endif
  limits.setLower(p_interval->lowerBound());
  limits.setUpper(p_interval->upperBound());
  return limits;
}

void cedar::aux::gui::QwtLinePlot::applyStyle(cedar::aux::ConstDataPtr data, size_t lineId, QwtPlotCurve *pCurve)
{
  static std::vector<QwtSymbol::Style> mLineSymbols;

  // initialize vectors, if this has not happened, yet
  if (mLineColors.empty() || mLineStyles.empty() || mLineSymbols.empty())
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

    mLineSymbols.push_back(QwtSymbol::Ellipse);
    mLineSymbols.push_back(QwtSymbol::Rect);
    mLineSymbols.push_back(QwtSymbol::Diamond);
    mLineSymbols.push_back(QwtSymbol::Triangle);
    mLineSymbols.push_back(QwtSymbol::Hexagon);
    mLineSymbols.push_back(QwtSymbol::Star2);

    // otherwise, the code below doesn't work
    CEDAR_DEBUG_ASSERT(mLineStyles.size() == mLineSymbols.size());
  }

  const size_t color_count = mLineColors.size();
  const size_t style_count = mLineStyles.size();
  const size_t max_line_id = mLineColors.size() * mLineStyles.size();

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

  // Use symbols instead of curves for discrete data
  if (data->hasAnnotation<cedar::aux::annotation::DiscreteMetric>())
  {
    pCurve->setStyle(QwtPlotCurve::NoCurve);
    QwtSymbol
#if (QWT_VERSION >= 0x060000)
    * // qwt 6.x expects a pointer
#endif
      symbol
#if (QWT_VERSION >= 0x060000)
      = new QwtSymbol
#endif
      (mLineSymbols.at(style_id), QBrush(mLineColors.at(color_id)), pen, QSize(10, 10));
    pCurve->setSymbol(symbol);
  }
}

bool cedar::aux::gui::QwtLinePlot::canAppend(cedar::aux::ConstDataPtr data) const
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

void cedar::aux::gui::QwtLinePlot::doAppend(cedar::aux::ConstDataPtr data, const std::string& title)
{
  PlotSeriesPtr plot_series(new PlotSeries());

  size_t line_id = mPlotSeriesVector.size();

  mpLock->lockForWrite();
  mPlotSeriesVector.push_back(plot_series);

  plot_series->mMatData = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);


  if (!plot_series->mMatData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Could not cast to cedar::aux::MatData in cedar::aux::gui::QwtLinePlot::plot.");
  }


  plot_series->mpCurve = new QwtPlotCurve(title.c_str());
  applyStyle(plot_series->mMatData, line_id, plot_series->mpCurve);

  data->lockForRead();
  const cv::Mat& mat = plot_series->mMatData->getData();

  CEDAR_DEBUG_ASSERT(cedar::aux::math::getDimensionalityOf(mat) <= 1);

  size_t num = cedar::aux::math::get1DMatrixSize(mat);
  data->unlock();

  // skip if the matrix is empty
  if (num == 0)
  {
    return;
  }

  plot_series->buildArrays(num);

#if (QWT_VERSION >= 0x060000)
  plot_series->mpCurve->setRawSamples(&plot_series->mXValues.at(0), &plot_series->mYValues.at(0), num);
#elif (QWT_VERSION >= 0x050000)
  plot_series->mpCurve->setData(&plot_series->mXValues.at(0), &plot_series->mYValues.at(0), num);
#else
#error unsupported qwt version
#endif
  plot_series->mpCurve->attach(this->mpPlot);

  mpLock->unlock();

  this->startTimer(30);
}

bool cedar::aux::gui::QwtLinePlot::canDetach(cedar::aux::ConstDataPtr data) const
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

void cedar::aux::gui::QwtLinePlot::doDetach(cedar::aux::ConstDataPtr data)
{
  mpLock->lockForWrite();
  auto new_end = std::remove_if(mPlotSeriesVector.begin(), mPlotSeriesVector.end(), [&](PlotSeriesPtr plot_series){
    return (boost::dynamic_pointer_cast<cedar::aux::ConstData>(plot_series->mMatData) == data);
  });
  mPlotSeriesVector.erase(new_end, mPlotSeriesVector.end());
  mpLock->unlock();
}

void cedar::aux::gui::QwtLinePlot::attachMarker(QwtPlotMarker *pMarker)
{
  pMarker->attach(this->mpPlot);
}

void cedar::aux::gui::QwtLinePlot::clearMarkers()
{
  this->mpPlot->detachItems(QwtPlotMarker::Rtti_PlotMarker, true);
}

void cedar::aux::gui::QwtLinePlot::plot(cedar::aux::ConstDataPtr data, const std::string& title)
{
  mpLock->lockForWrite();
  mPlotSeriesVector.clear();
  mpLock->unlock();

  this->append(data, title);
}

void cedar::aux::gui::QwtLinePlot::init()
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

  mConversionWorker = cedar::aux::gui::detail::QwtLinePlotWorkerPtr(new cedar::aux::gui::detail::QwtLinePlotWorker(this));
  mConversionWorker->moveToThread(mpWorkerThread);

  this->mpWorkerThread->start(QThread::LowPriority);

  QObject::connect(mConversionWorker.get(), SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
  QObject::connect(this, SIGNAL(convert()), mConversionWorker.get(), SLOT(convert()));
  QObject::connect(mConversionWorker.get(), SIGNAL(done()), this, SLOT(conversionDone()));
}

void cedar::aux::gui::QwtLinePlot::contextMenuEvent(QContextMenuEvent *pEvent)
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

  menu.addSeparator();
  QMenu* p_y_scaling_menu = menu.addMenu("y axis scaling");
  QAction* p_y_scaling_auto = p_y_scaling_menu->addAction("automatic");
  p_y_scaling_auto->setCheckable(true);
  p_y_scaling_auto->setChecked(this->mpPlot->axisAutoScale(QwtPlot::yLeft));
  QObject::connect(p_y_scaling_auto, SIGNAL(triggered()), this, SLOT(setAutomaticYAxisScaling()));

  QAction* p_y_scaling_fixed = p_y_scaling_menu->addAction("fixed");
  p_y_scaling_fixed->setCheckable(true);
  p_y_scaling_fixed->setChecked(this->mpPlot->axisAutoScale(QwtPlot::yLeft) == false);
  QObject::connect(p_y_scaling_fixed, SIGNAL(triggered()), this, SLOT(setFixedYAxisScaling()));

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

void cedar::aux::gui::QwtLinePlot::setAutomaticYAxisScaling()
{
  this->mpPlot->setAxisAutoScale(QwtPlot::yLeft);
}

void cedar::aux::gui::QwtLinePlot::setFixedYAxisScaling()
{
  QDialog* p_dialog = new QDialog();
  p_dialog->setModal(true);
  auto p_layout = new QGridLayout();
  p_dialog->setLayout(p_layout);
  QLabel* p_label;
  p_label = new QLabel("lower limit:");
  p_layout->addWidget(p_label, 0, 0);

  cedar::aux::math::Limits<double> y_limits = this->getYLimits();
  auto p_lower = new QDoubleSpinBox();
  p_layout->addWidget(p_lower, 0, 1);
  p_lower->setMinimum(boost::numeric::bounds<double>::lowest());
  p_lower->setMaximum(boost::numeric::bounds<double>::highest());
  p_lower->setValue(y_limits.getLower());

  p_label = new QLabel("upper limit:");
  p_layout->addWidget(p_label, 1, 0);

  auto p_upper = new QDoubleSpinBox();
  p_layout->addWidget(p_upper, 1, 1);
  p_upper->setMinimum(boost::numeric::bounds<double>::lowest());
  p_upper->setMaximum(boost::numeric::bounds<double>::highest());
  p_upper->setValue(y_limits.getUpper());

  auto p_buttons = new QDialogButtonBox();
  p_buttons->addButton(QDialogButtonBox::Ok);
  p_buttons->addButton(QDialogButtonBox::Cancel);
  p_layout->addWidget(p_buttons, 2, 0, 1, 2);

  QObject::connect(p_buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()), p_dialog, SLOT(accept()));
  QObject::connect(p_buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), p_dialog, SLOT(reject()));

  int res = p_dialog->exec();

  if (res == QDialog::Accepted)
  {
    this->setFixedYAxisScaling(p_lower->value(), p_upper->value());
  }
}

void cedar::aux::gui::QwtLinePlot::setFixedYAxisScaling(double lower, double upper)
{
  this->mpPlot->setAxisScale(QwtPlot::yLeft, lower, upper);
}

void cedar::aux::gui::QwtLinePlot::setFixedXAxisScaling(double lower, double upper)
{
  this->mpPlot->setAxisScale(QwtPlot::xBottom, lower, upper);
}

void cedar::aux::gui::QwtLinePlot::showLegend(bool show)
{
  if (show)
  {
    // show legend
    auto p_legend = this->mpPlot->legend();
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

void cedar::aux::gui::QwtLinePlot::PlotSeries::buildArrays(unsigned int new_size)
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
void cedar::aux::gui::detail::QwtLinePlotWorker::convert()
{
  QWriteLocker plot_locker(this->mpPlot->mpLock);
  for (size_t series_index = 0; series_index < this->mpPlot->mPlotSeriesVector.size(); ++series_index)
  {
    cedar::aux::gui::QwtLinePlot::PlotSeriesPtr series = this->mpPlot->mPlotSeriesVector.at(series_index);

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

    for (size_t x = 0; x < series->mXValues.size(); ++x)
    {
      series->mYValues.at(x) = cedar::aux::math::getMatrixEntry<double>(mat, x);
    }
  }

  plot_locker.unlock();

  emit done();
}
//!@endcond

void cedar::aux::gui::QwtLinePlot::conversionDone()
{
  QReadLocker locker(this->mpLock);

  double x_min = std::numeric_limits<double>::max(), x_max = 0.0;
  for (size_t i = 0; i < this->mPlotSeriesVector.size(); ++i)
  {
    PlotSeriesPtr series = this->mPlotSeriesVector.at(i);

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

    x_min = std::min(series->mXValues.front(), x_min);
    x_max = std::max(series->mXValues.back(), x_max);
  }

  this->setFixedXAxisScaling(x_min, x_max);

  this->mpPlot->replot();
}

void cedar::aux::gui::QwtLinePlot::timerEvent(QTimerEvent * /* pEvent */)
{
  if (!this->isVisible())
  {
    return;
  }

  emit convert();
}

#endif // CEDAR_USE_QWT
