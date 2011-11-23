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

#ifndef CEDAR_PROC_NETWORK_H
#define CEDAR_PROC_NETWORK_H

// LOCAL INCLUDES
#include "cedar/processing/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <vector>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 * @todo Change the name of the class to Module
 * @todo Add a slot, which reacts to name changes of elements (update map of names to ptrs)
 */
class cedar::proc::Network
{
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef std::vector<cedar::proc::StepPtr> StepVector;
  typedef std::vector<cedar::proc::TriggerPtr> TriggerVector;
  typedef std::vector<cedar::proc::GroupPtr> GroupVector;
  typedef std::vector<cedar::proc::DataConnectionPtr> DataConnectionVector;
  typedef std::vector<cedar::proc::TriggerConnectionPtr> TriggerConnectionVector;
  typedef std::map<std::string, cedar::proc::ElementPtr> ElementMap;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Network();

  //!@brief Destructor
  ~Network();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void readSteps(const cedar::aux::ConfigurationNode& root);
  void saveSteps(cedar::aux::ConfigurationNode& root);

  void readTriggers(const cedar::aux::ConfigurationNode& root);
  void saveTriggers(cedar::aux::ConfigurationNode& root);

//  void readGroups(const cedar::aux::ConfigurationNode& root);
//  void saveGroups(cedar::aux::ConfigurationNode& root);

  void readDataConnection(const cedar::aux::ConfigurationNode& root);
  void saveDataConnection(cedar::aux::ConfigurationNode& root, const cedar::proc::DataConnectionPtr connection);

  void readDataConnections(const cedar::aux::ConfigurationNode& root);
  void saveDataConnections(cedar::aux::ConfigurationNode& root);

  void readFrom(const cedar::aux::ConfigurationNode& root);
  void saveTo(cedar::aux::ConfigurationNode& root);

  void readFile(const std::string& filename);
  void writeFile(const std::string& filename);

//  void add(cedar::proc::StepPtr step);
  void remove(cedar::proc::StepPtr step);
//  void add(cedar::proc::TriggerPtr trigger);
  void remove(cedar::proc::TriggerPtr trigger);
  void add(cedar::proc::GroupPtr group);

  void add(std::string className, std::string instanceName);
  void add(cedar::proc::ElementPtr element, std::string instanceName);
  void add(cedar::proc::ElementPtr element);

  /*!@brief Returns the element with the given name as a pointer of the specified type.
   */
  template <class T>
  boost::shared_ptr<T> getElement(const std::string& name)
  {
    return boost::shared_dynamic_cast<T>(this->getElement(name));
  }

  cedar::proc::ElementPtr getElement(const std::string& name);

  void connectSlots(const std::string& source, const std::string& target);
  void connectTrigger(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target);
  bool isConnected(const std::string& source, const std::string& target);

  const StepVector& steps() const;
  StepVector& steps();
  const TriggerVector& triggers() const;
  TriggerVector& triggers();
  const GroupVector& groups() const;
  GroupVector& groups();
  const ElementMap& elements() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  StepVector mSteps;
  TriggerVector mTriggers;
  GroupVector mGroups;
  ElementMap mElements;
  DataConnectionVector mDataConnections;
  TriggerConnectionVector mTriggerConnections;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::Network

#endif // CEDAR_PROC_NETWORK_H

