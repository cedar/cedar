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

    File:        ExperimentDialog.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2014 01 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_EXPERIMENT_DIALOG_H
#define CEDAR_PROC_GUI_EXPERIMENT_DIALOG_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ui_ExperimentDialog.h"
#include "cedar/processing/experiment/Experiment.h"
#include "cedar/processing/gui/Ide.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/ExperimentDialog.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/signals2.hpp>
#endif
#include <QTimer>
#include <map>


/*!@brief A widget for setting up, save and load experiments
 *
 * @todo Rather than store a pointer to the Ide, this should just have a pointer to the group which gets updated by the Ide.
 */
class cedar::proc::gui::ExperimentDialog : public QWidget, public Ui_ExperimentDialog
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
  ExperimentDialog(cedar::proc::gui::Ide* parent);

  //!@brief The destructor.
  ~ExperimentDialog();


  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! access the experiment
  cedar::proc::experiment::ExperimentPtr getExperiment();

  //! Updates the group stored in the experiment. Must be called whenever the group of the Ide changes.
  //!@todo See comment on group (don't store pointer to ide); replace this by a setGroup function.
  void updateGroup();

public slots:
  //!@brief Redraws the action sequences of the widget
  void redraw();
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Removes all ActionSequences from the widget
  void clearActionSequences();

  //! Enables/disables all action widgets.
  void setActionSequenceWidgetsEnabled(bool enabled);

  void setExperiment(cedar::proc::experiment::ExperimentPtr experiment);

private slots:
  //!@brief Creates a new experiment
  void createNewExperiment();

  //!@brief Saves the experiment
  void save();

  //!@brief Saves the experiment to a new file
  void saveAs();

  //!@brief Loads the experiment
  void load();

  //!@brief Changes the name of the experiment
  void nameChanged();

  //!@brief Adds a new action sequence to the experiments
  void addActionSequence();

  //!@brief Sets the number of trials of the experiment
  void trialChanged();

  //!@brief Starts the experiment
  void runExperiment();

  //!@brief Stops the experiment
   void stopExperiment();

  //!@brief Tells the GUI when the experiment has been stopped
  void experimentRunning(bool status);

  //!@brief Tells the GUI when the actual trial number has changed
  void trialNumberChanged(int number);

  //!@brief Updates the group time in the GUI
  void timeUpdate();

  //! Slot for when the user checks the repeat slot.
  void repeatChecked(bool);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief The parent IDE
  cedar::proc::gui::Ide* mParent;

  //!@brief The pointer to the experiment
  cedar::proc::experiment::ExperimentPtr mExperiment;

  //!@brief The timer to update the group time every second
  QTimer* mpGroupTime;

}; // class cedar::proc::gui::ExperimentDialog

#endif // CEDAR_PROC_GUI_EXPERIMENT_DIALOG_H

