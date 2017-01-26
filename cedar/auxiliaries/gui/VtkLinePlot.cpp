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

    File:        VtkLinePlot.cpp

    Maintainer:  Kai Kuchenbecker
    Email:       Kai.Kuchenbecker@ini.ruhr-uni-bochum.de
    Date:        2012 11 07

    Description: 

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"

#ifdef CEDAR_USE_VTK

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/VtkLinePlot.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/gui/PlotManager.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <QContextMenuEvent>
#include <QVBoxLayout>
#include <QPalette>
#include <QMenu>
#include <QThread>
#include <QMessageBox>
#include <iostream>
#include <sstream>


//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

std::vector<cedar::aux::gui::VtkLinePlot::LineColor> cedar::aux::gui::VtkLinePlot::mLineColors;
std::vector<int> cedar::aux::gui::VtkLinePlot::mLineStyles;

//! VtkPen Line Types
const int cedar::aux::gui::VtkLinePlot::SOLID_LINE = 1;
const int cedar::aux::gui::VtkLinePlot::DASHED_LINE = 2;
const int cedar::aux::gui::VtkLinePlot::DOTTED_LINE = 3;
const int cedar::aux::gui::VtkLinePlot::DOT_DASHED_LINE = 4;
const int cedar::aux::gui::VtkLinePlot::DOT_DASH_DASHED_LINE = 5;


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::VtkLinePlot::VtkLinePlot(QWidget* pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpLock(new QReadWriteLock())
{
  this->init();
}

cedar::aux::gui::VtkLinePlot::VtkLinePlot(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget* pParent)
:
cedar::aux::gui::MultiPlotInterface(pParent),
mpLock(new QReadWriteLock())
{
  this->init();
  this->plot(matData, title);
}

cedar::aux::gui::VtkLinePlot::~VtkLinePlot()
{
  if (this->mpWorkerThread)
  {
    this->mpWorkerThread->quit();
    this->mpWorkerThread->wait();
    delete this->mpWorkerThread;
    this->mpWorkerThread = nullptr;
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

void cedar::aux::gui::VtkLinePlot::init()
{
  QPalette palette = this->palette();
  palette.setColor(QPalette::Window, Qt::white);
  this->setPalette(palette);

  QVBoxLayout* p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);

  mpVtkWidget = new QVTKWidget;
  this->mpView = vtkSmartPointer<vtkContextView>::New();
  this->mpView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  this->mpView->SetInteractor(mpVtkWidget->GetInteractor());
  mpVtkWidget->SetRenderWindow(this->mpView->GetRenderWindow());
  mpChart = vtkSmartPointer<vtkChartXY>::New();
  this->mpView->GetScene()->AddItem(mpChart);

  p_layout->addWidget(mpVtkWidget);
  
  //initialize dataTable
  mpVtkTable = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkDoubleArray> xcol = vtkSmartPointer<vtkDoubleArray>::New();
  xcol->SetName("xcol");
  mpVtkTable->AddColumn(xcol);

  // intialize interactor
  //this->mpView->GetInteractor()->Initialize();

  mpWorkerThread = new QThread();

  mConversionWorker = cedar::aux::gui::detail::VtkLinePlotWorkerPtr(new cedar::aux::gui::detail::VtkLinePlotWorker(this));
  mConversionWorker->moveToThread(mpWorkerThread);

  this->mpWorkerThread->start(QThread::LowPriority);

  QObject::connect(mConversionWorker.get(), SIGNAL(dataChanged()), this,                    SIGNAL(dataChanged()));
  QObject::connect(this,                    SIGNAL(convert()),     mConversionWorker.get(), SLOT(convert()));
  QObject::connect(mConversionWorker.get(), SIGNAL(done()),        this,                    SLOT(conversionDone()));
}

void cedar::aux::gui::VtkLinePlot::plot(cedar::aux::ConstDataPtr data, const std::string& title)
{
  mpLock->lockForWrite();
  mPlotSeriesVector.clear();
  mpLock->unlock();

  this->append(data, title);
}

void cedar::aux::gui::VtkLinePlot::applyStyle(size_t lineId, vtkPlot* pCurve)
{
  // initialize vectors, if this has not happened, yet
  if (mLineColors.empty() || mLineStyles.empty())
  {
    mLineColors.clear();
    mLineStyles.clear();

    mLineColors.push_back(LineColor(0, 121, 177));
    mLineColors.push_back(LineColor(48, 162, 26));
    mLineColors.push_back(LineColor(224, 49, 37));
    mLineColors.push_back(LineColor(217, 220, 0));
    mLineColors.push_back(LineColor(118, 0, 177));
    mLineColors.push_back(LineColor(46, 221, 190));
    mLineColors.push_back(LineColor(245, 156, 16));

    mLineStyles.push_back(SOLID_LINE);
    mLineStyles.push_back(DASHED_LINE);
    mLineStyles.push_back(DOTTED_LINE);
    mLineStyles.push_back(DOT_DASHED_LINE);
    mLineStyles.push_back(DOT_DASH_DASHED_LINE);
  }

  const size_t color_count = mLineColors.size();
  const size_t style_count = mLineStyles.size();
  const size_t max_line_id = mLineStyles.size() * mLineColors.size();

  size_t line_id = lineId % max_line_id;
  // first change the colors for lines, but keep the style the same
  size_t color_id = line_id % color_count;
  // if all colors were used for the line style, increase the linestyle
  size_t style_id = (line_id / color_count) % style_count;

  // get old pen
  vtkSmartPointer<vtkPen> pen = pCurve->GetPen();

  // modify accordingly
  pen->SetColor(mLineColors.at(color_id).toA());
  pen->SetLineType(mLineStyles.at(style_id));
  pen->SetWidth(2.0);
}

bool cedar::aux::gui::VtkLinePlot::canAppend(cedar::aux::ConstDataPtr data) const
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
    || cedar::aux::math::get1DMatrixSize(matrix) != this->mpVtkTable->GetNumberOfRows())
  {
    return false;
  }

  return true;
}

void cedar::aux::gui::VtkLinePlot::doAppend(cedar::aux::ConstDataPtr data, const std::string&)
{
  PlotSeriesPtr plot_series(new PlotSeries(mpVtkTable, mpChart));

  size_t line_id = mPlotSeriesVector.size();
  plot_series->mXColumn = 0;
  std::stringstream ss;
  ss << "line #" << line_id;
  plot_series->mYColumnName = ss.str();
  auto yColumn = this->mpVtkTable->GetNumberOfColumns();

  QWriteLocker locker(mpLock);
  mPlotSeriesVector.push_back(plot_series);

  plot_series->mMatData = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  if (!plot_series->mMatData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Could not cast to cedar::aux::MatData in cedar::aux::gui::VtkLinePlot::doAppend.");
  }

  plot_series->mpCurve = this->mpChart->AddPlot(vtkChart::LINE);
  applyStyle(line_id, plot_series->mpCurve);

  size_t num;
  QReadLocker mat_locker(&data->getLock());
  const cv::Mat& mat = plot_series->mMatData->getData();
  if(mat.rows != 1 && mat.cols != 1)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Cannot plot data of this dimensionality with cedar::aux::gui::VtkLinePlot.");
  }

  CEDAR_DEBUG_ASSERT(cedar::aux::math::getDimensionalityOf(mat) == 1);
  num = cedar::aux::math::get1DMatrixSize(mat);
  mat_locker.unlock();

  // skip if the matrix is empty
  if (num == 0)
  {
    return;
  }
  vtkSmartPointer<vtkDoubleArray> y_arr = vtkSmartPointer<vtkDoubleArray>::New();
  y_arr->SetName(plot_series->mYColumnName.c_str());
  this->mpVtkTable->AddColumn(y_arr);

  plot_series->buildXAxis(num);
  CEDAR_DEBUG_ASSERT(static_cast<size_t>(this->mpVtkTable->GetNumberOfRows()) == num);

  // assert that column was added.
  CEDAR_DEBUG_ASSERT(this->mpVtkTable->GetColumn(yColumn) != nullptr);

#if VTK_MAJOR_VERSION <= 5
  plot_series->mpCurve->SetInput(this->mpVtkTable, plot_series->mXColumn, yColumn);
#else
  plot_series->mpCurve->SetInputData(this->mpVtkTable, plot_series->mXColumn, yColumn);
#endif

  locker.unlock();

  this->startTimer(30);
}

bool cedar::aux::gui::VtkLinePlot::canDetach(cedar::aux::ConstDataPtr data) const
{
  if(this->mPlotSeriesVector.size() > 1)
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

void cedar::aux::gui::VtkLinePlot::doDetach(cedar::aux::ConstDataPtr data)
{
  QWriteLocker locker(mpLock);
  for(auto it = mPlotSeriesVector.begin(); it != mPlotSeriesVector.end(); ++it)
  { 
    auto plot_series = *it;
    if(boost::dynamic_pointer_cast<cedar::aux::ConstData>(plot_series->mMatData) == data)
    {
      mPlotSeriesVector.erase(it);
      break;
    }
  }
  locker.unlock();
}

void cedar::aux::gui::VtkLinePlot::PlotSeries::buildXAxis(unsigned int new_size)
{
  vtkIdType old_size = this->mpVtkTable->GetNumberOfRows();
  this->mpVtkTable->SetNumberOfRows(new_size);
  for (unsigned int i = old_size; i < new_size; ++i)
  {
    this->mpVtkTable->SetValue(i, this->mXColumn, static_cast<double>(i));
  }
}

//!@cond SKIPPED_DOCUMENTATION
void cedar::aux::gui::detail::VtkLinePlotWorker::convert()
{
  QWriteLocker plot_locker(this->mpPlot->mpLock);
  for (size_t i = 0; i < this->mpPlot->mPlotSeriesVector.size(); ++i)
  {
    cedar::aux::gui::VtkLinePlot::PlotSeriesPtr series = this->mpPlot->mPlotSeriesVector.at(i);

    QReadLocker locker(&series->mMatData->getLock());
    const cv::Mat& mat = series->mMatData->getData();
    if (cedar::aux::math::getDimensionalityOf(mat) != 1) // plot is no longer capable of displaying the data
    {
      emit dataChanged();
      return;
    }

    // Check if the size of the matrix has changed
    unsigned int size = cedar::aux::math::get1DMatrixSize(mat);

    // skip if the array is empty
    if (size == 0)
    {
      return;
    }

    if (this->mpPlot->mpVtkTable->GetColumnByName(series->mYColumnName.c_str()) == nullptr)
    {
      vtkSmartPointer<vtkDoubleArray> y_arr = vtkSmartPointer<vtkDoubleArray>::New();
      y_arr->SetNumberOfValues(this->mpPlot->mpVtkTable->GetNumberOfRows());
      y_arr->SetName(series->mYColumnName.c_str());
      this->mpPlot->mpVtkTable->AddColumn(y_arr);
    }
    if (this->mpPlot->mpVtkTable->GetNumberOfRows() != size)
    {
      series->buildXAxis(size);
    }
    for (size_t j = 0; j < size; ++j)
    {
      this->mpPlot->mpVtkTable->SetValueByName(j, series->mYColumnName.c_str(),cedar::aux::math::getMatrixEntry<double>(mat, j));
    }
  }

  plot_locker.unlock();

  emit done();
}

void cedar::aux::gui::VtkLinePlot::conversionDone()
{
  QReadLocker locker(this->mpLock);
  this->mpVtkTable->Modified();
  this->mpChart->RecalculateBounds();
  this->mpView->Render();
}
//!@endcond

void cedar::aux::gui::VtkLinePlot::timerEvent(QTimerEvent * /* pEvent */)
{
  if (!this->isVisible())
  {
    return;
  }

  emit convert();
}

#endif
