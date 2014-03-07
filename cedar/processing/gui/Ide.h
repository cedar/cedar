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

    File:        Ide.h

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

#ifndef CEDAR_PROC_GUI_MAIN_WINDOW_H
#define CEDAR_PROC_GUI_MAIN_WINDOW_H

// CEDAR INCLUDES
#include "cedar/processing/gui/ui_Ide.h"
#include "cedar/auxiliaries/LogInterface.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/CallFunctionInThread.fwd.h"
#include "cedar/processing/gui/PerformanceOverview.fwd.h"
#include "cedar/processing/gui/ArchitectureConsistencyCheck.fwd.h"
#include "cedar/processing/gui/BoostControl.fwd.h"
#include "cedar/processing/gui/ElementClassList.fwd.h"
#include "cedar/processing/gui/Ide.fwd.h"
#include "cedar/processing/gui/Network.fwd.h"

// SYSTEM INCLUDES
#include <QMainWindow>
#include <QKeyEvent>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <map>


/*!@brief This is the main window of the processingIde application.
 */
class cedar::proc::gui::Ide : public QMainWindow, public Ui_Ide
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The standard constructor.
   *
   * @param loadDefaultPlugins Loads all plugins set as default in the configuration.
   * @param redirectLogToGui   Enables or disables redirection of log messages to the gui (can help when too many log
   *                           messages lock up the user interface).
   */
  Ide(bool loadDefaultPlugins = true, bool redirectLogToGui = true);

  //!@brief Destructor
  ~Ide();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Handles key presses in the main window.
   */
  void keyPressEvent(QKeyEvent* pEvent);

  /*!@brief Resets the current scene and displays the new network.
   */
  void resetTo(cedar::proc::gui::NetworkPtr network);

public slots:
  /*!@brief Slot that displays notifications.
   */
  void notify(const QString& message);

  /*!@brief Changes the mode back to select when an architecture tool is finished.
   */
  void architectureToolFinished();
  
  /*!@brief Starts all looped triggers (and other derivatives of looped thread).
   */
  void startThreads();

  /*!@brief Stops all looped triggers (and other derivatives of looped thread).
   */
  void stopThreads();

  /*!@brief Single-step all looped triggers (and other derivatives of looped thread).
   */
  void stepThreads();

  /*!@brief Slot that is connected to the "new" item in the file menu.
   */
  void newFile();

  /*!@brief Slot that is connected to the "save" item in the file menu.
   */
  void save();

  /*!@brief Slot that is connected to the "save as" item in the file menu.
   */
  void saveAs();

  /*!@brief Slot that is connected to the "load" item in the file menu.
   */
  void load();

  /*!@brief Loads a network file specified by the path.
   */
  void loadFile(QString path);

  /*!@brief Opens the manage plugins dialog.
   */
  void showManagePluginsDialog();

  /*!@brief Shows the settings dialog.
   */
  void showSettingsDialog();

  /*!@brief Toggles the snap to grid functionality.
   */
  void toggleGrid(bool toggled);

  /*!@brief Fills the recent files list.
   */
  void fillRecentFilesList();

  /*!@brief Slot that is connected to clicking the entries in the recent files list.
   */
  void recentFileItemTriggered();

  /*!@brief Returns the architecture view used by the ide.
   */
  cedar::proc::gui::View* getArchitectureView();

  /*!@brief Displays the about dialog.
   */
  void showAboutDialog();

  /*!@brief Resets the root network
   */
  void resetRootNetwork();

  /*!@brief Opens a dialog that lets the user export the current scene as an SVG
   */
  void exportSvg();

  /*!@brief Opens a dialog that contains the robot manager widget.
   */
  void showRobotManager();
  
  /*!@brief Duplicates a selected step
   */
  void duplicateStep();

  /*!@brief Select all elements
   */
  void selectAll();

  /*!@brief Show/hide all trigger connections
   */
  void showTriggerConnections(bool show);

  /*!@brief Shows a dialog for architecture consistency checking.
   */
  void showConsistencyChecker();

  /*!@brief Opens a boost control widget.
   */
  void showBoostControl();

  //!@brief toggle smart connections
  void toggleSmartConnections(bool smart);

  //!@brief closes all plot windows of every step
  void closePlots();

  //!@brief shows/hides all plot windows of every step
  void toggleVisibilityOfPlots();

  //!@brief Starts or stops the recorder function();
  void toggleRecorder(bool status);

  //!@brief Takes a snap shot from the registered steps;
  void takeSnapshot();

  //!@brief opens dialogue to add plotgroup
  void addPlotGroup();
  
  //!@brief opens dialogue to edit selected plotgroup
  void editPlotGroup();

  //!@brief displays the selected plotgroup
  void displayPlotGroup();

  //!@brief deletes the selected plotgroup
  void deletePlotGroup();
  //! Returns the log widget of this ide.
  cedar::aux::gui::Log* getLog() const
  {
    return this->mpLog;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  /*!@brief Deletes the list of graphics items.
   */
  void deleteElements(QList<QGraphicsItem*>& items);

  /*!@brief Delete a single graphics item.
   */
  void deleteElement(QGraphicsItem* pItem);

  /*!@brief Deletes the elements currently selected in the scene.
   */
  void deleteSelectedElements();

  /*!@brief Reacts to closing the window.
   */
  void closeEvent(QCloseEvent *pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Resets the list of elements.
   */
  void resetStepList();

  /*!@brief Loads the default plugins.
   */
  void loadDefaultPlugins();

  /*!@brief Stores the window state and other settings.
   */
  void storeSettings();

  /*!@brief Restores window state and other settings that were previously stored with
   *        cedar::proc::gui::Ide::storeSettings.
   */
  void restoreSettings();

  /*!@brief Sets the filename in the title of the main window.
   */
  void displayFilename(const std::string& filename);

  //! Updates the start and stop triggers threads.
  void updateTriggerStartStopThreadCallers();

  /*!@brief sort two QGraphicsItems measuring their depth in relation to the root network.
   */
  static bool sortElements(QGraphicsItem* pFirstItem, QGraphicsItem* pSecondItem);

  //!@brief populates the Plot Groups Combobox with available Plot Groups
  void loadPlotGroupsIntoComboBox();

  void setNetwork(cedar::proc::gui::NetworkPtr network);

private slots:
  void globalTimeFactorSliderChanged(int newValue);

  void globalTimeFactorSpinboxChanged(double value);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Widgets for each list of element classes.
  std::map<std::string, cedar::proc::gui::ElementClassList*> mElementClassListWidgets;

  //! The network currently displayed.
  cedar::proc::gui::NetworkPtr mNetwork;

  //! Architecture consistency check widget.
  cedar::proc::gui::ArchitectureConsistencyCheck* mpConsistencyChecker;

  //! Performance overview.
  cedar::proc::gui::PerformanceOverview* mpPerformanceOverview;

  //! Dock widget for the consistency checker.
  QDockWidget* mpConsistencyDock;

  QString mDefaultWindowTitle;

  cedar::proc::gui::BoostControl* mpBoostControl;

  //! In which the user specifies the time step for single-step functionality.
  QDoubleSpinBox* mpCustomTimeStep;

  //! Used for starting all triggers in a separate thread
  cedar::aux::CallFunctionInThreadPtr mStartThreadsCaller;

  //! Used for stopping all triggers in a separate thread
  cedar::aux::CallFunctionInThreadPtr mStopThreadsCaller;

  //! Combobox to select plot groups
  QComboBox* mpPlotGroupsComboBox;

  //! Spinbox for controlling the global time step.
  QDoubleSpinBox* mpGlobalTimeFactor;

  //! Spinbox for controlling the global time step.
  QSlider* mpGlobalTimeFactorSlider;

}; // class cedar::MainWindow

#endif // CEDAR_PROC_GUI_MAIN_WINDOW_H
