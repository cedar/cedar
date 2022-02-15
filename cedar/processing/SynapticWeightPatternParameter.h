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

    File:        SynapticWeightPatternParameter.h

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de
    Date:        2022 02 13

    Description: Header file for the class cedar::proc::SynapticWeightPatternParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumType.h"

// FORWARD DECLARATIONS
#include "cedar/processing/SynapticWeightPatternParameter.fwd.h"

// SYSTEM INCLUDES


/*!@brief An enum class for the different SynapticWeightPattern that can be done by the SynapticConnection
 */
class cedar::proc::SynapticWeightPatternParameter
{
	//--------------------------------------------------------------------------------------------------------------------
	// nested types
	//--------------------------------------------------------------------------------------------------------------------
public:
	//! The type of the enum values.
	typedef cedar::aux::EnumId Id;

	//! The pointer type of the enum base object.
	typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

	//--------------------------------------------------------------------------------------------------------------------
	// constructors and destructor
	//--------------------------------------------------------------------------------------------------------------------
public:
	// none

	//--------------------------------------------------------------------------------------------------------------------
	// public methods
	//--------------------------------------------------------------------------------------------------------------------
public:
	/*!@brief Initializes the enum values.
 */
	static void construct();

	/*!@brief Returns a reference to the enum base object.
 */
	static const cedar::aux::EnumBase& type();

	/*!@brief Returns a pointer to the enum base object.
 */
	static const cedar::proc::SynapticWeightPatternParameter::TypePtr& typePtr();
	//--------------------------------------------------------------------------------------------------------------------
	// protected methods
	//--------------------------------------------------------------------------------------------------------------------
protected:
	// none yet

	//--------------------------------------------------------------------------------------------------------------------
	// private methods
	//--------------------------------------------------------------------------------------------------------------------
private:
	// none yet

	//--------------------------------------------------------------------------------------------------------------------
	// members
	//--------------------------------------------------------------------------------------------------------------------
public:
	//Values of the Enum
	static const Id StaticGain = 0;
	static const Id Convolution = 1;

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
	//! The enum object.
	static cedar::aux::EnumType<cedar::proc::SynapticWeightPatternParameter> mType;

}; // class cedar::proc::SynapticWeightPatternParameter

