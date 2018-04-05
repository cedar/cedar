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

    File:        Qt5HistoryPlot0D.cpp

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

#ifdef CEDAR_USE_QT5_PLOTS

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Qt5HistoryPlot0D.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/annotation/Dimensions.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/DoubleData.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/UnitData.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/GlobalClock.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <QContextMenuEvent>
#include <QMenu>
#include <QThread>
#include <iostream>
#include <limits.h>

// MACROS
// Enable to show information on locking/unlocking
// #define DEBUG_LOCKS

#ifdef DEBUG_LOGS
#  include "auxiliaries/System.h"
#endif

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::Qt5HistoryPlot0D::Qt5HistoryPlot0D(QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpHistoryPlot(nullptr)
{
  this->init();
}

cedar::aux::gui::Qt5HistoryPlot0D::Qt5HistoryPlot0D(cedar::aux::ConstDataPtr data, const std::string& title, QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpHistoryPlot(nullptr)
{
  this->init();
  this->plot(data, title);
}

cedar::aux::gui::Qt5HistoryPlot0D::~Qt5HistoryPlot0D()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::Qt5HistoryPlot0D::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  cedar::aux::gui::MultiPlotInterface::readConfiguration(node);
  this->mpHistoryPlot->readConfiguration(node);
}

void cedar::aux::gui::Qt5HistoryPlot0D::writeConfiguration(cedar::aux::ConfigurationNode& node) const
{
  cedar::aux::gui::MultiPlotInterface::writeConfiguration(node);
  this->mpHistoryPlot->writeConfiguration(node);
}

bool cedar::aux::gui::Qt5HistoryPlot0D::canAppend(cedar::aux::ConstDataPtr data) const
{
  auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);

  if (!mat_data)
  {
    return false;
  }

  QReadLocker locker(&mat_data->getLock());
  if (mat_data->isEmpty() || mat_data->getDimensionality() != 0)
  {
    return false;
  }

  return true;
}

bool cedar::aux::gui::Qt5HistoryPlot0D::canDetach(cedar::aux::ConstDataPtr data) const
{
  for (const auto& plot_data : this->mPlotData)
  {
    if (plot_data.mData == data)
    {
      return true;
    }
  }
  return false;
}

void cedar::aux::gui::Qt5HistoryPlot0D::init()
{
  auto p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);
//#ifdef CEDAR_USE_QWT
  this->mpHistoryPlot = new cedar::aux::gui::Qt5LinePlot();
  p_layout->addWidget(this->mpHistoryPlot);
  this->mpHistoryPlot->setAccepts0DData(true);
//#endif // CEDAR_USE_QWT

  this->mMaxHistorySize = this->mpHistoryPlot->getSamplingSize();
  this->mMaxHistorySizeLast = this->mMaxHistorySize;

  this->mTimerId = 0;
}

void cedar::aux::gui::Qt5HistoryPlot0D::advanceHistory()
{
  this->mMaxHistorySizeLast = this->mMaxHistorySize;
  this->mMaxHistorySize = this->mpHistoryPlot->getSamplingSize();
  // update the markers for the current value
//#ifdef CEDAR_USE_QWT
  for (auto& plot_data : this->mPlotData)
  {
    QReadLocker data_locker(&plot_data.mData->getLock());

    if (plot_data.mData->getDimensionality() != 0)
    {
      data_locker.unlock();
      emit dataChanged();
      return;
    }

    if (this->mMaxHistorySizeLast > this->mMaxHistorySize)
    {
      data_locker.unlock();
      emit dataChanged();
      return;
    }

    cv::Mat now = plot_data.mData->getData().clone();
    data_locker.unlock();

    double value = cedar::aux::math::getMatrixEntry<double>(now, 0);
//    plot_data.mMarkerSeries->clear();
    plot_data.mMarkerSeries->replace(0,-0.03,value);
//    plot_data.mpZeroMarker->setYValue(value);
//    plot_data.mMarkerPos->setY(value);
  }
//#endif // CEDAR_USE_QWT

  // if the clock hasn't changed, abort!
  cedar::unit::Time time_now = cedar::aux::GlobalClockSingleton::getInstance()->getTime();
  if (this->mTimeOfLastUpdate && this->mTimeOfLastUpdate.get() == time_now)
  {
    return;
  }

  this->mTimeOfLastUpdate = time_now;

  // add current value to history
  for (auto& plot_data : this->mPlotData)
  {
    QReadLocker data_locker(&plot_data.mData->getLock());

    if (plot_data.mData->getDimensionality() != 0)
    {
      data_locker.unlock();
      emit dataChanged();
      return;
    }

    cv::Mat now = plot_data.mData->getData().clone();
    data_locker.unlock();

    QWriteLocker hist_locker(&plot_data.mHistory->getLock());
    const cv::Mat& current_hist = plot_data.mHistory->getData();

    cv::Mat new_hist;

    int start = 0;
    int end = current_hist.rows;

    // if the last time in the history lies in the future (i.e., the timer was reset), discard all the old history
    if (plot_data.mXLabels.back() > time_now)
    {
      plot_data.mXLabels.clear();
      new_hist = now.clone();
    }
    else
    {
      new_hist
             = cv::Mat::zeros
               (
                 std::min(current_hist.rows + 1, static_cast<int>(this->mMaxHistorySize)),
                 1,
                 now.type()
               );

      if (current_hist.rows >= static_cast<int>(this->mMaxHistorySize))
      {
        start = 1;
      }

      // copy the old histogram to the beginning of the new history
      cv::Mat slice = new_hist(cv::Range(0, end - start), cv::Range::all());
      current_hist(cv::Range(start, end), cv::Range::all()).copyTo(slice);

      // append the new value at the end of the new history
      cv::Mat slice2 = new_hist(cv::Range(new_hist.rows - 1, new_hist.rows), cv::Range::all());
      now.copyTo(slice2);
    }

    // assign the new history to the underlying line plot
    plot_data.mHistory->setData(new_hist);

    // append the current time to the x labels
    plot_data.mXLabels.push_back(time_now);

    // make sure the number of x values does not exceed the maximum history size
    while (plot_data.mXLabels.size() > this->mMaxHistorySize)
    {
      plot_data.mXLabels.pop_front();
    }

    std::vector<double> time_values;
    time_values.resize(plot_data.mXLabels.size());
    for (size_t i = 0; i < plot_data.mXLabels.size(); ++i)
    {
      time_values.at(i) = (plot_data.mXLabels.at(i) - time_now) / (1.0 * cedar::unit::second);
    }
    auto time_annotation = plot_data.mHistory->getAnnotation<cedar::aux::annotation::Dimensions>();
    time_annotation->setSamplingPositions(0, time_values);

    CEDAR_DEBUG_NON_CRITICAL_ASSERT(static_cast<unsigned int>(new_hist.rows) == plot_data.mXLabels.size());
  }
}

void cedar::aux::gui::Qt5HistoryPlot0D::clear()
{
  while (!this->mPlotData.empty())
  {
    auto& first = *this->mPlotData.begin();
    this->detach(first.mData);
  }
}

void cedar::aux::gui::Qt5HistoryPlot0D::plot(cedar::aux::ConstDataPtr data, const std::string& title)
{
  if (!this->canAppend(data))
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData, "Cannot plot this data.");
  }

  this->clear();

  this->append(data, title);

  if (mTimerId != 0)
  {
    this->killTimer(this->mTimerId);
  }

  this->mTimerId = this->startTimer(30);
}

void cedar::aux::gui::Qt5HistoryPlot0D::doAppend(cedar::aux::ConstDataPtr data, const std::string& title)
{
  this->mPlotData.push_back(PlotData());
  auto& plot_data = this->mPlotData.back();
  plot_data.mData = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  plot_data.mHistory = cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat()));

  // initialize the new history to a 1x1 matrix.
  QReadLocker locker(&plot_data.mData->getLock());
  cv::Mat hist = plot_data.mData->getData().clone();
  locker.unlock();

  cv::Mat new_hist = cv::Mat::zeros(1, 1, hist.type());

  hist.copyTo(new_hist);

  QWriteLocker hlocker(&plot_data.mHistory->getLock());
  plot_data.mHistory->setData(new_hist);
  hlocker.unlock();

  cedar::aux::annotation::DimensionsPtr time_annotation(new cedar::aux::annotation::Dimensions(1));
  time_annotation->setLabel(0, "time [s]");
  plot_data.mXLabels.clear();
  plot_data.mXLabels.push_back(cedar::aux::GlobalClockSingleton::getInstance()->getTime());
  plot_data.mHistory->setAnnotation(time_annotation);

//  std::cout << "Append the History: " << plot_data.mHistory <<std::endl;

  this->mpHistoryPlot->append(plot_data.mHistory, title);
//#ifdef CEDAR_USE_QWT
//  plot_data.mpZeroMarker = new QwtPlotMarker();
  plot_data.mMarkerSeries = new QtCharts::QScatterSeries();
  QBrush brush(Qt::black); //!@todo proper color of line style
  QPen pen(Qt::black); //!@todo proper color of line style
  this->mpHistoryPlot->getStyleFor(plot_data.mHistory, pen, brush);
//#if QWT_VERSION >= 0x060000
//  auto symbol = new QwtSymbol
//#else
//  QwtSymbol symbol
//#endif // QWT_VERSION
//      (QwtSymbol::Ellipse, brush, pen, QSize(7, 7));
//  plot_data.mpZeroMarker->setSymbol(symbol);
//  plot_data.mpZeroMarker->setXValue(0.0);
  plot_data.mMarkerSeries->setBrush(pen.brush());
  plot_data.mMarkerSeries->setPen(pen);
  plot_data.mMarkerSeries->append(0.0,0.0);
  plot_data.mMarkerSeries->setMarkerSize(5);
  plot_data.mMarkerSeries->setMarkerShape(QtCharts::QScatterSeries::MarkerShapeCircle);
//  this->mpHistoryPlot->attachMarker(plot_data.mpZeroMarker);
  this->mpHistoryPlot->attachMarker(plot_data.mMarkerSeries);
//#endif // CEDAR_USE_QWT
}

void cedar::aux::gui::Qt5HistoryPlot0D::doDetach(cedar::aux::ConstDataPtr data)
{
  for (auto iter = mPlotData.begin(); iter != mPlotData.end(); )
  {
    if (iter->mData == data)
    {
      this->mpHistoryPlot->detach(iter->mHistory);
//      this->mpHistoryPlot->detachMarker(iter->mpZeroMarker);
      iter = mPlotData.erase(iter);
    }
    else
    {
      ++iter;
    }
  }
}

void cedar::aux::gui::Qt5HistoryPlot0D::timerEvent(QTimerEvent* /* pEvent */)
{
  if (!this->isVisible())
  {
    return;
  }

  this->advanceHistory();
}

#endif // CEDAR_USE_QT5_PLOTS

