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
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/GroupXMLFileFormatV1.fwd.h"
#include "cedar/auxiliaries/math/TransferFunction.fwd.h"
#include "cedar/auxiliaries/kernel/Kernel.fwd.h"
#include "cedar/auxiliaries/Configurable.fwd.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.fwd.h"
#include "cedar/auxiliaries/ObjectParameterTemplate.fwd.h"
#include "cedar/processing/steps/SynapticConnection.fwd.h"
#include "cedar/processing/Connectable.fwd.h"
#include "cedar/processing/DataConnection.fwd.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/processing/ProjectionMappingParameter.fwd.h"
#include "cedar/processing/Step.fwd.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"

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

  static std::string bimapNameLookupXML(boost::bimap<std::string, std::string> bimap,
                                        std::string name, bool directionCedarToXML = true);

  // Transformation functions, to transform and write a parameter's configuration according to the XML file format

  // Write kernel parameter
  static void writeKernelListParameter(
    cedar::aux::ObjectListParameterTemplate<cedar::aux::kernel::Kernel>* kernels, cedar::aux::ConfigurationNode& root);

  static void readKernelListParameter(
      cedar::aux::ObjectListParameterTemplate<cedar::aux::kernel::Kernel>* kernels,
      const cedar::aux::ConfigurationNode& root);

  // Write sigmoid parameter
  static void writeActivationFunctionParameter(
    cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction>*, cedar::aux::ConfigurationNode&, const std::string& name = "ActivationFunction");

  static void readActivationFunctionParameter(
    cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction>* sigmoid,
    const cedar::aux::ConfigurationNode& root, const std::string& name = "ActivationFunction");

  // Write dimensionality/sizes parameter
  static void writeDimensionsParameter(
    cedar::aux::UIntParameterPtr, cedar::aux::UIntVectorParameterPtr,
    std::vector<cedar::aux::math::Limits<double>> sizesRange, cedar::aux::ConfigurationNode&, const std::string& name = "Dimensions");

  static void readDimensionsParameter(
    cedar::aux::UIntParameterPtr dimensionality, cedar::aux::UIntVectorParameterPtr sizes,
    std::vector<cedar::aux::math::Limits<double>>& sizesRange, const cedar::aux::ConfigurationNode& node, const std::string& name = "Dimensions");

    // Read dimension mappings of a projection
  static void readProjectionMappingsParameter(
    cedar::proc::ProjectionMappingParameterPtr& mappingParameter,
    const cedar::aux::ConfigurationNode& root);

  // Checks if a synaptic connection chain is exportable (chain of StaticGain/Projection/Convolution)
  static bool isSynapticConnectionChainExportable(cedar::proc::Connectable* chainSource, std::string& errorMsg);


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

  /*!@brief Reads steps from the configuration node and adds them to the group.
  */
  void readSteps(cedar::proc::GroupPtr group, const cedar::aux::ConfigurationNode& root,
                 std::vector<std::string>& exceptions);

  /*!@brief Creates a step with given name class_id and properties and adds it to the group.
  */
  cedar::proc::StepPtr createStep(std::string name, std::string class_id, cedar::proc::GroupPtr group,
                  const cedar::aux::ConfigurationNode& stepNode, std::vector<std::string>& exceptions);

  // Returns true if provided step is on the blacklist for the common export for steps
  bool isStepBlacklisted(cedar::proc::Connectable* step) const;

  // Checks if a synaptic connection chain is exportable (chain of StaticGain/Projection/Convolution)
  static bool isSynapticConnectionChainExportableRecursive(cedar::proc::Connectable* chainSource, bool hasStaticGain,
                                                  bool hasConvolution, bool hasProjection);

  /*!@brief Writes a synaptic connection to the configuration node.
   */
  void writeSynapticConnection(cedar::aux::ConfigurationNode& root,
                               const cedar::proc::steps::SynapticConnectionPtr connection) const;

  /*!@brief Writes the data connections in the group to the configuration node.
   */
  void writeSynapticConnections(cedar::proc::ConstGroupPtr group, cedar::aux::ConfigurationNode& root) const;

  /*!@brief Writes a chained synaptic connection to the configuration node.
   */
  void writeChainedSynapticConnection(cedar::proc::Connectable* connectable, cedar::aux::ConfigurationNode synCon,
                                      cedar::aux::ConfigurationNode& root) const;

  /*!@brief Writes the chained synaptic connections in the group to the configuration node.
   */
  void writeChainedSynapticConnections(cedar::proc::ConstGroupPtr group, cedar::aux::ConfigurationNode& root) const;


  /*!@brief Writes a data connection to the configuration node.
   */
  void writeDataConnection(cedar::aux::ConfigurationNode& root, const cedar::proc::DataConnectionPtr connection) const;

  /*!@brief Writes the data connections in the group to the configuration node.
   */
  void writeDataConnections(cedar::proc::ConstGroupPtr group, cedar::aux::ConfigurationNode& root) const;

  /*!@brief Reads data connections and synaptic connections from a configuration node and adds them to the group.
 */
  void readConnections
  (
    cedar::proc::GroupPtr group,
    const cedar::aux::ConfigurationNode& root,
    std::vector<std::string>& exceptions
  );
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief: these are for switching between cedar and xml dft architecture naming conventions
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

