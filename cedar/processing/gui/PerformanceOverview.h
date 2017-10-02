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

    File:        PerformanceOverview.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 01 21

    Description: Header file for the class cedar::proc::gui::PerformanceOverview.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_PERFORMANCE_OVERVIEW_H
#define CEDAR_PROC_GUI_PERFORMANCE_OVERVIEW_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ui_PerformanceOverview.h"
#include "cedar/units/Time.h"

// FORWARD DECLARATIONS
#include "cedar/processing/Group.fwd.h"
#include "cedar/processing/gui/PerformanceOverview.fwd.h"
#include "cedar/processing/Step.fwd.h"

// SYSTEM INCLUDES
#include <QDialog>


/*!@brief A widget that shows the performance of an architecture.
 */
class cedar::proc::gui::PerformanceOverview : public QDialog, public Ui_PerformanceOverview
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  class TimeCellItem;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  PerformanceOverview(QWidget* pParent = NULL);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! set a group to monitor
  void setGroup(cedar::proc::GroupPtr group);

public slots:
  //! refresh all measurements
  void refresh();

  //! react to a change in auto refresh
  void autoRefreshToggled(bool enabled);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! the timer event for auto refresh
  void timerEvent(QTimerEvent*);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void addGroup(cedar::proc::ConstGroupPtr group);

  void addStepRow(cedar::proc::ConstStepPtr step);

  void clear();

  void addMeasurement(cedar::unit::Time measurement, int row, int column, bool isRunning);

  void addUnAvailableMeasurement(int row, int column);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::proc::GroupPtr mGroup;

  int mTimerId;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::PerformanceOverview

#endif // CEDAR_PROC_GUI_PERFORMANCE_OVERVIEW_H

