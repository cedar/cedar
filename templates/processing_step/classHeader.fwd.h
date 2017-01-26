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

    File:        <filename>

    Maintainer:  <first name> <last name>
    Email:       <email address>
    Date:        <creation date YYYY MM DD>

    Description: Forward declaration file for the class <full class name>.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_XXX_XXX_FWD_H
#define CEDAR_XXX_XXX_FWD_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
<if: in_cedar>
#include "<base namespace path>/lib.h"
<endif>

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/smart_ptr.hpp>
#endif // Q_MOC_RUN


<begin namespaces>
<namespaces indent>//!@cond SKIPPED_DOCUMENTATION
<if: in_cedar>
<namespaces indent>CEDAR_DECLARE_<CAP_SHORT_MAIN_NAMESPACE>_CLASS(<class name>);
<else>
<namespaces indent>class <class name>;
<endif>
<namespaces indent>//!@endcond
<end namespaces>

#endif // CEDAR_XXX_XXX_FWD_H

