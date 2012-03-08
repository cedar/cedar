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
#include "cedar/dynamics/SpaceCode.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/math/Sigmoid.h"
#include "cedar/auxiliaries/math/AbsSigmoid.h"
#include "cedar/auxiliaries/kernel/Gauss.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/convolution/FastConvolution.h"
#include "cedar/auxiliaries/math/tools.h"

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
mActivation(new cedar::dyn::SpaceCode(cv::Mat::zeros(10,10,CV_32F))),
mSigmoidalActivation(new cedar::dyn::SpaceCode(cv::Mat::zeros(10,10,CV_32F))),
mLateralInteraction(new cedar::dyn::SpaceCode(cv::Mat::zeros(10,10,CV_32F))),
mInputNoise(new cedar::aux::MatData(cv::Mat::zeros(10,10,CV_32F))),
mNeuralNoise(new cedar::aux::MatData(cv::Mat::zeros(10,10,CV_32F))),
mRestingLevel(new cedar::aux::DoubleParameter(this, "restingLevel", -5.0, -100, 0)),
mTau(new cedar::aux::DoubleParameter(this, "tau", 100.0, 1.0, 10000.0)),
mGlobalInhibition(new cedar::aux::DoubleParameter(this, "globalInhibition", -0.01, -100.0, 100.0)),
mSigmoid(new cedar::aux::math::AbsSigmoid(0.0, 10.0)),
_mDimensionality(new cedar::aux::UIntParameter(this, "dimensionality", 0, 1000)),
_mSizes(new cedar::aux::UIntVectorParameter(this, "sizes", 2, 10, 1, 1000)),
_mNumberOfKernels(new cedar::aux::UIntParameter(this, "numberOfKernels", 1, 1, 20)),
_mInputNoiseGain(new cedar::aux::DoubleParameter(this, "inputNoiseGain", 0.1, 0.0, 1000.0))
{
  _mDimensionality->setValue(2);
  _mSizes->makeDefault();
  //default is two modes/kernels for lateral interaction
  _mNumberOfKernels->setValue(2);
  QObject::connect(_mSizes.get(), SIGNAL(valueChanged()), this, SLOT(dimensionSizeChanged()));
  this->declareBuffer("activation");
  this->setBuffer("activation", mActivation);
  this->declareBuffer("lateralInteraction");
  this->setBuffer("lateralInteraction", mLateralInteraction);

  this->declareOutput("sigmoid(activation)");
  this->setOutput("sigmoid(activation)", mSigmoidalActivation);

  this->declareInputCollection("input");

  this->addConfigurableChild("sigmoid", this->mSigmoid);

  std::vector<double> sigmas;
  std::vector<double> shifts;
  sigmas.push_back(3.0);
  shifts.push_back(0.0);
  sigmas.push_back(3.0);
  shifts.push_back(0.0);

  for (unsigned int i = 0; i < _mNumberOfKernels->getValue(); i++)
  {
    cedar::aux::kernel::GaussPtr kernel = cedar::aux::kernel::GaussPtr(new cedar::aux::kernel::Gauss(
                                                                                                      1.0,
                                                                                                      sigmas,
                                                                                                      shifts,
                                                                                                      5.0,
                                                                                                      2
                                                                                                    ));
    mKernels.push_back(kernel);
    std::string kernel_name("lateralKernel");
    kernel_name += boost::lexical_cast<std::string>(i);
    this->declareBuffer(kernel_name);
    this->setBuffer(kernel_name, mKernels.at(i)->getKernelRaw());
    this->mKernels.at(i)->hideDimensionality(true);
    this->addConfigurableChild(kernel_name, this->mKernels.at(i));
  }
  mNoiseCorrelationKernel = cedar::aux::kernel::GaussPtr(new cedar::aux::kernel::Gauss(
                                                                                        0.0,
                                                                                        sigmas,
                                                                                        shifts,
                                                                                        5.0,
                                                                                        2
                                                                                      ));
  this->addConfigurableChild("noiseCorrelationKernel", mNoiseCorrelationKernel);
  QObject::connect(_mSizes.get(), SIGNAL(valueChanged()), this, SLOT(dimensionSizeChanged()));
  QObject::connect(_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(dimensionalityChanged()));
  QObject::connect(_mNumberOfKernels.get(), SIGNAL(valueChanged()), this, SLOT(numberOfKernelsChanged()));
  mOldNumberOfKernels = _mNumberOfKernels->getValue();
  // now check the dimensionality and sizes of all matrices
  this->updateMatrices();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

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
                                                           cedar::aux::DataPtr data
                                                         ) const
{
  if (slot->getRole() == cedar::proc::DataRole::INPUT && slot->getName() == "input")
  {
    if (cedar::dyn::SpaceCodePtr input = boost::shared_dynamic_cast<cedar::dyn::SpaceCode>(data))
    {
      if (!this->isMatrixCompatibleInput(input->getData()))
      {
        return cedar::proc::DataSlot::VALIDITY_ERROR;
      }
      else
      {
        return cedar::proc::DataSlot::VALIDITY_VALID;
      }
    }
    else if (cedar::aux::MatDataPtr input = boost::shared_dynamic_cast<cedar::aux::MatData>(data))
    {
      if (!this->isMatrixCompatibleInput(input->getData()))
      {
        return cedar::proc::DataSlot::VALIDITY_ERROR;
      }
      else
      {
        return cedar::proc::DataSlot::VALIDITY_WARNING;
      }
    }
    return cedar::proc::DataSlot::VALIDITY_ERROR;
  }
  return this->cedar::proc::Step::determineInputValidity(slot, data);
}

void cedar::dyn::NeuralField::eulerStep(const cedar::unit::Time& time)
{
  // get all members needed for the Euler step
  cedar::proc::ExternalDataPtr input_slot = this->getInputSlot("input");
  cv::Mat& u = this->mActivation->getData();
  cv::Mat& sigmoid_u = this->mSigmoidalActivation->getData();
  cv::Mat& lateral_interaction = this->mLateralInteraction->getData();
  cv::Mat& input_noise = this->mInputNoise->getData();
  cv::Mat& neural_noise = this->mNeuralNoise->getData();
  const double& h = mRestingLevel->getValue();
  const double& tau = mTau->getValue();
  const double& global_inhibition = mGlobalInhibition->getValue();

  // if the neural noise correlation kernel has an amplitude != 0, create new random values and convolve
  if (mNoiseCorrelationKernel->getAmplitude() != 0.0)
  {
    cv::randn(neural_noise, cv::Scalar(0), cv::Scalar(1));
    mNoiseCorrelationKernel->getReadWriteLock()->lockForRead();
    neural_noise = this->mNoiseCorrelationKernel->convolveWith(neural_noise);
    mNoiseCorrelationKernel->getReadWriteLock()->unlock();
    //!@todo not sure, if dividing time by 1000 (which is an implicit tau) makes any sense or should be a parameter
    sigmoid_u = mSigmoid->compute<float>(
                                          u
                                          + sqrt(cedar::unit::Milliseconds(time)/cedar::unit::Milliseconds(1000.0))
                                            * neural_noise
                                        );
  }
  else
  {
    // calculate output
    sigmoid_u = mSigmoid->compute<float>(u);
  }

  // calculate the lateral interactions for all kernels
  lateral_interaction = 0.0;
  //!@todo Wrap this in a cedar::aux::convolve function that automatically selects the proper things
  if (this->_mDimensionality->getValue() == 0)
  {
    for (unsigned int i = 0; i < _mNumberOfKernels->getValue() && i < this->mKernels.size(); i++)
    {
      //!@todo Should/does this not use the data->lock*
      mKernels.at(i)->getReadWriteLock()->lockForRead();
      lateral_interaction += mKernels.at(i)->getAmplitude() * sigmoid_u;
      mKernels.at(i)->getReadWriteLock()->unlock();
    }
  }
  else if (this->_mDimensionality->getValue() < 3)
  {
    for (unsigned int i = 0; i < _mNumberOfKernels->getValue() && i < this->mKernels.size(); i++)
    {
      //!@todo Should/does this not use the data->lock*
      mKernels.at(i)->getReadWriteLock()->lockForRead();
      cv::Mat convolution_buffer = this->mKernels.at(i)->convolveWith(sigmoid_u);
      mKernels.at(i)->getReadWriteLock()->unlock();
      lateral_interaction += convolution_buffer;

    }
  }
#ifdef FFTW
  else if (this->_mDimensionality->getValue() < 8)
  {
    for (unsigned int i = 0; i < _mNumberOfKernels->getValue() && i < this->mKernels.size(); i++)
    {
      //!@todo Should/does this not use the data->lock*
      mKernels.at(i)->getReadWriteLock()->lockForRead();
      //cv::Mat convolution_buffer = this->mKernels.at(i)->convolveWith(sigmoid_u);
      cedar::aux::conv::FastConvolution myConvolution;
      cv::Mat sigmoid_64;
      sigmoid_u.convertTo(sigmoid_64, CV_64F);
      cv::Mat kernel_64;
      this->mKernels.at(i)->getKernel().convertTo(kernel_64, CV_64F);
      cv::Mat convolution_buffer = myConvolution(sigmoid_64, kernel_64);
      mKernels.at(i)->getReadWriteLock()->unlock();
      cv::Mat buffer_32;
      convolution_buffer.convertTo(buffer_32, CV_32F);
//      lateral_interaction += convolution_buffer;
      lateral_interaction += buffer_32;
    }
  }
#endif

  CEDAR_ASSERT(u.size == sigmoid_u.size);
  CEDAR_ASSERT(u.size == lateral_interaction.size);

  // the field equation
  cv::Mat d_u = -u + h + lateral_interaction + global_inhibition * cv::sum(sigmoid_u)[0];
  // add all inputs to d_u
  for (size_t i = 0; i < input_slot->getDataCount(); ++i)
  {
    cedar::aux::DataPtr input = input_slot->getData(i);
    if (input)
    {
      cv::Mat& input_mat = input->getData<cv::Mat>();

      if (!cedar::aux::math::matrixSizesEqual(input_mat, d_u))
      {
        CEDAR_THROW_EXCEPTION(cedar::aux::MatrixMismatchException(input_mat, d_u));
      }

      if (this->_mDimensionality->getValue() == 1)
      {
        d_u += cedar::aux::math::canonicalRowVector(input_mat);
      }
      else
      {
        d_u += input_mat;
      }
    }
  }
  /* add input noise, but use the squared time only for Euler integration (divide by sqrt(time) here, because
   * the next line multiplies by time anyway)
   */
  cv::randn(input_noise, cv::Scalar(0), cv::Scalar(1));
  d_u += 1.0 / sqrt(cedar::unit::Milliseconds(time)/cedar::unit::Milliseconds(1.0))
         *_mInputNoiseGain->getValue() * input_noise;

  // integrate one time step
  u += cedar::unit::Milliseconds(time) / cedar::unit::Milliseconds(tau) * d_u;
}

bool cedar::dyn::NeuralField::isMatrixCompatibleInput(const cv::Mat& matrix) const
{
  // special cases due to opencv's strange handling of 1d-matrices
  if(matrix.dims == 2 && matrix.rows == 1 && matrix.cols == 1)
  {
    // if this field is set to more dimensions than the input (in this case 1), they are not compatible
    if (this->_mDimensionality->getValue() != 0)
      return false;
  }
  else if(matrix.dims == 2 && (matrix.rows == 1 || matrix.cols == 1))
  {
    // if this field is set to more dimensions than the input (in this case 1), they are not compatible
    if (this->_mDimensionality->getValue() != 1)
      return false;

    CEDAR_DEBUG_ASSERT(this->_mSizes->getValue().size() == 1);

    // if the dimensions are both 1, rows or cols must be the same as the field size
    if (static_cast<int>(this->_mSizes->at(0)) != matrix.rows
        && static_cast<int>(this->_mSizes->at(0)) != matrix.cols)
      return false;
  }
  else
  {
    if (static_cast<int>(this->_mDimensionality->getValue()) != matrix.dims)
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
  this->_mSizes->resize(_mDimensionality->getValue(), _mSizes->getDefaultValue());
  this->updateMatrices();
}

void cedar::dyn::NeuralField::dimensionSizeChanged()
{
  this->updateMatrices();
}

void cedar::dyn::NeuralField::updateMatrices()
{
  int dimensionality = static_cast<int>(_mDimensionality->getValue());

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
  }
  else if (dimensionality == 1)
  {
    this->mActivation->getData() = cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(mRestingLevel->getValue()));
    this->mSigmoidalActivation->getData() = cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(0));
    this->mLateralInteraction->getData() = cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(0));
    this->mInputNoise->getData() = cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(0));
    this->mNeuralNoise->getData() = cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(0));
  }
  else
  {
    this->mActivation->getData() = cv::Mat(dimensionality,&sizes.at(0), CV_32F, cv::Scalar(mRestingLevel->getValue()));
    this->mSigmoidalActivation->getData() = cv::Mat(dimensionality, &sizes.at(0), CV_32F, cv::Scalar(0));
    this->mLateralInteraction->getData() = cv::Mat(dimensionality, &sizes.at(0), CV_32F, cv::Scalar(0));
    this->mInputNoise->getData() = cv::Mat(dimensionality, &sizes.at(0), CV_32F, cv::Scalar(0));
    this->mNeuralNoise->getData() = cv::Mat(dimensionality, &sizes.at(0), CV_32F, cv::Scalar(0));
  }
  this->unlockAll();
  if (dimensionality > 0) // only adapt kernel in non-0D case
  {
    for (unsigned int i = 0; i < mKernels.size(); i++)
    {
      this->mKernels.at(i)->setDimensionality(dimensionality);
    }
    this->mNoiseCorrelationKernel->setDimensionality(dimensionality);
  }
}

void cedar::dyn::NeuralField::numberOfKernelsChanged()
{
  const unsigned int& new_number = _mNumberOfKernels->getValue();
  if (mOldNumberOfKernels < new_number) // more kernels
  {
    std::vector<double> sigmas;
    std::vector<double> shifts;
    const unsigned int& field_dimensionality = this->_mDimensionality->getValue();
    for (unsigned int dim = 0; dim < field_dimensionality; ++dim)
    {
      sigmas.push_back(3.0);
      shifts.push_back(0.0);
    }
    // create as many kernels as necessary
    for (unsigned int i = mOldNumberOfKernels; i < new_number; i++)
    {
      cedar::aux::kernel::GaussPtr kernel
        = cedar::aux::kernel::GaussPtr(new cedar::aux::kernel::Gauss(
                                                                      1.0,
                                                                      sigmas,
                                                                      shifts,
                                                                      5.0,
                                                                      field_dimensionality
                                                                      )
                                                                    );
      mKernels.push_back(kernel);
      std::string kernel_name("lateralKernel");
      kernel_name += boost::lexical_cast<std::string>(i);
      // try to create a new buffer - if kernel did exist previously, this buffer is already present
      try
      {
        this->declareBuffer(kernel_name);
        this->setBuffer(kernel_name, mKernels.at(i)->getKernelRaw());
      }
      catch(cedar::proc::DuplicateNameException& exc)
      {
        // buffer already exists...
      }
      this->mKernels.at(i)->hideDimensionality(true);
      this->addConfigurableChild(kernel_name, this->mKernels.at(i));
    }
  }
  else if(mOldNumberOfKernels > new_number) // less kernels
  {
    for (unsigned int i = mOldNumberOfKernels-1; i >= new_number; --i)
    {
      mKernels.pop_back();
      std::string kernel_name("lateralKernel");
      kernel_name += boost::lexical_cast<std::string>(i);
      this->removeConfigurableChild(kernel_name);
    }
  }
  // if mOldNumberOfKernels == new_number, nothing must be done

  // reset mOldNumberOfKernels
  mOldNumberOfKernels = new_number;
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
