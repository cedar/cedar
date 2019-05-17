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

    File:        SerialOrder.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2013 07 08

    Description: Implements the serial order architecture by Yulia Sandamirskaya.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/dynamics/steps/SerialOrder.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/transferFunctions/AbsSigmoid.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/annotation/DiscreteMetric.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <iostream>
#ifndef Q_MOC_RUN
  #include <boost/make_shared.hpp>
#endif
#include <string>

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
      new ElementDeclarationTemplate<cedar::dyn::SerialOrder>
      (
        "DFT Templates",
        "cedar.dynamics.SerialOrder"
      )
    );
    declaration->setIconPath(":/steps/serial_order.svg");
    declaration->setDescription
    (
      "Generates a sequences of transitions between nodes that "
      "represent the ordinal position of an action in a sequence."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dyn::SerialOrder::SerialOrder()
:
mOrdinalNodeActivationBuffer(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
mOrdinalNodeOutputBuffer(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
mMemoryNodeActivationBuffer(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
mMemoryNodeOutputBuffer(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
// parameters
_mNumberOfOrdinalPositions(new cedar::aux::UIntParameter(this, "number of ordinal positions", 3, 1, 10)),
_mTau
(
  new cedar::aux::DoubleParameter
  (
    this,
    "tau",
    100.0,
    cedar::aux::DoubleParameter::LimitType::positive()
  )
),
_mOrdinalNodeRestingLevel
(
  new cedar::aux::DoubleParameter
  (
    this,
    "ordinal resting level",
    -5,
    cedar::aux::DoubleParameter::LimitType::negative()
  )
),
_mOrdinalNodeSelfExcitationWeight
(
  new cedar::aux::DoubleParameter
  (
    this,
    "ordinal self excitation",
    6,
    cedar::aux::DoubleParameter::LimitType::positive()
  )
),
_mOrdinalNodeGlobalInhibitionWeight
(
  new cedar::aux::DoubleParameter
  (
    this,
    "ordinal global inhibition",
    -11,
    cedar::aux::DoubleParameter::LimitType::negative()
  )
),
_mMemoryNodeToNextOrdinalNodeWeight
(
  new cedar::aux::DoubleParameter
  (
    this,
    "memory to next ordinal",
    5,
    cedar::aux::DoubleParameter::LimitType::positive()
  )
),
_mMemoryNodeToSameOrdinalNodeWeight
(
  new cedar::aux::DoubleParameter
  (
    this,
    "memory to same ordinal",
    -5,
    cedar::aux::DoubleParameter::LimitType::negative()
  )
),
_mMemoryNodeRestingLevel
(
  new cedar::aux::DoubleParameter
  (
    this,
    "memory resting level",
    -2,
    cedar::aux::DoubleParameter::LimitType::negative()
  )
),
_mMemoryNodeSelfExcitationWeight
(
  new cedar::aux::DoubleParameter
  (
    this,
    "memory self excitation",
    7,
    cedar::aux::DoubleParameter::LimitType::positive()
  )
),
_mMemoryNodeGlobalInhibitionWeight
(
  new cedar::aux::DoubleParameter
  (
    this,
    "memory global inhibition",
    -2,
    cedar::aux::DoubleParameter::LimitType::negative()
  )
),
_mOrdinalNodeToSameMemoryNodeWeight
(
  new cedar::aux::DoubleParameter
  (
    this,
    "ordinal to same memory",
    7,
    cedar::aux::DoubleParameter::LimitType::positive()
  )
),
_mSigmoid
(
  new cedar::dyn::SerialOrder::SigmoidParameter
  (
    this,
    "sigmoid",
    cedar::aux::math::SigmoidPtr(new cedar::aux::math::AbsSigmoid(0.0, 100.0))
  )
)
{
  // declare the inputs
  this->declareInput("CoS signal input", false);
  // declare the buffers
  this->declareBuffer("ordinal node activation", this->mOrdinalNodeActivationBuffer);
  this->declareBuffer("memory node activation", this->mMemoryNodeActivationBuffer);
  this->declareBuffer("ordinal node output", this->mOrdinalNodeOutputBuffer);
  this->declareBuffer("memory node output", this->mMemoryNodeOutputBuffer);
  // set up the rest
  this->numberOfOrdinalPositionsChanged();

  // make the plots of these matrices appear as discrete points rather than lines
  this->mOrdinalNodeActivationBuffer->setAnnotation(boost::make_shared<cedar::aux::annotation::DiscreteMetric>());
  this->mMemoryNodeActivationBuffer->setAnnotation(boost::make_shared<cedar::aux::annotation::DiscreteMetric>());
  this->mOrdinalNodeOutputBuffer->setAnnotation(boost::make_shared<cedar::aux::annotation::DiscreteMetric>());
  this->mMemoryNodeOutputBuffer->setAnnotation(boost::make_shared<cedar::aux::annotation::DiscreteMetric>());

  // connect the parameter's change signal
  QObject::connect(_mNumberOfOrdinalPositions.get(), SIGNAL(valueChanged()), this, SLOT(numberOfOrdinalPositionsChanged()));
}

void cedar::dyn::SerialOrder::numberOfOrdinalPositionsChanged()
{
  // get the old and new number of ordinal positions
  unsigned int new_number_of_positions = _mNumberOfOrdinalPositions->getValue();
  unsigned int old_number_of_positions = mOrdinalNodes.size();

  // if the number of positions is being increased
  if (new_number_of_positions > old_number_of_positions)
  {
    // for all new ordinal positions
    for (unsigned int i = old_number_of_positions; i < new_number_of_positions; ++i)
    {
      // resize all relevant vectors and matrices to their new (larger) sizes and initialize their content
      mOrdinalNodes.push_back(cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat::ones(1, 1, CV_32F) * _mOrdinalNodeRestingLevel->getValue())));
      mMemoryNodes.push_back(cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat::ones(1, 1, CV_32F) * _mMemoryNodeRestingLevel->getValue())));
      mOrdinalNodeOutputs.push_back(cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))));
      mMemoryNodeOutputs.push_back(cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))));
      mOrdinalNodeActivationBuffer->getData().resize(new_number_of_positions, _mOrdinalNodeRestingLevel->getValue());
      mOrdinalNodeOutputBuffer->getData().resize(new_number_of_positions, 0.0);
      mMemoryNodeActivationBuffer->getData().resize(new_number_of_positions, _mMemoryNodeRestingLevel->getValue());
      mMemoryNodeOutputBuffer->getData().resize(new_number_of_positions, 0.0);

      // declare any new output nodes
      std::string slot_name = "ordinal node " + cedar::aux::toString<unsigned int>(i) + " output";
      if (!this->hasOutputSlot(slot_name))
      {
        this->declareOutput(slot_name, this->mOrdinalNodeOutputs.at(i));
      }
    }
  }
  // if the number of positions is being reduced
  else if (new_number_of_positions < old_number_of_positions)
  {
    // resize all relevant vectors and matrices to their new (smaller) sizes
    mOrdinalNodeOutputs.resize(new_number_of_positions);
    mOrdinalNodes.resize(new_number_of_positions);
    mMemoryNodes.resize(new_number_of_positions);
    mMemoryNodeOutputs.resize(new_number_of_positions);
    mOrdinalNodeActivationBuffer->getData().resize(new_number_of_positions);
    mOrdinalNodeOutputBuffer->getData().resize(new_number_of_positions);
    mMemoryNodeActivationBuffer->getData().resize(new_number_of_positions);
    mMemoryNodeOutputBuffer->getData().resize(new_number_of_positions);

    // for all ordinal positions that are being deleted
    for (unsigned int i = new_number_of_positions; i < old_number_of_positions; ++i)
    {
      // remove the output slots
      std::string slot_name = "ordinal node " + cedar::aux::toString<unsigned int>(i) + " output";
      if (this->hasOutputSlot(slot_name))
      {
        this->removeOutputSlot(slot_name);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dyn::SerialOrder::eulerStep(const cedar::unit::Time& time)
{
  // check that everything is of the same size
  CEDAR_ASSERT(mOrdinalNodes.size() == mMemoryNodes.size());
  CEDAR_ASSERT(mOrdinalNodes.size() == mOrdinalNodeOutputs.size());

  // to implement the global inhibition between the ordinal and memory nodes, we need to sum up
  // all outputs of the ordinal layer
  cv::Mat sum_of_ordinal_outputs = cv::Mat::zeros(1, 1, CV_32F);

  // go through all ordinal positions to compute the sigmoided output of all nodes
  for (unsigned int i = 0; i < mOrdinalNodes.size(); ++i)
  {
    // compute the sigmoided output of the ordinal nodes
    cv::Mat& d = mOrdinalNodes.at(i)->getData();
    cv::Mat& d_output = mOrdinalNodeOutputs.at(i)->getData();
    d_output = _mSigmoid->getValue()->compute<float>(d);
    // sum the outputs
    sum_of_ordinal_outputs += d_output;

    // compute the sigmoided output of the memory nodes
    cv::Mat& dm = mMemoryNodes.at(i)->getData();
    cv::Mat& dm_output = mMemoryNodeOutputs.at(i)->getData();
    dm_output = _mSigmoid->getValue()->compute<float>(dm);
  }

  // go through all ordinal positions again, this time to do the actual Euler approximation
  for (unsigned int i = 0; i < mOrdinalNodes.size(); ++i)
  {
    // this equation uses variable names similar to those in Sandamirskaya, Schoener, 2010
    // (see above for the full reference)
    cv::Mat& d = this->mOrdinalNodes.at(i)->getData();
    const float& tau = this->_mTau->getValue();
    const float& h = this->_mOrdinalNodeRestingLevel->getValue();
    const float& c0 = this->_mOrdinalNodeSelfExcitationWeight->getValue();
    const float& c1 = this->_mOrdinalNodeGlobalInhibitionWeight->getValue();
    const float& c2 = this->_mMemoryNodeToNextOrdinalNodeWeight->getValue();
    const float& c3 = this->_mMemoryNodeToSameOrdinalNodeWeight->getValue();

    cv::Mat& f_d = mOrdinalNodeOutputs.at(i)->getData();
    cv::Mat& f_dm = mMemoryNodeOutputs.at(i)->getData();

    // compute the change rate of the ordinal node
    cv::Mat d_dot = -d + h + c0 * f_d
                    + c1 * (sum_of_ordinal_outputs - f_d)
                    + c3 * f_dm;

    // for the first ordinal position only
    if (i == 0)
    {
      d_dot += c2;
    }
    // for all other nodes
    else
    {
      d_dot += c2 * mMemoryNodeOutputs.at(i-1)->getData();
    }

    //!@todo explain that the expected input is positive (CoS signal)
    if (this->mCosSignalInput)
    {
      d_dot -= this->mCosSignalInput->getData();
    }

    d += time / cedar::unit::Time(tau * cedar::unit::milli * cedar::unit::seconds) * d_dot;

    cv::Mat& dm = this->mMemoryNodes.at(i)->getData();
    const float& hm = this->_mMemoryNodeRestingLevel->getValue();
    const float& c4 = this->_mMemoryNodeSelfExcitationWeight->getValue(); // 3.5
    const float& c5 = this->_mMemoryNodeGlobalInhibitionWeight->getValue(); // 2.0
    const float& c6 = this->_mOrdinalNodeToSameMemoryNodeWeight->getValue(); // 2.6

    // compute the change rate of the memory node
    cv::Mat dm_dot = -dm + hm + c4 * f_dm
                    + c5 * (sum_of_ordinal_outputs - f_dm)
                    + c6 * f_d;

    dm += time / cedar::unit::Time(tau * cedar::unit::milli * cedar::unit::seconds) * dm_dot;

    // save a copy of the output in the buffer
    //!@todo this is only needed because we don't have any nice way to plot all the discrete values yet
    mOrdinalNodeActivationBuffer->getData().at<float>(i, 0) = d.at<float>(0, 0);
    mOrdinalNodeOutputBuffer->getData().at<float>(i, 0) = f_d.at<float>(0, 0);
    mMemoryNodeActivationBuffer->getData().at<float>(i, 0) = dm.at<float>(0, 0);
    mMemoryNodeOutputBuffer->getData().at<float>(i, 0) = f_dm.at<float>(0, 0);
  }
}

void cedar::dyn::SerialOrder::reset()
{
  for (unsigned int i = 0; i < mOrdinalNodes.size(); ++i)
  {
    cv::Mat& d = this->mOrdinalNodes.at(i)->getData();
    d = cv::Mat::ones(1, 1, CV_32F) * _mOrdinalNodeRestingLevel->getValue();

    cv::Mat f_d = _mSigmoid->getValue()->compute<float>(d);
    this->mOrdinalNodeOutputs.at(i)->getData() = f_d;


    cv::Mat& dm = this->mMemoryNodes.at(i)->getData();
    dm = cv::Mat::ones(1, 1, CV_32F) * _mMemoryNodeRestingLevel->getValue();

    cv::Mat f_dm = _mSigmoid->getValue()->compute<float>(dm);
    this->mMemoryNodeOutputs.at(i)->getData() = f_dm;
  }
}

cedar::proc::DataSlot::VALIDITY cedar::dyn::SerialOrder::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr CEDAR_DEBUG_ONLY(slot),
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  // First, let's make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(slot->getName() == "CoS signal input")

  if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
  {
    if (mat_data->getDimensionality() == 0 && mat_data->getData().type() == CV_32F)
    {
      // Mat data is accepted.
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }

  // Everything else is rejected.
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::dyn::SerialOrder::inputConnectionChanged(const std::string& inputName)
{
  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "CoS signal input");

  // Assign the input to the member. This saves us from casting in every computation step.
  if (inputName == "CoS signal input")
  {
    this->mCosSignalInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }
}
