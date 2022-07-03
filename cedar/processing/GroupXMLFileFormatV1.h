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

    File:        GroupXMLFileFormatV1.h

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2022 06 12

    Description: Header file for the class cedar::proc::GroupXMLFileFormatV1.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GROUP_XMLFILE_FORMAT_V1_H
#define CEDAR_PROC_GROUP_XMLFILE_FORMAT_V1_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/processing/GroupXMLFileFormatV1.fwd.h"
#include "cedar/auxiliaries/math/TransferFunction.fwd.h"
#include "cedar/auxiliaries/kernel/Kernel.fwd.h"
#include "cedar/auxiliaries/Configurable.fwd.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.fwd.h"
#include "cedar/auxiliaries/ObjectParameterTemplate.fwd.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/processing/DataConnection.fwd.h"

// SYSTEM INCLUDES
#include <boost/bimap.hpp>


/*!@brief Responsible for reading and writing groups to boost configuration trees.
 */
class cedar::proc::GroupXMLFileFormatV1
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  GroupXMLFileFormatV1();

  //!@brief Destructor
  virtual ~GroupXMLFileFormatV1();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*!@brief Writes the group to a configuration node using the first version of the format.
   */
  void write
  (
    cedar::proc::ConstGroupPtr group,
    cedar::aux::ConfigurationNode& root
  );

  static std::string bimapNameLookupXML(boost::bimap<std::string, std::string> bimap,
                                        std::string name, bool directionCedarToXML = true);

  // Transformation functions, to transform and write a parameter's configuration according to the XML file format

  // Write kernel parameter
  static void writeKernelListParameter(
    cedar::aux::ObjectListParameterTemplate<cedar::aux::kernel::Kernel>* kernels, cedar::aux::ConfigurationNode& root);

  // Write sigmoid parameter
  static void writeActivationFunctionParameter(
    cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction>*, cedar::aux::ConfigurationNode&);

  // Write dimensionality/sizes parameter
  static void writeDimensionsParameter(
    cedar::aux::UIntParameterPtr, cedar::aux::UIntVectorParameterPtr, cedar::aux::ConfigurationNode&);


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  /*!@brief Writes the steps in the group to the configuration node.
   */
  void writeSteps(cedar::proc::ConstGroupPtr group, cedar::aux::ConfigurationNode& root) const;

  /*!@brief Writes a data connection to the configuration node.
   */
  void writeDataConnection(cedar::aux::ConfigurationNode& root, const cedar::proc::DataConnectionPtr connection) const;

  /*!@brief Writes the data connections in the group to the configuration node.
   */
  void writeDataConnections(cedar::proc::ConstGroupPtr group, cedar::aux::ConfigurationNode& root) const;


  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  static boost::bimap<std::string, std::string> stepNameLookupTableXML;
  static boost::bimap<std::string, std::string> transferFunctionNameLookupTableXML;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::GroupXMLFileFormatV1

#endif // CEDAR_PROC_GROUP_XMLFILE_FORMAT_V1_H

