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

    File:        PlotData.cpp

    Maintainer:  Kai Kuchenbecker
    Email:       Kai.Kuchenbecker@ini.rub.de
    Date:        2013-08-02

    Description:

    Credits:

======================================================================================================================*/
#define NOMINMAX // prevents MSVC conflicts

// CEDAR INCLUDES
#include "cedar/configuration.h"
#include "PlotData.h"


// SYSTEM INCLUDES
// none yet

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::PlotData::PlotData
(
    cedar::proc::DataRole::Id id,
    const std::string& name,
    bool ignoreIfMissing,
    const std::string& plotDeclaration
)
:
mId(new cedar::aux::EnumParameter(this, "id", cedar::proc::DataRole::typePtr(), id)),
mName(new cedar::aux::StringParameter(this, "name", name)),
mIgnoreIfMissing(new cedar::aux::BoolParameter(this, "ignoreIfMissing", ignoreIfMissing)),
mPlotDeclaration(new cedar::aux::StringParameter(this, "plotDeclaration", plotDeclaration))
{
}

cedar::proc::PlotData::PlotData(const cedar::proc::PlotData& other)
:
mId(new cedar::aux::EnumParameter(this, "id", cedar::proc::DataRole::typePtr(), other.mId->getValue())),
mName(new cedar::aux::StringParameter(this, "name", other.mName->getValue())),
mIgnoreIfMissing(new cedar::aux::BoolParameter(this, "ignoreIfMissing", other.mIgnoreIfMissing->getValue())),
mPlotDeclaration(new cedar::aux::StringParameter(this, "plotDeclaration", other.mPlotDeclaration->getValue()))
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::PlotData::setPlotDeclaration(const std::string& newDeclaration)
{
  this->mPlotDeclaration->setValue(newDeclaration);
}
