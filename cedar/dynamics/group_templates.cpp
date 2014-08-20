/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        group_templates.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 08 19

    Description: Source file for the class cedar::dyn::group_templates.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/GroupDeclaration.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    cedar::proc::GroupDeclarationPtr group_declaration
                                   (
                                     new cedar::proc::GroupDeclaration
                                     (
                                       "two-layer field",
                                       "resource://groupTemplates/fieldTemplates.json",
                                       "two-layer",
                                       "DFT"
                                     )
                                   );
    group_declaration->setIconPath(":/cedar/dynamics/gui/steps/field_generic.svg");
    group_declaration->declare();

    cedar::proc::GroupDeclarationPtr field_declaration
                                   (
                                     new cedar::proc::GroupDeclaration
                                     (
                                       "one-dimensional field",
                                       "resource://groupTemplates/fieldTemplates.json",
                                       "one-dimensional field",
                                       "DFT"
                                     )
                                   );
    field_declaration->setIconPath(":/cedar/dynamics/gui/steps/field_generic.svg");
    field_declaration->declare();

    return true;
  }

  bool declared = declare();
}
