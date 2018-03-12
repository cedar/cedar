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

    File:        SerialOrder.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2013 07 08

    Description: Implements the serial order architecture by Yulia Sandamirskaya.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DYN_SERIAL_ORDER_H
#define CEDAR_DYN_SERIAL_ORDER_H

// CEDAR INCLUDES
#include "cedar/dynamics/Dynamics.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/ObjectParameterTemplate.h"
#include "cedar/auxiliaries/math/TransferFunction.h"

// FORWARD DECLARATIONS
#include "cedar/dynamics/steps/SerialOrder.fwd.h"

// SYSTEM INCLUDES
#include <vector>


/*!@brief A step that implements the serial order architecture. See the following publication for a detailed description
 * of the architecture.
 *
 * Sandamirskaya, Yulia and Schöner, Gregor (2010). An embodied account of serial order: how instabilities drive
 * sequence generation. Neural Networks, 23(10), 1164–1179.
 *
 * This step only implements the ordinal nodes and memory nodes as well as the CoS mechanism that allows switching from
 * one action to the next. The fields underneath and their connection to the nodes have to be implemented externally.
 */
class cedar::dyn::SerialOrder : public cedar::dyn::Dynamics
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  typedef cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction> SigmoidParameter;
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(SigmoidParameter);

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  SerialOrder();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public slots:
  //!@brief Slot that is called when the number of ordinal positions is changed.
  void numberOfOrdinalPositionsChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);

  //!@brief Determines whether the data item can be connected to the slot.
  cedar::proc::DataSlot::VALIDITY determineInputValidity
                                  (
                                    cedar::proc::ConstDataSlotPtr slot,
                                    cedar::aux::ConstDataPtr data
                                  ) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Updates the output.
  void eulerStep(const cedar::unit::Time& time);

  //!@brief Resets the step.
  void reset();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief the condition of satisfaction signal to the architecture
  //! If this input approaches 1, this will trigger a switch to the next ordinal
  // position. The next ordinal position is then activated once the input is reset to 0.
  cedar::aux::ConstMatDataPtr mCosSignalInput;

  //!@brief activation of the ordinal nodes of the architecture
  std::vector<cedar::aux::MatDataPtr> mOrdinalNodes;
  //!@brief the output of this step is determined by the sigmoided activation of all ordinal nodes.
  std::vector<cedar::aux::MatDataPtr> mOrdinalNodeOutputs;

  //!@brief activation of the memory nodes of the architecture
  std::vector<cedar::aux::MatDataPtr> mMemoryNodes;
  //!@brief the sigmoided output of the memory nodes
  std::vector<cedar::aux::MatDataPtr> mMemoryNodeOutputs;

  //!@todo the following members are only used to have a nice way to plot the values of discrete nodes in the architecture
  //!@brief buffer for the activation of all ordinal nodes
  cedar::aux::MatDataPtr mOrdinalNodeActivationBuffer;
  //!@brief buffer for the output of all ordinal nodes
  cedar::aux::MatDataPtr mOrdinalNodeOutputBuffer;
  //!@brief buffer for the activation of all memory nodes
  cedar::aux::MatDataPtr mMemoryNodeActivationBuffer;
  //!@brief buffer for the output of all memory nodes
  cedar::aux::MatDataPtr mMemoryNodeOutputBuffer;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief the number of ordinal positions this step manages
  cedar::aux::UIntParameterPtr _mNumberOfOrdinalPositions;

  //!@brief time constant of all ordinal and memory nodes
  cedar::aux::DoubleParameterPtr _mTau;

  // parameters for the ordinal nodes (the variable names from Yulia's paper are stated in comments)
  //!@brief resting level of all ordinal nodes
  cedar::aux::DoubleParameterPtr _mOrdinalNodeRestingLevel; // h_d
  //!@brief strength of the self-excitation of all ordinal nodes
  cedar::aux::DoubleParameterPtr _mOrdinalNodeSelfExcitationWeight; // c_0
  //!@brief strength of the global inhibition between the ordinal nodes
  cedar::aux::DoubleParameterPtr _mOrdinalNodeGlobalInhibitionWeight; // c_1
  //!@brief weight between a memory node and the subsequent ordinal node
  cedar::aux::DoubleParameterPtr _mMemoryNodeToNextOrdinalNodeWeight; // c_2
  //!@brief weight between a memory node and the corresponding ordinal node
  cedar::aux::DoubleParameterPtr _mMemoryNodeToSameOrdinalNodeWeight; // c_3

  // parameters for the memory nodes
  //!@brief resting level of all memory nodes
  cedar::aux::DoubleParameterPtr _mMemoryNodeRestingLevel; // h_dm
  //!@brief strength of the self-excitation of all memory nodes
  cedar::aux::DoubleParameterPtr _mMemoryNodeSelfExcitationWeight; // c_4
  //!@brief strength of the global inhibition between the memory nodes
  cedar::aux::DoubleParameterPtr _mMemoryNodeGlobalInhibitionWeight; // c_5
  //!@brief weight between an ordinal node and the corresponding memory node
  cedar::aux::DoubleParameterPtr _mOrdinalNodeToSameMemoryNodeWeight; // c_6


  //!@brief sigmoid function used to determine the output of all nodes
  SigmoidParameterPtr _mSigmoid;
}; // class cedar::dyn::SerialOrder

#endif // CEDAR_DYN_SERIAL_ORDER_H
