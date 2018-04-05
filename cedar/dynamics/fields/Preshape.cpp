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

    File:        Preshape.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 11 08

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/dynamics/fields/Preshape.h"
#include "cedar/auxiliaries/math/transferFunctions/AbsSigmoid.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/math/sigmoids.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration
    (
      new cedar::proc::ElementDeclarationTemplate<cedar::dyn::Preshape>("DFT", "cedar.dynamics.Preshape")
    );
    declaration->setIconPath(":/steps/preshape.svg");
    declaration->setDescription
    (
      "A dynamical system that slowly accumulates activation at locations of strongly active input. The activation "
      "decays slowly over time."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dyn::Preshape::Preshape()
:
mActivation(new cedar::aux::MatData(cv::Mat::zeros(50,50,CV_32F))),
_mDimensionality
(
  new cedar::aux::UIntParameter(this, "dimensionality", 2, cedar::aux::UIntParameter::LimitType::positiveZero(4))
),
_mSizes(new cedar::aux::UIntVectorParameter(this, "sizes", 2, 50, 1, 5000)),
_mTimeScaleBuildUp
(
  new cedar::aux::DoubleParameter(this, "time scale build up", 100.0, cedar::aux::DoubleParameter::LimitType::positive())
),
_mTimeScaleDecay
(
  new cedar::aux::DoubleParameter(this, "time scale decay", 1000.0, cedar::aux::DoubleParameter::LimitType::positive())
),
_mSigmoid
(
  new cedar::dyn::Preshape::SigmoidParameter
  (
    this,
    "sigmoid",
    cedar::aux::math::SigmoidPtr(new cedar::aux::math::AbsSigmoid(0.5, 1000.0))
  )
)
{
  _mSizes->makeDefault();
  QObject::connect(_mSizes.get(), SIGNAL(valueChanged()), this, SLOT(dimensionSizeChanged()));
  QObject::connect(_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(dimensionalityChanged()));
  auto activation_slot = this->declareOutput("activation", mActivation);
  activation_slot->setSerializable(true);

  this->declareInput("input", true);
  this->declareInput("peak detector", false);

  // now check the dimensionality and sizes of all matrices
  this->updateMatrices();

  this->registerFunction("reset memory", boost::bind(&cedar::dyn::Preshape::resetMemory, this));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dyn::Preshape::eulerStep(const cedar::unit::Time& time)
{
  cv::Mat& preshape = this->mActivation->getData();
  cedar::aux::ConstDataPtr input = this->getInput("input");
  const cv::Mat& input_mat = input->getData<cv::Mat>();
  const double& tau_build_up = this->_mTimeScaleBuildUp->getValue();
  const double& tau_decay = this->_mTimeScaleDecay->getValue();
  cv::Mat sigmoided_input = this->_mSigmoid->getValue()->compute(input_mat);
  double peak = 1.0;
  if (auto peak_detector = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->getInput("peak detector")))
  {
    peak = cedar::aux::math::getMatrixEntry<double>(peak_detector->getData(), 0, 0);
  }

  // one possible preshape dynamic
  preshape +=
  (
    time / cedar::unit::Time(tau_build_up * cedar::unit::milli * cedar::unit::seconds)
      * (-1.0 * preshape + input_mat).mul(sigmoided_input)
    + time / cedar::unit::Time(tau_decay * cedar::unit::milli * cedar::unit::seconds)
      * (-1.0 * preshape.mul((1.0 - sigmoided_input)))
  ) * peak;
}

cedar::proc::DataSlot::VALIDITY cedar::dyn::Preshape::determineInputValidity
                                                         (
                                                           cedar::proc::ConstDataSlotPtr slot,
                                                           cedar::aux::ConstDataPtr data
                                                         ) const
{
  if (slot->getRole() == cedar::proc::DataRole::INPUT && slot->getName() == "input")
  {
    if (cedar::aux::ConstMatDataPtr input = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
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
    return cedar::proc::DataSlot::VALIDITY_ERROR;
  }
  else if (slot->getRole() == cedar::proc::DataRole::INPUT && slot->getName() == "peak detector")
  {
    if (cedar::aux::ConstMatDataPtr input = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
    {
      if (cedar::aux::math::getDimensionalityOf(input->getData()) != 0)
      {
        return cedar::proc::DataSlot::VALIDITY_ERROR;
      }
      else
      {
        return cedar::proc::DataSlot::VALIDITY_VALID;
      }
    }
  }
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

bool cedar::dyn::Preshape::isMatrixCompatibleInput(const cv::Mat& matrix) const
{
  if (matrix.type() != CV_32F)
  {
    return false;
  }

  unsigned int matrix_dim = cedar::aux::math::getDimensionalityOf(matrix);
  return this->_mDimensionality->getValue() == matrix_dim
           && cedar::aux::math::matrixSizesEqual(matrix, this->mActivation->getData());
}

void cedar::dyn::Preshape::dimensionalityChanged()
{
  this->_mSizes->resize(_mDimensionality->getValue(), _mSizes->getDefaultValue());
  this->updateMatrices();
}

void cedar::dyn::Preshape::dimensionSizeChanged()
{
  this->updateMatrices();
}

void cedar::dyn::Preshape::resetMemory()
{
  mActivation->getData() = 0.0;
}

void cedar::dyn::Preshape::updateMatrices()
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
    if (max_size > std::numeric_limits<unsigned int>::max()/100.0) // heuristics
    {
      CEDAR_THROW(cedar::aux::RangeException, "cannot handle matrices of this size");
      return;
    }
  }
  this->lockAll();
  if (dimensionality == 0)
  {
    this->mActivation->getData() = cv::Mat(1, 1, CV_32F, cv::Scalar(0));
  }
  else if (dimensionality == 1)
  {
    this->mActivation->getData() = cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(0));
  }
  else
  {
    this->mActivation->getData() = cv::Mat(dimensionality,&sizes.at(0), CV_32F, cv::Scalar(0));
  }
  this->unlockAll();
  this->revalidateInputSlot("input");
  this->emitOutputPropertiesChangedSignal("activation");
}
