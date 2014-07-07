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

    File:        plugin.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 08 15

    Description: This is the plugin file describing the cedar::dyn plugin.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DYN_PLUGIN_H
#define CEDAR_DYN_PLUGIN_H

// CEDAR INCLUDES
#include "cedar/dynamics/lib.h"
#include "cedar/auxiliaries/PluginDeclarationList.h"

CEDAR_BEGIN_PLUGIN_DECLARATION
//!@brief Function that declares plugin exports.
CEDAR_DECLARE_DEPRECATED(CEDAR_DYN_LIB_EXPORT void pluginDeclaration(cedar::aux::PluginDeclarationListPtr plugin));
CEDAR_END_PLUGIN_DECLARATION

#endif // CEDAR_DYN_PLUGIN_H
