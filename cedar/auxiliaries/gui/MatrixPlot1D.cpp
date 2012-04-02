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

    File:        MatrixPlot2D.cpp

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
#include "cedar/auxiliaries/gui/MatrixPlot1D.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <qwt/qwt_legend.h>
#include <QContextMenuEvent>
#include <QVBoxLayout>
#include <QPalette>
#include <QMenu>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

std::vector<QColor> cedar::aux::gui::MatrixPlot1D::mLineColors;
std::vector<Qt::PenStyle> cedar::aux::gui::MatrixPlot1D::mLineStyles;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::MatrixPlot1D::MatrixPlot1D(QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpLock(new QReadWriteLock())
{
  this->init();
}

cedar::aux::gui::MatrixPlot1D::MatrixPlot1D(cedar::aux::DataPtr matData, const std::string& title, QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpLock(new QReadWriteLock())
{
  this->init();
  this->plot(matData, title);
}

cedar::aux::gui::MatrixPlot1D::~MatrixPlot1D()
{
  if (mpLock)
  {
    delete mpLock;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::MatrixPlot1D::applyStyle(size_t lineId, QwtPlotCurve *pCurve)
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

bool cedar::aux::gui::MatrixPlot1D::canAppend(cedar::aux::ConstDataPtr data) const
{
  cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data);
  if (!mat_data)
  {
    return false;
  }

  return true;
}

void cedar::aux::gui::MatrixPlot1D::doAppend(cedar::aux::DataPtr data, const std::string& title)
{
  PlotSeriesPtr plot_series(new PlotSeries());

  size_t line_id = mPlotSeriesVector.size();

  mpLock->lockForWrite();
  mPlotSeriesVector.push_back(plot_series);

  plot_series->mMatData = boost::shared_dynamic_cast<cedar::aux::MatData>(data);


  if (!plot_series->mMatData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Could not cast to cedar::aux::MatData in cedar::aux::gui::MatrixPlot1D::plot.");
  }

  plot_series->mpCurve = new QwtPlotCurve(title.c_str());
  applyStyle(line_id, plot_series->mpCurve);

  data->lockForRead();
  const cv::Mat& mat = plot_series->mMatData->getData();
  size_t num = cedar::aux::math::get1DMatrixSize(mat);
  data->unlock();

  // skip if the matrix is empty
  if (num == 0)
  {
    return;
  }

  this->buildArrays(plot_series, num);

  plot_series->mpCurve->setData(&plot_series->mXValues.at(0), &plot_series->mYValues.at(0), num);

  plot_series->mpCurve->attach(this->mpPlot);

  mpLock->unlock();

  this->startTimer(30); //!@todo make the refresh time configurable.
}

void cedar::aux::gui::MatrixPlot1D::attachMarker(QwtPlotMarker *pMarker)
{
  pMarker->attach(this->mpPlot);
}

void cedar::aux::gui::MatrixPlot1D::clearMarkers()
{
  this->mpPlot->detachItems(QwtPlotMarker::Rtti_PlotMarker, true);
}

void cedar::aux::gui::MatrixPlot1D::plot(cedar::aux::DataPtr data, const std::string& title)
{
  mpLock->lockForWrite();
  mPlotSeriesVector.clear();
  mpLock->unlock();

  this->append(data, title);

  this->startTimer(30); //!@todo make the refresh time configurable.
}

void cedar::aux::gui::MatrixPlot1D::init()
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

void cedar::aux::gui::MatrixPlot1D::contextMenuEvent(QContextMenuEvent *pEvent)
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

void cedar::aux::gui::MatrixPlot1D::showLegend(bool show)
{
  if (show)
  {
    // show legend
    QwtLegend *p_legend = this->mpPlot->legend();
    if (p_legend == NULL)
    {
      p_legend = new QwtLegend();
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

void cedar::aux::gui::MatrixPlot1D::buildArrays(PlotSeriesPtr series, unsigned int new_size)
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

void cedar::aux::gui::MatrixPlot1D::timerEvent(QTimerEvent * /* pEvent */)
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
    if (size == 0)
    {
      series->mMatData->unlock();
      return;
    }

    if (series->mXValues.size() != size)
    {
      this->buildArrays(series, size);
    }

    for (size_t i = 0; i < series->mXValues.size(); ++i)
    {
      series->mYValues.at(i) = cedar::aux::math::getMatrixEntry<double>(mat, i);
    }
    series->mMatData->unlock();

    series->mpCurve->setData(&series->mXValues.at(0),
                             &series->mYValues.at(0),
                             static_cast<int>(series->mXValues.size()));
  }

  this->mpPlot->replot();
  mpLock->unlock();
}
