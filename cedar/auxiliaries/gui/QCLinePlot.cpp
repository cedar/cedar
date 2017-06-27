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

    File:        Qt5LinePlot.cpp

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

#ifdef CEDAR_USE_QCUSTOMPLOT

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/QCLinePlot.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/annotation/Dimensions.h"
#include "cedar/auxiliaries/annotation/ValueRangeHint.h"
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


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::QCLinePlot::QCLinePlot(QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpLock(new QReadWriteLock())
{
  this->init();
}

cedar::aux::gui::QCLinePlot::QCLinePlot(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpLock(new QReadWriteLock())
{
  this->init();
  this->plot(matData, title);
}

cedar::aux::gui::QCLinePlot::~QCLinePlot()
{
  this->PlotSeriesDataVector.clear();
  if (mpLock)
  {
    delete mpLock;
  }
}

QCustomPlot* cedar::aux::gui::QCLinePlot::getPlot()
{
  return this->mpChart;
}


bool cedar::aux::gui::QCLinePlot::canAppend(cedar::aux::ConstDataPtr data) const
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

  return true;
}

void cedar::aux::gui::QCLinePlot::doAppend(cedar::aux::ConstDataPtr data, const std::string& title)
{
  QWriteLocker locker(mpLock);
  this->mpChart->addGraph();
  int c = this->mpChart->graphCount() - 1;
  unsigned long m = mLineColors.size() - 1;
  this->mpChart->graph(c)->setPen(mLineColors.at(c % m));
  this->mpChart->graph(c)->setName(QString::fromStdString(title));
  this->PlotSeriesDataVector.push_back(data);
  this->startTimer(30);
}

bool cedar::aux::gui::QCLinePlot::canDetach(cedar::aux::ConstDataPtr data) const
{
  if(this->mpChart != nullptr && this->PlotSeriesDataVector.size() > 1)
  {
    for(auto plot_series : this->PlotSeriesDataVector)
    {
      if(boost::dynamic_pointer_cast<cedar::aux::ConstData>(plot_series) == data)
      {
        return true;
      }
    }
  }

  return false;
}

void cedar::aux::gui::QCLinePlot::doDetach(cedar::aux::ConstDataPtr data)
{
  mpLock->lockForWrite();
  auto new_end = std::remove_if(this->PlotSeriesDataVector.begin(), this->PlotSeriesDataVector.end(), [&](cedar::aux::ConstDataPtr dataPtr){
    return (boost::dynamic_pointer_cast<cedar::aux::ConstData>(dataPtr) == data);
  });
  this->PlotSeriesDataVector.erase(new_end, this->PlotSeriesDataVector.end());
  mpLock->unlock();
}


void cedar::aux::gui::QCLinePlot::plot(cedar::aux::ConstDataPtr data, const std::string& title)
{
    this->append(data, title);
}

void cedar::aux::gui::QCLinePlot::init()
{
  this->mPlot0D = false;

// initialize vectors, if this has not happened, yet
  if (mLineColors.empty())
  {
    mLineColors.clear();
    mLineColors.push_back(QColor(0, 121, 177));
    mLineColors.push_back(QColor(48, 162, 26));
    mLineColors.push_back(QColor(224, 49, 37));
    mLineColors.push_back(QColor(217, 220, 0));
    mLineColors.push_back(QColor(118, 0, 177));
    mLineColors.push_back(QColor(46, 221, 190));
    mLineColors.push_back(QColor(245, 156, 16));
  }

  QPalette palette = this->palette();
  palette.setColor(QPalette::Window, Qt::white);
  this->setPalette(palette);

  QVBoxLayout *p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);

  this->mpChart = new QCustomPlot();

  //this->mpChart->setInteraction(QCP::iRangeDrag, true);
  //this->mpChart->setInteraction(QCP::iRangeZoom, true);
  //this->mpChart->setInteraction(QCP::iSelectLegend, true);
  //this->mpChart->setInteraction(QCP::iSelectPlottables, true);

  //this->mpChart->xAxis->setRange(0,50);
  //this->mpChart->yAxis->setRange(-5,5);
  //this->mpChart->legend->setVisible(true);

#ifdef QCUSTOMPLOT_USE_OPENGL
  //this->mpChart->setOpenGl(true); //todo QT4
#endif
  this->mpChart->noAntialiasingOnDrag();
  this->mpChart->replot();
  this->layout()->addWidget(this->mpChart);
}

void cedar::aux::gui::QCLinePlot::timerEvent(QTimerEvent * /* pEvent */)
{
  if (!this->isVisible())
  {
    return;
  }
  double x_min = std::numeric_limits<double>::max();
  double x_max = -std::numeric_limits<double>::max();
  double y_min = std::numeric_limits<double>::max();
  double y_max = -std::numeric_limits<double>::max();
  QWriteLocker locker(mpLock);
  for (size_t i = 0; i < PlotSeriesDataVector.size(); ++i)
  {
    if(auto matData = boost::dynamic_pointer_cast<const cedar::aux::MatData>(PlotSeriesDataVector.at(i)))
    {
      QReadLocker locker(&matData->getLock());
      const cv::Mat& plotMat = matData->getData();
      QVector<double> xd(plotMat.rows),yd(plotMat.rows);
      for(int i = 0; i< plotMat.rows;i++)
      {
        xd[i] = (double)i;
        yd[i] = (double)plotMat.at<float>(i,0);
        x_min = std::min(x_min,xd[i]);
        x_max = std::max(x_max,xd[i]);
        y_min = std::min(y_min,yd[i]);
        y_max = std::max(y_max,yd[i]);
      }
      this->mpChart->graph((int)i)->setData(xd,yd);
    }
  }
  this->mpChart->xAxis->setRange(x_min,x_max);
  this->mpChart->yAxis->setRange(y_min,y_max);
  this->mpChart->replot();
}

void cedar::aux::gui::QCLinePlot::setAccepts0DData(bool accept)
{
  this->mPlot0D = accept;
}

#endif

