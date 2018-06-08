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

    File:        PerformanceOverview.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 01 21

    Description: Source file for the class cedar::proc::gui::PerformanceOverview.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/PerformanceOverview.h"
#include "cedar/processing/sources/GroupSource.h"
#include "cedar/processing/sinks/GroupSink.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/Step.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <limits.h>

//----------------------------------------------------------------------------------------------------------------------
// private nested classes
//----------------------------------------------------------------------------------------------------------------------

//!@cond SKIPPED_DOCUMENTATION
class cedar::proc::gui::PerformanceOverview::TimeCellItem : public QTableWidgetItem
{
public:
  // constructor for an existing measurement
  TimeCellItem(cedar::unit::Time time, bool isRunning)
  :
  mIsRunning(isRunning)
  {
    double run_time_number = time / cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::seconds);
    this->setData(Qt::DisplayRole, QString("%1 ms").arg(run_time_number, 0, 'f', 1));
    this->setData(Qt::UserRole, run_time_number);
    this->setTextAlignment(Qt::AlignRight);

    this->applyStyle();
  }

  // constructor for an invalid measurement
  TimeCellItem()
  :
  mIsRunning(false)
  {
    this->setData(Qt::DisplayRole, "n/a");
    this->setData(Qt::UserRole, -1.0);

    this->applyStyle();
  }

  bool operator <(const QTableWidgetItem& other) const
  {
    try
    {
      const auto& other_tc = dynamic_cast<const TimeCellItem&>(other);
      if (this->mIsRunning && !other_tc.mIsRunning)
      {
        return false;
      }
      if (!this->mIsRunning && other_tc.mIsRunning)
      {
        return true;
      }
    }
    catch (std::bad_cast)
    {
      // could not cast -- ok
    }

    double this_value = this->data(Qt::UserRole).toDouble();
    bool ok;
    double other_value = other.data(Qt::UserRole).toDouble(&ok);

    if (ok)
    {
      return this_value < other_value;
    }
    else
    {
      return false;
    }
  }

private:
  void applyStyle()
  {
    this->setTextAlignment(this->textAlignment() | Qt::AlignVCenter);

    if (!this->mIsRunning)
    {
      this->setBackgroundColor(Qt::lightGray);
    }
  }

private:
  bool mIsRunning;
};
//!@endcond

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::PerformanceOverview::PerformanceOverview(QWidget* pParent)
:
QDialog(pParent),
mTimerId(0)
{
  setupUi(this);
  QObject::connect(this->mpRefreshButton, SIGNAL(clicked()), this, SLOT(refresh()));

  // make the first section (step name) stretch
#ifdef CEDAR_USE_QT5
  this->mpStepTimeOverview->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
#else
  this->mpStepTimeOverview->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
#endif
  // sort everything by the compute time (second column)
  this->mpStepTimeOverview->sortByColumn(1);

  this->autoRefreshToggled(this->mpAutoRefresh->isChecked());

  QObject::connect(this->mpAutoRefresh, SIGNAL(toggled(bool)), this, SLOT(autoRefreshToggled(bool)));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::PerformanceOverview::setGroup(cedar::proc::GroupPtr group)
{
  this->mGroup = group;
}

void cedar::proc::gui::PerformanceOverview::refresh()
{
  this->clear();

  if (!this->mGroup)
  {
    return;
  }

  this->addGroup(this->mGroup);
}

void cedar::proc::gui::PerformanceOverview::addGroup(cedar::proc::ConstGroupPtr group)
{
  for (const auto& name_element_pair : group->getElements())
  {
    auto element = name_element_pair.second;

    if (auto step = boost::dynamic_pointer_cast<cedar::proc::ConstStep>(element))
    {
      this->addStepRow(step);
    }
    else if (auto subgroup = boost::dynamic_pointer_cast<cedar::proc::ConstGroup>(element))
    {
      this->addGroup(subgroup);
    }
  }
}

void cedar::proc::gui::PerformanceOverview::addStepRow(cedar::proc::ConstStepPtr step)
{
  //!@todo This is a quickfix. A virtual function, isPerformanceRelevant (name?) in Step would be a better solution.
  if (boost::dynamic_pointer_cast<cedar::proc::sources::ConstGroupSource>(step)
      || boost::dynamic_pointer_cast<cedar::proc::sinks::ConstGroupSink>(step))
  {
    // group sources and sinks are ignored as they are not displayed to the user
    return;
  }

  int row = this->mpStepTimeOverview->rowCount();
  this->mpStepTimeOverview->setRowCount(row + 1);

  auto p_name = new QTableWidgetItem(QString::fromStdString(this->mGroup->findPath(step)));
  this->mpStepTimeOverview->setItem(row, 0, p_name);


  double steps_missed= step->getNumberOfStepsMissed();
  auto p_number = new QTableWidgetItem(
                        QString::number(
                          static_cast<unsigned long>(steps_missed) ) );
  this->mpStepTimeOverview->setItem(row,
                                    1, 
                                    p_number );

  if (!step->isStarted())
  {
    p_name->setBackgroundColor(Qt::lightGray);
    p_number->setBackgroundColor(Qt::lightGray);
  }


  // we have to use the row of the name item below as sorting may move it around
  if (step->hasRunTimeMeasurement())
  {
    this->addMeasurement(step->getRunTimeAverage(), p_name->row(), 2, step->isStarted());
  }
  else
  {
    this->addUnAvailableMeasurement(p_name->row(), 2);
  }

  if (step->hasRoundTimeMeasurement() && step->isStarted()) // steps that aren't started don't have proper round times
  {
    this->addMeasurement(step->getRoundTimeAverage(), p_name->row(), 3, step->isStarted());
  }
  else
  {
    this->addUnAvailableMeasurement(p_name->row(), 3);
  }

  if (step->hasLockTimeMeasurement())
  {
    this->addMeasurement(step->getLockTimeAverage(), p_name->row(), 4, step->isStarted());
  }
  else
  {
    this->addUnAvailableMeasurement(p_name->row(), 4);
  }
}

void cedar::proc::gui::PerformanceOverview::addUnAvailableMeasurement(int row, int column)
{
  this->mpStepTimeOverview->setItem(row, column, new cedar::proc::gui::PerformanceOverview::TimeCellItem());
}

void cedar::proc::gui::PerformanceOverview::addMeasurement(cedar::unit::Time measurement, int row, int column, bool isRunning)
{
  this->mpStepTimeOverview->setItem
  (
    row,
    column,
    new cedar::proc::gui::PerformanceOverview::TimeCellItem(measurement, isRunning)
  );
}

void cedar::proc::gui::PerformanceOverview::clear()
{
  while (this->mpStepTimeOverview->rowCount() > 0)
  {
    this->mpStepTimeOverview->removeRow(0);
  }
}

void cedar::proc::gui::PerformanceOverview::autoRefreshToggled(bool enabled)
{
  if (enabled)
  {
    if (this->mTimerId == 0)
    {
      this->mTimerId = this->startTimer(500); // update once every 0.5 seconds
    }
  }
  else
  {
    if (mTimerId != 0)
    {
      this->killTimer(mTimerId);
      mTimerId = 0;
    }
  }
}

void cedar::proc::gui::PerformanceOverview::timerEvent(QTimerEvent*)
{
  if (this->isVisible() && this->mpAutoRefresh->isChecked())
  {
    this->refresh();
  }
}
