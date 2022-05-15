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

    File:        ProjectionFunctions.h

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.rub.de
    Date:        2022 05 15

    Description: Header file for the class cedar::proc::ProjectionFunctions.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_PROJECTION_FUNCTIONS_H
#define CEDAR_AUX_PROJECTION_FUNCTIONS_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/processing/ProjectionMappingParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/ProjectionFunctions.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::ProjectionFunctions
{
	//--------------------------------------------------------------------------------------------------------------------
	// nested types
	//--------------------------------------------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------------------------------------------
	// constructors and destructor
	//--------------------------------------------------------------------------------------------------------------------
public:
	//!@brief The standard constructor.
	ProjectionFunctions();

	//!@brief Destructor
	virtual ~ProjectionFunctions();

	//--------------------------------------------------------------------------------------------------------------------
	// public methods
	//--------------------------------------------------------------------------------------------------------------------
public:
	//!@brief expands a 0D input to an ND output
	template<typename T>
	void expand0DtoND(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
										std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
										cedar::proc::ProjectionMappingParameterPtr dimensionMappings);

	void expand0DtoND(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
										std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
										cedar::proc::ProjectionMappingParameterPtr dimensionMappings);


	void expand1Dto2D(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
										std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
										cedar::proc::ProjectionMappingParameterPtr dimensionMappings);
	void expand2Dto3D(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
										std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
										cedar::proc::ProjectionMappingParameterPtr dimensionMappings);
	template<typename T>
	void expand2Dto3D(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
										std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
										cedar::proc::ProjectionMappingParameterPtr dimensionMappings);
	void expand1Dto3D(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
										std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
										cedar::proc::ProjectionMappingParameterPtr dimensionMappings);
	template<typename T>
	void expand1Dto3D(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
										std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
										cedar::proc::ProjectionMappingParameterPtr dimensionMappings);
	void expandMDtoND(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
										std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
										cedar::proc::ProjectionMappingParameterPtr dimensionMappings);
	template<typename T>
	void expandMDtoND(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
										std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
										cedar::proc::ProjectionMappingParameterPtr dimensionMappings);


	//!@brief compresses 2D input to 1D output
	void compress2Dto1D(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
											std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
											cedar::proc::ProjectionMappingParameterPtr dimensionMappings);

	template<typename T>
	void compress3Dto2D(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
											std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
											cedar::proc::ProjectionMappingParameterPtr dimensionMappings);

	void compress3Dto2D(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
											std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
											cedar::proc::ProjectionMappingParameterPtr dimensionMappings);

	template<typename T>
	void compress3Dto2DSwapped(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
														 std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
														 cedar::proc::ProjectionMappingParameterPtr dimensionMappings);

	void compress3Dto2DSwapped(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
														 std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
														 cedar::proc::ProjectionMappingParameterPtr dimensionMappings);

	template<typename T>
	void compress3Dto1D(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
											std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
											cedar::proc::ProjectionMappingParameterPtr dimensionMappings);

	void compress3Dto1D(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
											std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
											cedar::proc::ProjectionMappingParameterPtr dimensionMappings);

//!@brief compresses ND input to 0D output by computing the minimum over all positions
	void compressNDto0Dmin(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
												 std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
												 cedar::proc::ProjectionMappingParameterPtr dimensionMappings);
	//!@brief compresses ND input to 0D output by computing the maximum over all positions
	void compressNDto0Dmax(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
												 std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
												 cedar::proc::ProjectionMappingParameterPtr dimensionMappings);
	//!@brief compresses ND input to 0D output by computing the sum over all positions
	void compressNDto0Dsum(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
												 std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
												 cedar::proc::ProjectionMappingParameterPtr dimensionMappings);
	//!@brief compresses ND input to 0D output by computing the mean over all positions
	void compressNDto0Dmean(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
													std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
													cedar::proc::ProjectionMappingParameterPtr dimensionMappings);
	//!@brief compresses ND input to 0D output by computing the minimum over all positions (templated)
	template<typename T>
	void compressNDto0Dmin(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
												 std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
												 cedar::proc::ProjectionMappingParameterPtr dimensionMappings);
	//!@brief compresses ND input to 0D output by computing the maximum over all positions (templated)
	template<typename T>
	void compressNDto0Dmax(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
												 std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
												 cedar::proc::ProjectionMappingParameterPtr dimensionMappings);
	//!@brief compresses ND input to 0D output by computing the sum over all positions (templated)
	template<typename T>
	void compressNDto0Dsum(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
												 std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
												 cedar::proc::ProjectionMappingParameterPtr dimensionMappings);
	//!@brief compresses ND input to 0D output by computing the mean over all positions (templated)
	template<typename T>
	void compressNDto0Dmean(cedar::aux::MatDataPtr input, cedar::aux::MatDataPtr output,
													std::vector<unsigned int> indicesToCompress, cedar::aux::EnumParameterPtr compressionType,
													cedar::proc::ProjectionMappingParameterPtr dimensionMappings);

	//--------------------------------------------------------------------------------------------------------------------
	// protected methods
	//--------------------------------------------------------------------------------------------------------------------
protected:
	// none yet

	//--------------------------------------------------------------------------------------------------------------------
	// private methods
	//--------------------------------------------------------------------------------------------------------------------
private:
	//!@brief a lookup table for mappings
	std::vector<unsigned int> mMappingLookup;

	//--------------------------------------------------------------------------------------------------------------------
	// members
	//--------------------------------------------------------------------------------------------------------------------
protected:
	// none yet
private:
	// none yet

	//--------------------------------------------------------------------------------------------------------------------
	// parameters
	//--------------------------------------------------------------------------------------------------------------------
protected:
	// none yet

private:
	// none yet

}; // class cedar::proc::ProjectionFunctions

#endif // CEDAR_AUX_PROJECTION_FUNCTIONS_H

