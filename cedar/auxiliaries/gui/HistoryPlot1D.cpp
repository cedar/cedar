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

    File:        HistoryPlot1D.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 01 25

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/HistoryPlot1D.h"
#ifdef CEDAR_USE_QWTPLOT3D
  #include "cedar/auxiliaries/gui/QwtSurfacePlot.h"
#else // CEDAR_USE_QWTPLOT3D
  #include "cedar/auxiliaries/gui/ImagePlot.h"
#endif // CEDAR_USE_QWTPLOT3D
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/annotation/Dimensions.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::HistoryPlot1D::HistoryPlot1D(QWidget *pParent)
:
cedar::aux::gui::PlotInterface(pParent),
mHistory(new cedar::aux::MatData(cv::Mat())),
mMaxHistSize(150)
{
  this->init();
}

cedar::aux::gui::HistoryPlot1D::HistoryPlot1D(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget *pParent)
:
cedar::aux::gui::PlotInterface(pParent),
mHistory(new cedar::aux::MatData(cv::Mat())),
mMaxHistSize(150)
{
  this->init();
  this->plot(matData, title);
}

void cedar::aux::gui::HistoryPlot1D::init()
{
  auto p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);
#ifdef CEDAR_USE_QWTPLOT3D
  this->mpHistoryPlot = new cedar::aux::gui::QwtSurfacePlot();
#else // CEDAR_USE_QWTPLOT3D
  this->mpHistoryPlot = new cedar::aux::gui::ImagePlot();
#endif // CEDAR_USE_QWTPLOT3D
  p_layout->addWidget(this->mpHistoryPlot);

  cedar::aux::annotation::DimensionsPtr time_annotation(new cedar::aux::annotation::Dimensions(2));
  time_annotation->setLabel(0, "");
  time_annotation->setLabel(1, "time");
  this->mHistory->setAnnotation(time_annotation);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::HistoryPlot1D::plot(cedar::aux::ConstDataPtr data, const std::string& title)
{
  this->mData = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);

  if (!this->mData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData, "Cannot plot data that is not a matrix.");
  }

  if (this->mData->getDimensionality() != 1)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData, "Cannot plot data with this dimension.");
  }

  this->resetHistory();

  this->mpHistoryPlot->plot(this->mHistory, title + " (history)");

  this->startTimer(100);
}

void cedar::aux::gui::HistoryPlot1D::timerEvent(QTimerEvent *)
{
  if (!this->isVisible())
  {
    return;
  }

  this->advanceHistory();
}

void cedar::aux::gui::HistoryPlot1D::resetHistory()
{
  QReadLocker locker(&this->mData->getLock());
  cv::Mat hist = this->mData->getData().clone();
  locker.unlock();

  int size = static_cast<int>(cedar::aux::math::get1DMatrixSize(hist));
  cv::Mat new_hist = cv::Mat::zeros(size, this->mMaxHistSize, hist.type());
  if (hist.rows == 1)
  {
    hist = hist.t();
  }
  cv::Mat slice = new_hist(cv::Range::all(), cv::Range(this->mMaxHistSize - 1, this->mMaxHistSize));
  hist.copyTo(slice);

  QWriteLocker hlocker(&this->mHistory->getLock());
  this->mHistory->setData(new_hist);
  hlocker.unlock();
}


void cedar::aux::gui::HistoryPlot1D::advanceHistory()
{
  QReadLocker data_locker(&this->mData->getLock());

  if (this->mData->getDimensionality() != 1)
  {
    data_locker.unlock();
    emit dataChanged();
    return;
  }

  cv::Mat now = this->mData->getData().clone();
  data_locker.unlock();

  QWriteLocker hist_locker(&this->mHistory->getLock());
  const cv::Mat& current_hist = this->mHistory->getData();

  if (cedar::aux::math::get1DMatrixSize(now) != static_cast<unsigned int>(current_hist.rows))
  {
    hist_locker.unlock();
    this->resetHistory();
    return;
  }

  cv::Mat new_hist
     = cv::Mat::zeros
       (
         current_hist.rows,
         std::min(current_hist.cols + 1, this->mMaxHistSize),
         now.type()
       );

  if (now.rows == 1)
  {
    now = now.t();
  }

  int start = 0;
  int end = current_hist.cols;

  if (current_hist.cols >= this->mMaxHistSize)
  {
    start = 1;
  }

  cv::Mat slice = new_hist(cv::Range::all(), cv::Range(0, end - start));
  current_hist(cv::Range::all(), cv::Range(start, end)).copyTo(slice);
  cv::Mat slice2 = new_hist(cv::Range::all(), cv::Range(new_hist.cols - 1, new_hist.cols));
  now.copyTo(slice2);

  this->mHistory->setData(new_hist);
}
