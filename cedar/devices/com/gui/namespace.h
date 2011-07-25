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

    File:        namespace.h

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description:  Namespace file for cedar::dev::com::gui.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_COM_GUI_NAMESPACE_H
#define CEDAR_DEV_COM_GUI_NAMESPACE_H

// LOCAL INCLUDES
#include "devices/lib.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

#include <boost/smart_ptr.hpp>

namespace cedar
{
  namespace dev
  {
    namespace com
    {
      namespace gui
      {
      class CEDAR_DEV_LIB_EXPORT CommunicationWidget;
      //!@brief smart pointer for CommunicationWidget
      typedef boost::shared_ptr<CommunicationWidget> CommunicationWidgetPtr;
      }
    }
  }
}

#endif // CEDAR_DEV_COM_GUI_NAMESPACE_H
