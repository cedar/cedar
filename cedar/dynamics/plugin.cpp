/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        plugin.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 08 15

    Description: This is the plugin file describing the cedar::dyn plugin.

    Credits:

======================================================================================================================*/

#include "cedar/dynamics/plugin.h"
#include "cedar/dynamics/namespace.h"
#include "cedar/dynamics/fields/NeuralField.h"
#include "cedar/dynamics/fields/Preshape.h"
#include "cedar/dynamics/sources/Noise.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/namespace.h"
#include "cedar/auxiliaries/Singleton.h"


void pluginDeclaration(cedar::proc::PluginDeclarationPtr plugin)
{
  using cedar::proc::ElementDeclarationPtr;

  ElementDeclarationPtr field_decl
  (
    new cedar::proc::ElementDeclarationTemplate<cedar::dyn::NeuralField>("Fields", "cedar.dynamics.NeuralField")
  );
  field_decl->setIconPath(":/steps/field_temp.svg");

  // cedar::proc::PluginDeclarationPtr plugin(new cedar::proc::PluginDeclaration());
  plugin->add(field_decl);

  ElementDeclarationPtr preshape_decl
  (
    new cedar::proc::ElementDeclarationTemplate<cedar::dyn::Preshape>("Fields", "cedar.dynamics.Preshape")
  );
  preshape_decl->setIconPath(":/steps/preshape.svg");

  plugin->add(preshape_decl);
  // return plugin;

  ElementDeclarationPtr noise_decl
  (
    new cedar::proc::ElementDeclarationTemplate<cedar::dyn::Noise>("Sources", "cedar.dynamics.Noise")
  );
  noise_decl->setIconPath(":/steps/noise.svg");

  plugin->add(noise_decl);
  // return plugin;
}

void cedar::dyn::getPluginDesciption(cedar::proc::PluginDeclarationPtr plugin)
{
  ::pluginDeclaration(plugin);
}

