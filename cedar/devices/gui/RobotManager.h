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

    File:        RobotManager.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 02 27

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_GUI_ROBOT_MANAGER_H
#define CEDAR_DEV_GUI_ROBOT_MANAGER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/gui/namespace.h"
#include "cedar/devices/namespace.h"
#include "cedar/devices/gui/ui_RobotManager.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/Path.fwd.h"

// SYSTEM INCLUDES
#include <QTreeWidgetItem>
#include <QWidget>
#include <string>


/*!@brief A gui for the manager of robot instances.
 */
class cedar::dev::gui::RobotManager : public QWidget, public Ui_RobotManager
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
  RobotManager();

  //!@brief The destructor.
  ~RobotManager();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void loadConfiguration(const cedar::aux::Path& configuration);

signals:
  void robotNameAdded(QString addedRobotName);

  void robotConfigurationLoaded(QString addedRobotName);

  void closeRobotManager();

  void configurationChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void deselectRobot();

  void selectRobot(const std::string& robotName);

  std::string getSelectedRobotName() const;

  cedar::dev::RobotPtr getSelectedRobot() const;

  // signal translators
  void robotAddedSignalTranslator(const std::string& addedRobotName);

  void robotConfigurationLoadedSignalTranslator(const std::string& robotName);

  void fillSimpleRobotList();

  void robotRemoved(const std::string& robotName);

  void fillExistingRobots();

  void robotRenamed(const std::string& oldName, const std::string& newName);

private slots:
  void loadConfigurationTriggered();

  void loadConfigurationFromResourceTriggered();

  void addRobotClicked();

  void addRobotName(QString addedRobotName);

  void updateRobotConfiguration(QString addedRobotName);

  void robotNameSelected(int nameIndex);

  void partSelected(QTreeWidgetItem* pCurrent, QTreeWidgetItem* pPrevious);

  void simpleModeAdd();

  void removeClicked();

  void closeWindow();
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  boost::signals2::connection mRobotAddedConnection;
  boost::signals2::connection mRobotConfigurationChangedConnection;
  boost::signals2::connection mRobotRemovedConnection;
  boost::signals2::scoped_connection mRobotRenamedConnection;

  QTreeWidgetItem* mpComponentsNode;
  QTreeWidgetItem* mpChannelsNode;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::gui::RobotManager


#endif // CEDAR_DEV_GUI_ROBOT_MANAGER_H

