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

    File:        GroupFileFormatV1.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 12 20

    Description: Header file for the class cedar::proc::GroupFileFormatV1.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GROUP_FILE_FORMAT_V1_H
#define CEDAR_PROC_GROUP_FILE_FORMAT_V1_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/processing/Element.fwd.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/processing/GroupFileFormatV1.fwd.h"
#include "cedar/processing/DataConnection.fwd.h"
#include "cedar/auxiliaries/Configurable.fwd.h"
#include "cedar/auxiliaries/Parameter.fwd.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>


/*!@brief Responsible for reading and writing groups to boost configuration trees.
 */
class cedar::proc::GroupFileFormatV1
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Reads the group from a configuration node using the first version of the format.
   */
  void read
  (
    cedar::proc::GroupPtr group,
    const cedar::aux::ConfigurationNode& root,
    std::vector<std::string>& exceptions
  );

  /*!@brief Writes the group to a configuration node using the first version of the format.
   */
  void write
  (
    cedar::proc::ConstGroupPtr group,
    cedar::aux::ConfigurationNode& root
  );

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Reads steps from the configuration node and adds them to the group.
   */
  void readSteps
  (
    cedar::proc::GroupPtr group,
    const cedar::aux::ConfigurationNode& root,
    std::vector<std::string>& exceptions
  );

  /*!@brief Reads triggers from a configuration node and adds them to the group.
   */
  void readTriggers
  (
    cedar::proc::GroupPtr group,
    const cedar::aux::ConfigurationNode& root,
    std::vector<std::string>& exceptions
  );

  /*!@brief Reads the slots that should be registered in the recorder.
  */
  void readRecords
  (
    cedar::proc::GroupPtr group,
    const cedar::aux::ConfigurationNode& root,
    std::vector<std::string>& exceptions
  );

  /*!@brief Reads data connections from a configuration node and adds them to the group.
   */
  void readDataConnections
  (
    cedar::proc::GroupPtr group,
    const cedar::aux::ConfigurationNode& root,
    std::vector<std::string>& exceptions
  );

  /*!@brief Reads groups from a configuration node and adds them to the parent group.
   */
  void readGroups
  (
    cedar::proc::GroupPtr group,
    const cedar::aux::ConfigurationNode& root,
    std::vector<std::string>& exceptions
  );

  /*!@brief Reads groups from a configuration node and adds them to the parent group.
   */
  void readParameterLinks
  (
    cedar::proc::GroupPtr group,
    const cedar::aux::ConfigurationNode& root,
    std::vector<std::string>& exceptions
  );

  /*!@brief Reads scripts from a configuration node and adds them to the given group.
   */
  void readScripts
  (
    cedar::proc::GroupPtr group,
    const cedar::aux::ConfigurationNode& node,
    std::vector<std::string>& exceptions
  );

  void readCustomParameters
  (
    cedar::proc::GroupPtr group,
    const cedar::aux::ConfigurationNode& customParameters,
    std::vector<std::string>& exceptions
  );

  /*!@brief Writes the meta data to the configuration.
   */
  void writeMetaData(cedar::proc::ConstGroupPtr group, cedar::aux::ConfigurationNode& root) const;

  /*!@brief Writes the steps in the group to the configuration node.
   */
  void writeSteps(cedar::proc::ConstGroupPtr group, cedar::aux::ConfigurationNode& root) const;

  /*!@brief Writes the triggers in the group to the configuration node.
   */
  void writeTriggers(cedar::proc::ConstGroupPtr group, cedar::aux::ConfigurationNode& root) const;

  /*!@brief Writes the steps that are registered in the recorder in the group to the configuration node.
   */
  void writeRecords(cedar::proc::ConstGroupPtr group, cedar::aux::ConfigurationNode& root) const;

  /*!@brief Writes the child groups in the group to the configuration node.
   */
  void writeGroups(cedar::proc::ConstGroupPtr group, cedar::aux::ConfigurationNode& root) const;

  /*!@brief Writes a data connection to the configuration node.
   */
  void writeDataConnection(cedar::aux::ConfigurationNode& root, const cedar::proc::DataConnectionPtr connection) const;

  /*!@brief Writes the data connections in the group to the configuration node.
   */
  void writeDataConnections(cedar::proc::ConstGroupPtr group, cedar::aux::ConfigurationNode& root) const;

  void writeParameterLinks(cedar::proc::ConstGroupPtr group, cedar::aux::ConfigurationNode& root) const;

  void writeCustomParameters(cedar::proc::ConstGroupPtr group, cedar::aux::ConfigurationNode& root) const;

  void writeScripts(cedar::proc::ConstGroupPtr group, cedar::aux::ConfigurationNode& node) const;

  cedar::proc::ElementPtr getLinkElement
  (
    cedar::proc::GroupPtr group,
    const cedar::aux::ConfigurationNode& node,
    const std::string& name
  ) const;

  cedar::aux::ParameterPtr getLinkParameter
  (
    cedar::proc::ElementPtr element,
    const cedar::aux::ConfigurationNode& node,
    const std::string& name
  ) const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::GroupFileFormatV1

#endif // CEDAR_PROC_GROUP_FILE_FORMAT_V1_H

