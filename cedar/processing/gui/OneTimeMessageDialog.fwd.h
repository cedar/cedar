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

    File:        OneTimeMessageDialog.fwd.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 11 11

    Description: Forward declaration file for the class cedar::proc::gui::OneTimeMessageDialog.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_ONE_TIME_MESSAGE_DIALOG_FWD_H
#define CEDAR_PROC_GUI_ONE_TIME_MESSAGE_DIALOG_FWD_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/lib.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/smart_ptr.hpp>
#endif // Q_MOC_RUN


namespace cedar
{
  namespace proc
  {
    namespace gui
    {
      //!@cond SKIPPED_DOCUMENTATION
      CEDAR_DECLARE_PROC_CLASS(OneTimeMessageDialog);
      //!@endcond
    }
  }
}


#endif // CEDAR_PROC_GUI_ONE_TIME_MESSAGE_DIALOG_FWD_H

