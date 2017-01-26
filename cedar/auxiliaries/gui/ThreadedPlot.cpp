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
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <boost/make_shared.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::ThreadedPlot::ThreadedPlot(QWidget* pParent)
:
cedar::aux::gui::PlotInterface(pParent),
mTimerId(0),
mCaller(boost::bind(&cedar::aux::gui::ThreadedPlot::convert, this))
{
  QObject::connect(this, SIGNAL(conversionDoneSignal()), this, SLOT(conversionDone()), Qt::QueuedConnection);
  QObject::connect(this, SIGNAL(conversionFailedSignal()), this, SLOT(conversionFailed()), Qt::QueuedConnection);

  mCaller.start();
}

cedar::aux::gui::ThreadedPlot::~ThreadedPlot()
{
  // stop the timer (the thread itself will stop automatically)
  this->stop();

  // preemptively disconnect all slots so that no new events are posted to this class
  QObject::disconnect(this);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

// Called in the worker thread
void cedar::aux::gui::ThreadedPlot::convert()
{
  // make sure this is NOT called in the main (gui) thread
  CEDAR_DEBUG_NON_CRITICAL_ASSERT(QApplication::instance()->thread() != QThread::currentThread());

  if (this->doConversion())
  {
    emit this->conversionDoneSignal();
  }
  else
  {
    emit this->conversionFailedSignal();
  }
}

// called in the main gui thread
void cedar::aux::gui::ThreadedPlot::conversionDone()
{
  // make sure this IS called in the main (gui) thread
  CEDAR_DEBUG_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

  // update the display in the underlying plots
  this->updatePlot();
}

void cedar::aux::gui::ThreadedPlot::conversionFailed()
{
  // make sure this IS called in the main (gui) thread
  CEDAR_DEBUG_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
  // nothing else to do at the moment
}

void cedar::aux::gui::ThreadedPlot::timerEvent(QTimerEvent* /*pEvent*/)
{
  if (!this->isVisible()) // plot widget is not visible -- no need to plot
  {
    return;
  }

  if (!this->mCaller.isExecuting())
  {
    this->mCaller.execute();
  }
}

void cedar::aux::gui::ThreadedPlot::start()
{
  // start the timer anyway
  this->mTimerId = this->startTimer(70);
  CEDAR_DEBUG_ASSERT(mTimerId != 0);
}

void cedar::aux::gui::ThreadedPlot::stop()
{
  if (this->mTimerId != 0)
  {
    this->killTimer(mTimerId);
    this->mTimerId = 0;
  }
}

void cedar::aux::gui::ThreadedPlot::wait()
{
  CEDAR_NON_CRITICAL_ASSERT(this->mTimerId == 0);

  while (this->mCaller.isExecuting())
  {
    cedar::aux::sleep(cedar::unit::Time(10.0 * cedar::unit::milli * cedar::unit::seconds));
  }
}
