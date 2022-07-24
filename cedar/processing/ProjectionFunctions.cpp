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

    File:        ProjectionFunctions.cpp

    Maintainer:  Yogeshwar Agnihotri, Lars Janssen
    Email:       yogeshwar.agnihotri@ini.rub.de, lars.janssen@ini.rub.de
    Date:        2022 05 15

    Description: Source file for the class cedar::proc::ProjectionFunctions.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/ProjectionFunctions.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/MatrixIterator.h"
#include "cedar/processing/ProjectionMapping.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::ProjectionFunctions::ProjectionFunctions()
{
}

cedar::proc::ProjectionFunctions::~ProjectionFunctions()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


template<typename T>
void cedar::proc::ProjectionFunctions::expand0DtoND(cedar::aux::ConstMatDataPtr input,
																									 cedar::aux::MatDataPtr output,
																									 std::vector<unsigned int> indicesToCompress,
																									 cedar::aux::EnumParameterPtr compressionType,
																									 cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	CEDAR_DEBUG_ASSERT(input->getData().size[0] == 1);
	// set all values of the output matrix to the single value of the input
	output->getData() = cv::Scalar(input->getData().at<T>(0));
}

void cedar::proc::ProjectionFunctions::expand0DtoND(cedar::aux::ConstMatDataPtr input,
																									 cedar::aux::MatDataPtr output,
																									 std::vector<unsigned int> indicesToCompress,
																									 cedar::aux::EnumParameterPtr compressionType,
																									 cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	switch (input->getCvType())
	{
		case CV_32F:
		{
			this->expand0DtoND<float>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		case CV_64F:
		{
			this->expand0DtoND<double>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		default:
		CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
	}
}

//! not used at the moment
void cedar::proc::ProjectionFunctions::expand1Dto2D(cedar::aux::ConstMatDataPtr input,
																									 cedar::aux::MatDataPtr output,
																									 std::vector<unsigned int> indicesToCompress,
																									 cedar::aux::EnumParameterPtr compressionType,
																									 cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	const cv::Mat& inputMat = input->getData();
	cv::Mat& outputMat = output->getData();
	outputMat = outputMat * 0;

	bool transposed = false;

	// if dimension 0 is mapped onto 1
	if (dimensionMappings->getValue()->lookUp(0) == 1)
	{
		transposed = true;
		outputMat = outputMat.t();
	}

	for (int i = 0; i < inputMat.rows; ++i)
	{
		inputMat.row(i).copyTo(outputMat);
	}

	if (transposed)
	{
		outputMat = outputMat.t();
	}
}


void cedar::proc::ProjectionFunctions::expand2Dto3D(cedar::aux::ConstMatDataPtr input,
																									 cedar::aux::MatDataPtr output,
																									 std::vector<unsigned int> indicesToCompress,
																									 cedar::aux::EnumParameterPtr compressionType,
																									 cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	switch (input->getCvType())
	{
		case CV_32F:
		{
			this->expand2Dto3D<float>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		case CV_64F:
		{
			this->expand2Dto3D<double>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		default:
		CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
	}
}

template<typename T>
void cedar::proc::ProjectionFunctions::expand2Dto3D(cedar::aux::ConstMatDataPtr _input,
																									 cedar::aux::MatDataPtr _output,
																									 std::vector<unsigned int> indicesToCompress,
																									 cedar::aux::EnumParameterPtr compressionType,
																									 cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	const cv::Mat& input = _input->getData();
	cv::Mat& output = _output->getData();
	// sanity check
	CEDAR_DEBUG_ASSERT
	(
					cedar::aux::math::getDimensionalityOf(input) == 2
					&& cedar::aux::math::getDimensionalityOf(output) == 3
	);
	auto mapping = dimensionMappings->getValue();
	unsigned int number_of_mappings = mapping->getNumberOfMappings();
	unsigned int i = 0;
	// do this lookup only once, assuming that mapping does not change at run time...
	this->mMappingLookup.resize(number_of_mappings);
	for (i = 0; i < number_of_mappings; ++i)
	{
		this->mMappingLookup.at(i) = mapping->lookUp(i);
	}
	CEDAR_DEBUG_ASSERT(this->mMappingLookup.size() == 2);
	T value;
	int x;
	int y;
	int target_index;
	if (std::find(this->mMappingLookup.begin(), this->mMappingLookup.end(), 0) == this->mMappingLookup.end()) // expand over dim 0
	{
		if (this->mMappingLookup.at(0) == 1) // not flipped
		{
			for (x = 0; x < input.size[0]; ++x)
			{
				for (y = 0; y < input.size[1]; ++y)
				{
					value = input.at<T>(x, y);
					for (target_index = 0; target_index < output.size[0]; ++ target_index)
					{
						output.at<T>(target_index, x, y) = value;
					}
				}
			}
		}
		else // flipped
		{
			for (x = 0; x < input.size[0]; ++x)
			{
				for (y = 0; y < input.size[1]; ++y)
				{
					value = input.at<T>(x, y);
					for (target_index = 0; target_index < output.size[0]; ++ target_index)
					{
						output.at<T>(target_index, y, x) = value;
					}
				}
			}
		}
	}
	else if (std::find(this->mMappingLookup.begin(), this->mMappingLookup.end(), 1) == this->mMappingLookup.end()) // expand over dim 1
	{
		if (this->mMappingLookup.at(0) == 0) // not flipped
		{
			for (x = 0; x < input.size[0]; ++x)
			{
				for (y = 0; y < input.size[1]; ++y)
				{
					value = input.at<T>(x, y);
					for (target_index = 0; target_index < output.size[1]; ++ target_index)
					{
						output.at<T>(x, target_index, y) = value;
					}
				}
			}
		}
		else // flipped
		{
			for (x = 0; x < input.size[0]; ++x)
			{
				for (y = 0; y < input.size[1]; ++y)
				{
					value = input.at<T>(x, y);
					for (target_index = 0; target_index < output.size[0]; ++ target_index)
					{
						output.at<T>(y, target_index, x) = value;
					}
				}
			}
		}
	}
	else if (std::find(this->mMappingLookup.begin(), this->mMappingLookup.end(), 2) == this->mMappingLookup.end()) // expand over dim 2
	{
		if (this->mMappingLookup.at(0) == 0) // not flipped
		{
			for (x = 0; x < input.size[0]; ++x)
			{
				for (y = 0; y < input.size[1]; ++y)
				{
					value = input.at<T>(x, y);
					for (target_index = 0; target_index < output.size[2]; ++target_index)
					{
						output.at<T>(x, y, target_index) = value;
					}
				}
			}
		}
		else // flipped
		{
			for (x = 0; x < input.size[0]; ++x)
			{
				for (y = 0; y < input.size[1]; ++y)
				{
					value = input.at<T>(x, y);
					for (target_index = 0; target_index < output.size[2]; ++ target_index)
					{
						output.at<T>(y, x, target_index) = value;
					}
				}
			}
		}
	}
	else
	{
		CEDAR_ASSERT(false);
	}
}


void cedar::proc::ProjectionFunctions::expand1Dto3D(cedar::aux::ConstMatDataPtr input,
																									 cedar::aux::MatDataPtr output,
																									 std::vector<unsigned int> indicesToCompress,
																									 cedar::aux::EnumParameterPtr compressionType,
																									 cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	switch (input->getCvType())
	{
		case CV_32F:
		{
			this->expand1Dto3D<float>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		case CV_64F:
		{
			this->expand1Dto3D<double>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		default:
		CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
	}
}

template<typename T>
void cedar::proc::ProjectionFunctions::expand1Dto3D(cedar::aux::ConstMatDataPtr _input,
																									 cedar::aux::MatDataPtr _output,
																									 std::vector<unsigned int> indicesToCompress,
																									 cedar::aux::EnumParameterPtr compressionType,
																									 cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	const cv::Mat& input = _input->getData();
	cv::Mat& output = _output->getData();
	// sanity check
	CEDAR_DEBUG_ASSERT
	(
					cedar::aux::math::getDimensionalityOf(input) == 1
					&& cedar::aux::math::getDimensionalityOf(output) == 3
	);
	auto mapping = dimensionMappings->getValue();
	unsigned int number_of_mappings = mapping->getNumberOfMappings();
	unsigned int i = 0;
	// do this lookup only once, assuming that mapping does not change at run time...
	this->mMappingLookup.resize(number_of_mappings);
	for (i = 0; i < number_of_mappings; ++i)
	{
		this->mMappingLookup.at(i) = mapping->lookUp(i);
	}
	CEDAR_DEBUG_ASSERT(this->mMappingLookup.size() == 1);
	switch (this->mMappingLookup.at(0))
	{
		// do not allocate these for every for loop
		int x;
		int y;
		case 0:
		{
			T value;
			// outer loop
			for (int source_index = 0; source_index < input.rows; ++ source_index)
			{
				// get value
				value = input.at<T>(source_index, 0);
				for (x = 0; x < output.size[1]; ++x)
				{
					for (y = 0; y < output.size[2]; ++y)
					{
						output.at<T>(source_index, x, y) = value;
					}
				}
			}
			break;
		}
		case 1:
		{
			T value;
			// outer loop
			for (int source_index = 0; source_index < input.rows; ++ source_index)
			{
				// get value
				value = input.at<T>(source_index, 0);
				for (x = 0; x < output.size[0]; ++x)
				{
					for (y = 0; y < output.size[2]; ++y)
					{
						output.at<T>(x, source_index, y) = value;
					}
				}
			}
			break;
		}
		case 2:
		{
			T value;
			// outer loop
			for (int source_index = 0; source_index < input.rows; ++ source_index)
			{
				// get value
				value = input.at<T>(source_index, 0);
				for (x = 0; x < output.size[0]; ++x)
				{
					for (y = 0; y < output.size[1]; ++y)
					{
						output.at<T>(x, y, source_index) = value;
					}
				}
			}
			break;
		}
		default:
		{
			CEDAR_ASSERT(false);
		}
	}
}


void cedar::proc::ProjectionFunctions::expandMDtoND(cedar::aux::ConstMatDataPtr input,
																									 cedar::aux::MatDataPtr output,
																									 std::vector<unsigned int> indicesToCompress,
																									 cedar::aux::EnumParameterPtr compressionType,
																									 cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	switch (input->getCvType())
	{
		case CV_32F:
		{
			this->expandMDtoND<float>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		case CV_64F:
		{
			this->expandMDtoND<double>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		default:
		CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
	}
}

template<typename T>
void cedar::proc::ProjectionFunctions::expandMDtoND(cedar::aux::ConstMatDataPtr input,
																									 cedar::aux::MatDataPtr output,
																									 std::vector<unsigned int> indicesToCompress,
																									 cedar::aux::EnumParameterPtr compressionType,
																									 cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	unsigned int input_dimensionality = cedar::aux::math::getDimensionalityOf(input->getData());

	// create an iterator for the output matrix
	cedar::aux::MatrixIterator output_iterator(output->getData());

	// iterate through all elements of the output matrix
	std::vector<int> input_index;
	input_index.resize(input_dimensionality);
	// if the input dimensionality is 1 ...
	if (input_dimensionality == 1)
	{
		// ... we still need to have an index tuple because of OpenCV limitations
		input_index.push_back(0);
	}

	auto mapping = dimensionMappings->getValue();
	unsigned int number_of_mappings = mapping->getNumberOfMappings();
	unsigned int i = 0;
	// do this lookup only once, assuming that mapping does not change at run time...
	this->mMappingLookup.resize(number_of_mappings);
	for (i = 0; i < number_of_mappings; ++i)
	{
		this->mMappingLookup.at(i) = mapping->lookUp(i);
	}
	do
	{
		// get index pointing to the current element in the output matrix
		const std::vector<int>& output_index = output_iterator.getCurrentIndexVector();

		// compute the corresponding index in the input matrix
		for (i = 0; i < number_of_mappings; ++i)
		{
			input_index[i] = output_index.at(this->mMappingLookup.at(i));
		}

		// copy the activation value in the input matrix to the corresponding output matrix
		output->getData().at<T>(&(output_index.front()))
						= input->getData().at<T>(&(input_index.front()));
	}
	while (output_iterator.increment());
}


void cedar::proc::ProjectionFunctions::compress2Dto1D(cedar::aux::ConstMatDataPtr input,
																										 cedar::aux::MatDataPtr output,
																										 std::vector<unsigned int> indicesToCompress,
																										 cedar::aux::EnumParameterPtr compressionType,
																										 cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	CEDAR_DEBUG_ASSERT(indicesToCompress.size() == 1);

	cv::reduce(input->getData(), output->getData(), indicesToCompress.at(0), compressionType->getValue());

	if (output->getData().rows == 1)
	{
		output->getData() = output->getData().t();
	}
}





void cedar::proc::ProjectionFunctions::compress3Dto2D(cedar::aux::ConstMatDataPtr input,
																										 cedar::aux::MatDataPtr output,
																										 std::vector<unsigned int> indicesToCompress,
																										 cedar::aux::EnumParameterPtr compressionType,
																										 cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	switch (input->getCvType())
	{
		case CV_32F:
		{
			this->compress3Dto2D<float>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		case CV_64F:
		{
			this->compress3Dto2D<double>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		default:
		CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
	}
}

template<typename T>
void cedar::proc::ProjectionFunctions::compress3Dto2D(cedar::aux::ConstMatDataPtr input,
																										 cedar::aux::MatDataPtr output,
																										 std::vector<unsigned int> indicesToCompress,
																										 cedar::aux::EnumParameterPtr compressionType,
																										 cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	CEDAR_DEBUG_ASSERT(indicesToCompress.size() == 1);

	cedar::aux::math::reduceCvMat3D<T>
					(
									input->getData(),
									output->getData(),
									indicesToCompress.at(0),
									compressionType->getValue(),
									false // don't flip
					);
}

void cedar::proc::ProjectionFunctions::compress3Dto2DSwapped(cedar::aux::ConstMatDataPtr input,
																														cedar::aux::MatDataPtr output,
																														std::vector<unsigned int> indicesToCompress,
																														cedar::aux::EnumParameterPtr compressionType,
																														cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	switch (input->getCvType())
	{
		case CV_32F:
		{
			this->compress3Dto2DSwapped<float>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		case CV_64F:
		{
			this->compress3Dto2DSwapped<double>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		default:
		CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
	}
}

template<typename T>
void cedar::proc::ProjectionFunctions::compress3Dto2DSwapped(cedar::aux::ConstMatDataPtr input,
																														cedar::aux::MatDataPtr output,
																														std::vector<unsigned int> indicesToCompress,
																														cedar::aux::EnumParameterPtr compressionType,
																														cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	CEDAR_DEBUG_ASSERT(indicesToCompress.size() == 1);

	cedar::aux::math::reduceCvMat3D<T>
					(
									input->getData(),
									output->getData(),
									indicesToCompress.at(0),
									compressionType->getValue(),
									true // don't flip
					);
}


void cedar::proc::ProjectionFunctions::compress3Dto1D(cedar::aux::ConstMatDataPtr input,
																										 cedar::aux::MatDataPtr output,
																										 std::vector<unsigned int> indicesToCompress,
																										 cedar::aux::EnumParameterPtr compressionType,
																										 cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	switch (input->getCvType())
	{
		case CV_32F:
		{
			this->compress3Dto1D<float>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		case CV_64F:
		{
			this->compress3Dto1D<double>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		default:
		CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
	}
}

template<typename T>
void cedar::proc::ProjectionFunctions::compress3Dto1D(cedar::aux::ConstMatDataPtr input,
																										 cedar::aux::MatDataPtr output,
																										 std::vector<unsigned int> indicesToCompress,
																										 cedar::aux::EnumParameterPtr compressionType,
																										 cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	CEDAR_DEBUG_ASSERT(indicesToCompress.size() == 2);

	unsigned int input_dimensionality = cedar::aux::math::getDimensionalityOf(input->getData());

	// the compression from 3D to 1D first compresses the input
	// cube to a temporary 2D matrix and then compresses that to 1D

	// in order to do that, we first have to find a vector containing
	// the sizes of the temporary 2D matrix ...
	std::vector<int> sizes;
	for (unsigned int i = 0; i < input_dimensionality; ++i)
	{
		if (i != indicesToCompress.at(0))
		{
			sizes.push_back(input->getData().size[i]);
		}
	}

	// ... and create the temporary 2D matrix with those sizes
	cv::Mat tmp_2d(input_dimensionality - 1, &sizes.front(), input->getCvType(), cv::Scalar(0.0));

	// reduce the 3D input to the (temporary) 2D matrix
	cedar::aux::math::reduceCvMat3D<T>(input->getData(), tmp_2d, indicesToCompress.at(0), compressionType->getValue());

	// reduce the temporary 2D matrix to the final 1D output of the projection
	cv::reduce(tmp_2d, output->getData(), indicesToCompress.at(1) - 1, compressionType->getValue());

	if (output->getData().rows == 1)
	{
		output->getData() = output->getData().t();
	}
}



void cedar::proc::ProjectionFunctions::compressNDto0Dsum(cedar::aux::ConstMatDataPtr input,
																												cedar::aux::MatDataPtr output,
																												std::vector<unsigned int> indicesToCompress,
																												cedar::aux::EnumParameterPtr compressionType,
																												cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	switch (input->getCvType())
	{
		case CV_32F:
		{
			this->compressNDto0Dsum<float>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		case CV_64F:
		{
			this->compressNDto0Dsum<double>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		default:
		CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
	}
}

template<typename T>
void cedar::proc::ProjectionFunctions::compressNDto0Dsum(cedar::aux::ConstMatDataPtr input,
																												cedar::aux::MatDataPtr output,
																												std::vector<unsigned int> indicesToCompress,
																												cedar::aux::EnumParameterPtr compressionType,
																												cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	output->getData().at<T>(0) = cv::sum(input->getData())[0];
}

void cedar::proc::ProjectionFunctions::compressNDto0Dmean(cedar::aux::ConstMatDataPtr input,
																												 cedar::aux::MatDataPtr output,
																												 std::vector<unsigned int> indicesToCompress,
																												 cedar::aux::EnumParameterPtr compressionType,
																												 cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	switch (input->getCvType())
	{
		case CV_32F:
		{
			this->compressNDto0Dmean<float>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		case CV_64F:
		{
			this->compressNDto0Dmean<double>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		default:
		CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
	}
}
template<typename T>
void cedar::proc::ProjectionFunctions::compressNDto0Dmean(cedar::aux::ConstMatDataPtr input,
																												 cedar::aux::MatDataPtr output,
																												 std::vector<unsigned int> indicesToCompress,
																												 cedar::aux::EnumParameterPtr compressionType,
																												 cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	output->getData().at<T>(0) = cv::mean(input->getData())[0];
}

void cedar::proc::ProjectionFunctions::compressNDto0Dmin(cedar::aux::ConstMatDataPtr input,
																												cedar::aux::MatDataPtr output,
																												std::vector<unsigned int> indicesToCompress,
																												cedar::aux::EnumParameterPtr compressionType,
																												cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	switch (input->getCvType())
	{
		case CV_32F:
		{
			this->compressNDto0Dmin<float>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		case CV_64F:
		{
			this->compressNDto0Dmin<double>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		default:
		CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
	}
}
template<typename T>
void cedar::proc::ProjectionFunctions::compressNDto0Dmin(cedar::aux::ConstMatDataPtr input,
																												cedar::aux::MatDataPtr output,
																												std::vector<unsigned int> indicesToCompress,
																												cedar::aux::EnumParameterPtr compressionType,
																												cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	double minimum = std::numeric_limits<double>::max();
	unsigned int input_dimensionality = cedar::aux::math::getDimensionalityOf(input->getData());

	// for a dimensionality below 3 ...
	if (input_dimensionality < 3)
	{
		// ... we can use an OpenCV function to determine the minimum
		double maximum;
		cv::minMaxLoc(input->getData(), &minimum, &maximum);
	}
		// for all other dimensionalities, we have to iterate through the
		// input matrix and find the minimum ourselves
	else
	{
		// create an iterator for the input matrix
		cedar::aux::MatrixIterator matrix_iterator(input->getData());

		// iterate over input matrix and find the maximum value
		do
		{
			double current_value = input->getData().at<T>(matrix_iterator.getCurrentIndex());

			if (current_value < minimum)
			{
				minimum = current_value;
			}
		}
		while (matrix_iterator.increment());
	}

	// set the minimum of the input matrix as the output of the projection
	output->getData().at<T>(0) = minimum;
}

void cedar::proc::ProjectionFunctions::compressNDto0Dmax(cedar::aux::ConstMatDataPtr input,
																												cedar::aux::MatDataPtr output,
																												std::vector<unsigned int> indicesToCompress,
																												cedar::aux::EnumParameterPtr compressionType,
																												cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	switch (input->getCvType())
	{
		case CV_32F:
		{
			this->compressNDto0Dmax<float>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		case CV_64F:
		{
			this->compressNDto0Dmax<double>(input, output, indicesToCompress, compressionType, dimensionMappings);
			break;
		}
		default:
		CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
	}
}

template<typename T>
void cedar::proc::ProjectionFunctions::compressNDto0Dmax(cedar::aux::ConstMatDataPtr input,
																												cedar::aux::MatDataPtr output,
																												std::vector<unsigned int> indicesToCompress,
																												cedar::aux::EnumParameterPtr compressionType,
																												cedar::proc::ProjectionMappingParameterPtr dimensionMappings)
{
	double maximum = std::numeric_limits<double>::min();
	unsigned int input_dimensionality = cedar::aux::math::getDimensionalityOf(input->getData());

	// for a dimensionality below 3 ...
	if (input_dimensionality < 3)
	{
		// ... we can use an OpenCV function to determine the maximum
		double minimum;
		cv::minMaxLoc(input->getData(), &minimum, &maximum);
	}
		// for all other dimensionalities, we have to iterate through the
		// input matrix and find the maximum ourselves
	else
	{
		// create an iterator for the input matrix
		cedar::aux::MatrixIterator matrix_iterator(input->getData());

		// iterate over input matrix and find the maximum value
		do
		{
			double current_value = input->getData().at<T>(matrix_iterator.getCurrentIndex());

			if (current_value > maximum)
			{
				maximum = current_value;
			}
		}
		while (matrix_iterator.increment());
	}

	// set the maximum of the input matrix as the output of the projection
	output->getData().at<T>(0) = maximum;
}
