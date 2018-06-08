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
textTicker(new QCPAxisTickerText),
mpLock(new QReadWriteLock())
{
  this->init();
}

cedar::aux::gui::QCLinePlot::QCLinePlot(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
textTicker(new QCPAxisTickerText),
mpLock(new QReadWriteLock())
{
  this->init();
  this->plot(matData, title);
}

cedar::aux::gui::QCLinePlot::~QCLinePlot()
{
  this->PlotSeriesDataVector.clear();
  if(this->mpChart != nullptr)
  {
    delete this->mpChart;
  }
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
  if (data->hasAnnotation<cedar::aux::annotation::DiscreteMetric>())
  {
    this->mpChart->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,10));
    this->mpChart->graph()->setLineStyle(QCPGraph::lsNone);
    this->DiscreteMetric = true;
  }
  if(this->DiscreteMetricLabels)
  {
    this->mpChart->xAxis->setTickLabelRotation(90);
    this->mpChart->xAxis->setTicker(textTicker);
  }
  this->PlotSeriesDataVector.push_back(data);
  locker.unlock();
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
  for (unsigned int i = 0; i < this->PlotSeriesDataVector.size(); ++i)
  {
    if(boost::dynamic_pointer_cast<cedar::aux::ConstData>(this->PlotSeriesDataVector[i]) == data)
    {
      this->mpChart->removeGraph(i);
      this->PlotSeriesDataVector.erase(this->PlotSeriesDataVector.begin() + i);
      break;
    }
  }
  mpLock->unlock();
}


void cedar::aux::gui::QCLinePlot::plot(cedar::aux::ConstDataPtr data, const std::string& title)
{
    this->append(data, title);
}

void cedar::aux::gui::QCLinePlot::init()
{

  this->_mAutoScalingEnabled = new cedar::aux::BoolParameter(this, "autoscaling", true);
  this->_mMajorGridVisible = new cedar::aux::BoolParameter(this, "major grid visible", false);
  this->_mMinorGridVisible = new cedar::aux::BoolParameter(this, "minor grid visible", false);
  this->_mYAxisLimits = new cedar::aux::math::DoubleLimitsParameter(this, "y axis limits", 0.0, 1.0);
  QObject::connect(this->_mAutoScalingEnabled.get(), SIGNAL(valueChanged()), this, SLOT(autoScalingChanged()));
  QObject::connect(this->_mYAxisLimits.get(), SIGNAL(valueChanged()), this, SLOT(axisLimitsChanged()));
  QObject::connect(this->_mMajorGridVisible.get(), SIGNAL(valueChanged()), this, SLOT(gridVisibilityChanged()));
  QObject::connect(this->_mMinorGridVisible.get(), SIGNAL(valueChanged()), this, SLOT(gridVisibilityChanged()));


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

  this->mpChart->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this->mpChart, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));


#ifdef QCUSTOMPLOT_USE_OPENGL
  //this->mpChart->setOpenGl(true); //todo QT4
#endif
  this->mpChart->noAntialiasingOnDrag();
  this->mpChart->replot();
  this->layout()->addWidget(this->mpChart);
}

void cedar::aux::gui::QCLinePlot::gridVisibilityChanged()
{
  bool major_visible = this->_mMajorGridVisible->getValue();
  bool minor_visible = this->_mMinorGridVisible->getValue();
  bool any_visible = minor_visible || major_visible;
  this->mpChart->xAxis->grid()->setVisible(any_visible);
  this->mpChart->yAxis->grid()->setVisible(any_visible);
}

void cedar::aux::gui::QCLinePlot::autoScalingChanged()
{
//  if (!this->SettingFixedYAxisScaling)
//  {
//    this->SettingFixedYAxisScaling = true;
//  }
//  else
//  {
//    this->axisLimitsChanged();
//  }

  this->axisLimitsChanged();

}

void cedar::aux::gui::QCLinePlot::axisLimitsChanged()
{
  if (!this->_mAutoScalingEnabled->getValue())
  {
    double lower = this->_mYAxisLimits->getLowerLimit();
    double upper = this->_mYAxisLimits->getUpperLimit();
    this->FixedYLimitMin = lower;
    this->FixedYLimitMax = upper;

  }
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
      QReadLocker locker2(&matData->getLock());
      const cv::Mat& plotMat = matData->getData();

      auto dim = cedar::aux::math::getDimensionalityOf(plotMat);
      if (dim != 1) // plot is no longer capable of displaying the data
      {
        if (dim != 0 || !this->mPlot0D)
        {
          locker.unlock();
          locker2.unlock();
          emit dataChanged();
          return;
        }
      }
      // Check if the size of the matrix has changed
      unsigned int size = cedar::aux::math::get1DMatrixSize(plotMat);
      // skip if the array is empty
      if (size == 0)
      {
        continue;
      }
      double local_min, local_max;
      local_min = std::numeric_limits<double>::max();
      local_max = -std::numeric_limits<double>::max();
      cedar::aux::annotation::ConstDimensionsPtr dimensions;
      if (matData->hasAnnotation<cedar::aux::annotation::Dimensions>())
      {
        dimensions = matData->getAnnotation<cedar::aux::annotation::Dimensions>();
      }
      QVector<double> xd(plotMat.rows),yd(plotMat.rows);
      for(int ia = 0; ia< plotMat.rows;ia++)
      {
        if (!dimensions || !dimensions->hasSamplingPositions(0))
        {
          xd[ia] = (double)ia;
          local_min = std::min(local_min,xd[ia]);
          local_max = std::max(local_max,xd[ia]);
        }
        else
        {
          const auto& sampling_positions = dimensions->getSamplingPositions(0);
          double sampling_position;
          if (static_cast<unsigned int>(ia) < sampling_positions.size())
          {
            sampling_position = sampling_positions.at(ia);
          }
          else
          {
            sampling_position = static_cast<double>(ia);
          }
          xd[ia] = sampling_position;
          local_min = std::min(local_min, sampling_position);
          local_max = std::max(local_max, sampling_position);
        }
        yd[ia] = (double)plotMat.at<float>(ia,0);
        y_min = std::min(y_min,yd[ia]);
        y_max = std::max(y_max,yd[ia]);
      }
      x_min = std::min(x_min, local_min);
      x_max = std::max(x_max, local_max);
      this->mpChart->graph((int)i)->setData(xd,yd);

      if (this->mPlot0D && matData->hasAnnotation<cedar::aux::annotation::Dimensions>())
      {
        auto annotation = matData->getAnnotation<cedar::aux::annotation::Dimensions>();
        if (annotation->getDimensionality() >= 1)
        {
          //this->mpChart->graph((int)i)->setName(QString::fromStdString(annotation->getLabel(0)));
          this->mpChart->xAxis->setLabel(QString::fromStdString(annotation->getLabel(0)));
        }
      }
      locker2.unlock();
    }
  }
  locker.unlock();
  this->YLimitMin = y_min;
  this->YLimitMax = y_max;

  if(!this->_mAutoScalingEnabled->getValue())
  {
    y_min = this->FixedYLimitMin;
    y_max = this->FixedYLimitMax;
  }else{
    y_min -= 0.5;
    y_max += 0.5;
  }

  if(this->DiscreteMetric)
  {
    x_min -= 0.5;
    x_max += 0.5;
    y_min -= 0.5;
    y_max += 0.5;
  }

  this->mpChart->xAxis->setRange(x_min,x_max);
  this->mpChart->xAxis->grid()->setVisible(this->SettingShowGrid);
  this->mpChart->yAxis->setRange(y_min,y_max);
  this->mpChart->yAxis->grid()->setVisible(this->SettingShowGrid);
  this->mpChart->replot();
}

void cedar::aux::gui::QCLinePlot::setAccepts0DData(bool accept)
{
  this->mPlot0D = accept;
}

void cedar::aux::gui::QCLinePlot::setAutomaticYAxisScaling()
{
  this->_mAutoScalingEnabled->setValue(true,true);
}

void cedar::aux::gui::QCLinePlot::setFixedYAxisScaling(double lower, double upper)
{
  this->FixedYLimitMin = lower;
  this->FixedYLimitMax = upper;
//  this->SettingFixedYAxisScaling = true;

  this->_mAutoScalingEnabled->setValue(false,true);
  this->_mYAxisLimits->setValue(cedar::aux::math::Limits<double>(lower, upper), true);
}

void cedar::aux::gui::QCLinePlot::setFixedXAxisScaling(double /*lower*/, double /*upper*/)
{
  //todo
}

void cedar::aux::gui::QCLinePlot::setFixedYAxisScaling()
{
  if(this->_mAutoScalingEnabled->getValue())
  {
    QDialog* p_dialog = new QDialog();
    p_dialog->setModal(true);
    auto p_layout = new QGridLayout();
    p_dialog->setLayout(p_layout);
    QLabel* p_label;
    p_label = new QLabel("lower limit:");
    p_layout->addWidget(p_label, 0, 0);

    auto p_lower = new QDoubleSpinBox();
    p_layout->addWidget(p_lower, 0, 1);
    p_lower->setMinimum(boost::numeric::bounds<double>::lowest());
    p_lower->setMaximum(boost::numeric::bounds<double>::highest());
    p_lower->setValue(this->YLimitMin);

    p_label = new QLabel("upper limit:");
    p_layout->addWidget(p_label, 1, 0);

    auto p_upper = new QDoubleSpinBox();
    p_layout->addWidget(p_upper, 1, 1);
    p_upper->setMinimum(boost::numeric::bounds<double>::lowest());
    p_upper->setMaximum(boost::numeric::bounds<double>::highest());
    p_upper->setValue(this->YLimitMax);

    auto p_buttons = new QDialogButtonBox();
    p_buttons->addButton(QDialogButtonBox::Ok);
    p_buttons->addButton(QDialogButtonBox::Cancel);
    p_layout->addWidget(p_buttons, 2, 0, 1, 2);

    QObject::connect(p_buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()), p_dialog, SLOT(accept()));
    QObject::connect(p_buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), p_dialog, SLOT(reject()));

    int res = p_dialog->exec();

    if (res == QDialog::Accepted)
    {
      this->FixedYLimitMin = p_lower->value();
      this->FixedYLimitMax = p_upper->value();
      this->_mYAxisLimits->setValue(cedar::aux::math::Limits<double>(this->FixedYLimitMin,this->FixedYLimitMax),true);
      this->_mAutoScalingEnabled->setValue(false,true);
    }else{
      this->_mAutoScalingEnabled->setValue(true,true);
    }
  }
  else
  {
    this->_mAutoScalingEnabled->setValue(true,true);
  }
}

void cedar::aux::gui::QCLinePlot::showGrid(bool show)
{
  this->SettingShowGrid = show;
}

void cedar::aux::gui::QCLinePlot::showLegend(bool show)
{
  this->SettingShowLegend = show;
  this->mpChart->legend->setVisible(this->SettingShowLegend);
}

void cedar::aux::gui::QCLinePlot::contextMenuRequest(QPoint pos)
{
  QMenu *menu = new QMenu(this);
  menu->setAttribute(Qt::WA_DeleteOnClose);

  QAction *p_legend = menu->addAction("legend", this, SLOT(showLegend(bool)));
  p_legend->setCheckable(true);
  p_legend->setChecked(this->SettingShowLegend);

  QAction *p_grid = menu->addAction("grid", this, SLOT(showGrid(bool)));
  p_grid->setCheckable(true);
  p_grid->setChecked(this->SettingShowGrid);

  QAction *p_fixedaxis = menu->addAction("fixed y axis scaling", this, SLOT(setFixedYAxisScaling()));
  p_fixedaxis->setCheckable(true);
  p_fixedaxis->setChecked(!this->_mAutoScalingEnabled->getValue());

  menu->popup(this->mpChart->mapToGlobal(pos));
}
