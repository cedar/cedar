/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/auxiliaries/gui/HistoryPlot0D.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/DoubleData.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <qwt_legend.h>
#include <QVBoxLayout>
#include <QContextMenuEvent>
#include <QMenu>
#include <iostream>

// MACROS
// Enable to show information on locking/unlocking
// #define DEBUG_LOCKS

#ifdef DEBUG_LOGS
#  include "auxiliaries/System.h"
#endif

//!@todo This class and the MatrixPlot1D share a lot of common code -- unify them

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

std::vector<QColor> cedar::aux::gui::HistoryPlot0D::mLineColors;
std::vector<Qt::PenStyle> cedar::aux::gui::HistoryPlot0D::mLineStyles;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::gui::HistoryPlot0D::HistoryPlot0D(QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpCurrentPlotWidget(NULL)
{
  this->init();
}

cedar::aux::gui::HistoryPlot0D::HistoryPlot0D(cedar::aux::DataPtr data, const std::string& title, QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpCurrentPlotWidget(NULL)
{
  this->init();
  this->plot(data, title);
}

cedar::aux::gui::HistoryPlot0D::~HistoryPlot0D()
{
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
  p_legend->setChecked(this->mpPlot->legend() != NULL);

  QAction *p_action = menu.exec(pEvent->globalPos());
  if (p_action == NULL)
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

bool cedar::aux::gui::HistoryPlot0D::canAppend(cedar::aux::ConstDataPtr data) const
{
  if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
  {
    return cedar::aux::math::getDimensionalityOf(mat_data->getData()) == 0;
  }
  else
  {
    return false;
  }
}

void cedar::aux::gui::HistoryPlot0D::init()
{
  mMaxHistorySize = 500;

  mXArray.resize(mMaxHistorySize);
  this->mCurves.clear();
  this->mCurves.push_back(CurveInfoPtr(new CurveInfo()));

  QPalette palette = this->palette();
  palette.setColor(QPalette::Window, Qt::white);
  this->setPalette(palette);

  QVBoxLayout *p_layout = new QVBoxLayout();
  this->setLayout(p_layout);

  mpPlot = new QwtPlot(this);
  this->layout()->addWidget(mpPlot);
}

void cedar::aux::gui::HistoryPlot0D::applyStyle(size_t lineId, QwtPlotCurve *pCurve)
{
  //!@todo This is a carbon-copy from MatrixPlot1D; unify!
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

void cedar::aux::gui::HistoryPlot0D::plot(cedar::aux::DataPtr data, const std::string& title)
{
  this->mCurves.clear();
  this->doAppend(data, title);

  this->startTimer(30); //!@todo make the refresh time configurable.
}

double cedar::aux::gui::HistoryPlot0D::getDataValue(size_t index)
{
  CEDAR_DEBUG_ASSERT(index < this->mCurves.size());

  // lock the data
  this->mCurves[index]->mData->lockForRead();

  // read out the double value
  double val;
  if (this->mCurves[index]->mDoubleData)
  {
    val = this->mCurves[index]->mDoubleData->getData();
  }
  else if (this->mCurves[index]->mMatData)
  {
    cv::Mat matrix = this->mCurves[index]->mMatData->getData();
    //!@todo This check if the data still has the correct format should be somewhere else (probably)
    if (cedar::aux::math::getDimensionalityOf(matrix) != 0) // plot is no longer capable of displaying the data
    {
      this->mCurves[index]->mData->unlock();
      emit dataChanged();
      return 0.0;
    }
    val = cedar::aux::math::getMatrixEntry<double>(matrix, 0, 0);
  }

  this->mCurves[index]->mData->unlock();
  return val;
}

void cedar::aux::gui::HistoryPlot0D::CurveInfo::setData(cedar::aux::DataPtr data)
{
  this->mData = data;
  this->mDoubleData = boost::shared_dynamic_cast<cedar::aux::DoubleData>(data);
  this->mMatData = boost::shared_dynamic_cast<cedar::aux::MatData>(data);

  if (!this->mDoubleData && !this->mMatData)
  {
    CEDAR_THROW
    (
      cedar::aux::gui::InvalidPlotData,
      "Could not cast to cedar::aux::DoubleData or cedar::aux::MatData "
      "in cedar::aux::gui::HistoryPlot0D::CurveInfo::setData."
    );
  }
}

void cedar::aux::gui::HistoryPlot0D::doAppend(cedar::aux::DataPtr data, const std::string& title)
{
  CurveInfoPtr curve(new CurveInfo());
  this->mCurves.push_back(curve);
  size_t index = this->mCurves.size() - 1;

  curve->setData(data);

  curve->mCurve = new QwtPlotCurve(QString::fromStdString(title));
  curve->mCurve->attach(this->mpPlot);
  this->applyStyle(index, curve->mCurve);

  double val = this->getDataValue(index);
  curve->mYValues.assign(this->mpXValues.size(), val);
}


void cedar::aux::gui::HistoryPlot0D::timerEvent(QTimerEvent* /* pEvent */)
{
  if (!this->isVisible())
  {
    return;
  }

  //!@todo: Use actual time measurements here
  if (mpXValues.empty())
  {
    mpXValues.push_back(0);
  }
  else
  {
    mpXValues.push_back(mpXValues.back() + 1);
  }

  while (mpXValues.size() > this->mMaxHistorySize)
  {
    mpXValues.pop_front();
  }

  for (size_t curve_index = 0; curve_index < this->mCurves.size(); ++curve_index)
  {
    CurveInfoPtr curve = this->mCurves[curve_index];
    curve->mYValues.push_back(this->getDataValue(curve_index));

    while (curve->mYValues.size() > this->mMaxHistorySize)
    {
      curve->mYValues.pop_front();
    }

    mXArray.assign(mpXValues.begin(), mpXValues.end());
    curve->mYArray.assign(curve->mYValues.begin(), curve->mYValues.end());

    CEDAR_DEBUG_ASSERT(mpXValues.size() == curve->mYValues.size());
    CEDAR_DEBUG_ASSERT(mXArray.size() == curve->mYArray.size());

    // choose the right function depending on the qwt version
    //!@todo write a macro that does this check (see MatrixPlot1D.cpp)
#if (QWT_VERSION >> 16) == 5
    curve->mCurve->setData
    (
      &this->mXArray.front(),
      &curve->mYArray.front(),
      static_cast<int>(this->mpXValues.size())
    );
#elif (QWT_VERSION >> 16) == 6
    curve->mCurve->setRawSamples
    (
      &this->mXArray.front(),
      &curve->mYArray.front(),
      static_cast<int>(this->mpXValues.size())
    );
#else
#error unsupported qwt version
#endif
  }
  this->mpPlot->replot();
}
