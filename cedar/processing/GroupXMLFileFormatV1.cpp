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

    File:        GroupXMLFileFormatV1.cpp

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2022 06 12

    Description: Source file for the class cedar::proc::GroupXMLFileFormatV1.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/GroupXMLFileFormatV1.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/ObjectParameterTemplate.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.h"
#include "cedar/auxiliaries/math/TransferFunction.h"
#include "cedar/auxiliaries/kernel/Kernel.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/processing/sources/GroupSource.h"
#include "cedar/processing/sinks/GroupSink.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/steps/SynapticConnection.h"
#include "cedar/auxiliaries/kernel/Gauss.h"
#include "cedar/auxiliaries/math/transferFunctions/AbsSigmoid.h"
#include "cedar/auxiliaries/math/transferFunctions/ExpSigmoid.h"

// SYSTEM INCLUDES
#include <boost/assign.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::GroupXMLFileFormatV1::GroupXMLFileFormatV1()
{
}

cedar::proc::GroupXMLFileFormatV1::~GroupXMLFileFormatV1()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::GroupXMLFileFormatV1::read
(
	cedar::proc::GroupPtr group,
	const cedar::aux::ConfigurationNode& root,
	std::vector<std::string>& exceptions
)
{
	auto dftArchitectureIterator = root.find("DFTArchitecture");
	CEDAR_ASSERT(dftArchitectureIterator != root.not_found())
	auto dftArchitecture = dftArchitectureIterator->second;


	auto steps = dftArchitecture.find("Components");
	if (steps != dftArchitecture.not_found())
	{
		this->readSteps(group, steps->second, exceptions);
	}

	auto connections = dftArchitecture.find("Connections");
  if (connections != dftArchitecture.not_found())
	{
		this->readDataConnections(group, connections->second, exceptions);
	}
}


void cedar::proc::GroupXMLFileFormatV1::write
(
  cedar::proc::ConstGroupPtr group,
  cedar::aux::ConfigurationNode& dftArchitecture
)
{
  cedar::aux::ConfigurationNode root;
  if (group->isLinked())
  {
    // TODO
  }
  else
  {
    cedar::aux::ConfigurationNode steps;
    this->writeSteps(group, steps);
    if (!steps.empty())
    {
      root.add_child("Components", steps);
    }

    cedar::aux::ConfigurationNode connections;
    this->writeSynapticConnections(group, connections);
    this->writeDataConnections(group, connections);
    if (!connections.empty())
    {
      root.add_child("Connections", connections);
    }
  }
  dftArchitecture.add_child("DFTArchitecture", root);
}

void cedar::proc::GroupXMLFileFormatV1::writeSteps
(
	cedar::proc::ConstGroupPtr group,
	cedar::aux::ConfigurationNode& steps
) const
{
  for (auto& name_element_pair : group->getElements())
  {
    auto element = name_element_pair.second;
    // if this is a step, write this to the configuration tree
    if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(element))
    {
      if
      (
        boost::dynamic_pointer_cast<cedar::proc::sinks::GroupSink>(step)
        || boost::dynamic_pointer_cast<cedar::proc::sources::GroupSource>(step)
        || boost::dynamic_pointer_cast<cedar::proc::steps::SynapticConnection>(step)
      )
      {
        continue;
      }
      CEDAR_ASSERT(step->isXMLExportable())
      std::string class_name = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(step);
      std::string node_name = cedar::proc::GroupXMLFileFormatV1::bimapNameLookupXML(
        cedar::proc::GroupXMLFileFormatV1::stepNameLookupTableXML, class_name);
      cedar::aux::ConfigurationNode step_node = cedar::aux::ConfigurationNode();
      step_node.add("<xmlattr>.name", name_element_pair.first);
      step->writeConfigurationXML(step_node);
      steps.push_back(cedar::aux::ConfigurationNode::value_type(node_name, step_node));
    }
  }
}

void cedar::proc::GroupXMLFileFormatV1::writeSynapticConnection
  (
    cedar::aux::ConfigurationNode& root,
    const cedar::proc::steps::SynapticConnectionPtr connection
  )
const
{
  // Write the properties of this connection to a template node
  cedar::aux::ConfigurationNode properties;
  connection->writeConfigurationXML(properties);

  // Enumerate all source and target steps of the connection
  std::vector<std::string> sources;
  std::vector<std::string> targets;
  std::map<std::string, cedar::proc::DataSlotPtr> outputSlots = connection->getDataSlots(DataRole::OUTPUT);
  std::map<std::string, cedar::proc::DataSlotPtr> inputSlots = connection->getDataSlots(DataRole::INPUT);
  for(auto slot : outputSlots)
  {
    for(cedar::proc::DataConnectionPtr outputConn : slot.second->getDataConnections())
    {
      if(auto targetElement = dynamic_cast<cedar::proc::Element*>(outputConn.get()->getTarget()->getParentPtr()))
      {
        // This is the target step
        targets.push_back(targetElement->getFullPath());
      }
    }
  }
  for(auto slot : inputSlots)
  {
    for(cedar::proc::DataConnectionPtr outputConn : slot.second->getDataConnections())
    {
      if(auto sourceElement = dynamic_cast<cedar::proc::Element*>(outputConn.get()->getSource()->getParentPtr()))
      {
        // This is the target step
        sources.push_back(sourceElement->getFullPath());
      }
    }
  }

  // Save a Synpatic connection for all source/target pairs
  for(std::string source : sources)
  {
    for(std::string target : targets)
    {
      cedar::aux::ConfigurationNode connection_node(properties);
      connection_node.put("Source", source);
      connection_node.put("Target", target);
      root.push_back(cedar::aux::ConfigurationNode::value_type("SynapticConnection", connection_node));
    }
  }
}

void cedar::proc::GroupXMLFileFormatV1::writeSynapticConnections
  (
    cedar::proc::ConstGroupPtr group,
    cedar::aux::ConfigurationNode& root
  ) const
{

  for (auto& name_element_pair : group->getElements())
  {
    if(auto connection = boost::dynamic_pointer_cast<cedar::proc::steps::SynapticConnection>(name_element_pair.second))
    {
      this->writeSynapticConnection(root, connection);
    }
  }
}

void cedar::proc::GroupXMLFileFormatV1::writeDataConnection
  (
    cedar::aux::ConfigurationNode& root,
    const cedar::proc::DataConnectionPtr connection
  )
const
{
  std::string source_str = connection->getSource()->getParent();// + "." + connection->getSource()->getName();
  std::string target_str = connection->getTarget()->getParent();// + "." + connection->getTarget()->getName();

  cedar::aux::ConfigurationNode connection_node;
  connection_node.put("Source", source_str);
  connection_node.put("Target", target_str);
  root.push_back(cedar::aux::ConfigurationNode::value_type("SynapticConnection", connection_node));
}

void cedar::proc::GroupXMLFileFormatV1::writeDataConnections
  (
    cedar::proc::ConstGroupPtr group,
    cedar::aux::ConfigurationNode& root
  ) const
{
  for (auto data_connection : group->getDataConnections())
  {
    if(dynamic_cast<cedar::proc::steps::SynapticConnection*>(data_connection.get()->getTarget()->getParentPtr())
      || dynamic_cast<cedar::proc::steps::SynapticConnection*>(data_connection.get()->getSource()->getParentPtr()))
    {
      continue;
    }
    this->writeDataConnection(root, data_connection);
  }
}

boost::bimap<std::string, std::string> cedar::proc::GroupXMLFileFormatV1::stepNameLookupTableXML =
  boost::assign::list_of< boost::bimap<std::string, std::string>::relation >
    ("cedar.dynamics.NeuralField", "Field")
    ("cedar.processing.sources.GaussInput", "GaussInput");

boost::bimap<std::string, std::string> cedar::proc::GroupXMLFileFormatV1::transferFunctionNameLookupTableXML =
  boost::assign::list_of< boost::bimap<std::string, std::string>::relation >
    ("cedar.aux.math.AbsSigmoid", "AbsSigmoid")
    ("cedar.aux.math.ExpSigmoid", "ExpSigmoid")
    ("cedar.aux.math.HeavisideSigmoid","HeavisideSigmoid")
    ("cedar.aux.math.LinearTransferFunction","LinearTransferFunction")
    ("cedar.aux.math.Logarithm","Logarithm")
    ("cedar.aux.math.SemiLinearTransferFunction","SemiLinearTransferFunction");


std::string cedar::proc::GroupXMLFileFormatV1::bimapNameLookupXML(boost::bimap<std::string, std::string> bimap, std::string name, bool directionCedarToXML)
{
  if(directionCedarToXML)
  {
    boost::bimap<std::string, std::string>::left_const_iterator iter = bimap.left.find(name);
    if(iter != bimap.left.end())
    {
      return iter->second;
    }
  }
  else
  {
    boost::bimap<std::string, std::string>::right_const_iterator iter = bimap.right.find(name);
    if(iter != bimap.right.end())
    {
      return iter->second;
    }
  }
  return name;
}


void cedar::proc::GroupXMLFileFormatV1::writeKernelListParameter(
  cedar::aux::ObjectListParameterTemplate<cedar::aux::kernel::Kernel>* kernels, cedar::aux::ConfigurationNode& root)
{
  cedar::aux::ConfigurationNode sumWeightPattern;
  // Write configuration of each kernel
  for(int i = 0; i < kernels->size(); i++)
  {
    cedar::aux::ConfigurationNode gaussWeightPattern;
    cedar::aux::kernel::KernelPtr kernel = kernels->at(i);
    // This must be a Gauss kernel, otherwise the architecture would not be exportable
    auto gauss = dynamic_cast<cedar::aux::kernel::Gauss*>(kernel.get());
    CEDAR_ASSERT(gauss != nullptr);
    gaussWeightPattern.add("Height", gauss->getAmplitude());
    // Write sigma data
    if(gauss->getDimensionality() > 0)
    {
      cedar::aux::ConfigurationNode sigma;
      std::ostringstream sigmaString;
      sigmaString << gauss->getSigma(0);
      for(int j = 1; j < gauss->getDimensionality(); j++)
      {
        sigmaString << ",";
        sigmaString << gauss->getSigma(j);
      }
      gaussWeightPattern.add("Sigmas", sigmaString.str());
    }
    sumWeightPattern.add_child("GaussWeightPattern", gaussWeightPattern);
  }
  root.add_child("SumWeightPattern", sumWeightPattern);
}

void cedar::proc::GroupXMLFileFormatV1::readKernelListParameter(
    cedar::aux::ObjectListParameterTemplate<cedar::aux::kernel::Kernel>* kernels, const cedar::aux::ConfigurationNode& root)
{
  cedar::aux::ConfigurationNode sumWeightPattern = root.get_child("InteractionKernel.SumWeightPattern");

  //Clear the single kernel in the list by default
  kernels->clear();

  int dimensionCounter = 0;

  for(auto iter = sumWeightPattern.begin(); iter != sumWeightPattern.end(); iter++)
  {
    cedar::aux::ConfigurationNode gaussWeightPattern = iter->second;

    //Create a new kernel that is pushed onto the ObjectListParameterTemplate
    cedar::aux::kernel::GaussPtr kernel (new cedar::aux::kernel::Gauss());

    //Get the "height" (in cedar "amplitude")
    kernel->setAmplitude(gaussWeightPattern.get<double>("Height"));

    //The sigma string is concatinated by ",", therefore split them and set the Sigma of the kernel
    std::vector<std::string> sigmas;
    cedar::aux::split(gaussWeightPattern.get<std::string>("Sigmas"), ",", sigmas);

    for(int i = 0; i < sigmas.size(); i++)
    {
      kernel->setSigma(i,std::stod(sigmas[i]));
    }

    kernels->pushBack(kernel);
  }
}

void cedar::proc::GroupXMLFileFormatV1::writeActivationFunctionParameter(
  cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction>* sigmoid, cedar::aux::ConfigurationNode& root)
{
  cedar::aux::ConfigurationNode activationFunction;
  cedar::aux::math::TransferFunctionPtr transferFunction = sigmoid->getValue();
  cedar::aux::NumericParameter<double>* beta = dynamic_cast<cedar::aux::NumericParameter<double>*>(
            transferFunction->getParameter("beta").get());
  CEDAR_ASSERT(beta != nullptr)
  activationFunction.put("Beta", beta->getValue());
  root.add_child("ActivationFunction", activationFunction);
  root.add("ActivationFunction.<xmlattr>.type", cedar::proc::GroupXMLFileFormatV1::bimapNameLookupXML(
    cedar::proc::GroupXMLFileFormatV1::transferFunctionNameLookupTableXML,
    cedar::aux::math::TransferFunctionManagerSingleton::getInstance()->getTypeId(transferFunction)
    ));
}

void cedar::proc::GroupXMLFileFormatV1::readActivationFunctionParameter(
  cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction>* sigmoid,
  const cedar::aux::ConfigurationNode& root)
{
  //get transfere function and beta values from node
  cedar::aux::ConfigurationNode transfereFunctionNode = root.get_child("ActivationFunction");
  std::string transfereFunctionTypeId = cedar::proc::GroupXMLFileFormatV1::bimapNameLookupXML(
          cedar::proc::GroupXMLFileFormatV1::transferFunctionNameLookupTableXML,
          transfereFunctionNode.get<std::string>("<xmlattr>.type"),
          false);
  double beta = transfereFunctionNode.get<double>("Beta");

  //set the values in the given sigmoid field
  if(transfereFunctionTypeId == "cedar.aux.math.ExpSigmoid")
  {
    // create new exp sigmoid transfere funcion with given parameter (threshhold is not export and therefore set to the
    // default value of 0) and set it to the sigmoid
    sigmoid->setValue(cedar::aux::math::SigmoidPtr(new cedar::aux::math::ExpSigmoid(0.0, beta)));
  }
}

void cedar::proc::GroupXMLFileFormatV1::writeDimensionsParameter(cedar::aux::UIntParameterPtr dimensionality,
  cedar::aux::UIntVectorParameterPtr sizes, std::vector<cedar::aux::math::Limits<double>> sizesRange,
  cedar::aux::ConfigurationNode& root)
{
  cedar::aux::ConfigurationNode dimensions;
  std::vector<unsigned int> sizesVector = sizes->getValue();
  CEDAR_ASSERT(sizesVector.size() == dimensionality->getValue());
  CEDAR_ASSERT(sizesRange.size() == dimensionality->getValue());
  for(int i = 0; i < dimensionality->getValue(); i++)
  {
    cedar::aux::ConfigurationNode dimension = cedar::aux::ConfigurationNode();
    dimension.add("<xmlattr>.name", "dim_" + std::to_string(i));
    dimension.add("<xmlattr>.lower", sizesRange.at(i).getLower());
    dimension.add("<xmlattr>.upper", sizesRange.at(i).getUpper());
    dimension.add("<xmlattr>.size", sizesVector[i]);
    dimensions.add_child("Dimension", dimension);
  }
  root.add_child("Dimensions", dimensions);
}

void cedar::proc::GroupXMLFileFormatV1::readDimensionsParameter(cedar::aux::UIntParameterPtr& dimensionality,
                                                                cedar::aux::UIntVectorParameterPtr& sizes,
                                                                const cedar::aux::ConfigurationNode& node)
{
  std::vector<unsigned int> sizeList;
  unsigned int dimensionalitySize = 0;

  //First find dimensions
  auto dimensions_iter = node.find("Dimensions");
  const cedar::aux::ConfigurationNode& dimensions_value = dimensions_iter->second;

  //Count the dimensionalitySize and add the sizes to the sizeList
  for (auto iter = dimensions_value.begin(); iter != dimensions_value.end(); iter++)
  {
    std::string size = iter->second.get<std::string>("<xmlattr>.size");
    sizeList.push_back(std::stoi(size));
    dimensionalitySize++;
  }

  sizes->setValue(sizeList);
  dimensionality->setValue(dimensionalitySize);
}

void cedar::proc::GroupXMLFileFormatV1::readSteps
(
	cedar::proc::GroupPtr group,
	const cedar::aux::ConfigurationNode& root,
	std::vector<std::string>& exceptions
)
{
	for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin(); iter != root.end(); ++iter)
	{
		const std::string node_name = iter->first;
		std::string class_id = cedar::proc::GroupXMLFileFormatV1::bimapNameLookupXML(
						cedar::proc::GroupXMLFileFormatV1::stepNameLookupTableXML, node_name, false);

		const cedar::aux::ConfigurationNode& step_node = iter->second;

		// find the name of the step here and not using parameter later since the name is included as an xml node
		std::string name = step_node.get<std::string>("<xmlattr>.name");
		bool step_exists = false;

		cedar::proc::ElementPtr element;

		if (name.empty() || !group->nameExists(name))
		{
			try
			{
        element = cedar::proc::ElementManagerSingleton::getInstance()->allocate(class_id);
			}
			catch (cedar::aux::ExceptionBase& e)
			{
				exceptions.push_back(e.exceptionInfo());
			}
		}
		else
		{
      element = group->getElement(name);
			step_exists = true;
		}

    // if this is a step, write this to the configuration tree
    if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(element))
    {
      if
          (
          boost::dynamic_pointer_cast<cedar::proc::sinks::GroupSink>(step)
          || boost::dynamic_pointer_cast<cedar::proc::sources::GroupSource>(step)
          )
      {
        continue;
      }

      if (step)
      {
        try
        {
          step->readConfigurationXML(step_node);
        }
        catch (cedar::aux::ExceptionBase &e)
        {
          exceptions.push_back(e.exceptionInfo());
        }

        if (!step_exists)
        {
          try
          {
            group->add(step, name);
          }
          catch (cedar::aux::ExceptionBase &e)
          {
            exceptions.push_back(e.exceptionInfo());
          }
        }

        step->resetChangedStates(false);
      }
    }
  }
}

void cedar::proc::GroupXMLFileFormatV1::readDataConnections
(
	cedar::proc::GroupPtr group,
	const cedar::aux::ConfigurationNode& root,
	std::vector<std::string>& exceptions
)
{
	for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
			 iter != root.end();
			 ++iter)
	{
		std::string source = iter->second.get<std::string>("Source");
		std::string target = iter->second.get<std::string>("Target");
    // Look for slots to append slot names to the step names
    cedar::proc::ElementPtr sourceElement = group->getElement(source);
    cedar::proc::ElementPtr targetElement = group->getElement(target);

    auto sourceConnectable = dynamic_cast<cedar::proc::Connectable*>(sourceElement.get());
    CEDAR_ASSERT(sourceConnectable != nullptr)
    auto sourceSlots = sourceConnectable->getOrderedDataSlots(DataRole::OUTPUT);
    CEDAR_ASSERT(sourceSlots.size() > 0)

    auto targetConnectable = dynamic_cast<cedar::proc::Connectable*>(targetElement.get());
    CEDAR_ASSERT(targetConnectable != nullptr)
    auto targetSlots = targetConnectable->getOrderedDataSlots(DataRole::INPUT);
    CEDAR_ASSERT(targetSlots.size() > 0)
		try
		{
      group->connectSlots(source + "." + sourceSlots.at(0)->getName(),
                          target + "." + targetSlots.at(0)->getName());
    }
		catch (cedar::aux::ExceptionBase& e)
		{
			std::string info = "Exception occurred while connecting \"" + source + "\" to \"" + target + "\": "
												 + e.exceptionInfo();
			exceptions.push_back(info);
		}
		catch (const std::exception& e)
		{
			std::string info = "Exception occurred while connecting \"" + source + "\" to \"" + target + "\": "
												 + std::string(e.what());
			exceptions.push_back(info);
		}
	}
}