/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Network.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 19

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_NETWORK_H
#define CEDAR_PROC_GUI_NETWORK_H

// LOCAL INCLUDES
#include "cedar/processing/gui/namespace.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/Network.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief The representation of a cedar::proc::Network in a cedar::proc::gui::Scene.
 *
 * @todo More descriptions.
 */
class cedar::proc::gui::Network
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Network(QMainWindow *pMainWindow, cedar::proc::gui::Scene* pScene);

  //!@brief Destructor
  ~Network();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief write network to file
  void write();
  //!@brief write network to file given by destination
  void write(const std::string& destination);
  //!@brief read network from given file
  void read(const std::string& source);

  //!@brief access the underlying cedar::proc::Network
  cedar::proc::NetworkPtr network();

  //!@brief add all elements contained in this network to the scene
  void addToScene();

  //!@brief get the current file, to which the network configuration can be saved
  const std::string& getFileName() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //!@brief write scene to a node
  void writeScene(cedar::aux::ConfigurationNode& root);
  //!@brief read scene from a node
  void readScene(cedar::aux::ConfigurationNode& root);

  //!@brief add all steps contained in this network to a scene
  void addStepsToScene();

  //!@brief add all triggers contained in this network to a scene
  void addTriggersToScene();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief represented network
  cedar::proc::NetworkPtr mNetwork;
  //!@brief a scene, which displays the elements contained in this network
  cedar::proc::gui::Scene* mpScene;
  //!@brief a filename from which to load a network configuration, or to which to save a configuration
  std::string mFileName;
  //!@brief a main window
  QMainWindow *mpMainWindow;

  //!@brief a vector of steps, which contains all steps that should be added to the scene after reading a configuration
  std::vector<cedar::proc::gui::StepItem*> mpStepsToAdd;
  //!@brief a vector of triggers, which contains all steps that should be added to the scene
  //        after reading a configuration
  std::vector<cedar::proc::gui::TriggerItem*> mpTriggersToAdd;
}; // class cedar::proc::gui::NetworkFile

#endif // CEDAR_PROC_GUI_NETWORK_FILE_H

