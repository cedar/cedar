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
#include "cedar/auxiliaries/kernel/Gauss.h"

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

	auto connections = root.find("Connections");
	if (connections != root.not_found())
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
      )
      {
        continue;
      }
      CEDAR_ASSERT(step->isXMLExportable());
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
  root.add_child("InteractionKernel.SumWeightPattern", sumWeightPattern);
}

void cedar::proc::GroupXMLFileFormatV1::writeActivationFunctionParameter(
  cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction>* sigmoid, cedar::aux::ConfigurationNode& root)
{
  cedar::aux::ConfigurationNode activationFunction;
  cedar::aux::math::TransferFunctionPtr transferFunction = sigmoid->getValue();
  transferFunction->writeConfigurationXML(activationFunction, true);
  root.add_child("ActivationFunction", activationFunction);
  root.add("ActivationFunction.<xmlattr>.type", cedar::proc::GroupXMLFileFormatV1::bimapNameLookupXML(
    cedar::proc::GroupXMLFileFormatV1::transferFunctionNameLookupTableXML,
    cedar::aux::math::TransferFunctionManagerSingleton::getInstance()->getTypeId(transferFunction)
    ));
}

void cedar::proc::GroupXMLFileFormatV1::writeDimensionsParameter(cedar::aux::UIntParameterPtr dimensionality,
  cedar::aux::UIntVectorParameterPtr sizes, cedar::aux::ConfigurationNode& root)
{
  cedar::aux::ConfigurationNode dimensions;
  std::vector<unsigned int> sizesVector = sizes->getValue();
  CEDAR_ASSERT(sizesVector.size() == dimensionality->getValue());
  for(int i = 0; i < dimensionality->getValue(); i++)
  {
    cedar::aux::ConfigurationNode dimension = cedar::aux::ConfigurationNode();
    dimension.add("<xmlattr>.name", "dim_" + std::to_string(i));
    dimension.add("<xmlattr>.lower", 0);
    dimension.add("<xmlattr>.upper", sizesVector[i] - 1);
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
	for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
			 iter != root.end();
			 ++iter)
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
		std::string source = iter->second.get<std::string>("source");
		std::string target = iter->second.get<std::string>("target");
		try
		{
			group->connectSlots(source, target);
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