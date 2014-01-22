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
#include "cedar/processing/Network.h"
#include "cedar/processing/Step.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <limits.h>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::PerformanceOverview::PerformanceOverview(QWidget* pParent)
:
QWidget(pParent)
{
  setupUi(this);
  QObject::connect(this->mpRefreshButton, SIGNAL(clicked()), this, SLOT(refresh()));

  // make the first section (step name) stretch
  this->mpStepTimeOverview->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);

  // sort everything by the compute time (second column)
  this->mpStepTimeOverview->sortByColumn(1);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::PerformanceOverview::setNetwork(cedar::proc::NetworkPtr network)
{
  this->mNetwork = network;
}

void cedar::proc::gui::PerformanceOverview::refresh()
{
  this->clear();

  if (!this->mNetwork)
  {
    return;
  }

  for (const auto& name_element_pair : this->mNetwork->getElements())
  {
    auto element = name_element_pair.second;

    if (auto step = boost::dynamic_pointer_cast<cedar::proc::ConstStep>(element))
    {
      this->addStepRow(step);
    }
  }
}

void cedar::proc::gui::PerformanceOverview::addStepRow(cedar::proc::ConstStepPtr step)
{
  int row = this->mpStepTimeOverview->rowCount();
  this->mpStepTimeOverview->setRowCount(row + 1);

  auto p_name = new QTableWidgetItem(QString::fromStdString(step->getName()));
  this->mpStepTimeOverview->setItem(row, 0, p_name);

  if (step->hasRunTimeMeasurement())
  {
    this->addMeasurement(step->getRunTimeAverage(), row, 1);
  }
  else
  {
    this->addUnAvailableMeasurement(row, 1);
  }

  if (step->hasRoundTimeMeasurement())
  {
    this->addMeasurement(step->getRoundTimeAverage(), row, 2);
  }
  else
  {
    this->addUnAvailableMeasurement(row, 2);
  }

  if (step->hasLockTimeMeasurement())
  {
    this->addMeasurement(step->getLockTimeAverage(), row, 3);
  }
  else
  {
    this->addUnAvailableMeasurement(row, 3);
  }
}

void cedar::proc::gui::PerformanceOverview::addUnAvailableMeasurement(int row, int column)
{
  auto p_run_time_measurement = new QTableWidgetItem();
  p_run_time_measurement->setData(Qt::DisplayRole, "n/a");
  p_run_time_measurement->setData(Qt::UserRole, -1.0); // -1.0 should get sorted behind everything else
  this->mpStepTimeOverview->setItem(row, column, p_run_time_measurement);
}

void cedar::proc::gui::PerformanceOverview::addMeasurement(cedar::unit::Time measurement, int row, int column)
{
  QString run_time_str;
  double run_time_number = measurement / cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::seconds);
  run_time_str = QString("%1 ms").arg(run_time_number);

  auto p_run_time_measurement = new QTableWidgetItem();
  p_run_time_measurement->setData(Qt::DisplayRole, run_time_str);
  p_run_time_measurement->setData(Qt::UserRole, run_time_number);
  this->mpStepTimeOverview->setItem(row, column, p_run_time_measurement);
}

void cedar::proc::gui::PerformanceOverview::clear()
{
  this->mpStepTimeOverview->setRowCount(0);
}
