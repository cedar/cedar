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

    File:        SynapticWeightPatternParameter.cpp

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de
    Date:        2022 02 13

    Description: Source file for the class cedar::proc::SynapticWeightPatternParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/SynapticWeightPatternParameter.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// Static members
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::proc::SynapticWeightPatternParameter> cedar::proc::SynapticWeightPatternParameter::mType("cedar::proc::SynapticWeightPatternParameter::");

// From Enum.dox:"MSVC doesn't need (read: want) you to declare these again because they have values assigned in the
// header and thus are const expressions; gcc is the opposite, thus the ifdef"
#ifndef CEDAR_COMPILER_MSVC
const cedar::proc::SynapticWeightPatternParameter::Id cedar::proc::SynapticWeightPatternParameter::StaticGain;
const cedar::proc::SynapticWeightPatternParameter::Id cedar::proc::SynapticWeightPatternParameter::Convolution;
#endif


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::SynapticWeightPatternParameter::construct()
{
	mType.type()->def(cedar::aux::Enum(cedar::proc::SynapticWeightPatternParameter::StaticGain,
					"StaticGain","Static Gain"));
	mType.type()->def(cedar::aux::Enum(cedar::proc::SynapticWeightPatternParameter::Convolution,
					"Convolution","Convolution"));
}

const cedar::aux::EnumBase& cedar::proc::SynapticWeightPatternParameter::type()
{
	return *cedar::proc::SynapticWeightPatternParameter::mType.type();
}

const cedar::proc::SynapticWeightPatternParameter::TypePtr& cedar::proc::SynapticWeightPatternParameter::typePtr()
{
	return cedar::proc::SynapticWeightPatternParameter::mType.type();
}