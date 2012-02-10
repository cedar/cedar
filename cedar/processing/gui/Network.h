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

// CEDAR INCLUDES
#include "cedar/processing/gui/namespace.h"
#include "cedar/processing/gui/GraphicsBase.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/Network.h"

// SYSTEM INCLUDES
#include <boost/signals2/signal.hpp>
#include <boost/signals2/connection.hpp>

/*!@brief The representation of a cedar::proc::Network in a cedar::proc::gui::Scene.
 *
 *        This class takes care of loading cedar::proc::Networks in a manner that allows them to be added into
 *        cedar::proc::gui::Scenes as either the root network or a subnetwork.
 */
class cedar::proc::gui::Network : public cedar::proc::gui::GraphicsBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief mapping from data slot names to their graphical representation
  typedef std::map<std::string, cedar::proc::gui::DataSlotItem*> DataSlotNameMap;
  //!@brief mapping from data role id to a map of all data slots fitting this id
  typedef std::map<cedar::proc::DataRole::Id, DataSlotNameMap> DataSlotMap;
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Network
  (
    QMainWindow *pMainWindow,
    qreal width = 0,
    qreal height = 0,
    cedar::proc::NetworkPtr network = cedar::proc::NetworkPtr()
  );

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
  void addElementsToScene(cedar::proc::gui::Scene* pScene);

  //!@brief get the current file, to which the network configuration can be saved
  const std::string& getFileName() const;

  //!@brief Resizes the network to fit all its contents.
  void fitToContents();

  //!@brief Adds an element to the network.
  void addElement(cedar::proc::gui::GraphicsBase *pElement);

  //!@brief Sets the scene containing this item.
  void setScene(cedar::proc::gui::Scene* pScene);

  //!@brief reads a configuration from a node
  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  //!@brief saves a configuration to a node
  void writeConfiguration(cedar::aux::ConfigurationNode& root) const;

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
  void writeScene(cedar::aux::ConfigurationNode& root, cedar::aux::ConfigurationNode& scene);
  //!@brief read scene from a node
  void readScene(cedar::aux::ConfigurationNode& root);

  //!@brief add all steps contained in this network to a scene
  void addStepsToScene();

  //!@brief add all triggers contained in this network to a scene
  void addTriggersToScene();

  //!@brief Reacts to elements being added in the underlying network.
  void elementAdded(cedar::proc::Network* network, cedar::proc::ElementPtr pElement);

  //!@brief Determines whether the network is the root network.
  bool isRootNetwork();

  void checkSlots();

  void addDataItems();

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
  //!@brief a map of all data slots of the current step
  DataSlotMap mSlotMap;

  //!@brief a vector of steps, which contains all steps that should be added to the scene after reading a configuration
  std::vector<cedar::proc::gui::StepItem*> mpStepsToAdd;
  //!@brief a vector of triggers, which contains all steps that should be added to the scene
  //        after reading a configuration
  std::vector<cedar::proc::gui::TriggerItem*> mpTriggersToAdd;

  //! Connection to Network's slot changed signal.
  boost::signals2::connection mSlotConnection;
}; // class cedar::proc::gui::NetworkFile

#endif // CEDAR_PROC_GUI_NETWORK_FILE_H
