/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        GroupSource.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2013 11 08

    Description: Source file for the class cedar::proc::sources::GroupSource.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/sources/GroupSource.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/Data.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr input_declaration
    (
      new ElementDeclarationTemplate<cedar::proc::sources::GroupSource>
      (
        "Sources",
        "cedar.processing.sources.GroupSource"
      )
    );
    input_declaration->setIconPath(":/steps/group_source.svg");
    input_declaration->setDescription("Do not use this.");

    input_declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::sources::GroupSource::GroupSource()
{
  this->declareOutput("output", cedar::aux::DataPtr(new cedar::aux::Data()));
}

cedar::proc::sources::GroupSource::~GroupSource()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::proc::sources::GroupSource::compute(const cedar::proc::Arguments& /*arguments*/)
{
  // do nothing
}

void cedar::proc::sources::GroupSource::setData(cedar::aux::DataPtr data)
{
  this->setOutput("output", data);
  this->emitOutputPropertiesChangedSignal("output");
}
