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

    File:        SimulationControl.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 10 21

    Description: Header file for the class cedar::proc::gui::SimulationControl.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_SIMULATION_CONTROL_H
#define CEDAR_PROC_GUI_SIMULATION_CONTROL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ui_SimulationControl.h"
#include "cedar/auxiliaries/LockableMember.h"

// FORWARD DECLARATIONS
#include "cedar/processing/Element.fwd.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/processing/LoopedTrigger.fwd.h"
#include "cedar/processing/gui/Group.fwd.h"
#include "cedar/processing/gui/SimulationControl.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QLabel>
#include <QFuture>
#include <QPushButton>
#ifndef Q_MOC_RUN
  #include <boost/signals2.hpp>
#endif // Q_MOC_RUN
#include <map>

//!@cond SKIPPED_DOCUMENTATION
// "Hidden" class; would be a private nested class of cedar::proc::gui::SimulationControl, but the moc cannot do that
namespace cedar
{
  namespace proc
  {
    namespace gui
    {
      namespace SimulationControlPrivate
      {
        /* A widget that can be used to start/stop a looped trigger. Also reacts to when the trigger is started from
         * elsewhere
         */
        class TriggerControlWidget : public QWidget
        {
          Q_OBJECT

        public:
          TriggerControlWidget(cedar::proc::LoopedTriggerPtr loopedTrigger);

        private slots:
          void startStopTriggerClicked();

          void triggerStarted();

          void triggerStopped();

          void triggerChangingState();

          void singleStepClicked();

        private:
          cedar::proc::LoopedTriggerPtr mLoopedTrigger;

          QPushButton* mpStartStopButton;

          QPushButton* mpSingleStepButton;
        };
      }
    }
  }
}
//!@endcond

/*!@brief A widget for controlling the simulation of a cedar::proc::Group.
 */
class cedar::proc::gui::SimulationControl : public QWidget, public Ui_SimulationControl
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  SimulationControl();

  //! The destructor.
  ~SimulationControl();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Sets the group whose simulation is controlled by this widget.
  void setGroup(cedar::proc::gui::GroupPtr group);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! Periodically updates the simulation quality displays.
  void timerEvent(QTimerEvent* pEvent);

signals:
  void signalTriggerCountChanged(QString path);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void elementAdded(QTreeWidgetItem* pItem, cedar::proc::ElementPtr element);
  void elementRemoved(QTreeWidgetItem* pItem, cedar::proc::ElementPtr element);

  void loopedTriggerAdded(QTreeWidgetItem* pItem, cedar::proc::LoopedTriggerPtr loopedTrigger);
  void loopedTriggerRemoved(QTreeWidgetItem* pItem, cedar::proc::LoopedTriggerPtr loopedTrigger);

  void updateSimulationRunningIcon(bool running);

  QWidget* getColorWidget(QTreeWidgetItem* pItem, int column = 0);

  QWidget* getQualityWidget(QTreeWidgetItem* pItem);

  cedar::proc::LoopedTriggerPtr getItemTrigger(QTreeWidgetItem* pItem);

  void updateItemTriggerColor(QTreeWidgetItem* pItem);

  void updateItemColorWidgetColor(QWidget* pColorWidget, cedar::proc::LoopedTriggerPtr loopedTrigger);

  void updateItemQualityWidget(QWidget* pQualityWidget, cedar::proc::LoopedTriggerPtr trigger);

  void sortItems();

  void updateTriggerQualities();

  QLabel* addColorWidget(QTreeWidgetItem* pItem, int column);

  void applyBrushToColorWidget(QWidget* pWidget, const QBrush& brush);

  void updateTriggerConnectionCount(cedar::proc::LoopedTriggerPtr trigger);

  void translateTriggerCountChangedSignal(size_t, cedar::proc::LoopedTriggerPtr trigger);

private slots:
  void startPauseSimulationClicked();

  void triggerStarted();

  void allTriggersStopped();

  void createClicked();

  void removeClicked();

  void updateAllTriggerColors();

  void elementNameChanged();

  void triggerCountChanged(QString triggerPath);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::proc::gui::GroupPtr mGroup;

  boost::signals2::scoped_connection mElementAddedConnection;

  std::map<cedar::proc::LoopedTriggerPtr, boost::shared_ptr<boost::signals2::scoped_connection> > mTriggerCountChangedConnections;

  cedar::aux::LockableMember<bool> mSimulationRunning;

  QFuture<void> mStartSimulationResult;

  QFuture<void> mStopSimulationResult;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Path to the icon that indicates that the simulation is started.
  static QString M_STARTED_ICON_PATH;

  //! Path to the icon that indicates that the simulation is stopped.
  static QString M_PAUSED_ICON_PATH;

  //! The column containing the color widget.
  static const int M_COLUMN_COLOR_WIDGET;

  //! The column containing the quality indicator.
  static const int M_COLUMN_QUALITY_INDICATOR;

  //! The column containing the number of connections of the trigger.
  static const int M_COLUMN_CONNECTION_COUNT;

  //! The column from which the parameters start.
  static const int M_COLUMN_PARAMETERS_START;

protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::SimulationControl

#endif // CEDAR_PROC_GUI_SIMULATION_CONTROL_H
