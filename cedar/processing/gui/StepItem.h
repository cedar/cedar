/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/processing/gui/namespace.h"
#include "cedar/processing/gui/GraphicsBase.h"
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/EnumType.h"

// SYSTEM INCLUDES
#include <QMainWindow>
#include <QIcon>
#include <QObject>
#include <map>


/*!@brief A representation of a cedar::proc::Step object in a cedar::proc::gui::Scene.
 */
class cedar::proc::gui::StepItem : public QObject, public cedar::proc::gui::GraphicsBase
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::proc::gui::Network;
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief mapping from data slot names to their graphical representation
  typedef std::map<std::string, cedar::proc::gui::DataSlotItem*> DataSlotNameMap;

  //!@brief mapping from data role id to a map of all data slots fitting this id
  typedef std::map<cedar::proc::DataRole::Id, DataSlotNameMap> DataSlotMap;

  //! Enum-class for the display mode of steps.
  class DisplayMode
  {
    public:
      //! the id of an enum entry
      typedef cedar::aux::EnumId Id;

    public:
      /*! @brief Construct method that fills the enum.
       *  @see cedar::aux::EnumBase
       */
      static void construct()
      {
        mType.type()->def(cedar::aux::Enum(ICON_AND_TEXT, "ICON_AND_TEXT", "icon and text"));
        mType.type()->def(cedar::aux::Enum(ICON_ONLY, "ICON_ONLY", "icon only"));
      }

      //! @returns A const reference to the base enum object.
      static const cedar::aux::EnumBase& type()
      {
        return *mType.type();
      }

      //! Display icon and text
      static const Id ICON_AND_TEXT = 0;

      //! Display an icon only
      static const Id ICON_ONLY = 1;

    private:
      //! The base enum object.
      static cedar::aux::EnumType<cedar::proc::gui::StepItem::DisplayMode> mType;
  };

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

  //!@brief handles events in the context menu
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

  //!@brief returns one of the data slots associated with this step
  cedar::proc::gui::DataSlotItem* getSlotItem(cedar::proc::DataRole::Id role, const std::string& name);

  //!@brief returns one of the data slots associated with this step
  cedar::proc::gui::DataSlotItem const* getSlotItem(cedar::proc::DataRole::Id role, const std::string& name) const;

  //!@brief returns a map of all data slots of the same id
  cedar::proc::gui::StepItem::DataSlotNameMap& getSlotItems(cedar::proc::DataRole::Id role);

  //!@brief reads a configuration from a node
  void readConfiguration(const cedar::aux::ConfigurationNode& node);
  //!@brief saves a configuration to a node
  void writeConfiguration(cedar::aux::ConfigurationNode& root) const;

  //!@brief resets the internal step
  void resetPointer()
  {
    mStep.reset();
  }

  //!@brief helper function to remove all connections to other graphical elements
  void disconnect();

  //!@brief Checks whether there is a gui connection between this step's output and the given input of the target.
  bool hasGuiConnection
  (
    const std::string& fromSlot,
    cedar::proc::gui::StepItem* pToItem,
    const std::string& toSlot
  ) const;

public slots:
  //!@brief handles changes in the state of a step (e.g. from error to non-error state)
  void stepStateChanged();

  //!@brief handles a redraw of the graphical representation
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
  //!@briefs adds graphical representations for all data items
  void addDataItems();

  //!@brief adds graphical representations for all triggers contained in the step
  void addTriggerItems();

  //!@brief sets the represented step
  void setStep(cedar::proc::StepPtr step);

  void addRoleSeparator(const cedar::aux::Enum& e, QMenu* pMenu);

  //!@brief Fills the menu with available plots
  void fillPlots
  (
    QMenu* pMenu,
    std::map<QAction*, std::pair<cedar::aux::gui::PlotDeclarationPtr, cedar::aux::Enum> >& declMap
  );

  //! Fills in the actions for the display style.
  void fillDisplayStyleMenu(QMenu* pMenu);

  //!@brief Opens a new DockWidget to show the plot.
  void showPlot
  (
    const QPoint& position,
    cedar::aux::gui::PlotInterface* plot,
    cedar::proc::DataSlotPtr slot,
    std::string title = ""
  );

  //! Opens plots for all data in this step.
  void plotAll(const QPoint& position);

  //! Updates the display of the step's run time measurements.
  void timerEvent(QTimerEvent *pEvent);

  //! Sets the current display mode.
  void setDisplayMode(cedar::proc::gui::StepItem::DisplayMode::Id mode);

  //! Updates the positions of the data slot items.
  void updateDataSlotPositions();

  void slotAdded(cedar::proc::DataRole::Id role, const std::string& name);

  void slotRemoved(cedar::proc::DataRole::Id role, const std::string& name);

  void addDataItemFor(cedar::proc::DataSlotPtr slot);

private slots:
  void displayStyleMenuTriggered(QAction* pAction);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief the represented step
  cedar::proc::StepPtr mStep;

  //!@brief a map of all data slots of the current step
  DataSlotMap mSlotMap;

  //!@brief a vector of all triggers of the current step
  std::vector<cedar::proc::gui::TriggerItem*> mTriggers;

  //!@brief Identifier of the timer used for updating the run time measurements.
  int mRunTimeMeasurementTimerId;

  //! Size used for displaying the step icons.
  static const int mIconSize;

  //! The width of newly created steps.
  static const qreal mDefaultWidth;

  //! The height of newly created steps.
  static const qreal mDefaultHeight;

  //! The height of newly created steps.
  static const qreal mBaseDataSlotSize;

  boost::signals2::connection mSlotAddedConnection;
  boost::signals2::connection mSlotRemovedConnection;

  //!@brief the class id of the step
  cedar::proc::ElementDeclarationPtr mClassId;

  //!@brief the main window in which the current graphical representation is embedded
  QMainWindow* mpMainWindow;

  //!@brief the icon representing the contained step
  QIcon mStepIcon;

  //!@brief connection to state changed signal of step
  boost::signals2::connection mStateChangedConnection;

  //!@brief The current display mode of the step.
  cedar::proc::gui::StepItem::DisplayMode::Id mDisplayMode;
  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:

private:

}; // class StepItem

#endif // CEDAR_PROC_STEP_ITEM_H
