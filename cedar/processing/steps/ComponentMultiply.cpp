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

    File:        ComponentMultiply.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 12 22

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/ComponentMultiply.h"
#include "cedar/dynamics/fields/NeuralField.h"
#include "cedar/processing/sources/GaussInput.h"
#include "cedar/processing/typecheck/SameSizedCollection.h"
#include "cedar/processing/typecheck/SameTypeCollection.h"
#include "cedar/processing/typecheck/And.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/GroupXMLFileFormatV1.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/Arguments.h"
#include "cedar/auxiliaries/annotation/SizesRangeHint.h"
#include "cedar/auxiliaries/math/transferFunctions/HeavisideSigmoid.h"
#include "cedar/auxiliaries/math/transferFunctions/LinearTransferFunction.h"
#include "cedar/auxiliaries/DataTemplate.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/utilities.h"

// SYSTEM INCLUDES
#include <boost/make_shared.hpp>


//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr multiply_decl
    (
      new ElementDeclarationTemplate<cedar::proc::steps::ComponentMultiply>
      (
        "Algebra",
        "cedar.processing.ComponentMultiply"
      )
    );
    multiply_decl->setIconPath(":/steps/component_multiply.svg");
    multiply_decl->setDescription
    (
      "Multiplies two tensors element-wise."
    );

    multiply_decl->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ComponentMultiply::ComponentMultiply()
:
mOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F)))
{
  auto input = this->declareInputCollection("operands");
  cedar::proc::typecheck::SameSizedCollection size_check(true);
  cedar::proc::typecheck::SameTypeCollection type_check;
  cedar::proc::typecheck::And sum_check;
  sum_check.addCheck(size_check);
  sum_check.addCheck(type_check);
  input->setCheck(sum_check);
  this->declareOutput("product", mOutput);

  this->mInputs = boost::dynamic_pointer_cast<cedar::proc::ExternalData>(this->getInputSlot("operands"));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::ComponentMultiply::inputConnectionChanged(const std::string& inputName)
{
  cedar::proc::ConstExternalDataPtr slot = this->getInputSlot(inputName);

  // first, check if all inputs are valid
  if (!this->allInputsValid())
  {
    return;
  }

  // Find the first non-0d input. If all are 0d, just use the last one as a "template".
  cv::Mat in_mat;
  for (unsigned int i = 0; i < this->mInputs->getDataCount(); ++i)
  {
    if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(slot->getData(i)))
    {
      in_mat = mat_data->getData().clone();
      if (mat_data->getDimensionality() != 0)
      {
        break;
      }
    }
  }

  if (!in_mat.empty())
  {
    mOutput->getData() = in_mat.clone();
    this->callComputeWithoutTriggering();
  }
  this->emitOutputPropertiesChangedSignal("product");
  this->onTrigger();
}

void cedar::proc::steps::ComponentMultiply::compute(const cedar::proc::Arguments&)
{
  cv::Mat& prod = mOutput->getData();
  prod = cv::Scalar(1);

  for (unsigned int i = 0; i < this->mInputs->getDataCount(); ++i)
  {
    cedar::aux::MatDataPtr mat_data = boost::dynamic_pointer_cast<cedar::aux::MatData>(this->mInputs->getData(i));
    if (!mat_data)
    {
      continue;
    }
    cv::Mat input = mat_data->getData();

    if (!mat_data->isEmpty())
    {
      if (mat_data->getDimensionality() == 0)
      {
        prod = cedar::aux::math::getMatrixEntry<double>(input, 0) * prod;
      }
      else
      {
        prod = prod.mul(input);
      }
    }
  }
}

bool cedar::proc::steps::ComponentMultiply::isXMLExportable(std::string& errorMsg)
{
  std::vector<cedar::proc::DataConnectionPtr> inCons = this->mInputs->getDataConnections();
  if(inCons.size() == 2)
  {
    bool gauss = false;
    bool node = false;
    for (int i = 0; i < inCons.size(); i++)
    {
      cedar::proc::Connectable *connectable = inCons.at(i)->getSource()->getParentPtr();
      CEDAR_ASSERT(connectable != nullptr)
      if (dynamic_cast<cedar::proc::sources::GaussInput *>(connectable))
      {
        gauss = true;
      }
      if (auto neuralField = dynamic_cast<cedar::dyn::NeuralField *>(connectable))
      {
        if (neuralField->getDimensionality() == 0)
        {
          node = true;
        }
      }
    }
    if(gauss && node)
    {
      return true;
    }
  }
  errorMsg = "ComponentMultiply is only exportable with one incoming GaussInput and one incoming Node.";
  return false;
}

void cedar::proc::steps::ComponentMultiply::writeConfigurationXML(cedar::aux::ConfigurationNode& root) const
{
  cedar::aux::Configurable::writeConfigurationXML(root);
  root.put("LearningRate", 0.01);
  root.put("FixedRewardDuration", false);
  root.put("RewardDuration", 200);
  root.put("ManualWeights", true);

  std::vector<cedar::proc::DataConnectionPtr> inCons = this->mInputs->getDataConnections();
  CEDAR_ASSERT(inCons.size() == 2)
  for (int i = 0; i < inCons.size(); i++)
  {
    cedar::proc::Connectable *connectable = inCons.at(i)->getSource()->getParentPtr();
    CEDAR_ASSERT(connectable != nullptr)
    if (auto gauss = dynamic_cast<cedar::proc::sources::GaussInput *>(connectable))
    {
      for(std::string paramName : {"Centers", "Sigma"})
      {
        auto param = boost::dynamic_pointer_cast<cedar::aux::DoubleVectorParameter>(gauss->getParameter(cedar::aux::toLower(paramName)));
        cedar::aux::ConfigurationNode vector_node;
        for (double value: param->getValue())
        {
          cedar::aux::ConfigurationNode value_node;
          value_node.put_value(value);
          vector_node.push_back(cedar::aux::ConfigurationNode::value_type("Weight" + paramName + "Elem", value_node));
        }
        root.push_back(cedar::aux::ConfigurationNode::value_type("Weight" + paramName, vector_node));
      }
      root.put("WeightAmplitude", gauss->getAmplitude());
      // dimensionality/sizes parameter
      root.add_child("SourceDimensions", cedar::aux::ConfigurationNode());
      auto output = gauss->getOutput("Gauss input");
      CEDAR_ASSERT(output != nullptr)
      cedar::proc::GroupXMLFileFormatV1::writeDimensionsParameter(boost::dynamic_pointer_cast<cedar::aux::UIntParameter>(
          gauss->getParameter("dimensionality")), boost::dynamic_pointer_cast<cedar::aux::UIntVectorParameter>(gauss->getParameter("sizes")),
          output->getAnnotation<cedar::aux::annotation::SizesRangeHint>()->getRange(), root);
      // Rename parameter to the name conventions of the HebbianConnection Parameters
      root.add_child("TargetDimensions", root.get_child("Dimensions"));
      root.erase("Dimensions");
      break;
    }
  }
  cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction>* pHeavisideSigmoid =
      new cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction>(nullptr, "temp",
      cedar::aux::math::SigmoidPtr(new cedar::aux::math::HeavisideSigmoid()));
  cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction>* pLinearTransferFunction =
      new cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction>(nullptr, "temp",
      cedar::aux::math::LinearTransferFunctionPtr(new cedar::aux::math::LinearTransferFunction()));
  cedar::proc::GroupXMLFileFormatV1::writeActivationFunctionParameter(pHeavisideSigmoid, root, "SourceFunction");
  cedar::proc::GroupXMLFileFormatV1::writeActivationFunctionParameter(pHeavisideSigmoid, root, "RewardFunction");
  cedar::proc::GroupXMLFileFormatV1::writeActivationFunctionParameter(pLinearTransferFunction, root, "TargetFunction");
}
