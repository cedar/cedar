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

    File:        Convolution.h

    Maintainer:  Ulja van Hengel
    Email:       ulja@ini.ruhr-uni-bochum.de
    Date:        2012 06 04

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_CONVOLUTION_H
#define CEDAR_PROC_STEPS_CONVOLUTION_H

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/convolution/Convolution.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.h"
#include "cedar/auxiliaries/kernel/Kernel.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/Convolution.fwd.h"

// SYSTEM INCLUDES

/*!@brief This step performs a convolution between an input and a kernel list or an explicitly specified kernel matrix.
 */
class cedar::proc::steps::Convolution : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT
  
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief a parameter for kernel objects
  typedef cedar::aux::ObjectListParameterTemplate<cedar::aux::kernel::Kernel> KernelListParameter;

  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(KernelListParameter);
  //!@endcond

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Convolution();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Updates the output matrix.
  void compute(const cedar::proc::Arguments& arguments);

  //!@brief Returns the border type of the current convolution.
  inline cedar::aux::conv::BorderType::Id getBorderType() const
  {
    return this->mConvolution->getBorderType();
  }

  //!@brief Returns the border type of the current convolution.
  inline cedar::aux::conv::Mode::Id getMode() const
  {
    return this->mConvolution->getMode();
  }

public slots:
  //!@brief Triggers a computation of a step.
  void recompute();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Determines whether the data item can be connected to the slot.
  cedar::proc::DataSlot::VALIDITY determineInputValidity
                                  (
                                    cedar::proc::ConstDataSlotPtr slot,
                                    cedar::aux::ConstDataPtr data
                                  ) const;

  /*!@brief reads a configuration from a node
   * @param node Said node.
   */
  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);

  /*!@brief Updates the convolution object when a new kernel is added.
   */
  void slotKernelAdded(size_t kernelIndex);

  /*!@brief Adds a kernel to the convolution object.
   */
  void addKernelToConvolution(cedar::aux::kernel::KernelPtr kernel);

  /*!@brief Removes a kernel from the convolution object.
   */
  void removeKernelFromConvolution(size_t index);

  //!@brief Makes the kernel list stored in the convolution equal to the one in the field.
  void transferKernelsToConvolution();

  /*!@brief Returns the convolution object currently selected.
   */
  inline cedar::aux::conv::ConvolutionPtr getConvolution()
  {
    return this->mConvolution;
  }

  unsigned int getDimensionality() const;

  void inputDimensionalityChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief The Data containing the input matrix
  cedar::aux::ConstMatDataPtr mMatrix;
  
  //!@brief The Data containing the input kernel
  cedar::aux::ConstMatDataPtr mKernel;
  
  //!@brief The data containing the result of the convolution.
  cedar::aux::MatDataPtr mOutput;

  //!@brief Containing convolution methods and parameter
  cedar::aux::conv::ConvolutionPtr mConvolution;

  boost::signals2::connection mKernelAddedConnection;
  boost::signals2::connection mKernelRemovedConnection;

  bool mRevalidating;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  KernelListParameterPtr _mKernels;

}; // class cedar::proc::steps::Convolution

#endif // CEDAR_PROC_STEPS_CONVOLUTION_H
