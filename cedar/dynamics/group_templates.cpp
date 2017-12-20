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
                                       "DFT Templates"
                                     )
                                   );
    group_declaration->setIconPath(":/cedar/dynamics/gui/steps/field_generic.svg");
    group_declaration->declare();

    cedar::proc::GroupDeclarationPtr elementary_behavior_declaration
                                   (
                                     new cedar::proc::GroupDeclaration
                                     (
                                       "elementary behavior",
                                       "resource://groupTemplates/elementaryBehavior.json",
                                       "Elementary behavior",
                                       "DFT Templates"
                                     )
                                   );
    elementary_behavior_declaration->setIconPath(":/cedar/dynamics/gui/steps/elementary_behavior.svg");
    elementary_behavior_declaration->declare();

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
    field_declaration->setIconPath(":/cedar/dynamics/gui/steps/field_1d.svg");
    field_declaration->declare();

    cedar::proc::GroupDeclarationPtr node_declaration
                                   (
                                     new cedar::proc::GroupDeclaration
                                     (
                                       "node",
                                       "resource://groupTemplates/fieldTemplates.json",
                                       "node",
                                       "DFT"
                                     )
                                   );
    node_declaration->setIconPath(":/cedar/dynamics/gui/steps/field_0d.svg");
    node_declaration->declare();

    cedar::proc::GroupDeclarationPtr sustained_field_declaration
            (
                    new cedar::proc::GroupDeclaration
                            (
                                    "self-sustained field (1D)",
                                    "resource://groupTemplates/fieldTemplates.json",
                                    "1D sustained field",
                                    "DFT Templates"
                            )
            );
    sustained_field_declaration->setIconPath(":/cedar/dynamics/gui/steps/field_1d_active.svg");
    sustained_field_declaration->declare();

    cedar::proc::GroupDeclarationPtr selective_field_declaration
            (
                    new cedar::proc::GroupDeclaration
                            (
                                    "selective field (1D)",
                                    "resource://groupTemplates/fieldTemplates.json",
                                    "1D selective field",
                                    "DFT Templates"
                            )
            );
    selective_field_declaration->setIconPath(":/cedar/dynamics/gui/steps/field_1d_selective.svg");
    selective_field_declaration->declare();

    return true;
  }

  bool declared = declare();
}
