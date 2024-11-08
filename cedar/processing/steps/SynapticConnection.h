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

    File:        SynapticConnection.h

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.rub.de
    Date:        2022 02 10

    Description: Header file for the class cedar::proc::steps::SynapticConnection.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_SYNAPTIC_CONNECTION_H
#define CEDAR_PROC_STEPS_SYNAPTIC_CONNECTION_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include <cedar/processing/Step.h>
#include <cedar/auxiliaries/MatData.h>
#include <cedar/auxiliaries/ObjectListParameterTemplate.h>
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/kernel/Kernel.h"
#include "cedar/auxiliaries/convolution/Convolution.h"
#include "cedar/processing/ProjectionMappingParameter.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"
#include "cedar/processing/ProjectionMapping.h"
#include "cedar/processing/steps/Projection.h"
#include "cedar/processing/ProjectionFunctions.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/SynapticConnection.fwd.h"

// SYSTEM INCLUDES



/*! SynapticConnection Step
 *
 * This step combines the StaticGain, Projection and Convolution step and is used primarily for the XML import/export
 * to serve as a visualization of the properties of a connection.
 */
class cedar::proc::steps::SynapticConnection : public cedar::proc::Step, cedar::proc::ProjectionFunctions
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

  //! typedef for the projection method function pointer
  //! (function pointer to a void method in cedar::proc::steps::Projection)
  typedef void (cedar::proc::ProjectionFunctions::
								*ProjectionFunctionPtr)( cedar::aux::ConstMatDataPtr,
                                                                  cedar::aux::MatDataPtr,
                                                                  std::vector<unsigned int>,
                                                                  cedar::aux::EnumParameterPtr,
                                                                  cedar::proc::ProjectionMappingParameterPtr);

	//!@cond SKIPPED_DOCUMENTATION
	CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(KernelListParameter);
	//!@endcond

public:
  /*!@brief Reduced compression type for synaptic connection projection (copied from Projection.h)
   */
  class ReducedCompressionType
  {
  public:
    //! Type of the enum's identifiers.
    typedef cedar::aux::EnumId Id;

    //! Pointer to the enum's base type.
    typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

    /*!@brief Static construction method that defines all values.
     */
    static void construct()
    {
      mType.type()->def(cedar::aux::Enum(SUM, "SUM", "Sum"));
      mType.type()->def(cedar::aux::Enum(MAXIMUM, "MAXIMUM", "Maximum"));
    }

    //!@brief Returns the base object of the enum.
    static const cedar::aux::EnumBase& type()
    {
      return *mType.type();
    }

    //!@brief Returns a pointer to the base object of the enum.
    static const TypePtr& typePtr()
    {
      return mType.type();
    }

  public:

    //! Compression is done by calculating the sum along the compressed direction.
    static const Id SUM = CEDAR_OPENCV_CONSTANT(REDUCE_SUM);

    //! Compression is done by calculating the maximum along the compressed direction.
    static const Id MAXIMUM = CEDAR_OPENCV_CONSTANT(REDUCE_MAX);

  private:
    //!@brief The type object of the enum.
    static cedar::aux::EnumType<ReducedCompressionType> mType;
  };
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  SynapticConnection();

  //!@brief Destructor
  virtual ~SynapticConnection();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  bool isXMLExportable(std::string& errorMsg) override;

  void writeConfigurationXML(cedar::aux::ConfigurationNode& root) const;

  void readConfigurationXML(const cedar::aux::ConfigurationNode& root);

public slots:
  void recompute();
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

protected slots:
  //Slots for Projection
  //!@brief this slot is triggered whenever the dimensionality of its output is changed by the user
  void projectionOutputDimensionalityChanged();
  //!@brief this slot is triggered whenever the size of any dimension of the output is changed by the user
  void projectionOutputDimensionSizesChanged();
  //!@brief chooses the appropriate projection method for the current input and output
  void reconfigure(bool triggerSubsequent = true);
  //!@brief this slot is triggered whenever the projectionEnabled parameter is toggled
  void projectionEnabledToggled();
  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void compute(const Arguments &);

  void updateInputSum();

	////Functions for Convolution Step

  void inputConnectionChanged(const std::string& inputName);

	/*!@brief Updates the convolution object when a new kernel is added.
 	*/
	void convolutionSlotKernelAdded(cedar::aux::conv::ConvolutionPtr convolution, KernelListParameterPtr kernelsParameter,
                                  size_t kernelIndex);

	/*!@brief Adds a kernel to the convolution object.
 */
	void addKernelToConvolution(cedar::aux::conv::ConvolutionPtr convolution, cedar::aux::kernel::KernelPtr kernel);

	/*!@brief Removes a kernel from the convolution object.
 	*/
	void removeKernelFromConvolution(cedar::aux::conv::ConvolutionPtr convolution, size_t index);

	//!@brief Makes the kernel list stored in the convolution equal to the one in the field.
	void transferKernelsToConvolution(cedar::aux::conv::ConvolutionPtr convolution,
                                    KernelListParameterPtr kernelsParameter);

	/*!@brief Returns the convolution object currently selected.
 	*/
	inline cedar::aux::conv::ConvolutionPtr getConvolution()
	{
		return this->mConvolution;
	}

	unsigned int convolutionGetDimensionality() const;

	void convolutionInputDimensionalityChanged();

  void preparePointWiseWeightMat(cv::Mat& pointWiseMat);

  ////Functions for Projection Step
  //!@brief initializes or reconfigures the output matrix
  void initializeOutputMatrix();
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
	//!@brief The Data containing the input matrix
	cedar::aux::MatDataPtr mMatrix;

  //// Members for Convolution
  cedar::aux::conv::ConvolutionPtr mConvolution;
	KernelListParameterPtr mKernelsParameter;

  //// Members for PointWiseWeight operation
  cedar::aux::conv::ConvolutionPtr mPointWiseWeightConvolution;
  KernelListParameterPtr mPointWiseWeightKernelsParameter;
  cedar::aux::MatDataPtr mPreparedPointWiseMat;

	boost::signals2::connection mKernelAddedConnection;
	boost::signals2::connection mKernelRemovedConnection;

	bool mRevalidating;

  cedar::aux::MatDataPtr mInputSum;

  cedar::aux::MatDataPtr mConvolutionOutput;

  ////Members for StaticGain
  cedar::aux::DoubleParameterPtr mGainFactorParameter;
  cedar::aux::MatDataPtr mStaticGainOutput;

  ////Members for Projection
  //!@brief Boolean that enables the projection
  cedar::aux::BoolParameterPtr mProjectionEnabled;

  //!@brief mapping between the input and output dimensions
  cedar::proc::ProjectionMappingParameterPtr mProjectionDimensionMappings;

  //!@brief dimensionality of the output
  cedar::aux::UIntParameterPtr mProjectionOutputDimensionality;

  //!@brief sizes of all dimensions of the output of the projection
  cedar::aux::UIntVectorParameterPtr mProjectionOutputDimensionSizes;

  //!@brief type of compression used when reducing the dimensionality (maximum, minimum, average, sum)
  cedar::aux::EnumParameterPtr mProjectionCompressionType;

  //!@brief final output of step
  cedar::aux::MatDataPtr mOutput;

  //!@brief vector holding all indices of dimensions that have to be compressed
  //! this is only in use when the projection is set up to compress the dimensionality of the input
  std::vector<unsigned int> mProjectionIndicesToCompress;

  //!@brief function pointer to one of the projection member functions
	ProjectionFunctionPtr mpProjectionMethod;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet
};

#endif // CEDAR_PROC_STEPS_SYNAPTIC_CONNECTION_H

