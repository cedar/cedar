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

    File:        SynapticConnection.cpp

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.rub.de
    Date:        2022 02 10

    Description: Source file for the class cedar::proc::steps::SynapticConnection.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include <iostream>

// CLASS HEADER
#include "cedar/processing/steps/SynapticConnection.h"

// CEDAR INCLUDES
#include "cedar/processing/DataConnection.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/GroupXMLFileFormatV1.h"
#include "cedar/processing/SynapticWeightPatternParameter.h"
#include "cedar/processing/typecheck/IsMatrix.h"
#include "cedar/auxiliaries/kernel/Kernel.h"
#include "cedar/processing/steps/Convolution.h"
#include "cedar/auxiliaries/convolution/FFTW.h"
#include "cedar/processing/steps/Projection.h"
#include "cedar/auxiliaries/Configurable.h"

// SYSTEM INCLUDES

cedar::aux::EnumType<cedar::proc::steps::SynapticConnection::ReducedCompressionType>
        cedar::proc::steps::SynapticConnection::ReducedCompressionType::mType;

#ifndef CEDAR_COMPILER_MSVC
const cedar::proc::steps::SynapticConnection::ReducedCompressionType::Id cedar::proc::steps::SynapticConnection::ReducedCompressionType::SUM;
const cedar::proc::steps::SynapticConnection::ReducedCompressionType::Id cedar::proc::steps::SynapticConnection::ReducedCompressionType::MAXIMUM;
#endif // CEDAR_COMPILER_MSVC

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::DataRole;
    using cedar::proc::ElementDeclaration;
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    //Todo: Add right name, icon and description
    ElementDeclarationPtr declaration
            (
                    new ElementDeclarationTemplate<cedar::proc::steps::SynapticConnection>
                            (
                                    "Programming",
                                    "cedar.processing.SynapticConnection"
                            )
            );
    declaration->setIconPath(":/steps/<em>YOURICON</em>.svg");
    declaration->setDescription
            (
                    "<em>ADD A DESCRIPTION FOR THE TOOLTIP OF YOUR STEP IN HERE</em>"
            );
    ElementDeclaration::PlotDefinition kernel_plot_data("convolution kernel", ":/cedar/dynamics/gui/kernel_plot.svg");
    kernel_plot_data.appendData(DataRole::BUFFER, "convolution kernel");
    declaration->definePlot(kernel_plot_data);

    ElementDeclaration::PlotDefinition pointWiseWeight_plot_data("pointWiseWeights", ":/cedar/dynamics/gui/kernel_plot.svg");
    pointWiseWeight_plot_data.appendData(DataRole::BUFFER, "point wise weights");
    declaration->definePlot(pointWiseWeight_plot_data);

    ElementDeclaration::PlotDefinition pointWiseWeight_padded_plot_data("pointWiseWeights (padded)", ":/cedar/dynamics/gui/kernel_plot.svg");
    pointWiseWeight_padded_plot_data.appendData(DataRole::BUFFER, "point wise weights (padded)");
    declaration->definePlot(pointWiseWeight_padded_plot_data);

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::SynapticConnection::SynapticConnection()
        :
        mConvolution(new cedar::aux::conv::Convolution()),
        mPointWiseWeightConvolution(new cedar::aux::conv::Convolution()),
        mPreparedPointWiseMat(new cedar::aux::MatData()),
        mRevalidating(false),
        mConvolutionOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
        mStaticGainOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
        mOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
        mpProjectionMethod(nullptr)
{
  ////General
  this->mXMLExportable = true;
  this->declareInput("matrix", true);
  this->declareOutput("result", mOutput);
  this->declareBuffer("convolution kernel", this->mConvolution->getCombinedKernel());
  this->declareBuffer("point wise weights", this->mPreparedPointWiseMat);

  ////Convolution code
  QObject::connect(this->mConvolution.get(), SIGNAL(configurationChanged()), this,
                   SLOT(recompute()));
  QObject::connect(this->mPointWiseWeightConvolution.get(), SIGNAL(configurationChanged()), this,
                   SLOT(recompute()));

  //Add convolution parameter to "grey grouping box"
  cedar::aux::ConfigurablePtr convolutionConfigurable = boost::shared_ptr<cedar::aux::Configurable>(
          new cedar::aux::Configurable());
  this->mKernelsParameter = new cedar::proc::steps::SynapticConnection::KernelListParameter(
          convolutionConfigurable.get(),
          "kernels",
          std::vector<cedar::aux::kernel::KernelPtr>());
  this->addConfigurableChild("convolution", convolutionConfigurable);

  //Add PointWiseWeight convolution parameter to "grey grouping box"
  cedar::aux::ConfigurablePtr pointWiseWeightConvolutionConfigurable = boost::shared_ptr<cedar::aux::Configurable>(
          new cedar::aux::Configurable());
  mPointWiseWeightKernelsParameter = new cedar::proc::steps::SynapticConnection::KernelListParameter(
          pointWiseWeightConvolutionConfigurable.get(),
          "kernels",std::vector<cedar::aux::kernel::KernelPtr>());
  this->addConfigurableChild("PointWiseWeight", pointWiseWeightConvolutionConfigurable);

  //Setting whitelist to limit the kernels for SynapticConnection
  std::vector<std::string> whitelist = {"cedar.aux.kernel.Gauss"};
  this->mKernelsParameter->setWhitelist(whitelist);
  this->mPointWiseWeightKernelsParameter->setWhitelist(whitelist);

  // Connect added and removed signal of kernel parameters
  this->mKernelAddedConnection = this->mKernelsParameter->connectToObjectAddedSignal(boost::bind(
          &cedar::proc::steps::SynapticConnection::convolutionSlotKernelAdded,
          this,
          this->mConvolution,
          this->mKernelsParameter,
          _1));

  this->mPointWiseWeightKernelsParameter->connectToObjectAddedSignal(boost::bind(
          &cedar::proc::steps::SynapticConnection::convolutionSlotKernelAdded,
          this,
          this->mPointWiseWeightConvolution,
          this->mPointWiseWeightKernelsParameter,
          _1));

  this->mKernelRemovedConnection = this->mKernelsParameter->connectToObjectRemovedSignal(boost::bind(
          &cedar::proc::steps::SynapticConnection::removeKernelFromConvolution,
          this,
          this->mConvolution,_1));

  this->mPointWiseWeightKernelsParameter->connectToObjectRemovedSignal(boost::bind(
          &cedar::proc::steps::SynapticConnection::removeKernelFromConvolution,
          this,
          this->mPointWiseWeightConvolution,
          _1));

  this->transferKernelsToConvolution(this->mConvolution,
                                     this->mKernelsParameter);

  this->transferKernelsToConvolution(this->mPointWiseWeightConvolution,
                                     this->mPointWiseWeightKernelsParameter);

  ////Static Gain code
  //Add static gain parameter to "grey grouping box"
  cedar::aux::ConfigurablePtr staticGainConfigurable = boost::shared_ptr<cedar::aux::Configurable>(
          new cedar::aux::Configurable());
  this->mGainFactorParameter = new cedar::aux::DoubleParameter(staticGainConfigurable.get(),
                                                               "Synapse Weight",
                                                               1.0,
                                                               -10000.0,
                                                               10000.0);
  this->addConfigurableChild("static gain", staticGainConfigurable);

  QObject::connect(this->mGainFactorParameter.get(), SIGNAL(valueChanged()),this,
                   SLOT(recompute()));

  ////Projection code
  //Add projectionConfigurable parameter to "grey grouping box"
  cedar::aux::ConfigurablePtr projectionConfigurable = boost::shared_ptr<cedar::aux::Configurable>(
          new cedar::aux::Configurable());
  mProjectionDimensionMappings = new cedar::proc::ProjectionMappingParameter(projectionConfigurable.get(),
                                                                             "dimension mapping");
  mProjectionOutputDimensionality = new cedar::aux::UIntParameter(projectionConfigurable.get(),
                                                                  "output dimensionality",
                                                                  1,
                                                                  0,
                                                                  4);
  mProjectionOutputDimensionSizes = new cedar::aux::UIntVectorParameter(projectionConfigurable.get(),
                                                                        "output dimension sizes",
                                                                        1,
                                                                        50,
                                                                        1,
                                                                        1000);
  mProjectionCompressionType = new cedar::aux::EnumParameter(projectionConfigurable.get(),
                                                             "compression type",
                                                             cedar::proc::steps::SynapticConnection::ReducedCompressionType::typePtr(),
                                                             cedar::proc::steps::SynapticConnection::ReducedCompressionType::SUM);
  this->addConfigurableChild("projection", projectionConfigurable);

  // initialize the output buffer to the correct size
  this->projectionOutputDimensionalityChanged();

  // connect signals and slots
  QObject::connect(mProjectionDimensionMappings.get(), SIGNAL(valueChanged()), this,
                   SLOT(reconfigure()), Qt::DirectConnection);
  QObject::connect(mProjectionCompressionType.get(), SIGNAL(valueChanged()), this,
                   SLOT(reconfigure()), Qt::DirectConnection);
  QObject::connect(mProjectionOutputDimensionality.get(), SIGNAL(valueChanged()), this,
                   SLOT(projectionOutputDimensionalityChanged()), Qt::DirectConnection);
  QObject::connect(mProjectionOutputDimensionSizes.get(), SIGNAL(valueChanged()), this,
                   SLOT(projectionOutputDimensionSizesChanged()), Qt::DirectConnection);
}

cedar::proc::steps::SynapticConnection::~SynapticConnection()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
// The arguments are unused here
void cedar::proc::steps::SynapticConnection::compute(const cedar::proc::Arguments&)
{
  ////First: Convolution
  const cv::Mat& matrix = mMatrix->getData();
  if (this->mKernelsParameter->size() > 0)
  {
    if (this->mKernelsParameter->at(0)->getDimensionality() != this->convolutionGetDimensionality())
    {
      this->convolutionInputDimensionalityChanged();
    }
    mConvolutionOutput->setData(this->mConvolution->convolve(matrix));
  }
  else
  {
    this->mConvolutionOutput->setData(this->mMatrix->getData().clone());
  }

  if (this->mPointWiseWeightKernelsParameter->size() > 0)
  {
    if (this->mPointWiseWeightKernelsParameter->at(0)->getDimensionality() != this->convolutionGetDimensionality())
    {
      this->convolutionInputDimensionalityChanged();
    }
    cv::Mat pointWiseMat = this->mPointWiseWeightConvolution->getCombinedKernel()->getData();
    this->preparePointWiseWeightMat(pointWiseMat);
    this->mPreparedPointWiseMat->setData(pointWiseMat);
    mConvolutionOutput->setData(pointWiseMat.mul(mConvolutionOutput->getData()));
  }
  else
  {
    this->mConvolutionOutput->setData(this->mMatrix->getData().clone());
  }

  ////Second: Static Gain
  this->mStaticGainOutput->setData(this->mConvolutionOutput->getData() * this->mGainFactorParameter->getValue());

  ////Third: Projection
  if (!mStaticGainOutput) // quickfix
    return;

  // call the appropriate projection method via the function pointer
  if(mpProjectionMethod)
  {
    (this->*mpProjectionMethod)(mStaticGainOutput, mOutput, mProjectionIndicesToCompress, mProjectionCompressionType,
                                mProjectionDimensionMappings);
  }
}

void cedar::proc::steps::SynapticConnection::recompute()
{
  this->onTrigger();
}

void cedar::proc::steps::SynapticConnection::preparePointWiseWeightMat(cv::Mat& pointWiseMat)
{
  // Pad the pointWiseWeight kernelsum with zeros if needed
  if(pointWiseMat.rows < mConvolutionOutput->getData().rows)
  {
    int topPadding = (mConvolutionOutput->getData().rows - pointWiseMat.rows) / 2;
    cv::copyMakeBorder(pointWiseMat, pointWiseMat, topPadding,
                       (mConvolutionOutput->getData().rows - pointWiseMat.rows) - topPadding, 0, 0,
                       cv::BORDER_CONSTANT, 0);
  }
  if(pointWiseMat.cols < mConvolutionOutput->getData().cols)
  {
    int leftPadding = (mConvolutionOutput->getData().cols - pointWiseMat.cols) / 2;
    cv::copyMakeBorder(pointWiseMat, pointWiseMat, 0, 0, leftPadding,
                       (mConvolutionOutput->getData().cols - pointWiseMat.cols) - leftPadding,
                       cv::BORDER_CONSTANT, 0);
  }

  // Crop the pointWiseMat if too large
  int topCrop = 0, leftCrop = 0;
  if(pointWiseMat.rows > mConvolutionOutput->getData().rows)
  {
    topCrop = (pointWiseMat.rows - mConvolutionOutput->getData().rows) / 2;
  }
  if(pointWiseMat.cols > mConvolutionOutput->getData().cols)
  {
    leftCrop = (pointWiseMat.cols - mConvolutionOutput->getData().cols) / 2;
  }
  pointWiseMat = pointWiseMat(cv::Rect(leftCrop, topCrop, mConvolutionOutput->getData().cols,
                                       mConvolutionOutput->getData().rows));
}

bool cedar::proc::steps::SynapticConnection::isXMLExportable(std::string& errorMsg){
  SlotMap outputSlots = this->getDataSlots(DataRole::OUTPUT);
  for(auto slot : outputSlots)
  {
    for(cedar::proc::DataConnectionPtr outputConn : slot.second->getDataConnections())
    {
      if(dynamic_cast<cedar::proc::steps::SynapticConnection*>(outputConn.get()->getTarget()->getParentPtr()))
      {
        errorMsg = "The XML export does not support chains of SynapticConnections";
        return false;
      }
    }
  }
  return true;
}

void cedar::proc::steps::SynapticConnection::writeConfigurationXML(cedar::aux::ConfigurationNode& root) const
{
  // Write kernel weights
  cedar::aux::ConfigurationNode sumWeightPattern;
  cedar::proc::GroupXMLFileFormatV1::writeKernelListParameter(this->mKernelsParameter.get(), sumWeightPattern);
  root.add_child("KernelWeights", sumWeightPattern);

  // Write PointWiseWeights
  cedar::aux::ConfigurationNode sumWeightPatternPWW;
  cedar::proc::GroupXMLFileFormatV1::writeKernelListParameter(this->mPointWiseWeightKernelsParameter.get(),
                                                              sumWeightPatternPWW);
  root.add_child("PointWiseWeights", sumWeightPatternPWW);

  // Write static gain
  root.put("ScalarWeight", this->mGainFactorParameter->getValue());

  // Write projection
  cedar::aux::ConfigurationNode dimensionMapping;
  this->mProjectionDimensionMappings->writeToNodeXML(dimensionMapping);
  root.add_child("DimensionMapping", dimensionMapping);
}

void cedar::proc::steps::SynapticConnection::readConfigurationXML(const cedar::aux::ConfigurationNode& root)
{
  auto kernelWeights = root.find("KernelWeights");
  auto pointWiseWeights = root.find("PointWiseWeights");
  auto scalarWeight = root.find("ScalarWeight");
  auto dimensionMapping = root.find("DimensionMapping");

  // Read kernel weights
  if (kernelWeights != root.not_found())
  {
    cedar::proc::GroupXMLFileFormatV1::readKernelListParameter(this->mKernelsParameter.get(),
                                                               root.get_child("KernelWeights"));
  }

  // Read point wise weights
  if (pointWiseWeights != root.not_found())
  {
    cedar::proc::GroupXMLFileFormatV1::readKernelListParameter(this->mPointWiseWeightKernelsParameter.get(),
                                                                root.get_child("PointWiseWeights"));
  }
  // Read static gain
  if (scalarWeight != root.not_found())
  {
    this->mGainFactorParameter->setValue(root.get<double>("ScalarWeight"));
  }
  // Read projection
  if (dimensionMapping != root.not_found())
  {
    cedar::proc::GroupXMLFileFormatV1::readProjectionMappingsParameter(
      this->mProjectionDimensionMappings,
      root.get_child("DimensionMapping"));
  }
}

////Functions copied from cedar::proc::steps::Convolution

//Todo: This is currently only the code from convolution, because the input is the same as convolution and static gain
// and projection come after the convoltion
void cedar::proc::steps::SynapticConnection::inputConnectionChanged(const std::string& inputName)
{
// Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "matrix" || inputName == "kernel");

  cv::Mat old_output = this->mOutput->getData().clone();

  if (inputName == "matrix")
  {
    // Assign the input to the member. This saves us from casting in every computation step.
    this->mMatrix = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
    // This should always work since other types should not be accepted.
    if (!this->mMatrix || this->mMatrix->isEmpty())
    {
      return;
    }

    this->mOutput->copyAnnotationsFrom(this->mMatrix);

    const auto& list = this->getConvolution()->getKernelList();
    std::vector<bool> blocked(list->size());

    // Block signals from the kernel because they might otherwise call onTrigger (via kernelChanged -> recompute),
    // which leads to trouble in inputConnectionChanged.
    for (size_t i = 0; i < list->size(); ++i)
    {
      blocked[i] = list->getKernel(i)->blockSignals(true);
    }

    this->convolutionInputDimensionalityChanged();

    // restore blocked state for each kernel
    for (size_t i = 0; i < list->size(); ++i)
    {
      list->getKernel(i)->blockSignals(blocked[i]);
    }

    unsigned int inputDimensionality = cedar::aux::math::getDimensionalityOf(this->mMatrix->getData());

    this->mProjectionDimensionMappings->initialize(inputDimensionality);

    this->reconfigure(false);
  }
  this->callComputeWithoutTriggering();

  if(!mRevalidating // if the step is revalidating, the revalidating call will also check this after revalidation is
     // complete.
     && (!cedar::aux::math::matrixSizesEqual(old_output, this->mOutput->getData()) ||
         old_output.type() != this->mOutput->getData().type()))
  {
    this->emitOutputPropertiesChangedSignal("result");
  }
}

void cedar::proc::steps::SynapticConnection::convolutionSlotKernelAdded(cedar::aux::conv::ConvolutionPtr convolution,
                                                                        KernelListParameterPtr kernelsParameter, size_t kernelIndex)
{
  cedar::aux::kernel::KernelPtr kernel = kernelsParameter->at(kernelIndex);
  this->addKernelToConvolution(convolution, kernel);

  this->onTrigger();
}

void cedar::proc::steps::SynapticConnection::addKernelToConvolution(cedar::aux::conv::ConvolutionPtr convolution,
                                                                    cedar::aux::kernel::KernelPtr kernel)
{
  kernel->setDimensionality(this->convolutionGetDimensionality());
  convolution->getKernelList()->append(kernel);
  QObject::connect(kernel.get(), SIGNAL(kernelUpdated()), this, SLOT(recompute()));
}

void cedar::proc::steps::SynapticConnection::removeKernelFromConvolution(cedar::aux::conv::ConvolutionPtr convolution,
                                                                         size_t index)
{
  auto kernel = convolution->getKernelList()->getKernel(index);
  //!@todo remove this const cast
  const_cast<cedar::aux::kernel::Kernel*>(kernel.get())->disconnect(SIGNAL(kernelUpdated()), this,
                                                                    SLOT(recompute()));
  convolution->getKernelList()->remove(index);
  this->onTrigger();
}

void cedar::proc::steps::SynapticConnection::transferKernelsToConvolution(cedar::aux::conv::ConvolutionPtr convolution,
                                                                          KernelListParameterPtr kernelsParameter)
{
  convolution->getKernelList()->clear();
  for (size_t kernel = 0; kernel < kernelsParameter->size(); ++ kernel)
  {
    this->addKernelToConvolution(convolution, kernelsParameter->at(kernel));
  }
}

void cedar::proc::steps::SynapticConnection::convolutionInputDimensionalityChanged()
{
  unsigned int new_dimensionality = this->convolutionGetDimensionality();
#ifdef CEDAR_USE_FFTW
  if (new_dimensionality >= 3)
  {
    this->mConvolution->setEngine(cedar::aux::conv::FFTWPtr(new cedar::aux::conv::FFTW()));
  }
#endif // CEDAR_USE_FFTW


  for (size_t i = 0; i < this->mKernelsParameter->size(); ++i)
  {
    this->mKernelsParameter->at(i)->setDimensionality(new_dimensionality);
  }
  for (size_t i = 0; i < this->mPointWiseWeightKernelsParameter->size(); ++i)
  {
    this->mPointWiseWeightKernelsParameter->at(i)->setDimensionality(new_dimensionality);
  }
}

unsigned int cedar::proc::steps::SynapticConnection::convolutionGetDimensionality() const
{
  if (this->mMatrix)
  {
    return cedar::aux::math::getDimensionalityOf(this->mMatrix->getData());
  }
  else if (this->mKernelsParameter->size() > 0)
  {
    return this->mKernelsParameter->at(0)->getDimensionality();
  }
  else
  {
    return 2;
  }
}

void cedar::proc::steps::SynapticConnection::projectionOutputDimensionalityChanged()
{
  // get the new output dimensionality
  unsigned int new_dimensionality = mProjectionOutputDimensionality->getValue();

  // resize the dimensionality of the output buffer
  this->mProjectionOutputDimensionSizes->resize(new_dimensionality,
                                                mProjectionOutputDimensionSizes->getDefaultValue());

  // the number of mappings from input to output is constrained by the output dimensionality
  this->mProjectionDimensionMappings->setOutputDimensionality(new_dimensionality);

  this->initializeOutputMatrix();

  this->reconfigure();
  this->emitOutputPropertiesChangedSignal("result");
}

void cedar::proc::steps::SynapticConnection::projectionOutputDimensionSizesChanged()
{
  this->initializeOutputMatrix();
  this->emitOutputPropertiesChangedSignal("result");
}

void cedar::proc::steps::SynapticConnection::initializeOutputMatrix()
{
  int dimensionality = static_cast<int>(mProjectionOutputDimensionality->getValue());
  // use the input type if possible
  int type = CV_32F;
  if (this->mStaticGainOutput)
  {
    type = this->mStaticGainOutput->getData().type();
  }

  if (dimensionality == 0)
  {
    cedar::aux::Lockable::Locker locker(this);
    this->mOutput->getData() = cv::Mat(1, 1, type, cv::Scalar(0));
    locker.unlock();
  }
  else
  {
    // convert the sizes of the output dimensions to signed integers so that
    // OpenCV can handle it
    std::vector<int> sizes(dimensionality);
    for (int dim = 0; dim < dimensionality; ++dim)
    {
      sizes[dim] = mProjectionOutputDimensionSizes->at(dim);
    }

    cedar::aux::Lockable::Locker locker(this);
    this->mOutput->getData() = cv::Mat(dimensionality, &sizes.at(0), type, cv::Scalar(0));
    locker.unlock();
  }
}

void cedar::proc::steps::SynapticConnection::reconfigure(bool triggerSubsequent)
{
  if (!this->mStaticGainOutput)
  {
    this->setState(
            cedar::proc::Triggerable::STATE_EXCEPTION,
            "Cannot reconfigure without an input."
    );
    return;
  }

  if (mStaticGainOutput->getData().empty())
    return; // silently ignore. for example if a NetReader isn't initialized, yet

  unsigned int input_dimensionality = cedar::aux::math::getDimensionalityOf(this->mStaticGainOutput->getData());
  unsigned int output_dimensionality = mProjectionOutputDimensionality->getValue();

  // if the projection compresses ...
  if (input_dimensionality > output_dimensionality)
  {
    CEDAR_DEBUG_ASSERT(input_dimensionality == mProjectionDimensionMappings->getValue()->getNumberOfMappings())

    // ... compute which indices need to be compressed
    mProjectionIndicesToCompress.clear();

    for (unsigned int index = 0; index < input_dimensionality; ++index)
    {
      if (mProjectionDimensionMappings->getValue()->isDropped(index))
      {
        mProjectionIndicesToCompress.push_back(index);
      }
    }

    // set up the appropriate function pointer for different combinations of
    // input and output dimensionality
    if (input_dimensionality == 3 && output_dimensionality == 2)
    {
      std::vector<unsigned int> mapped_indices;

      for (unsigned int index = 0; index < input_dimensionality; ++index)
      {
        if (!mProjectionDimensionMappings->getValue()->isDropped(index))
        {
          mapped_indices.push_back(mProjectionDimensionMappings->getValue()->lookUp(index));
        }
      }

      if (mapped_indices.size() == 2)
      {
        if (mapped_indices.at(0) == mapped_indices.at(1))
        {
          this->setState(
                  cedar::proc::Triggerable::STATE_EXCEPTION,
                  "Cannot map the same dimension onto multiple dimensions."
          );
          return;
        }
        bool swapped = mapped_indices.at(0) > mapped_indices.at(1);

        if (swapped)
        {
          mpProjectionMethod = &cedar::proc::ProjectionFunctions::compress3Dto2DSwapped;
        }
        else
        {
          mpProjectionMethod = &cedar::proc::ProjectionFunctions::compress3Dto2D;
        }
      }
    }
    else if (input_dimensionality == 3 && output_dimensionality == 1)
    {
      mpProjectionMethod = &cedar::proc::ProjectionFunctions::compress3Dto1D;
    }
    else if (input_dimensionality == 2 && output_dimensionality == 1)
    {
      mpProjectionMethod = &cedar::proc::ProjectionFunctions::compress2Dto1D;
    }
    else if (output_dimensionality == 0)
    {
      if (mProjectionCompressionType->getValue() == cedar::proc::steps::Projection::CompressionType::MAXIMUM)
      {
        mpProjectionMethod = &cedar::proc::ProjectionFunctions::compressNDto0Dmax;
      }
      else if (mProjectionCompressionType->getValue() == cedar::proc::steps::Projection::CompressionType::MINIMUM)
      {
        mpProjectionMethod = &cedar::proc::ProjectionFunctions::compressNDto0Dmin;
      }
      else if (mProjectionCompressionType->getValue() == cedar::proc::steps::Projection::CompressionType::AVERAGE)
      {
        mpProjectionMethod = &cedar::proc::ProjectionFunctions::compressNDto0Dmean;
      }
      else if (mProjectionCompressionType->getValue() == cedar::proc::steps::Projection::CompressionType::SUM)
      {
        mpProjectionMethod = &cedar::proc::ProjectionFunctions::compressNDto0Dsum;
      }
    }
    else
    {
      mpProjectionMethod = NULL;
      CEDAR_THROW(cedar::aux::NotImplementedException, "The projection for this configuration is not implemented.");
    }
  }
    // if the projection expands ...
  else
  {
    // ... set up the appropriate function pointer
    if (input_dimensionality == 0)
    {
      this->mpProjectionMethod = &cedar::proc::ProjectionFunctions::expand0DtoND;
    }
    else if (input_dimensionality == 1 && output_dimensionality == 3)
    {
      this->mpProjectionMethod = &cedar::proc::ProjectionFunctions::expand1Dto3D;
    }
    else if (input_dimensionality == 2 && output_dimensionality == 3)
    {
      this->mpProjectionMethod = &cedar::proc::ProjectionFunctions::expand2Dto3D;
    }
    else
    {
      this->mpProjectionMethod = &cedar::proc::ProjectionFunctions::expandMDtoND;
    }
  }

  if (this->mProjectionDimensionMappings->getValue()->getValidity() == cedar::proc::ProjectionMapping::VALIDITY_ERROR)
  {
    this->setState(
            cedar::proc::Triggerable::STATE_EXCEPTION,
            "The projection, as you have set it up, does not work in the given context. Please revise the "
            "mapping parameters."
    );
  }
  else
  {
    this->setState(
            cedar::proc::Triggerable::STATE_UNKNOWN,
            "Projection mapping is set up correctly."
    );
  }

  // reset constness of all mappings
  this->mProjectionOutputDimensionSizes->unconstAll();

  for (unsigned int input_dim = 0; input_dim < input_dimensionality; ++input_dim)
  {
    if (mProjectionDimensionMappings->getValue()->isDropped(input_dim))
    {
      continue;
    }
    unsigned int output_dim = mProjectionDimensionMappings->getValue()->lookUp(input_dim);
    CEDAR_ASSERT(output_dim < output_dimensionality);
    this->mProjectionOutputDimensionSizes->setValue(output_dim,
                                                    this->mStaticGainOutput->getData().size[input_dim]);
    this->mProjectionOutputDimensionSizes->setConstantAt(output_dim, true);
  }

  // if input type and output type do not match, we have to re-initialize the output matrix
  if (this->mStaticGainOutput->getCvType() != this->mOutput->getCvType())
  {
    this->initializeOutputMatrix();
    this->emitOutputPropertiesChangedSignal("result");
  }

  // now do a final step and try to calculate an output with the new configuration
  if (triggerSubsequent)
  {
    this->onTrigger(cedar::proc::ArgumentsPtr());
  }
  else
  {
    this->callComputeWithoutTriggering();
  }
}