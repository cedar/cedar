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

    File:        NeuralCoordinateTransform.h

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de
    Date:        2023 05 03

    Description: Header file for the class cedar::proc::steps::NeuralCoordinateTransform.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_NEURAL_COORDINATE_TRANSFORM_H
#define CEDAR_PROC_STEPS_NEURAL_COORDINATE_TRANSFORM_H

// CEDAR CONFIGURATION
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/convolution/Convolution.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.h"
#include "cedar/auxiliaries/kernel/Kernel.h"

// CEDAR INCLUDES
#include <cedar/processing/Step.h>
#include "cedar/auxiliaries/UIntVectorParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/NeuralCoordinateTransform.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::steps::NeuralCoordinateTransform : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
Q_OBJECT

private:
  //!@brief a parameter for kernel objects
  typedef cedar::aux::ObjectListParameterTemplate<cedar::aux::kernel::Kernel> KernelListParameter;

  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(KernelListParameter);
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  NeuralCoordinateTransform();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  ////FOR ALL STEPS
  //!@brief Updates the output matrix.
  void compute(const cedar::proc::Arguments& arguments);

  void writeConfigurationXML(cedar::aux::ConfigurationNode& root) const;

  bool isXMLExportable(std::string& errorMsg) override;

  ////CONVOLUTION
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
  ////FOR ALL STEPS
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
  ////FOR ALL STEPS

  unsigned int getDimensionality() const;

  void inputDimensionalityChanged();

  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);

  ////CONVULTION STUFF
  /*!@brief Adds a kernel to the convolution object.
 */
  void addKernelToConvolution(cedar::aux::kernel::KernelPtr kernel);

  /*!@brief Removes a kernel from the convolution object.
   */
  void removeKernelFromConvolution(size_t index);

  /*!@brief Returns the convolution object currently selected.
 */
  inline cedar::aux::conv::ConvolutionPtr getConvolution()
  {
    return this->mConvolution;
  }

  ////MATRIX SLICE STUFF
  void allocateMatricSliceOutputMatrix();

  void updateMatrixSliceDimensionality();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  ////CONVULTION STUFF
  //!@brief The Data containing the input matrix
  cedar::aux::ConstMatDataPtr mInputMatrix;

  //!@brief The Data containing the input kernel
  cedar::aux::ConstMatDataPtr mKernel;

  //!@brief The data containing the
  //! result of the convolution.
  cedar::aux::MatDataPtr mConvolutionOutput;

  //!@brief Containing convolution methods and parameter
  cedar::aux::conv::ConvolutionPtr mConvolution;

  bool mRevalidating;

  ////MATRIX SLICE STUFF
  //! Vector containing the ranges used for slicing the matrix.
  std::vector<cv::Range> mRanges;

  //! Output matrix.
  cedar::aux::MatDataPtr mMatrixSliceOutput;

  ////FLIP STUFF
  cedar::aux::BoolParameterPtr mFlipAllDimensions;

  cedar::aux::MatDataPtr mFlipAllDimensionsOutput;
  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:

}; // class cedar::proc::steps::NeuralCoordinateTransform

#endif // CEDAR_PROC_STEPS_NEURAL_COORDINATE_TRANSFORM_H

