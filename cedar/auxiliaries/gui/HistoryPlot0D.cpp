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
#include <QVBoxLayout>
#include <iostream>

// MACROS
// Enable to show information on locking/unlocking
// #define DEBUG_LOCKS

#ifdef DEBUG_LOGS
#  include "auxiliaries/System.h"
#endif

//!@todo This class and the MatrixPlot1D share a lot of common code -- unify them

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::gui::HistoryPlot0D::HistoryPlot0D(QWidget *pParent)
:
cedar::aux::gui::PlotInterface(pParent),
mpCurrentPlotWidget(NULL)
{
  this->init();
}

cedar::aux::gui::HistoryPlot0D::HistoryPlot0D(cedar::aux::DataPtr data, const std::string& title, QWidget *pParent)
:
cedar::aux::gui::PlotInterface(pParent),
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
void cedar::aux::gui::HistoryPlot0D::init()
{
  mMaxHistorySize = 500;
  mpCurve = NULL;

  mXArray.resize(mMaxHistorySize);
  mYArray.resize(mMaxHistorySize);

  QPalette palette = this->palette();
  palette.setColor(QPalette::Window, Qt::white);
  this->setPalette(palette);

  QVBoxLayout *p_layout = new QVBoxLayout();
  this->setLayout(p_layout);

  mpPlot = new QwtPlot(this);
  this->layout()->addWidget(mpPlot);
}

void cedar::aux::gui::HistoryPlot0D::plot(cedar::aux::DataPtr data, const std::string& /* title */)
{
  this->mData = data;
  this->mDoubleData = boost::shared_dynamic_cast<cedar::aux::DoubleData>(this->mData);

  if (!this->mDoubleData)
  {
    this->mMatData = boost::shared_dynamic_cast<cedar::aux::MatData>(this->mData);

    if (!this->mMatData)
    {
      CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                  "Could not cast to cedar::aux::DoubleData or cedar::aux::MatData in cedar::aux::gui::HistoryPlot0D::plot.");
    }
  }

  if (this->mpCurve != NULL)
  {
    delete this->mpCurve;
  }

  this->mpCurve = new QwtPlotCurve("data");
//  this->setPlotStyle(this->mpCurve);

  data->lockForRead();
  double val;
  if (this->mDoubleData)
  {
    val = this->mDoubleData->getData();
  }
  else
  {
    cv::Mat matrix = this->mMatData->getData();
    if (matrix.type() == CV_32F)
    {
      val = matrix.at<float>(0,0);
    }
    else if (matrix.type() == CV_64F)
    {
      val = matrix.at<double>(0,0);
    }
    else
    {
      CEDAR_THROW(cedar::aux::UnknownTypeException, "Unhandled matrix type in cedar::aux::gui::HistoryPlot0D::display.");
    }
  }
  data->unlock();
  mpXValues.clear();
  mpYValues.clear();

  mpXValues.push_back(0);
  mpYValues.push_back(val);

  this->mpCurve->setData(&this->mpXValues.at(0),
                         &this->mpYValues.at(0),
                         1);

  this->mpCurve->attach(this->mpPlot);

  this->startTimer(30); //!@todo make the refresh time configurable.
}

void cedar::aux::gui::HistoryPlot0D::timerEvent(QTimerEvent * /* pEvent */)
{
  if (!this->isVisible())
  {
    return;
  }

#ifdef DEBUG_LOCKS
  cedar::aux::System::mCOutLock.lockForWrite();
  std::cout << "readlocking lock " << (&this->mData->getLock()) << std::endl;
  cedar::aux::System::mCOutLock.unlock();
#endif // DEBUG_LOCKS
  this->mData->lockForRead();

#ifdef DEBUG_LOCKS
  cedar::aux::System::mCOutLock.lockForWrite();
  std::cout << "readlocked " << (&this->mData->getLock()) << std::endl;
  cedar::aux::System::mCOutLock.unlock();
#endif // DEBUG_LOCKS

  double val;
  if (this->mDoubleData)
  {
    val = this->mDoubleData->getData();
  }
  else
  {
    cv::Mat matrix = this->mMatData->getData();
    if (cedar::aux::math::getDimensionalityOf(matrix) != 0) // plot is no longer capable of displaying the data
    {
      this->mData->unlock();
      emit dataChanged();
      return;
    }
    if (matrix.type() == CV_32F)
    {
      val = matrix.at<float>(0,0);
    }
    else if (matrix.type() == CV_64F)
    {
      val = matrix.at<double>(0,0);
    }
    else
    {
      CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot handle the type of the matrix.");
    }
  }
  mpYValues.push_back(val);

#ifdef DEBUG_LOCKS
  cedar::aux::System::mCOutLock.lockForWrite();
  std::cout << "unlocking lock " << (&this->mData->getLock()) << std::endl;
  cedar::aux::System::mCOutLock.unlock();
#endif // DEBUG_LOCKS
  this->mData->unlock();
  //!@todo: Use actual time measurements here
  mpXValues.push_back(mpXValues.back() + 1);

  while (mpXValues.size() > this->mMaxHistorySize)
  {
    mpXValues.pop_front();
    mpYValues.pop_front();
  }

  CEDAR_DEBUG_ASSERT(mpXValues.size() == mpYValues.size());
  CEDAR_DEBUG_ASSERT(mXArray.size() == mYArray.size());

  for (size_t i = 0; i < mpXValues.size(); ++i)
  {
    mXArray[i] = mpXValues[i];
    mYArray[i] = mpYValues[i];
  }

  this->mpCurve->setData(&this->mXArray.at(0),
                         &this->mYArray.at(0),
                         static_cast<int>(this->mpXValues.size()));
  this->mpPlot->replot();
}
