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

    File:        StepItem.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 05

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEP_ITEM_H
#define CEDAR_PROC_STEP_ITEM_H

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/gui/Connectable.h"
#include "cedar/processing/gui/PlotWidget.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/EnumType.h"

// FORWARD DECLARATION
#include "cedar/auxiliaries/gui/PlotDeclaration.fwd.h"
#include "cedar/processing/gui/DataSlotItem.fwd.h"
#include "cedar/processing/gui/Group.fwd.h"
#include "cedar/processing/gui/PlotWidget.fwd.h"
#include "cedar/processing/gui/StepItem.fwd.h"
#include "cedar/processing/gui/TriggerItem.fwd.h"

// SYSTEM INCLUDES
#include <QMainWindow>
#include <QIcon>
#include <utility>
#include <vector>
#include <map>


/*!@brief A representation of a cedar::proc::Step object in a cedar::proc::gui::Scene.
 */
class cedar::proc::gui::StepItem : public cedar::proc::gui::Connectable
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::proc::gui::Group;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  StepItem(QMainWindow* pMainWindow);

  //!@brief constructor that directly associates a step to the graphical representation
  StepItem(cedar::proc::StepPtr step, QMainWindow* pMainWindow);

  //!@brief Destructor
  ~StepItem();

private:
  //!@brief create the graphical representation
  void construct();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief draw the graphical representation
  void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);

  //!@brief return the represented step
  cedar::proc::StepPtr getStep();

  //!@brief return the represented step
  cedar::proc::ConstStepPtr getStep() const;

  //!@brief handles events in the context menu
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

  //!@brief reads a configuration from a node
  void readConfiguration(const cedar::aux::ConfigurationNode& node);
  //!@brief saves a configuration to a node
  void writeConfiguration(cedar::aux::ConfigurationNode& root) const;

  //!@brief helper function to remove all connections to other graphical elements
  void disconnect();

  //!@brief Checks whether there is a gui connection between this step's output and the given input of the target.
  bool hasGuiConnection
  (
    const std::string& fromSlot,
    cedar::proc::gui::StepItem* pToItem,
    const std::string& toSlot
  ) const;

//  //!@brief Sets a Decoration that shows that the step is registered in the recorder
//  void setRecorded(bool status);

public slots:
  //!@brief handles changes in the state of a step (e.g. from error to non-error state)
  void updateStepState();

  //!@brief handles a redraw of the graphical representation
  void redraw();

  //!@brief handles name-change of the underlying step
  void handleStepNameChanged();

  //! Calls reset on the underlying step
  void reset();
signals:
  /*!@brief Emitted whenever the state of the step displayed by this step item changes.
   *
   * @remarks This signal is used to transfer the underlying signal from the processing thread to the gui thread.
   */
  void stepStateChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private slots:
  //! Slot that triggers an action based on a button in the action widget that can be opened for a step item.
  void handleExternalActionButtons();

private:
  void emitStepStateChanged();

  //!@brief sets the represented step
  void setStep(cedar::proc::StepPtr step);

  //! Updates the display of the step's run time measurements.
  void updateToolTip();

  void updateIconGeometry();

  qreal getContentsPadding() const;

  void displayModeChanged();

private slots:
  void openDefinedPlotAction();

  void openActionsDock();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief a vector of all triggers of the current step
  std::vector<cedar::proc::gui::TriggerItem*> mTriggers;

  //!@brief connection to state changed signal of step
  boost::signals2::connection mStateChangedConnection;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:

private:

  void openKinematicChainWidget();

  void openViewer();
}; // class StepItem

#endif // CEDAR_PROC_STEP_ITEM_H
