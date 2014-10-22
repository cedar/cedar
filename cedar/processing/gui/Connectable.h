/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Connectable.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2013 11 13

    Description: Header file for the class cedar::proc::gui::Connectable.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_CONNECTABLE_H
#define CEDAR_PROC_GUI_CONNECTABLE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/GraphicsBase.h"
#include "cedar/processing/DataRole.h"

// FORWARD DECLARATIONS
#include "cedar/processing/Connectable.fwd.h"
#include "cedar/processing/DataSlot.fwd.h"
#include "cedar/processing/gui/Connectable.fwd.h"
#include "cedar/processing/gui/ConnectableIconView.fwd.h"
#include "cedar/processing/gui/DataSlotItem.fwd.h"
#include "cedar/processing/gui/PlotWidget.fwd.h"
#include "cedar/auxiliaries/PluginDeclaration.fwd.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.fwd.h"

// SYSTEM INCLUDES
#include <QAction>
#include <QObject>
#include <QGraphicsSvgItem>
#include <QMainWindow>
#include <map>
#include <vector>


/*!@brief A gui base class for all items that represent cedar::proc::Connectables.
 */
class cedar::proc::gui::Connectable : public QObject, public cedar::proc::gui::GraphicsBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

public:
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
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
      static cedar::aux::EnumType<cedar::proc::gui::Connectable::DisplayMode> mType;
  };


protected:
  //! Decoration item that is attached to a step icon.
  class Decoration
  {
    public:
      //! Constructor.
      Decoration(Connectable* pConnectable, const QString& icon, const QString& description, const QColor& bg = QColor(255, 255, 255));

      //! Destructor.
      ~Decoration()
      {
        delete mpIcon;
        delete mpRectangle;
      }

      //! Sets the position of the decoration.
      void setPosition(const QPointF& pos);

      //! Sets the size of the decoration.
      void setSize(double sizeFactor);

      //! Shows or hides the decoration.
      void setVisible(bool visible)
      {
        this->mpIcon->setVisible(visible);
        this->mpRectangle->setVisible(visible);
      }

      //! Sets the backgroud color of the decoration.
      void setBackgroundColor(const QColor& background);

      //! Resets the backgroud color of the decoration to the default.
      void resetBackgroundColor();

    private:
      QGraphicsSvgItem* mpIcon;

      QGraphicsRectItem* mpRectangle;

      QString mIconFile;

      QColor mDefaultBackground;
  };

  CEDAR_GENERATE_POINTER_TYPES(Decoration);


  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Connectable(qreal width, qreal height, cedar::proc::gui::GraphicsBase::GraphicsGroup group, QMainWindow* pMainWindow = 0);

  //!@brief Destructor
  virtual ~Connectable();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief return the represented step
  cedar::proc::ConnectablePtr getConnectable();

  //!@brief return the represented step
  cedar::proc::ConstConnectablePtr getConnectable() const;

  //!@brief set the represented step
  void setConnectable(cedar::proc::ConnectablePtr);

  //!@brief resets the internal step
  void resetPointer()
  {
    mConnectable.reset();
  }

  //! Resizes slots that are close to the mouse pointer in connection mode.
  void magnetizeSlots(const QPointF& mousePositionInScene);

  //! Removes all effects of magnetization
  void demagnetizeSlots();

  //! Returns the slot item of the given role and name.
  cedar::proc::gui::DataSlotItem* getSlotItem(cedar::proc::DataRole::Id role, const std::string& name);

  //!@brief returns one of the data slots associated with this step
  cedar::proc::gui::DataSlotItem const* getSlotItem(cedar::proc::DataRole::Id role, const std::string& name) const;

  //!@brief returns a map of all data slots of the same id
  cedar::proc::gui::Connectable::DataSlotNameMap& getSlotItems(cedar::proc::DataRole::Id role);

  //!@brief returns a const map of all data slots of the same id
  const cedar::proc::gui::Connectable::DataSlotNameMap& getSlotItems(cedar::proc::DataRole::Id role) const;

  //!@brief sets the "read only"-ness of this connectable
  void setReadOnly(bool readOnly);

  //! Adds a PlotWidget to the step (usually after loading a stored network that had open Plots)
  void addPlotWidget(cedar::proc::gui::PlotWidget* pPlotWidget, int x, int y, int width, int height);

public slots:
  //! reacts to the removal of a data slot
  void reactToSlotRemoved(cedar::proc::DataRole::Id role, QString name);

  //! reacts to the addition of a data slot
  void reactToSlotAdded(cedar::proc::DataRole::Id role, QString name);

  //! reacts to the renaming of a data slot
  void reactToSlotRenamed(cedar::proc::DataRole::Id role, QString oldName, QString newName);

  //!@brief Closes all plots that were opened for this step.
  void closeAllPlots();

  //!@brief toggles visibility of the plots this step has opened
  void toggleVisibilityOfPlots(bool visible = true);

  //!@brief plots all data (!)
  void plotAll();

  //!@brief removes the reference of a child widget from the mChildWidgets vector (called when child got destroyed)
  void removeChildWidget();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //!@brief reacts to added slots
  void slotAdded(cedar::proc::DataRole::Id role, const std::string& name);

  //!@brief reacts to removed slots
  virtual void slotRemoved(cedar::proc::DataRole::Id role, const std::string& name);

  //!@brief reacts to renamed slots
  void slotRenamed(cedar::proc::DataRole::Id role, const std::string& oldName, const std::string& newName);

  //!@brief add the graphical representation of a data slot
  void addDataItemFor(cedar::proc::DataSlotPtr slot);

  //! Updates the positions of all items attached to this one (decorations and data slots).
  void updateAttachedItems();

  //! Updates the positions of the decoration items.
  void updateDecorationPositions();

  //!@brief adds graphical representations for all data items
  void addDataItems();

  //! updates attached items if size changes
  void sizeChanged();

  //! Assigns an offset for the input and output slots.
  void setInputOutputSlotOffset(qreal offset);

  //! Returns the offset for input and output slots.
  qreal getInputOutputSlotOffset() const;

  //! Updates the positions of the data slot items.
  void updateDataSlotPositions();

  //! Sets the size and position of the icon display. The icon will, at most, be size x size.
  void setIconBounds(const qreal& x, const qreal& y, const qreal& size);

  //!@brief Adds the decorations to the step.
  void updateDecorations();

  //! Adds a given decoration to this step.
  void addDecoration(cedar::proc::gui::Connectable::DecorationPtr decoration);

  //! Removes a given decoration from this step. Does nothing if the decoration is not in this step.
  void removeDecoration(cedar::proc::gui::Connectable::DecorationPtr decoration);

  //! Fill plot menu
  void fillConnectableMenu(QMenu& menu, QGraphicsSceneContextMenuEvent* event);

  //!@brief Fills the defined plots into the given menu.
  void fillDefinedPlots(QMenu& menu, const QPoint& plotPosition);

  //! adds the "plot all" action to a menu
  void addPlotAllAction(QMenu& menu, const QPoint& plotPosition);

  //! add a separator for a specific role to a menu
  void addRoleSeparator(const cedar::aux::Enum& e, QMenu* pMenu);

  //!@brief Gets the default plotter and then opens a new DockWidget to show the plot.
  void showPlot
  (
    const QPoint& position,
    std::string& dataName,
    const cedar::aux::Enum& role
  );

  //!@brief Opens a new DockWidget to show the plot.
  void showPlot
  (
    const QPoint& position,
    std::string& dataName,
    const cedar::aux::Enum& role,
    cedar::aux::gui::ConstPlotDeclarationPtr declaration
  );

  //!@brief Opens a new DockWidget to show the plot.
  void showPlot
  (
    const QPoint& position,
    std::string& dataName,
    const cedar::aux::Enum& role,
    const std::string& plotClass
  );

  //! creates a dock widget for plots
  QWidget* createDockWidgetForPlots(const std::string& title, cedar::proc::gui::PlotWidget* pPlotWidget, const QPoint& position);

  //! creates a dock widget
  QWidget* createDockWidget(const std::string& title, QWidget* pWidget);

  //! handles context menu actions defined by this class
  void handleContextMenuAction(QAction* action, QGraphicsSceneContextMenuEvent* event);

  //! write all open child widgets to a configuration node
  void writeOpenChildWidgets(cedar::aux::ConfigurationNode& node) const;

  /*! Returns a formatted string that can be used for labels/window titles.
   */
  std::string getNameForTitle() const;
  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void itemSelected(bool selected);

  //!@brief Fills the menu with available plots
  void fillPlots
  (
    QMenu* pMenu,
    std::map<QAction*, std::pair<cedar::aux::gui::ConstPlotDeclarationPtr, cedar::aux::Enum> >& declMap
  );

  void closeAllChildWidgets();

  void translateStartedSignal();

  void translateStoppedSignal();

  //! Fill plot menu
  void fillPlotMenu(QMenu& menu, QGraphicsSceneContextMenuEvent* event);

private slots:
  void triggerableStarted();

  void triggerableStopped();

  void assignTriggerClicked();

signals:
  //! translates a slot removed signal to Qt
  void reactToSlotRemovedSignal(cedar::proc::DataRole::Id role, QString name);

  //! translates a slot renamed signal to Qt
  void reactToSlotRenamedSignal(cedar::proc::DataRole::Id role, QString oldName, QString newName);

  //! translates a slot added signal to Qt
  void reactToSlotAddedSignal(cedar::proc::DataRole::Id role, QString name);

  //! Emitted whenever the triggerable is started.
  void triggerableStartedSignal();

  //! Emitted whenever the triggerable is stopped.
  void triggerableStoppedSignal();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! The base size of data slots (modified by display mode and other factors).
  static const qreal M_BASE_DATA_SLOT_SIZE;

  //! Amount of padding between data slots.
  static const qreal M_DATA_SLOT_PADDING;

protected:
  //! Used to display the icon of the step.
  cedar::proc::gui::ConnectableIconView* mpIconView;

  //!@brief the class id of the step
  cedar::aux::ConstPluginDeclarationPtr mClassId;

  //!@brief The current display mode of the step.
  cedar::proc::gui::Connectable::DisplayMode::Id mDisplayMode;

  //!@brief the decoration symbolizing that this connectable is being recorded
  DecorationPtr mpRecordedDecoration;

  //! The decorations for this connectable.
  std::vector<DecorationPtr> mDecorations;

  //!@brief the main window in which the current graphical representation is embedded
  QMainWindow* mpMainWindow;

  //!@brief a vector of all child widgets fo the current step
  std::vector<QWidget*> mChildWidgets;

private:
  DecorationPtr mpLoopedDecoration;

  //! An offset to be added to in- and output slot positions.
  qreal mInputOutputSlotOffset;

  //!@brief a map of all data slots of the current step
  DataSlotMap mSlotMap;

  //!@brief the represented step
  cedar::proc::ConnectablePtr mConnectable;

  boost::signals2::connection mSlotAddedConnection;
  boost::signals2::connection mSlotRenamedConnection;
  boost::signals2::connection mSlotRemovedConnection;

  boost::signals2::scoped_connection mStartedConnection;
  boost::signals2::scoped_connection mStoppedConnection;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::Connectable

#endif // CEDAR_PROC_GUI_CONNECTABLE_H

