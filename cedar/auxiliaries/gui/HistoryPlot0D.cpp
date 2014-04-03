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

    File:        HistoryPlot0D.cpp

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
#include "cedar/auxiliaries/gui/HistoryPlot0D.h"
#include "cedar/auxiliaries/gui/exceptions.h"
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
#include <qwt_legend.h>
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

std::vector<QColor> cedar::aux::gui::HistoryPlot0D::mLineColors;
std::vector<Qt::PenStyle> cedar::aux::gui::HistoryPlot0D::mLineStyles;
boost::posix_time::ptime cedar::aux::gui::HistoryPlot0D::mPlotStartTime = boost::posix_time::microsec_clock::local_time();

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::gui::HistoryPlot0D::HistoryPlot0D(QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpCurrentPlotWidget(nullptr)
{
  this->init();
}

cedar::aux::gui::HistoryPlot0D::HistoryPlot0D(cedar::aux::ConstDataPtr data, const std::string& title, QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpCurrentPlotWidget(nullptr)
{
  this->init();
  this->plot(data, title);
}

cedar::aux::gui::HistoryPlot0D::CurveInfo::CurveInfo()
:
mCurve(NULL)
{
}

cedar::aux::gui::HistoryPlot0D::~HistoryPlot0D()
{
  if (this->mpWorkerThread)
  {
    this->mpWorkerThread->quit();
    this->mpWorkerThread->wait();
    delete this->mpWorkerThread;
    this->mpWorkerThread = nullptr;
  }

  this->mCurves.clear();
}

cedar::aux::gui::HistoryPlot0D::CurveInfo::~CurveInfo()
{
  if (mCurve != NULL)
  {
    mCurve->detach();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::HistoryPlot0D::contextMenuEvent(QContextMenuEvent *pEvent)
{
  QMenu menu(this);
  QAction *p_antialiasing = menu.addAction("antialiasing");
  p_antialiasing->setCheckable(true);

  bool combined = true;
  for (size_t i = 0; i < this->mCurves.size(); ++i)
  {
    CurveInfoPtr curve = this->mCurves[i];
    combined &= curve->mCurve->testRenderHint(QwtPlotItem::RenderAntialiased);
  }
  p_antialiasing->setChecked(combined);

  QAction *p_legend = menu.addAction("legend");
  p_legend->setCheckable(true);
  QObject::connect(p_legend, SIGNAL(toggled(bool)), this, SLOT(showLegend(bool)));
  p_legend->setChecked(this->mpPlot->legend() != nullptr);

  QAction *p_action = menu.exec(pEvent->globalPos());
  if (p_action == nullptr)
  {
    return;
  }

  if (p_action == p_antialiasing)
  {
    for (size_t i = 0; i < this->mCurves.size(); ++i)
    {
      CurveInfoPtr curve = this->mCurves[i];
      curve->mCurve->setRenderHint(QwtPlotItem::RenderAntialiased, p_action->isChecked());
    }
  }
}

void cedar::aux::gui::HistoryPlot0D::showLegend(bool show)
{
  if (show)
  {
    // show legend
    auto p_legend = this->mpPlot->legend();
    if (p_legend == nullptr)
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

bool cedar::aux::gui::HistoryPlot0D::canAppend(cedar::aux::ConstDataPtr data) const
{
  cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  cedar::aux::ConstUnitDataPtr unit_data = boost::dynamic_pointer_cast<cedar::aux::ConstUnitData>(data);
  if (mat_data)
  {
    return mat_data->getDimensionality() == 0;
  }
  else if (unit_data)
  {
    // no curves yet, can append with no problem!
    if (this->mCurves.empty())
    {
      return true;
    }
    // otherwise, there's already a curve, need to make sure it has the same suffix.
    else
    {
      if (this->mCurves.front()->mUnitData && this->mCurves.front()->mUnitData->getSuffix() == unit_data->getSuffix())
      {
        return true;
      }
      else
      {
        return false;
      }
    }
  }
  else
  {
    return false;
  }
}

bool cedar::aux::gui::HistoryPlot0D::canDetach(cedar::aux::ConstDataPtr data) const
{
  if (this->mpPlot != nullptr && this->mCurves.size() >= 1)
  {
    for (auto curve : this->mCurves)
    {
      if (curve->mData == data)
      {
        return true;
      }
    }
  }
  
  return false;
}

void cedar::aux::gui::HistoryPlot0D::init()
{
  mMaxHistorySize = 500;

  this->mCurves.clear();
  this->mCurves.push_back(CurveInfoPtr(new CurveInfo()));

  QPalette palette = this->palette();
  palette.setColor(QPalette::Window, Qt::white);
  this->setPalette(palette);

  QVBoxLayout *p_layout = new QVBoxLayout();
  this->setLayout(p_layout);

  this->mpPlot = new QwtPlot(this);
  this->layout()->addWidget(mpPlot);
  this->mpPlot->setAxisTitle(QwtPlot::xBottom, "time (s)");

  mpWorkerThread = new QThread();
  mWorker = cedar::aux::gui::detail::HistoryPlot0DWorkerPtr(new cedar::aux::gui::detail::HistoryPlot0DWorker(this));
  mWorker->moveToThread(this->mpWorkerThread);

  QObject::connect(this, SIGNAL(convert()), mWorker.get(), SLOT(convert()));
  QObject::connect(mWorker.get(), SIGNAL(done()), this, SLOT(conversionDone()));

  this->mpWorkerThread->start(QThread::LowPriority);
}

void cedar::aux::gui::HistoryPlot0D::applyStyle(size_t lineId, QwtPlotCurve *pCurve)
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

void cedar::aux::gui::HistoryPlot0D::plot(cedar::aux::ConstDataPtr data, const std::string& title)
{
  this->mCurves.clear();
  if (auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
  {
    if (mat_data->getData().channels() != 1)
    {
      CEDAR_THROW(cedar::aux::TypeMismatchException, "Cannot plot data with more than one channel.");
    }
  }
  this->append(data, title);

  this->startTimer(30);
}

double cedar::aux::gui::HistoryPlot0D::getDataValue(size_t index)
{
  CEDAR_DEBUG_ASSERT(index < this->mCurves.size());

  // lock the data
  QReadLocker data_locker(&this->mCurves[index]->mData->getLock());

  // read out the double value
  double val = 0.0;
  if (this->mCurves[index]->mDoubleData)
  {
    val = this->mCurves[index]->mDoubleData->getData();
  }
  else if (this->mCurves[index]->mUnitData)
  {
    val = this->mCurves[index]->mUnitData->doubleValueForSuffix();
  }
  else if (this->mCurves[index]->mMatData)
  {
    cv::Mat matrix = this->mCurves[index]->mMatData->getData();
    // check if plot is no longer capable of displaying the data
    if (cedar::aux::math::getDimensionalityOf(matrix) != 0 || matrix.empty())
    {
      data_locker.unlock();
      emit dataChanged();
      return 0.0;
    }
    val = cedar::aux::math::getMatrixEntry<double>(matrix, 0, 0);
  }

  data_locker.unlock();
  return val;
}

void cedar::aux::gui::HistoryPlot0D::CurveInfo::setData(cedar::aux::ConstDataPtr data)
{
  this->mData = data;
  this->mDoubleData = boost::dynamic_pointer_cast<cedar::aux::ConstDoubleData>(data);
  this->mMatData = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  this->mUnitData = boost::dynamic_pointer_cast<cedar::aux::ConstUnitData>(data);

  if (!this->mDoubleData && !this->mMatData && !this->mUnitData)
  {
    CEDAR_THROW
    (
      cedar::aux::gui::InvalidPlotData,
      "Could not cast to cedar::aux::DoubleData, cedar::aux::MatData or cedar::aux::UnitData "
      "in cedar::aux::gui::HistoryPlot0D::CurveInfo::setData."
    );
  }
}

void cedar::aux::gui::HistoryPlot0D::doAppend(cedar::aux::ConstDataPtr data, const std::string& title)
{
  CurveInfoPtr curve(new CurveInfo());
  this->mCurves.push_back(curve);
  size_t index = this->mCurves.size() - 1;

  curve->setData(data);

  if (curve->mUnitData)
  {
    this->mpPlot->setAxisTitle(QwtPlot::yLeft, QString::fromStdString(curve->mUnitData->getSuffix()));
  }

  curve->mCurve = new QwtPlotCurve(QString::fromStdString(title));
  curve->mCurve->attach(this->mpPlot);
  this->applyStyle(index, curve->mCurve);
}

void cedar::aux::gui::HistoryPlot0D::doDetach(cedar::aux::ConstDataPtr data)
{
  auto new_end = std::remove_if(mCurves.begin(), mCurves.end(), [&](CurveInfoPtr curve_info){
    return (curve_info->mData == data);
  });
  mCurves.erase(new_end, mCurves.end());
}

//!@cond SKIPPED_DOCUMENTATION
void cedar::aux::gui::detail::HistoryPlot0DWorker::convert()
{
  for (size_t curve_index = 0; curve_index < this->mpPlot->mCurves.size(); ++curve_index)
  {
    cedar::aux::gui::HistoryPlot0D::CurveInfoPtr curve = this->mpPlot->mCurves[curve_index];

    // update x value
    using namespace cedar::unit;
    Time time_quantity = cedar::aux::GlobalClockSingleton::getInstance()->getTime();
    unsigned int time = static_cast<unsigned int>(time_quantity / Time(1.0 * milli * second));

    curve->mXValues.push_back(time);

    while (curve->mXValues.size() > this->mpPlot->mMaxHistorySize)
    {
      curve->mXValues.pop_front();
    }

    // update y value
    curve->mYValues.push_back(this->mpPlot->getDataValue(curve_index));

    while (curve->mYValues.size() > this->mpPlot->mMaxHistorySize)
    {
      curve->mYValues.pop_front();
    }

    // convert the x values to now - time
    curve->mXArray.resize(curve->mXValues.size());
    for (size_t i = 0; i < curve->mXValues.size(); ++i)
    {
      curve->mXArray.at(i) = static_cast<double>(curve->mXValues.at(i) - time)/1000.0;
    }
    curve->mYArray.assign(curve->mYValues.begin(), curve->mYValues.end());

    CEDAR_DEBUG_ASSERT(curve->mXValues.size() == curve->mYValues.size());
    CEDAR_DEBUG_ASSERT(curve->mXArray.size() == curve->mYArray.size());
  }

  emit done();
}
//!@endcond

void cedar::aux::gui::HistoryPlot0D::conversionDone()
{
  double x_min = std::numeric_limits<double>::max(), x_max = 1.0;
  for (size_t curve_index = 0; curve_index < this->mCurves.size(); ++curve_index)
  {
    CurveInfoPtr curve = this->mCurves[curve_index];

    x_min = std::min(x_min, curve->mXArray.front());
    x_max = std::max(x_max, curve->mXArray.back());

    // choose the right function depending on the qwt version
#if (QWT_VERSION >= 0x060000)
    curve->mCurve->setRawSamples
    (
      &curve->mXArray.front(),
      &curve->mYArray.front(),
      static_cast<int>(curve->mXValues.size())
    );
#elif (QWT_VERSION >= 0x050000)
    curve->mCurve->setData
    (
      &curve->mXArray.front(),
      &curve->mYArray.front(),
      static_cast<int>(curve->mXValues.size())
    );
#else
#error unsupported qwt version
#endif
  }

  this->mpPlot->setAxisScale(QwtPlot::xBottom, x_min, x_max);

  this->mpPlot->replot();
}

void cedar::aux::gui::HistoryPlot0D::timerEvent(QTimerEvent* /* pEvent */)
{
  if (!this->isVisible())
  {
    return;
  }

  emit convert();
}

#endif // CEDAR_USE_QWT
