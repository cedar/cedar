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

#ifdef CEDAR_USE_QT5_PLOTS

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Qt5LinePlot.h"
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
//#include <qwt_legend.h>
//#include <qwt_scale_div.h>
//#include <qwt_symbol.h>
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

std::vector<QColor> cedar::aux::gui::Qt5LinePlot::mLineColors;
std::vector<Qt::PenStyle> cedar::aux::gui::Qt5LinePlot::mLineStyles;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::Qt5LinePlot::Qt5LinePlot(QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpLock(new QReadWriteLock())
{
  this->init();
}

cedar::aux::gui::Qt5LinePlot::Qt5LinePlot(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget *pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpLock(new QReadWriteLock())
{
  this->init();
  this->plot(matData, title);
}

cedar::aux::gui::Qt5LinePlot::~Qt5LinePlot()
{
  if (this->mpWorkerThread)
  {
    this->mpWorkerThread->quit();
    this->mpWorkerThread->wait();
    delete this->mpWorkerThread;
    this->mpWorkerThread = NULL;
  }

  this->mPlotSeriesDataVector.clear();

  if (mpLock)
  {
    delete mpLock;
  }
}

QtCharts::QChartView* cedar::aux::gui::Qt5LinePlot::getPlot()
{
  return this->mpChartView;
}

void cedar::aux::gui::Qt5LinePlot::contextMenuEvent(QContextMenuEvent *pEvent)
{
  QMenu menu(this);

  //QAction *p_antialiasing = menu.addAction("antialiasing");
  //p_antialiasing->setCheckable(true);
  //p_antialiasing->setChecked(this->SettingAntiAliasing);

  QAction *p_legend = menu.addAction("legend");
  p_legend->setCheckable(true);
  QObject::connect(p_legend, SIGNAL(toggled(bool)), this, SLOT(showLegend(bool)));
  p_legend->setChecked(this->SettingShowLegend);


  QAction *p_grid = menu.addAction("grid");
  p_grid->setCheckable(true);
  QObject::connect(p_grid, SIGNAL(toggled(bool)), this, SLOT(showGrid(bool)));
  p_grid->setChecked(this->SettingShowGrid);


  QAction *p_fixedaxis = menu.addAction("fixed y axis scaling");
  p_fixedaxis->setCheckable(true);
  QObject::connect(p_fixedaxis, SIGNAL(triggered()), this, SLOT(setFixedYAxisScaling()));
  p_fixedaxis->setChecked(this->SettingFixedYAxisScaling);


  if(this->mPlot0D)
  {
    QAction *p_fixedsampling = menu.addAction("history buffer size");
    p_fixedsampling->setCheckable(true);
    QObject::connect(p_fixedsampling, SIGNAL(triggered()), this, SLOT(setFixedSampling()));
    p_fixedsampling->setChecked(this->SettingFixedSampling);
  }

  QAction *p_action = menu.exec(pEvent->globalPos());
  if (p_action == NULL)
  {
    return;
  }

  //if (p_action == p_antialiasing)
  //{
    //this->SettingAntiAliasing = p_action->isChecked();
  //}
}

void cedar::aux::gui::Qt5LinePlot::setFixedSampling()
{
  if(!this->SettingFixedSampling)
  {
    QDialog* p_dialog = new QDialog();
    p_dialog->setModal(true);
    auto p_layout = new QGridLayout();
    p_dialog->setLayout(p_layout);
    QLabel* p_label;
    p_label = new QLabel("sampling size:");
    p_layout->addWidget(p_label, 0, 0);

    auto p_lower = new QSpinBox();
    p_layout->addWidget(p_lower, 0, 1);
    p_lower->setMinimum(boost::numeric::bounds<int>::lowest());
    p_lower->setMaximum(boost::numeric::bounds<int>::highest());
    p_lower->setValue(static_cast<int>(this->SamplingSize));

    auto p_buttons = new QDialogButtonBox();
    p_buttons->addButton(QDialogButtonBox::Ok);
    p_buttons->addButton(QDialogButtonBox::Cancel);
    p_layout->addWidget(p_buttons, 2, 0, 1, 2);

    QObject::connect(p_buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()), p_dialog, SLOT(accept()));
    QObject::connect(p_buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), p_dialog, SLOT(reject()));

    int res = p_dialog->exec();

    if (res == QDialog::Accepted)
    {
      this->SamplingSize = static_cast<size_t>(p_lower->value());
      this->SettingFixedSampling = true;
    }else{
      this->SettingFixedSampling = false;
      this->SamplingSize = 100;
    }
  }else{
    this->SettingFixedSampling = false;
    this->SamplingSize = 100;
  }
}


void cedar::aux::gui::Qt5LinePlot::setFixedYAxisScaling()
{
  if(!this->SettingFixedYAxisScaling)
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
      this->SettingFixedYAxisScaling = true;
    }else{
      this->SettingFixedYAxisScaling = false;
    }
  }else{
    this->SettingFixedYAxisScaling = false;
  }
}

void cedar::aux::gui::Qt5LinePlot::showGrid(bool show)
{
  this->SettingShowGrid = show;
}

void cedar::aux::gui::Qt5LinePlot::showLegend(bool show)
{
  this->SettingShowLegend = show;
  if (show)
  {
    this->mpChart->legend()->show();
    auto markerList = this->mpChart->legend()->markers();
    for(int i=0;i<markerList.size();i++)
    {
      auto curMarker = markerList.at(i);
      if(qobject_cast<QtCharts::QScatterSeries *>(curMarker->series()))
      {
        curMarker->setVisible(false);
      }
    }
  }
  else {
    this->mpChart->legend()->hide();
  }
}

bool cedar::aux::gui::Qt5LinePlot::canAppend(cedar::aux::ConstDataPtr data) const
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

void cedar::aux::gui::Qt5LinePlot::doAppend(cedar::aux::ConstDataPtr data, const std::string& title)
{

  QWriteLocker locker(mpLock);
  if(auto matData = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
  {
    const cv::Mat& plotMat = matData->getData();
    QtCharts::QLineSeries *tempSeries = new QtCharts::QLineSeries;

//    if (data->hasAnnotation<cedar::aux::annotation::Dimensions>())
//    {
//      auto annotation = data->getAnnotation<cedar::aux::annotation::Dimensions>();
//      if (annotation->getDimensionality() >= 1)
//      {
//        tempSeries->setName(QString::fromStdString(annotation->getLabel(0)));
//      }
//    }
//    else
//    {
      tempSeries->setName(QString::fromStdString(title));
//    }

    if (data->hasAnnotation<cedar::aux::annotation::DiscreteMetric>())
    {
      //pCurve->setStyle(QwtPlotCurve::NoCurve); //todo sinn?
    }

    tempSeries->setUseOpenGL(true);
   // tempSeries->attachAxis(this->axisX);
   // tempSeries->attachAxis(this->axisY);
    for(int i = 0; i< plotMat.rows;i++)
    {
      tempSeries->append(QPointF(qreal(i), qreal(plotMat.at<float>(i,0))));
    }
    this->mpChart->addSeries(tempSeries);

    PlotSeriesDataPtr seriesData(new PlotSeriesData(this->mpChart,tempSeries,matData));
    mPlotSeriesDataVector.push_back(seriesData);
  }
  //this->mpChart->createDefaultAxes();
  this->startTimer(30);
}

bool cedar::aux::gui::Qt5LinePlot::canDetach(cedar::aux::ConstDataPtr data) const
{
  if(this->mpChart != nullptr && this->mPlotSeriesDataVector.size() > 1)
  {
    for(auto plot_series : this->mPlotSeriesDataVector)
    {
      if(boost::dynamic_pointer_cast<cedar::aux::ConstData>(plot_series->mMatData) == data)
      {
        return true;
      }
    }
  }

  return false;
}

void cedar::aux::gui::Qt5LinePlot::doDetach(cedar::aux::ConstDataPtr data)
{
  mpLock->lockForWrite();
  auto new_end = std::remove_if(mPlotSeriesDataVector.begin(), mPlotSeriesDataVector.end(), [&](PlotSeriesDataPtr plot_series){
    return (boost::dynamic_pointer_cast<cedar::aux::ConstData>(plot_series->mMatData) == data);
  });
  mPlotSeriesDataVector.erase(new_end, mPlotSeriesDataVector.end());
  mpLock->unlock();
}


void cedar::aux::gui::Qt5LinePlot::plot(cedar::aux::ConstDataPtr data, const std::string& title)
{
 // std::cout << "called plot" << std::endl;
    QWriteLocker locker(mpLock);
    //this->mpSeriesList.clear(); TODO clear chart?
    //this->mpChart->removeAllSeries();
    locker.unlock();
    this->append(data, title);
}

void cedar::aux::gui::Qt5LinePlot::init()
{
  this->mPlot0D = false;
  //std::cout << "called init" << std::endl;
  QPalette palette = this->palette();
  palette.setColor(QPalette::Window, Qt::white);
  this->setPalette(palette);

  QVBoxLayout *p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);

  this->mpChart = new QtCharts::QChart();
  this->mpChart->setMargins(QMargins(1, 1, 1, 1));
  this->mpChart->layout()->setContentsMargins(0, 0, 0, 0);
  this->mpChart->setContentsMargins(0, 0, 0, 0);
  this->mpChart->setBackgroundRoundness(0);

//  this->axisX = new QtCharts::QValueAxis;
//  this->axisY = new QtCharts::QValueAxis;
//  this->mpChart->addAxis(this->axisX, Qt::AlignBottom);
//  this->mpChart->addAxis(this->axisY, Qt::AlignLeft);

  this->mpChartView = new QtCharts::QChartView(this->mpChart);

  this->mpChartView->setRenderHint(QPainter::Antialiasing);
  this->mpChartView->setRenderHint(QPainter::HighQualityAntialiasing);
  if(!this->SettingShowLegend)this->mpChart->legend()->hide();
  this->layout()->addWidget(mpChartView);

  this->mpWorkerThread = new QThread();

  mConversionWorker = cedar::aux::gui::detail::Qt5LinePlotWorkerPtr(new cedar::aux::gui::detail::Qt5LinePlotWorker(this));
  mConversionWorker->moveToThread(mpWorkerThread);

  this->mpWorkerThread->start(QThread::LowPriority);

  QObject::connect(mConversionWorker.get(), SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
  QObject::connect(this, SIGNAL(convert()), mConversionWorker.get(), SLOT(convert()));
  QObject::connect(mConversionWorker.get(), SIGNAL(done(double, double)), this, SLOT(conversionDone(double, double)));
}

void cedar::aux::gui::Qt5LinePlot::timerEvent(QTimerEvent * /* pEvent */)
{
  if (!this->isVisible())
  {
    return;
  }
  emit convert();
}

void cedar::aux::gui::Qt5LinePlot::setAccepts0DData(bool accept)
{
  this->mPlot0D = accept;
}

void cedar::aux::gui::Qt5LinePlot::attachMarker(QtCharts::QScatterSeries* markerSeries)
{
  mMarkers.push_back(markerSeries);

  this->mpChart->addSeries(markerSeries);
}

size_t cedar::aux::gui::Qt5LinePlot::getSamplingSize()
{
  return this->SamplingSize;
}

void cedar::aux::gui::Qt5LinePlot::getStyleFor(cedar::aux::MatDataPtr data, QPen& pen, QBrush& brush) const
{
  for (auto plot_series : this->mPlotSeriesDataVector)
  {
    if (plot_series->mMatData == data)
    {
      pen = plot_series->mQLineSeries->pen();
      brush = plot_series->mQLineSeries->brush();
      return;
    }
  }
  CEDAR_THROW(cedar::aux::NotFoundException, "Could not find the given data.");
}


void cedar::aux::gui::detail::Qt5LinePlotWorker::convert()
{
  QWriteLocker plot_locker(this->mpPlot->mpLock);
  double x_min = std::numeric_limits<double>::max();
  double x_max = -std::numeric_limits<double>::max();

  for (size_t i = 0; i < this->mpPlot->mPlotSeriesDataVector.size(); ++i) {
    cedar::aux::gui::Qt5LinePlot::PlotSeriesDataPtr series = this->mpPlot->mPlotSeriesDataVector.at(i);
    QReadLocker locker(&series->mMatData->getLock());
    const cv::Mat& mat = series->mMatData->getData();
    auto dim = cedar::aux::math::getDimensionalityOf(mat);
    if (dim != 1) // plot is no longer capable of displaying the data
    {
      if (dim != 0 || !this->mpPlot->mPlot0D)
      {
        emit dataChanged();
        return;
      }
    }
    // Check if the size of the matrix has changed
    unsigned int size = cedar::aux::math::get1DMatrixSize(mat);
    // skip if the array is empty
    if (size == 0)
    {
      continue;
    }
    double local_min, local_max;

    cedar::aux::annotation::ConstDimensionsPtr dimensions;
    if (series->mMatData->hasAnnotation<cedar::aux::annotation::Dimensions>())
    {
      dimensions = series->mMatData->getAnnotation<cedar::aux::annotation::Dimensions>();
    }

    if (!dimensions || !dimensions->hasSamplingPositions(0))
    {
      local_min = 0;
      local_max = (double)mat.rows;
      if(this->mpPlot->mPlot0D)
      {
        local_min = -0.5;
        local_max = 0.5;
      }
    }
    else
    {
      local_min = std::numeric_limits<double>::max();
      local_max = -std::numeric_limits<double>::max();
      const auto& sampling_positions = dimensions->getSamplingPositions(0);

      for (int i = 0; i < mat.rows; ++i)
      {
        double sampling_position;
        if (static_cast<unsigned int>(i) < sampling_positions.size())
        {
          sampling_position = sampling_positions.at(i);
        }
        else
        {
          sampling_position = static_cast<double>(i);
        }
        local_min = std::min(local_min, sampling_position);
        local_max = std::max(local_max, sampling_position);
      }
    }
//    std::cout<<"Convert: local_min: " << local_min << "local max: " << local_max<< std::endl; //This is Time in 0D-Plot Range -X to 0
    x_min = std::min(x_min, local_min);
    x_max = std::max(x_max, local_max);
  }
  plot_locker.unlock();
  emit done(x_min, x_max);
}


void cedar::aux::gui::Qt5LinePlot::conversionDone(double x_min, double x_max)
{

  QReadLocker locker(this->mpLock);

  double y_min = std::numeric_limits<double>::max();
  double y_max = -std::numeric_limits<double>::max();
 // double x_max = -std::numeric_limits<double>::max();

  for (size_t i = 0; i < this->mPlotSeriesDataVector.size(); ++i)
  {
    PlotSeriesDataPtr series = this->mPlotSeriesDataVector.at(i);
    if(series->mQLineSeries->isVisible())
    {
      QReadLocker locker(&series->mMatData->getLock());
      const cv::Mat& plotMat = series->mMatData->getData();

      //x_max = std::max(x_max, (double)plotMat.rows);

      for (int i = 0; i < plotMat.rows; i++) {
//      qreal x = qreal(i);
        qreal y = qreal(plotMat.at<float>(i, 0));
        y_min = std::min(y_min, y);
        y_max = std::max(y_max, y);
      }
    }
  }

  QtCharts::QValueAxis *axisX;
  QtCharts::QValueAxis *axisY;

  axisX = new QtCharts::QValueAxis;
  axisY = new QtCharts::QValueAxis;

//  std::cout<<"conversionDone: y_min: " << y_min <<" y_max" << y_max <<  std::endl;

  this->YLimitMin = y_min;
  this->YLimitMax = y_max;

  if(this->SettingFixedYAxisScaling)
  {
    y_min = this->FixedYLimitMin;
    y_max = this->FixedYLimitMax;
  }else{
    y_min -= 0.5;
    y_max += 0.5;
  }

  axisY->setRange(y_min, y_max);
  axisX->setRange(x_min, x_max);

  axisX->setGridLineVisible(this->SettingShowGrid);
  axisY->setGridLineVisible(this->SettingShowGrid);

  this->mpChart->setAxisX(axisX);
  this->mpChart->setAxisY(axisY);

  for (size_t i = 0; i < this->mPlotSeriesDataVector.size(); ++i)
  {
    PlotSeriesDataPtr series = this->mPlotSeriesDataVector.at(i);
    if(series->mQLineSeries->isVisible())
    {
      QReadLocker locker(&series->mMatData->getLock());
      const cv::Mat& plotMat = series->mMatData->getData();

      if (this->mPlot0D && series->mMatData->hasAnnotation<cedar::aux::annotation::Dimensions>())
      {
        auto annotation = series->mMatData->getAnnotation<cedar::aux::annotation::Dimensions>();
        if (annotation->getDimensionality() >= 1)
        {
          axisX->setTitleText(QString::fromStdString(annotation->getLabel(0)));
        }
      }

      series->mQLineSeries->clear();
      series->mQLineSeries->attachAxis(axisX);
      series->mQLineSeries->attachAxis(axisY);
//    std::cout<<"PlotMat Rows:"<< plotMat.rows << " Cols: "<< plotMat.cols <<std::endl;
//    std::cout<<"Mat: " << plotMat <<std::endl;
      for(int i = 0; i< plotMat.rows;i++)
      {
        qreal x = qreal((((x_max-x_min)*i)/ static_cast<double>(plotMat.rows))+x_min);
        qreal y = qreal(plotMat.at<float>(i,0));
        series->mQLineSeries->append(x, y);
      }
    }
  }

  for(unsigned int i = 0 ; i<mMarkers.size();i++)
  {
    mMarkers.at(i)->attachAxis(axisX);
    mMarkers.at(i)->attachAxis(axisY);
  }

  // Connect all markers to handler
  foreach (QtCharts::QLegendMarker* marker, this->mpChart->legend()->markers()) {
      // Disconnect possible existing connection to avoid multiple connections
      QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
      QObject::connect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }
  showLegend(this->SettingShowLegend);
//  auto markerSeries = mMarkers.at(0);
//  if(markerSeries!= NULL)
//  {
//    markerSeries->clear();
//    markerSeries->attachAxis(axisX);
//    markerSeries->attachAxis(axisY);
//    markerSeries->append(-0.01, -0.01);
//  }
}


void cedar::aux::gui::Qt5LinePlot::handleMarkerClicked()
{
  QtCharts::QLegendMarker* marker = qobject_cast<QtCharts::QLegendMarker*> (sender());
  Q_ASSERT(marker);

  switch (marker->type())
  {
    case QtCharts::QLegendMarker::LegendMarkerTypeXY:
    {
      // Toggle visibility of series
      marker->series()->setVisible(!marker->series()->isVisible());

      // Turn legend marker back to visible, since hiding series also hides the marker
      // and we don't want it to happen now.
      marker->setVisible(true);

      // Dim the marker, if series is not visible
      qreal alpha = 1.0;

      if (!marker->series()->isVisible()) {
        alpha = 0.5;
      }

      QColor color;
      QBrush brush = marker->labelBrush();
      color = brush.color();
      color.setAlphaF(alpha);
      brush.setColor(color);
      marker->setLabelBrush(brush);

      brush = marker->brush();
      color = brush.color();
      color.setAlphaF(alpha);
      brush.setColor(color);
      marker->setBrush(brush);

      QPen pen = marker->pen();
      color = pen.color();
      color.setAlphaF(alpha);
      pen.setColor(color);
      marker->setPen(pen);

      break;
    }
    default:
    {
      qDebug() << "Unknown marker type";
      break;
    }
  }

  for(unsigned int i = 0 ; i<mMarkers.size();i++)
  {
    if(this->mPlotSeriesDataVector.size() > i)
    {
      this->mMarkers.at(i)->setVisible(this->mPlotSeriesDataVector.at(i)->mQLineSeries->isVisible());
    }
  }
  showLegend(this->SettingShowLegend);
}

#endif // CEDAR_USE_QT5_PLOTS