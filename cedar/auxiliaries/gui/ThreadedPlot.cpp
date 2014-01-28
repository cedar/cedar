/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ThreadedPlot.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 01 25

    Description: Source file for the class cedar::aux::gui::ThreadedPlot.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/ThreadedPlot.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <boost/make_shared.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::ThreadedPlot::ThreadedPlot(QWidget* pParent)
:
cedar::aux::gui::PlotInterface(pParent),
mpWorkerThread(new QThread()),
mConverting(false),
mTimerId(0)
{
  this->mWorker = boost::make_shared<cedar::aux::gui::detail::ThreadedPlotWorker>(this);
  this->mWorker->moveToThread(this->mpWorkerThread);
  
  QObject::connect(this, SIGNAL(convert()), this->mWorker.get(), SLOT(convert()));
  QObject::connect(this->mWorker.get(), SIGNAL(conversionDone()), this, SLOT(conversionDone()));
  QObject::connect(this->mWorker.get(), SIGNAL(conversionFailed()), this, SLOT(conversionFailed()));

  this->mpWorkerThread->start(QThread::LowPriority);
}

cedar::aux::gui::ThreadedPlot::~ThreadedPlot()
{
  if (this->mpWorkerThread)
  {
    this->mpWorkerThread->quit();
    this->mpWorkerThread->wait();
    delete this->mpWorkerThread;
    this->mpWorkerThread = nullptr;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::ThreadedPlot::startConversion()
{
  emit this->convert();
}

void cedar::aux::gui::detail::ThreadedPlotWorker::convert()
{
  if (this->mpPlot->doConversion())
  {
    emit this->conversionDone();
  }
  else
  {
    emit this->conversionFailed();
  }
}

void cedar::aux::gui::ThreadedPlot::conversionDone()
{
  this->updatePlot();

  this->mConverting = false;
}

void cedar::aux::gui::ThreadedPlot::conversionFailed()
{
  this->mConverting = false;
}

void cedar::aux::gui::ThreadedPlot::timerEvent(QTimerEvent* /*pEvent*/)
{
  if
  (
    !this->isVisible() // plot widget is not visible -- no need to plot
    || this->mConverting // already converting -- skip this timer event
  )
  {
    return;
  }

  this->mConverting = true;
  emit convert();
}

void cedar::aux::gui::ThreadedPlot::start()
{
  // start the timer anyway
  this->mTimerId = this->startTimer(70);
  CEDAR_DEBUG_ASSERT(mTimerId != 0);
}

void cedar::aux::gui::ThreadedPlot::stop()
{
  if (mTimerId != 0)
  {
    this->killTimer(mTimerId);
  }
}