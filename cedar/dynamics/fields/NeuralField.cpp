/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        NeuralField.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 04

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/dynamics/fields/NeuralField.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/convolution/Convolution.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/math/Sigmoid.h"
#include "cedar/auxiliaries/math/sigmoids/AbsSigmoid.h"
#include "cedar/auxiliaries/kernel/Gauss.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <iostream>
#include <boost/lexical_cast.hpp>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr field_decl
    (
      new cedar::proc::ElementDeclarationTemplate<cedar::dyn::NeuralField>("Fields", "cedar.dynamics.NeuralField")
    );
    field_decl->setIconPath(":/steps/field_temp.svg");
    field_decl->setDescription
    (
      "An implementation of Amari's dynamic neural fields."
    );

    cedar::aux::Singleton<cedar::proc::DeclarationRegistry>::getInstance()->declareClass(field_decl);

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dyn::NeuralField::NeuralField()
:
mActivation(new cedar::aux::MatData(cv::Mat::zeros(50, 50, CV_32F))),
mSigmoidalActivation(new cedar::aux::MatData(cv::Mat::zeros(50, 50, CV_32F))),
mLateralInteraction(new cedar::aux::MatData(cv::Mat::zeros(50, 50, CV_32F))),
mInputSum(new cedar::aux::MatData(cv::Mat::zeros(50, 50, CV_32F))),
mInputNoise(new cedar::aux::MatData(cv::Mat::zeros(50, 50, CV_32F))),
mNeuralNoise(new cedar::aux::MatData(cv::Mat::zeros(50, 50, CV_32F))),
mRestingLevel
(
  new cedar::aux::DoubleParameter
  (
    this,
    "resting level",
    -5.0,
    cedar::aux::DoubleParameter::LimitType::negativeZero()
  )
),
mTau
(
  new cedar::aux::DoubleParameter
  (
    this,
    "time scale",
    100.0,
    cedar::aux::DoubleParameter::LimitType::positive()
  )
),
mGlobalInhibition
(
  new cedar::aux::DoubleParameter
  (
    this,
    "global inhibition",
    -0.01,
    cedar::aux::DoubleParameter::LimitType::negativeZero()
  )
),
// parameters
_mDimensionality
(
  new cedar::aux::UIntParameter
  (
    this,
    "dimensionality",
    2,
    cedar::aux::UIntParameter::LimitType::positiveZero(1000)
  )
),
_mSizes
(
  new cedar::aux::UIntVectorParameter
  (
    this,
    "sizes",
    2,
    10,
    cedar::aux::UIntParameter::LimitType::positive(1000)
  )
),
_mOutputActivation(new cedar::aux::BoolParameter(this, "activation as output", false)),
_mInputNoiseGain
(
  new cedar::aux::DoubleParameter
  (
    this,
    "input noise gain",
    0.1,
    cedar::aux::DoubleParameter::LimitType::positiveZero()
  )
),
_mSigmoid
(
  new cedar::dyn::NeuralField::SigmoidParameter
  (
    this,
    "sigmoid",
    cedar::aux::math::SigmoidPtr(new cedar::aux::math::AbsSigmoid(0.0, 100.0))
  )
),
_mLateralKernelConvolution(new cedar::aux::conv::Convolution()),
_mNoiseCorrelationKernelConvolution(new cedar::aux::conv::Convolution())
{
  this->declareBuffer("activation", mActivation);
  this->declareBuffer("lateral interaction", mLateralInteraction);
  this->declareBuffer("lateral kernel", this->_mLateralKernelConvolution->getCombinedKernel());
  this->declareBuffer("neural noise kernel", this->_mNoiseCorrelationKernelConvolution->getCombinedKernel());
  this->declareBuffer("input sum", mInputSum);

  this->declareOutput("sigmoided activation", mSigmoidalActivation);

  this->declareInputCollection("input");

  // setup default kernels
  std::vector<cedar::aux::kernel::KernelPtr> kernel_defaults;
  for (unsigned int i = 0; i < 1; i++)
  {
    cedar::aux::kernel::GaussPtr kernel
      = cedar::aux::kernel::GaussPtr(new cedar::aux::kernel::Gauss(this->getDimensionality()));
    kernel_defaults.push_back(kernel);
  }
  _mKernels = KernelListParameterPtr
              (
                new KernelListParameter
                (
                  this,
                  "lateral kernels",
                  kernel_defaults
                )
              );

  // setup noise correlation kernel
  mNoiseCorrelationKernel
    = cedar::aux::kernel::GaussPtr
      (
        new cedar::aux::kernel::Gauss
        (
          this->getDimensionality(),
          0.0 // default amplitude
        )
      );
  std::set<cedar::aux::conv::Mode::Id> allowed_convolution_modes;
  allowed_convolution_modes.insert(cedar::aux::conv::Mode::Same);

  this->addConfigurableChild("noise correlation kernel", mNoiseCorrelationKernel);
  this->_mNoiseCorrelationKernelConvolution->getKernelList()->append(mNoiseCorrelationKernel);
  this->_mNoiseCorrelationKernelConvolution->setMode(cedar::aux::conv::Mode::Same);
  this->_mNoiseCorrelationKernelConvolution->setBorderType(cedar::aux::conv::BorderType::Zero);

  this->addConfigurableChild("lateral kernel convolution", _mLateralKernelConvolution);
  this->_mLateralKernelConvolution->setAllowedModes(allowed_convolution_modes);

  QObject::connect(_mSizes.get(), SIGNAL(valueChanged()), this, SLOT(dimensionSizeChanged()));
  QObject::connect(_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(dimensionalityChanged()));
  QObject::connect(_mOutputActivation.get(), SIGNAL(valueChanged()), this, SLOT(activationAsOutputChanged()));

  mKernelAddedConnection
    = this->_mKernels->connectToObjectAddedSignal(boost::bind(&cedar::dyn::NeuralField::slotKernelAdded, this, _1));
  mKernelRemovedConnection
    = this->_mKernels->connectToObjectRemovedSignal
      (
        boost::bind(&cedar::dyn::NeuralField::removeKernelFromConvolution, this, _1)
      );

  this->transferKernelsToConvolution();

  // now check the dimensionality and sizes of all matrices
  this->updateMatrices();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dyn::NeuralField::activationAsOutputChanged()
{
  bool act_is_output = this->_mOutputActivation->getValue();
  static std::string slot_name = "activation";

  if (act_is_output)
  {
    if (this->hasBufferSlot(slot_name))
    {
      this->removeBufferSlot(slot_name);
    }

    if (!this->hasOutputSlot(slot_name))
    {
      this->declareOutput(slot_name, this->mActivation);
    }
  }
  else
  {
    if (this->hasOutputSlot(slot_name))
    {
      this->removeOutputSlot(slot_name);
    }

    if (!this->hasBufferSlot(slot_name))
    {
      this->declareBuffer(slot_name, this->mActivation);
    }
  }
}

void cedar::dyn::NeuralField::slotKernelAdded(size_t kernelIndex)
{
  cedar::aux::kernel::KernelPtr kernel = this->_mKernels->at(kernelIndex);
  this->addKernelToConvolution(kernel);
}

void cedar::dyn::NeuralField::transferKernelsToConvolution()
{
  this->getConvolution()->getKernelList()->clear();
  for (size_t kernel = 0; kernel < this->_mKernels->size(); ++ kernel)
  {
    this->addKernelToConvolution(this->_mKernels->at(kernel));
  }
}

void cedar::dyn::NeuralField::addKernelToConvolution(cedar::aux::kernel::KernelPtr kernel)
{
  kernel->setDimensionality(this->getDimensionality());
  this->getConvolution()->getKernelList()->append(kernel);
}

void cedar::dyn::NeuralField::removeKernelFromConvolution(size_t index)
{
  this->getConvolution()->getKernelList()->remove(index);
}

void cedar::dyn::NeuralField::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  // disconnect kernel slots (kernels first have to be loaded completely)
  mKernelAddedConnection.disconnect();
  mKernelRemovedConnection.disconnect();

  this->cedar::proc::Step::readConfiguration(node);

  this->transferKernelsToConvolution();

  // reconnect slots
  mKernelAddedConnection
    = this->_mKernels->connectToObjectAddedSignal(boost::bind(&cedar::dyn::NeuralField::slotKernelAdded, this, _1));
  mKernelRemovedConnection
    = this->_mKernels->connectToObjectRemovedSignal
      (
        boost::bind(&cedar::dyn::NeuralField::removeKernelFromConvolution, this, _1)
      );

  // legacy code for reading kernels with the old format
  cedar::aux::ConfigurationNode::const_assoc_iterator iter = node.find("numberOfKernels");
  if (iter != node.not_found())
  {
    unsigned int num_kernels = iter->second.get_value<unsigned int>();

    if (num_kernels > 0)
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Reading kernels for field \"" + this->getName() + "\" with legacy mode. "
        "This overrides all kernels previously set!",
        "cedar::dyn::NeuralField::readConfiguration(const cedar::aux::ConfigurationNode&)"
      );

      /* we have to clear everything here because it is not known whether the kernels already in the list are default
         values or values read from the configuration.
       */
      this->_mKernels->clear();
    }

    for (unsigned int i = 0; i < num_kernels; ++i)
    {
      // find the configuration node for the kernel
      cedar::aux::ConfigurationNode::const_assoc_iterator kernel_iter;
      kernel_iter = node.find("lateralKernel" + cedar::aux::toString(i));

      // check if the kernel node was found
      if (kernel_iter != node.not_found())
      {
        // the old kernels were all Gauss kernels
        cedar::aux::kernel::GaussPtr kernel (new cedar::aux::kernel::Gauss());

        // read the kernel's configuration
        kernel->readConfiguration(kernel_iter->second);

        // add the kernel to the managed list
        this->_mKernels->pushBack(kernel);
      }
      else
      {
        cedar::aux::LogSingleton::getInstance()->warning
        (
          "Could not find legacy kernel description for kernel " + cedar::aux::toString(i)
           + " in field \"" + this->getName() + "\". Skipping kernel!",
          "cedar::dyn::NeuralField::readConfiguration(const cedar::aux::ConfigurationNode&)"
        );
      }
    }
  }

  // legacy reading of the sigmoid
  cedar::aux::ConfigurationNode::const_assoc_iterator sigmoid_iter = node.find("sigmoid");
  if (sigmoid_iter != node.not_found())
  {
    cedar::aux::ConfigurationNode::const_assoc_iterator type_iter = sigmoid_iter->second.find("type");

    // if there is no type entry in the sigmoid, this must be the old format
    if (type_iter == sigmoid_iter->second.not_found())
    {
      /* If we get to this point, the sigmoid should already contain a pointer to a proper object, but with the default
         settings. Thus, we let it read the values from the sigmoid node.
       */
      CEDAR_DEBUG_ASSERT(this->_mSigmoid->getValue());

      this->_mSigmoid->getValue()->readConfiguration(sigmoid_iter->second);
    }
  }
}

void cedar::dyn::NeuralField::reset()
{
  this->mActivation->getData() = mRestingLevel->getValue();
  this->mSigmoidalActivation->getData() = cv::Scalar(0);
  this->mLateralInteraction->getData() = cv::Scalar(0);
  this->mInputNoise->getData() = cv::Scalar(0);
  this->mNeuralNoise->getData() = cv::Scalar(0);
}

cedar::proc::DataSlot::VALIDITY cedar::dyn::NeuralField::determineInputValidity
                                                         (
                                                           cedar::proc::ConstDataSlotPtr slot,
                                                           cedar::aux::ConstDataPtr data
                                                         ) const
{
  if (slot->getRole() == cedar::proc::DataRole::INPUT && slot->getName() == "input")
  {
    if (cedar::aux::ConstMatDataPtr input = boost::shared_dynamic_cast<const cedar::aux::MatData>(data))
    {
      if (!this->isMatrixCompatibleInput(input->getData()))
      {
        return cedar::proc::DataSlot::VALIDITY_ERROR;
      }
      else
      {
        //!@todo Output warning if the input is not space code
        /* return cedar::proc::DataSlot::VALIDITY_WARNING; */
        return cedar::proc::DataSlot::VALIDITY_VALID;
      }
    }
    return cedar::proc::DataSlot::VALIDITY_ERROR;
  }
  return this->cedar::proc::Step::determineInputValidity(slot, data);
}

void cedar::dyn::NeuralField::eulerStep(const cedar::unit::Time& time)
{
  // get all members needed for the Euler step
  cv::Mat& u = this->mActivation->getData();
  cv::Mat& sigmoid_u = this->mSigmoidalActivation->getData();
  cv::Mat& lateral_interaction = this->mLateralInteraction->getData();
  cv::Mat& input_noise = this->mInputNoise->getData();
  cv::Mat& neural_noise = this->mNeuralNoise->getData();
  cv::Mat& input_sum = this->mInputSum->getData();
  const double& h = mRestingLevel->getValue();
  const double& tau = mTau->getValue();
  const double& global_inhibition = mGlobalInhibition->getValue();
  // having a pointer to the convolution in the same scope as the reference prevents the object from being deleted.
  cedar::aux::conv::ConvolutionPtr convolution_ptr = this->getConvolution();
  cedar::aux::conv::Convolution& lateral_convolution = *convolution_ptr;

  // if the neural noise correlation kernel has an amplitude != 0, create new random values and convolve
  if (mNoiseCorrelationKernel->getAmplitude() != 0.0)
  {
    cv::randn(neural_noise, cv::Scalar(0), cv::Scalar(1));
    neural_noise = this->_mNoiseCorrelationKernelConvolution->convolve(neural_noise);
    //!@todo not sure, if dividing time by 1000 (which is an implicit tau) makes any sense or should be a parameter
    //!@todo not sure what sqrt(time) does here (i.e., within the sigmoid); check if this is correct, and, if so, explain it
    sigmoid_u = _mSigmoid->getValue()->compute<float>
                (
                  u
                  + sqrt(cedar::unit::Milliseconds(time)/cedar::unit::Milliseconds(1000.0))
                    * neural_noise
                );
  }
  else
  {
    // calculate output
    sigmoid_u = _mSigmoid->getValue()->compute<float>(u);
  }

  lateral_interaction = lateral_convolution(sigmoid_u);

  this->updateInputSum();

  CEDAR_ASSERT(u.size == sigmoid_u.size);
  CEDAR_ASSERT(u.size == lateral_interaction.size);
  CEDAR_ASSERT(u.size == input_sum.size);

  // the field equation
  cv::Mat d_u = -u + h + lateral_interaction + global_inhibition * cv::sum(sigmoid_u)[0] + input_sum;

  /* add input noise, but use the squared time only for Euler integration (divide by sqrt(time) here, because
   * the next line multiplies by time anyway)
   */
  cv::randn(input_noise, cv::Scalar(0), cv::Scalar(1));
  d_u += 1.0 / sqrt(cedar::unit::Milliseconds(time)/cedar::unit::Milliseconds(1.0))
         *_mInputNoiseGain->getValue() * input_noise;

  // integrate one time step
  u += cedar::unit::Milliseconds(time) / cedar::unit::Milliseconds(tau) * d_u;
}

void cedar::dyn::NeuralField::updateInputSum()
{
  cedar::proc::ExternalDataPtr input_slot = this->getInputSlot("input");
  cv::Mat& input_sum = this->mInputSum->getData();

  input_sum = cv::Scalar(0);

  // add all inputs to d_u
  for (size_t i = 0; i < input_slot->getDataCount(); ++i)
  {
    cedar::aux::DataPtr input = input_slot->getData(i);
    if (input)
    {
      //!@todo This is probably slow -- store the type of each input and static_cast instead.
      cv::Mat& input_mat = input->getData<cv::Mat>();

      CEDAR_DEBUG_ASSERT(cedar::aux::math::matrixSizesEqual(input_mat, input_sum))

      if (this->getDimensionality() == 1)
      {
        input_sum += cedar::aux::math::canonicalRowVector(input_mat);
      }
      else
      {
        input_sum += input_mat;
      }
    }
  }
}

bool cedar::dyn::NeuralField::isMatrixCompatibleInput(const cv::Mat& matrix) const
{
  // special cases due to opencv's strange handling of 1d-matrices
  if(matrix.dims == 2 && matrix.rows == 1 && matrix.cols == 1)
  {
    // if this field is set to more dimensions than the input (in this case 1), they are not compatible
    if (this->getDimensionality() != 0)
      return false;
  }
  else if(matrix.dims == 2 && (matrix.rows == 1 || matrix.cols == 1))
  {
    // if this field is set to more dimensions than the input (in this case 1), they are not compatible
    if (this->getDimensionality() != 1)
      return false;

    CEDAR_DEBUG_ASSERT(this->_mSizes->getValue().size() == 1);

    // if the dimensions are both 1, rows or cols must be the same as the field size
    if (static_cast<int>(this->_mSizes->at(0)) != matrix.rows
        && static_cast<int>(this->_mSizes->at(0)) != matrix.cols)
      return false;
  }
  else
  {
    if (static_cast<int>(this->getDimensionality()) != matrix.dims)
      return false;
    for (unsigned int dim = 0; dim < this->_mSizes->getValue().size(); ++dim)
    {
      if (matrix.size[static_cast<int>(dim)] != static_cast<int>(this->_mSizes->at(dim)))
        return false;
    }
  }
  return true;
}

void cedar::dyn::NeuralField::dimensionalityChanged()
{
  this->_mSizes->resize(this->getDimensionality(), _mSizes->getDefaultValue());
  this->updateMatrices();
}

void cedar::dyn::NeuralField::dimensionSizeChanged()
{
  this->updateMatrices();
}

void cedar::dyn::NeuralField::updateMatrices()
{
  int dimensionality = static_cast<int>(this->getDimensionality());

  std::vector<int> sizes(dimensionality);
  for (int dim = 0; dim < dimensionality; ++dim)
  {
    sizes[dim] = _mSizes->at(dim);
  }
  // check if matrices become too large
  double max_size = 1.0;
  for (int dim = 0; dim < dimensionality; ++dim)
  {
    max_size *= sizes[dim];
    if (max_size > std::numeric_limits<unsigned int>::max()/500.0) // heuristics
    {
      CEDAR_THROW(cedar::aux::RangeException, "cannot handle matrices of this size");
      return;
    }
  }
  this->lockAll();
  if (dimensionality == 0)
  {
    this->mActivation->getData() = cv::Mat(1, 1, CV_32F, cv::Scalar(mRestingLevel->getValue()));
    this->mSigmoidalActivation->getData() = cv::Mat(1, 1, CV_32F, cv::Scalar(0));
    this->mLateralInteraction->getData() = cv::Mat(1, 1, CV_32F, cv::Scalar(0));
    this->mInputNoise->getData() = cv::Mat(1, 1, CV_32F, cv::Scalar(0));
    this->mNeuralNoise->getData() = cv::Mat(1, 1, CV_32F, cv::Scalar(0));
    this->mInputSum->setData(cv::Mat(1, 1, CV_32F, cv::Scalar(0)));
  }
  else if (dimensionality == 1)
  {
    this->mActivation->getData() = cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(mRestingLevel->getValue()));
    this->mSigmoidalActivation->getData() = cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(0));
    this->mLateralInteraction->getData() = cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(0));
    this->mInputNoise->getData() = cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(0));
    this->mNeuralNoise->getData() = cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(0));
    this->mInputSum->setData(cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(0)));
  }
  else
  {
    this->mActivation->getData() = cv::Mat(dimensionality,&sizes.at(0), CV_32F, cv::Scalar(mRestingLevel->getValue()));
    this->mSigmoidalActivation->getData() = cv::Mat(dimensionality, &sizes.at(0), CV_32F, cv::Scalar(0));
    this->mLateralInteraction->getData() = cv::Mat(dimensionality, &sizes.at(0), CV_32F, cv::Scalar(0));
    this->mInputNoise->getData() = cv::Mat(dimensionality, &sizes.at(0), CV_32F, cv::Scalar(0));
    this->mNeuralNoise->getData() = cv::Mat(dimensionality, &sizes.at(0), CV_32F, cv::Scalar(0));
    this->mInputSum->setData(cv::Mat(dimensionality, &sizes.at(0), CV_32F, cv::Scalar(0)));
  }
  this->unlockAll();
  if (dimensionality > 0) // only adapt kernel in non-0D case
  {
    for (unsigned int i = 0; i < _mKernels->size(); i++)
    {
      this->_mKernels->at(i)->setDimensionality(dimensionality);
    }
    this->mNoiseCorrelationKernel->setDimensionality(dimensionality);
  }
}

void cedar::dyn::NeuralField::onStart()
{
  this->_mDimensionality->setConstant(true);
  this->_mSizes->setConstant(true);
}

void cedar::dyn::NeuralField::onStop()
{
  this->_mDimensionality->setConstant(false);
  this->_mSizes->setConstant(false);
}
