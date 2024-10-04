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
#include "cedar/auxiliaries/math/transferFunctions/ExpSigmoid.h"
#include "cedar/auxiliaries/math/transferFunctions/HeavisideSigmoid.h"
#include "cedar/auxiliaries/math/transferFunctions/LinearTransferFunction.h"
#include "cedar/auxiliaries/math/TransferFunction.h"
#include "cedar/auxiliaries/kernel/Box.h"
#include "cedar/auxiliaries/kernel/Gauss.h"
#include "cedar/auxiliaries/kernel/Kernel.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/processing/sources/GroupSource.h"
#include "cedar/processing/sinks/GroupSink.h"
#include "cedar/processing/sources/GaussInput.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/steps/ComponentMultiply.h"
#include "cedar/processing/steps/Convolution.h"
#include "cedar/processing/steps/Projection.h"
#include "cedar/processing/steps/StaticGain.h"
#include "cedar/processing/steps/Sum.h"
#include "cedar/processing/steps/SynapticConnection.h"

// SYSTEM INCLUDES
#include <boost/assign.hpp>
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
		this->readConnections(group, steps->second, connections->second, exceptions);
	}
}

void cedar::proc::GroupXMLFileFormatV1::write
(
  cedar::proc::ConstGroupPtr group,
  cedar::aux::ConfigurationNode& dftArchitecture
)
{
  this->mBlacklistedSteps.clear();
  cedar::aux::ConfigurationNode root;
  if (group->isLinked())
  {
    // TODO
  }
  else
  {
    this->markBlacklistedSteps(group);
    cedar::aux::ConfigurationNode steps;
    this->writeSteps(group, steps);

    cedar::aux::ConfigurationNode connections;
    this->writeSynapticConnections(group, connections);
    this->writeChainedSynapticConnections(group, connections);
    this->writeDataConnections(group, connections);

    if (!steps.empty())
    {
      root.add_child("Components", steps);
    }
    if (!connections.empty())
    {
      root.add_child("Connections", connections);
    }
  }
  dftArchitecture.add_child("DFTArchitecture", root);
}

void cedar::proc::GroupXMLFileFormatV1::markBlacklistedSteps(cedar::proc::ConstGroupPtr group)
{
  for (auto& name_element_pair : group->getElements())
  {
    auto element = name_element_pair.second;
    if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::steps::ComponentMultiply>(element))
    {
      auto inputSlot = step->getInputSlot("operands");
      CEDAR_ASSERT(inputSlot != nullptr)
      for (auto inCon : inputSlot->getDataConnections())
      {
        cedar::proc::Connectable *connectable = inCon->getSource()->getParentPtr();
        CEDAR_ASSERT(connectable != nullptr)
        if (auto gauss = dynamic_cast<cedar::proc::sources::GaussInput*>(connectable))
        {
          auto gaussOutputSlot = gauss->getOutputSlot("Gauss input");
          bool gaussHasOtherTargets = false;
          for(auto outCon : gaussOutputSlot->getDataConnections())
          {
            if(!dynamic_cast<cedar::proc::steps::ComponentMultiply*>(outCon->getTarget()->getParentPtr()))
            {
              gaussHasOtherTargets = true;
            }
          }
          if(gaussHasOtherTargets || std::find(this->mBlacklistedSteps.begin(), this->mBlacklistedSteps.end(), gauss->getFullPath()) != this->mBlacklistedSteps.end())
          {
            continue;
          }
          this->mBlacklistedSteps.push_back(gauss->getFullPath());
        }
      }
    }
  }
}

void cedar::proc::GroupXMLFileFormatV1::writeSteps
(
	cedar::proc::ConstGroupPtr group,
	cedar::aux::ConfigurationNode& steps
) const
{
  bool hasGroup = false;
  for (auto& name_element_pair : group->getElements())
  {
    auto element = name_element_pair.second;
    // if this is a step, write this to the configuration tree
    if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(element))
    {
      if(this->isStepBlacklisted(step.get()) && !(cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(boost::dynamic_pointer_cast<cedar::proc::Element>(step)).compare("cedar.processing.ComponentMultiply") == 0))
      {
        continue;
      }
      CEDAR_ASSERT(step->isXMLExportable())

      //Node_name is the name later used in the xml and class_name the typeId of cedar
      std::string class_name = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(step);
      std::string node_name = cedar::proc::GroupXMLFileFormatV1::bimapNameLookupXML(
        cedar::proc::GroupXMLFileFormatV1::stepNameLookupTableXML, class_name);

      cedar::aux::ConfigurationNode step_node = cedar::aux::ConfigurationNode();
      //<xmlattr> writes the name into the xml tag (called xml attribute)
      step_node.add("<xmlattr>.name", name_element_pair.first);
      //There is a base writeConfigurationXML function which writes all basic fields into the step node.
      //However, some steps with more special fields overrite this function in their own class (still calling the
      //base class)
      step->writeConfigurationXML(step_node);

      steps.push_back(cedar::aux::ConfigurationNode::value_type(node_name, step_node));
    }
    // Print a warning if the architecture contains groups
    else if (boost::dynamic_pointer_cast<cedar::proc::Group>(element))
    {
      hasGroup = true;
    }
  }
  if(hasGroup)
  {
    cedar::aux::LogSingleton::getInstance()->warning
      (
        "Groups are not xml-exportable. Groups and all steps within are not considered for the XML export.",
        "cedar::proc::GroupXMLFileFormatV1::writeSteps()"
      );
  }
}

void cedar::proc::GroupXMLFileFormatV1::readSteps(cedar::proc::GroupPtr group,
                                                  const cedar::aux::ConfigurationNode& root,
                                                  std::vector<std::string>& exceptions)
{
  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin(); iter != root.end(); ++iter)
  {
    // Get name, class id and step node
    const std::string node_name = iter->first;
    std::string class_id = cedar::proc::GroupXMLFileFormatV1::bimapNameLookupXML(
      cedar::proc::GroupXMLFileFormatV1::stepNameLookupTableXML, node_name, false);
    const cedar::aux::ConfigurationNode &stepNode = iter->second;

    // find the name of the step here and not using parameter later since the name is included as an xml node
    std::string name = stepNode.get<std::string>("<xmlattr>.name");

    // Create the step
    cedar::proc::StepPtr step = this->createStep(name, class_id, group, stepNode, exceptions);
    CEDAR_ASSERT(step != nullptr)
    try
    {
      // Analog to writeSteps. The base readConfiguration reads only the fields which are set in the whitelist of
      // each step. Everything else is done by the steps itself in their own overwritten readConfiguration function
      step->readConfigurationXML(stepNode);
    }
    catch (cedar::aux::ExceptionBase &e)
    {
      exceptions.push_back(e.exceptionInfo());
    }
  }
}

cedar::proc::StepPtr cedar::proc::GroupXMLFileFormatV1::createStep(std::string name, std::string class_id, cedar::proc::GroupPtr group,
const cedar::aux::ConfigurationNode& stepNode, std::vector<std::string>& exceptions)
{
  cedar::proc::ElementPtr element;
  bool step_exists = false;

  //Check if step exists, if not allocate a new one if yes then just get the existing
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
    if(boost::dynamic_pointer_cast<cedar::proc::sinks::GroupSink>(step) ||
            boost::dynamic_pointer_cast<cedar::proc::sources::GroupSource>(step))
    {
      return nullptr;
    }

    if (step)
    {

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
    return step;
  }
  return nullptr;
}

bool cedar::proc::GroupXMLFileFormatV1::isStepBlacklisted(cedar::proc::Connectable* step) const
{
  for(std::string name : this->mBlacklistedSteps)
  {
    if(step->getFullPath().compare(name) == 0)
    {
      return true;
    }
  }

  if(auto stepPtr = step->shared_from_this())
  {
    for (std::string classID : this->mBlacklistedClassIDs)
    {
      if (cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(boost::dynamic_pointer_cast<cedar::proc::Element>(stepPtr)).compare(classID) == 0)
      {
        return true;
      }
    }
  }
  return false;
}

bool cedar::proc::GroupXMLFileFormatV1::isConnectionBlacklisted(cedar::proc::DataConnection* connection) const
{
  return this->isConnectionBlacklisted(connection->getSource()->getParentPtr(), connection->getSource(),
                                connection->getTarget()->getParentPtr(), connection->getTarget());
}

bool cedar::proc::GroupXMLFileFormatV1::isConnectionBlacklisted(cedar::proc::Connectable* source,
                                              cedar::proc::OwnedDataPtr sourceSlot, cedar::proc::Connectable* target,
                                              cedar::proc::ExternalDataPtr targetSlot) const
{
  // Special case: Connections from GaussInput to ComponentMultiply should not be exported
  if(dynamic_cast<cedar::proc::sources::GaussInput*>(source) &&
      dynamic_cast<cedar::proc::steps::ComponentMultiply*>(target))
  {
    return true;
  }
  // Special case: Connections from HebbianConnection on slot "weighted input sum" or to HebbianConnection on slot "target field" should not be exported
  if((cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(boost::dynamic_pointer_cast<cedar::proc::Element>(source->shared_from_this())).compare("cedar.dynamics.HebbianConnection") == 0 &&
      sourceSlot->getName().compare("weighted input sum") == 0) ||
      (cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(boost::dynamic_pointer_cast<cedar::proc::Element>(target->shared_from_this())).compare("cedar.dynamics.HebbianConnection") == 0 &&
       targetSlot->getName().compare("target field") == 0))
  {
    return true;
  }

  // Normal case: If source or target is blacklisted, do not export connection
  return (this->isStepBlacklisted(source) && !(cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(boost::dynamic_pointer_cast<cedar::proc::Element>(source->shared_from_this())).compare("cedar.processing.ComponentMultiply") == 0))
     || (this->isStepBlacklisted(target) && !(cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(boost::dynamic_pointer_cast<cedar::proc::Element>(target->shared_from_this())).compare("cedar.processing.ComponentMultiply") == 0));
}

bool cedar::proc::GroupXMLFileFormatV1::isSynapticConnectionChainExportable(cedar::proc::Connectable* chainSource, std::string& errorMsg)
{
  // Get the source step of the incoming connection
  std::vector<cedar::proc::DataSlotPtr> inputSlots = chainSource->getOrderedDataSlots(cedar::proc::DataRole::INPUT);
  CEDAR_ASSERT(inputSlots.size() > 0)
  cedar::proc::DataSlotPtr inputSlot = inputSlots.at(0);
  if(inputSlot->getDataConnections().size() == 0)
  {
    errorMsg = "Without an incoming connection, this step cannot be chained to a SynapticConnection.";
    return false;
  }
  for(cedar::proc::DataConnectionPtr inputConn : inputSlot->getDataConnections())
  {
    cedar::proc::Connectable *source = inputConn.get()->getSource()->getParentPtr();
    CEDAR_ASSERT(source != nullptr);
    // If the source step is one of the chainable steps, this is not the beginning of the chain => return true
    if (!cedar::proc::GroupXMLFileFormatV1::isChainable(boost::dynamic_pointer_cast<cedar::proc::Element>
                (source->shared_from_this())))
    {
      // This is the beginning of the chain, thus check recursively if this whole chain is exportable
      bool chainExportable = cedar::proc::GroupXMLFileFormatV1::isSynapticConnectionChainExportableRecursive(
        chainSource,
        dynamic_cast<cedar::proc::steps::StaticGain *>(chainSource),
        dynamic_cast<cedar::proc::steps::Convolution *>(chainSource),
        dynamic_cast<cedar::proc::steps::Projection *>(chainSource));
      if (!chainExportable)
      {
        errorMsg = "Cannot export chain of steps as SynapticConnection. "
                   "Do you have more than one of the same type of step in the chain?";
        return false;
      }
    }
  }
  return true;
}


bool cedar::proc::GroupXMLFileFormatV1::isSynapticConnectionChainExportableRecursive(cedar::proc::Connectable* chainSource, bool hasStaticGain,
                                         bool hasConvolution, bool hasProjection)
{
  // Get all target steps of outgoing connections
  std::vector<cedar::proc::DataSlotPtr> outputSlots = chainSource->getOrderedDataSlots(cedar::proc::DataRole::OUTPUT);
  CEDAR_ASSERT(outputSlots.size() > 0)
  cedar::proc::DataSlotPtr outputSlot = outputSlots.at(0);
  for(cedar::proc::DataConnectionPtr outputConn : outputSlot->getDataConnections())
  {
    cedar::proc::Connectable *target = outputConn->getTarget()->getParentPtr();
    // If the target is not a chainable step, skip this and return true
    // If the target is chainable, make sure this is the only steo of that type in the chain.
    if(dynamic_cast<cedar::proc::steps::StaticGain*>(target))
    {
      // If there already was a static gain in the chain, or if the rest of the chain is not exportable, return false
      if(hasStaticGain
         || !cedar::proc::GroupXMLFileFormatV1::isSynapticConnectionChainExportableRecursive(target, true,
                                                                                   hasConvolution, hasProjection))
      {
        return false;
      }
    }
    else if(dynamic_cast<cedar::proc::steps::Convolution*>(target))
    {
      if(hasConvolution
         || !cedar::proc::GroupXMLFileFormatV1::isSynapticConnectionChainExportableRecursive(target, hasStaticGain,
                                                                                   true, hasProjection))
      {
        return false;
      }
    }
    else if(dynamic_cast<cedar::proc::steps::Projection*>(target))
    {
      if(hasProjection
         || !cedar::proc::GroupXMLFileFormatV1::isSynapticConnectionChainExportableRecursive(target, hasStaticGain,
                                                                                   hasConvolution, true))
      {
        return false;
      }
    }
    else if(dynamic_cast<cedar::proc::steps::Sum*>(target))
    {
      if(!cedar::proc::GroupXMLFileFormatV1::isSynapticConnectionChainExportableRecursive(target, hasStaticGain,
                                                                                   hasConvolution, hasProjection))
      {
        return false;
      }
    }
  }
  return true;
}

void cedar::proc::GroupXMLFileFormatV1::writeSynapticConnection(cedar::aux::ConfigurationNode& root,
                                                                const cedar::proc::steps::SynapticConnectionPtr connection)
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
    if(this->isConnectionBlacklisted(data_connection.get()))
    {
      continue;
    }
    this->writeDataConnection(root, data_connection);
  }
}

void cedar::proc::GroupXMLFileFormatV1::connectSteps(cedar::proc::GroupPtr group, cedar::proc::Connectable* source, cedar::proc::Connectable* target)
{
  std::string sourceSlot = source->getOrderedDataSlots(DataRole::OUTPUT).at(0)->getName();
  std::string targetSlot = target->getOrderedDataSlots(DataRole::INPUT).at(0)->getName();
  if(cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(boost::dynamic_pointer_cast<cedar::proc::Element>(target->shared_from_this())).compare("cedar.dynamics.HebbianConnection") == 0)
  {
    targetSlot = "source node";
  }
  CEDAR_ASSERT(source->hasSlot(DataRole::OUTPUT, sourceSlot));
  CEDAR_ASSERT(target->hasSlot(DataRole::INPUT, targetSlot));
  group->connectSlots(source->getName() + "." + sourceSlot,
                      target->getName() + "." + targetSlot);
}

void cedar::proc::GroupXMLFileFormatV1::readConnections
(
  cedar::proc::GroupPtr group,
  const cedar::aux::ConfigurationNode& steps,
  const cedar::aux::ConfigurationNode& root,
  std::vector<std::string>& exceptions
)
{
  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
       iter != root.end();
       ++iter)
  {
    // These are used later to determine whether this is a connection with special synaptic properties or not
    auto kernelWeights = iter->second.find("KernelWeights");
    auto pointWiseWeights = iter->second.find("PointWiseWeights");
    auto scalarWeight = iter->second.find("ScalarWeight");
    auto dimensionMapping = iter->second.find("DimensionMapping");

    std::string source = iter->second.get<std::string>("Source");
    std::string target = iter->second.get<std::string>("Target");
    // Look for slots to append slot names to the step names
    cedar::proc::ElementPtr sourceElement = group->getElement(source);
    cedar::proc::ElementPtr targetElement = group->getElement(target);

    auto sourceConnectable = dynamic_cast<cedar::proc::Connectable *>(sourceElement.get());
    CEDAR_ASSERT(sourceConnectable != nullptr)
    auto sourceSlots = sourceConnectable->getOrderedDataSlots(DataRole::OUTPUT);
    CEDAR_ASSERT(sourceSlots.size() > 0)

    auto targetConnectable = dynamic_cast<cedar::proc::Connectable *>(targetElement.get());
    CEDAR_ASSERT(targetConnectable != nullptr)
    auto targetSlots = targetConnectable->getOrderedDataSlots(DataRole::INPUT);
    CEDAR_ASSERT(targetSlots.size() > 0)

    bool isSynaptic = kernelWeights    != iter->second.not_found() || pointWiseWeights != iter->second.not_found() ||
                      scalarWeight     != iter->second.not_found() || dimensionMapping != iter->second.not_found();
    try
    {
      if (isSynaptic)
      {
        std::string conName;
        cedar::proc::StepPtr conStep;
        // Insert Synaptic step between source and target
        conName = group->getUniqueIdentifier("Synaptic Connection");
        conStep = this->createStep(conName, "cedar.processing.SynapticConnection", group, iter->second, exceptions);

        // Get the input and output slot of the connection step
        auto conConnectable = dynamic_cast<cedar::proc::Connectable *>(conStep.get());
        CEDAR_ASSERT(conConnectable != nullptr)

        auto conInputSlots = conConnectable->getOrderedDataSlots(DataRole::INPUT);
        auto conOutputSlots = conConnectable->getOrderedDataSlots(DataRole::OUTPUT);
        CEDAR_ASSERT(conInputSlots.size() > 0)
        CEDAR_ASSERT(conOutputSlots.size() > 0)

        this->connectSteps(group, sourceConnectable, conConnectable);
        this->connectSteps(group, conConnectable, targetConnectable);
        try
        {
          // Analog to writeSteps. The base readConfiguration reads only the fields which are set in the whitelist of
          // each step. Everything else is done by the steps itself in their own overwritten readConfiguration function
          conStep->readConfigurationXML(iter->second);
        }
        catch (cedar::aux::ExceptionBase &e)
        {
          exceptions.push_back(e.exceptionInfo());
        }
      }
      else
      {
        // No special properties, insert normal connection
        this->connectSteps(group, sourceConnectable, targetConnectable);
      }

      // For HebbianConnections some extra connections have to be added depending on the configuration of the step
      bool hebbianIsTarget = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(targetElement).compare("cedar.dynamics.HebbianConnection") == 0;
      bool hebbianIsSource = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(sourceElement).compare("cedar.dynamics.HebbianConnection") == 0;
      cedar::proc::Connectable* hebbian = sourceConnectable;
      if(hebbianIsTarget)
      {
        hebbian = targetConnectable;
      }
      if(hebbianIsSource || hebbianIsTarget)
      {
        // Find HebbianConnection XML properties
        cedar::aux::ConfigurationNode stepNode;
        for (cedar::aux::ConfigurationNode::const_iterator iter = steps.begin(); iter != steps.end(); ++iter)
        {
          // find the name of the step here and not using parameter later since the name is included as an xml node
          std::string name = iter->second.get<std::string>("<xmlattr>.name");
          if(hebbian->getName().compare(name) == 0)
          {
            stepNode = iter->second;
            break;
          }
        }
        CEDAR_ASSERT(stepNode.get_optional<std::string>("<xmlattr>.name").has_value())

        if(hebbianIsTarget)
        {
          auto bidir = stepNode.get_optional<std::string>("bidir");
          if (bidir.has_value())
          {
            if (bidir.value().compare("TRUE") == 0)
            {
              bool has_gain = false;
              auto weightedInputSumGainStr = stepNode.get_optional<std::string>("WeightedInputSumGain");
              if (weightedInputSumGainStr.has_value())
              {
                double weightedInputSumGain;
                std::istringstream(weightedInputSumGainStr.get()) >> weightedInputSumGain;
                if (weightedInputSumGain != 1)
                {
                  has_gain = true;
                  // Create the step
                  std::string staticGainName = group->getUniqueIdentifier("Static Gain");
                  auto staticGain = dynamic_cast<cedar::proc::steps::StaticGain *>(this->createStep(staticGainName,
                                  "cedar.processing.StaticGain", group, cedar::aux::ConfigurationNode(), exceptions).get());
                  CEDAR_ASSERT(staticGain != nullptr)
                  staticGain->setGainFactor(weightedInputSumGain);
                  group->connectSlots(hebbian->getName() + ".weighted input sum",
                          staticGainName + "." + staticGain->getOrderedDataSlots(DataRole::INPUT).at(0)->getName());
                  group->connectSlots(staticGainName + "." + staticGain->getOrderedDataSlots(DataRole::OUTPUT).at(0)->getName(),
                          sourceConnectable->getName() + "." + sourceConnectable->getOrderedDataSlots(DataRole::INPUT).at(0)->getName());
                }
              }
              if (!has_gain)
              {
                group->connectSlots(hebbian->getName() + ".weighted input sum",
                            sourceConnectable->getName() + "." + sourceConnectable->getOrderedDataSlots(DataRole::INPUT).at(0)->getName());
              }
            }
          }
        }
        else // hebbian is source
        {
          bool has_gain = false;
          auto targetFieldGainStr = stepNode.get_optional<std::string>("TargetFieldGain");
          if (targetFieldGainStr.has_value())
          {
            double targetFieldGain;
            std::istringstream(targetFieldGainStr.get()) >> targetFieldGain;
            if (targetFieldGain != 1)
            {
              has_gain = true;
              // Create the step
              std::string staticGainName = group->getUniqueIdentifier("Static Gain");
              auto staticGain = dynamic_cast<cedar::proc::steps::StaticGain *>(this->createStep(staticGainName,
                                                                                                "cedar.processing.StaticGain",
                                                                                                group,
                                                                                                cedar::aux::ConfigurationNode(),
                                                                                                exceptions).get());
              CEDAR_ASSERT(staticGain != nullptr)
              staticGain->setGainFactor(targetFieldGain);
              group->connectSlots(targetConnectable->getName() + "." + targetConnectable->getOrderedDataSlots(DataRole::OUTPUT).at(0)->getName(),
                        staticGainName + "." + staticGain->getOrderedDataSlots(DataRole::INPUT).at(0)->getName());
              group->connectSlots(staticGainName + "." + staticGain->getOrderedDataSlots(DataRole::OUTPUT).at(0)->getName(),
                        hebbian->getName() + ".target field");
            }
          }
          if (!has_gain)
          {
            group->connectSlots(targetConnectable->getName() + "." + targetConnectable->getOrderedDataSlots(DataRole::OUTPUT).at(0)->getName(),
                          hebbian->getName() + ".target field");
          }
        }
      }
    }
    catch (cedar::aux::ExceptionBase &e)
    {
      std::string info = "Exception occurred while connecting \"" + source + "\" to \"" + target + "\": "
                         + e.exceptionInfo();
      exceptions.push_back(info);
    }
    catch (const std::exception &e)
    {
      std::string info = "Exception occurred while connecting \"" + source + "\" to \"" + target + "\": "
                         + std::string(e.what());
      exceptions.push_back(info);
    }
  }
}

std::vector<std::string> cedar::proc::GroupXMLFileFormatV1::chainableSteps = {"cedar.processing.steps.Convolution",
          "cedar.processing.steps.Sum", "cedar.processing.Projection", "cedar.processing.StaticGain"};

boost::bimap<std::string, std::string> cedar::proc::GroupXMLFileFormatV1::stepNameLookupTableXML =
  boost::assign::list_of< boost::bimap<std::string, std::string>::relation >
    ("cedar.processing.sources.Boost", "Boost")
    ("cedar.dynamics.HebbianConnection", "HebbianConnection")
    ("cedar.dynamics.ComponentMultiply", "HebbianConnection")
    ("cedar.dynamics.NeuralField", "Field")
    ("cedar.processing.sources.GaussInput", "GaussInput")
    ("cedar.dynamics.Preshape", "Preshape")
    ("cedar.processing.sources.SpatialTemplate", "SpatialTemplate");

boost::bimap<std::string, std::string> cedar::proc::GroupXMLFileFormatV1::transferFunctionNameLookupTableXML =
  boost::assign::list_of< boost::bimap<std::string, std::string>::relation >
    ("cedar.aux.math.AbsSigmoid", "AbsSigmoid")
    ("cedar.aux.math.ExpSigmoid", "ExpSigmoid")
    ("cedar.aux.math.HeavisideSigmoid","HeavisideSigmoid");

std::vector<std::string> cedar::proc::GroupXMLFileFormatV1::mBlacklistedClassIDs =
    {"cedar.processing.sinks.GroupSink", "cedar.processing.sources.GroupSource", "cedar.processing.SynapticConnection",
     "cedar.processing.ComponentMultiply", "cedar.processing.StaticGain",
     "cedar.processing.steps.Convolution", "cedar.processing.Projection", "cedar.processing.steps.Sum"};


std::string cedar::proc::GroupXMLFileFormatV1::bimapNameLookupXML(boost::bimap<std::string, std::string> bimap, std::string name, bool directionCedarToXML)
{
  if(directionCedarToXML)
  {
    // Special unidirectional translations
    if(name.compare("cedar.processing.ComponentMultiply") == 0)
    {
      name = "cedar.dynamics.HebbianConnection";
    }
    // Normal translations
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
    // This must be a Gauss or Box kernel, otherwise the architecture would not be exportable
    double height;
    std::ostringstream sigmaString;
    if(auto gauss = dynamic_cast<cedar::aux::kernel::Gauss*>(kernel.get()))
    {
      height = gauss->getDenormalizedAmplitude();
      // Write sigma data
      if(gauss->getDimensionality() > 0)
      {
        cedar::aux::ConfigurationNode sigma;

        sigmaString << gauss->getSigma(0);
        for(int j = 1; j < gauss->getDimensionality(); j++)
        {
          sigmaString << ",";
          sigmaString << gauss->getSigma(j);
        }
      }
    }
    else
    {
      auto box = dynamic_cast<cedar::aux::kernel::Box*>(kernel.get());
      CEDAR_ASSERT(box != nullptr);
      height = box->getAmplitude();
      sigmaString << "0";
    }
    gaussWeightPattern.add("Sigmas", sigmaString.str());
    gaussWeightPattern.add("Height", height);
    sumWeightPattern.add_child("GaussWeightPattern", gaussWeightPattern);
  }
  root.add_child("SumWeightPattern", sumWeightPattern);
}

void cedar::proc::GroupXMLFileFormatV1::readKernelListParameter(
    cedar::aux::ObjectListParameterTemplate<cedar::aux::kernel::Kernel>* kernels,
    const cedar::aux::ConfigurationNode& root)
{
  cedar::aux::ConfigurationNode sumWeightPattern = root.get_child("SumWeightPattern");

  //Clear the single kernel in the list by default
  kernels->clear();

  for(auto iter = sumWeightPattern.begin(); iter != sumWeightPattern.end(); iter++)
  {
    cedar::aux::ConfigurationNode gaussWeightPattern = iter->second;

    //The sigma string is concatinated by ",", therefore split them and set the Sigma of the kernel
    std::vector<std::string> sigmas;
    cedar::aux::split(gaussWeightPattern.get<std::string>("Sigmas"), ",", sigmas);

    //Create a new kernel that is pushed onto the ObjectListParameterTemplate
    cedar::aux::kernel::GaussPtr kernel (new cedar::aux::kernel::Gauss(sigmas.size()));

    //Get the "height" (in cedar "amplitude")
    kernel->setAmplitude(gaussWeightPattern.get<double>("Height"));

    // Set normalized to false
    kernel->setNormalize(false);

    for(int i = 0; i < sigmas.size(); i++)
    {
      kernel->setSigma(i,std::stod(sigmas[i]));
    }
    kernels->pushBack(kernel);
  }
}

void cedar::proc::GroupXMLFileFormatV1::readProjectionMappingsParameter(
  cedar::proc::ProjectionMappingParameterPtr& mappingParameter, const cedar::aux::ConfigurationNode& root)
{
  unsigned int outputDimensionality = 1;
  // Calculate output dimensionality by max{to-dimensions}
  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
       iter != root.end();
       ++iter)
  {
    if(iter->second.get<std::string>("<xmlattr>.to").compare("drop"))
    {
      unsigned int to = iter->second.get<int>("<xmlattr>.to");
      if(to >= outputDimensionality)
      {
        outputDimensionality = to + 1;
      }
    }
  }
  mappingParameter->setOutputDimensionality(outputDimensionality);

  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
     iter != root.end();
     ++iter)
  {
    unsigned int from = iter->second.get<int>("<xmlattr>.from");
    if (!iter->second.get<std::string>("<xmlattr>.to").compare("drop"))
    {
      mappingParameter->drop(from);
    }
    else
    {
      mappingParameter->changeMapping(from, iter->second.get<int>("<xmlattr>.to"));
    }
  }
}

void cedar::proc::GroupXMLFileFormatV1::writeActivationFunctionParameter(
        cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction>* sigmoid,
        cedar::aux::ConfigurationNode& root, const std::string & name)
{
  cedar::aux::ConfigurationNode activationFunction;

  // get transfer function and beta parameter
  cedar::aux::math::TransferFunctionPtr transferFunction = sigmoid->getValue();
  double beta = 0;
  if(dynamic_cast<cedar::aux::math::HeavisideSigmoid*>(transferFunction.get()))
  {
    beta = 9001;
  }
  else if(dynamic_cast<cedar::aux::math::LinearTransferFunction*>(transferFunction.get()))
  {
    // Should only be the case for HebbianConnections
    beta = 100;
  }
  else
  {
    cedar::aux::NumericParameter<double>* betaParam = dynamic_cast<cedar::aux::NumericParameter<double>*>(
      transferFunction->getParameter("beta").get());
    CEDAR_ASSERT(betaParam != nullptr)
    beta = betaParam->getValue();
  }
  activationFunction.put("Beta", beta);
  // add beta parameter to node
  root.add_child(name, activationFunction);

  // add transfer function name to xml attribute
  root.add(name + ".<xmlattr>.type", "ExpSigmoid");
  // We are hard-coding ExpSigmoid here as we are exporting Abs and HeavisideSigmoid as an approximation of ExpSigmoid
  // If this should change in the future, use the next line to properly map type-ids to xml names
  /*root.add(name + ".<xmlattr>.type", cedar::proc::GroupXMLFileFormatV1::bimapNameLookupXML(
          cedar::proc::GroupXMLFileFormatV1::transferFunctionNameLookupTableXML,
          cedar::aux::math::TransferFunctionManagerSingleton::getInstance()->getTypeId(transferFunction)
  ));*/
}

void cedar::proc::GroupXMLFileFormatV1::readActivationFunctionParameter(
  cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction>* sigmoid,
  const cedar::aux::ConfigurationNode& root, const std::string& name)
{
  //get transfer function and beta values from node
  cedar::aux::ConfigurationNode transferFunctionNode = root.get_child(name);
  std::string transferFunctionTypeId = cedar::proc::GroupXMLFileFormatV1::bimapNameLookupXML(
          cedar::proc::GroupXMLFileFormatV1::transferFunctionNameLookupTableXML,
          transferFunctionNode.get<std::string>("<xmlattr>.type"),
          false);
  double beta = transferFunctionNode.get<double>("Beta");

  //set the values in the given sigmoid field
  if(transferFunctionTypeId == "cedar.aux.math.ExpSigmoid")
  {
    // create new exp sigmoid transfere funcion with given parameter (threshhold is not export and therefore set to the
    // default value of 0) and set it to the sigmoid
    sigmoid->setValue(cedar::aux::math::SigmoidPtr(new cedar::aux::math::ExpSigmoid(0.0, beta)));
  }
}

void cedar::proc::GroupXMLFileFormatV1::writeDimensionsParameter(cedar::aux::UIntParameterPtr dimensionality,
  cedar::aux::UIntVectorParameterPtr sizes, std::vector<cedar::aux::math::Limits<double>> sizesRange,
  cedar::aux::ConfigurationNode& root, const std::string & name)
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
  root.add_child(name, dimensions);
}

void cedar::proc::GroupXMLFileFormatV1::writeChainedSynapticConnection(cedar::proc::Connectable* source,
                                        cedar::proc::OwnedDataPtr sourceSlot, cedar::proc::Connectable* connectable,
                                        cedar::aux::ConfigurationNode synCon, cedar::aux::ConfigurationNode& root) const
{
  // write configuration to synCon
  // Make sure another step of the same class has not already been added to the current chain previously, however,
  // this should never happen because the isXMLExportable() function should take care of checking this beforehand.
  if (dynamic_cast<cedar::proc::steps::StaticGain*>(connectable))
  {
    auto child = synCon.get_child_optional("ScalarWeight");
    CEDAR_ASSERT(!child)
    connectable->writeConfigurationXML(synCon);
  }
  else if (dynamic_cast<cedar::proc::steps::Convolution*>(connectable))
  {
    auto child = synCon.get_child_optional("KernelWeights");
    CEDAR_ASSERT(!child)
    connectable->writeConfigurationXML(synCon);
  }
  else if (dynamic_cast<cedar::proc::steps::Projection*>(connectable))
  {
    auto child = synCon.get_child_optional("DimensionMapping");
    CEDAR_ASSERT(!child)
    connectable->writeConfigurationXML(synCon);
  }
  else if (!dynamic_cast<cedar::proc::steps::Sum*>(connectable))
  {
    return;
  }

  // Get the target step of any outgoing connections
  std::vector<cedar::proc::DataSlotPtr> outputSlots = connectable->getOrderedDataSlots(DataRole::OUTPUT);
  CEDAR_ASSERT(outputSlots.size() > 0)
  cedar::proc::DataSlotPtr outputSlot = outputSlots.at(0);
  for(cedar::proc::DataConnectionPtr outputConn : outputSlot->getDataConnections())
  {
    cedar::proc::Connectable *target = outputConn->getTarget()->getParentPtr();
    if (cedar::proc::GroupXMLFileFormatV1::isChainable(boost::dynamic_pointer_cast<cedar::proc::Element>(target->shared_from_this())))
    {
      // Pass the synCon node on along the chain
      this->writeChainedSynapticConnection(source, sourceSlot, target, synCon, root);
    }
    else
    {
      // This is the end of the chain, write the target of the synaptic connection
      if(this->isConnectionBlacklisted(source, sourceSlot, target, outputConn->getTarget()))
      {
        continue;
      }
      cedar::aux::ConfigurationNode connection_node(synCon);
      if(auto targetElem = dynamic_cast<cedar::proc::Element*>(target))
      {
        connection_node.put("Target", targetElem->getFullPath());
      }
      root.push_back(cedar::aux::ConfigurationNode::value_type("SynapticConnection", connection_node));
    }
  }
}

void cedar::proc::GroupXMLFileFormatV1::writeChainedSynapticConnections(cedar::proc::ConstGroupPtr group,
                                                                        cedar::aux::ConfigurationNode& root) const
{
  // Find all steps that are chainable
  for (auto& name_element_pair : group->getElements())
  {
    auto element = name_element_pair.second;
    if (auto step = boost::dynamic_pointer_cast<cedar::proc::Step>(element))
    {
      // Check if this is a step that belongs to a chain that can be merged into a SynapticConnection
      if (this->isChainable(step))
      {
        CEDAR_ASSERT(step->isXMLExportable())

        // Check if this is the first step of the chain
        std::vector<cedar::proc::DataSlotPtr> inputSlots = step->getOrderedDataSlots(DataRole::INPUT);
        CEDAR_ASSERT(inputSlots.size() > 0)
        cedar::proc::DataSlotPtr inputSlot = inputSlots.at(0);
        for (cedar::proc::DataConnectionPtr inputConn : inputSlot->getDataConnections())
        {
          cedar::proc::Connectable *source = inputConn.get()->getSource()->getParentPtr();
          CEDAR_ASSERT(source != nullptr);
          if (!this->isChainable(boost::dynamic_pointer_cast<cedar::proc::Element>(source->shared_from_this())))
          {
            // First step of the chain.
            cedar::aux::ConfigurationNode synCon;
            synCon.put("Source", source->getFullPath());
            this->writeChainedSynapticConnection(source, inputConn.get()->getSource(), step.get(), synCon, root);
          }
        }
      }
    }
  }
}

bool cedar::proc::GroupXMLFileFormatV1::isChainable(cedar::proc::ElementPtr element)
{
  std::string typeId = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(element);
  for(std::string chainable : cedar::proc::GroupXMLFileFormatV1::chainableSteps)
  {
    if(typeId == chainable)
    {
      return true;
    }
  }
  return false;
}

void cedar::proc::GroupXMLFileFormatV1::readDimensionsParameter(cedar::aux::UIntParameterPtr dimensionality,
                                                              cedar::aux::UIntVectorParameterPtr sizes,
                                                              std::vector<cedar::aux::math::Limits<double>>& sizesRange,
                                                              const cedar::aux::ConfigurationNode& node,  const std::string & name)
{
  std::vector<unsigned int> sizeList;
  unsigned int dimensionalitySize = 0;
  sizesRange.clear();

  //First find dimensions
  auto dimensions_iter = node.find(name);
  const cedar::aux::ConfigurationNode& dimensions_value = dimensions_iter->second;

  //Count the dimensionalitySize and add the sizes to the sizeList
  for (auto iter = dimensions_value.begin(); iter != dimensions_value.end(); iter++)
  {
    std::string size = iter->second.get<std::string>("<xmlattr>.size");
    sizeList.push_back(std::stoi(size));
    double lower = iter->second.get<double>("<xmlattr>.lower");
    double upper = iter->second.get<double>("<xmlattr>.upper");
    sizesRange.push_back(cedar::aux::math::Limits<double>(lower, upper));
    dimensionalitySize++;
  }

  sizes->setValue(sizeList);
  dimensionality->setValue(dimensionalitySize);
}
