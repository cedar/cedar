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

    File:        StepManager.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 24

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_MANAGER_H
#define CEDAR_PROC_MANAGER_H

// LOCAL INCLUDES
#include "processing/namespace.h"
#include "processing/Registry.h"
#include "processing/StepDeclaration.h"
#include "processing/TriggerDeclaration.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <map>
#include <set>
#include <vector>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::Manager
{
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
public:
  typedef std::set<std::string> CategoryList;
  typedef std::vector<StepDeclarationPtr> StepCategoryEntries;
  typedef std::map<std::string, StepCategoryEntries> StepCategories;

  typedef cedar::proc::Registry<Step, StepDeclaration> StepRegistry;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Destructor
  ~Manager();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  static Manager& getInstance();
  void declareTriggerClass(TriggerDeclarationPtr pDeclaration);
  void readStep(const std::string& classId, const ConfigurationNode& root);
  void readSteps(const ConfigurationNode& root);
  void readTrigger(const std::string& classId, const ConfigurationNode& root);
  void readTriggers(const ConfigurationNode& root);
  void readDataConnection(const ConfigurationNode& root);
  void readDataConnections(const ConfigurationNode& root);
  void readAll(const ConfigurationNode& root);
  void readFile(const std::string& filename);

  void registerTrigger(cedar::proc::TriggerPtr trigger);
  cedar::proc::TriggerPtr getTrigger(const std::string& name);
  StepRegistry& steps();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  TriggerPtr allocateTrigger(const std::string& classId) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The standard constructor.
  Manager();

  static void parseDataName(const std::string& instr, std::string& stepName, std::string& dataName);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  static Manager mpManager;

  StepRegistry mStepRegistry;

  std::map<std::string, TriggerDeclarationPtr> mTriggerDeclarations;
  std::map<std::string, TriggerPtr> mTriggers;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::Manager

#endif // CEDAR_PROC_MANAGER_H

