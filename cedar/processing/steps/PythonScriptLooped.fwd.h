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

    File:        PythonScriptLooped.fwd.h

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2019 04 01

    Description: Forward declaration file for the class PythonScriptLooped.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_PYTHON_SCRIPT_LOOPED_FWD_H
#define CEDAR_PROC_STEPS_PYTHON_SCRIPT_LOOPED_FWD_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"
#ifdef CEDAR_USE_PYTHONSTEP
// CEDAR INCLUDES
#include "cedar/processing/lib.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/smart_ptr.hpp>
#endif // Q_MOC_RUN


//!@cond SKIPPED_DOCUMENTATION
namespace cedar
{
  namespace proc
  {
    namespace steps
    {
      CEDAR_DECLARE_PROC_CLASS(PythonScriptLooped);
    }
  }
}
//!@endcond

#endif // CEDAR_USE_PYTHONSTEP
#endif // CEDAR_PROC_STEPS_PYTHON_SCRIPT_LOOPED_FWD_H

