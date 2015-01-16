/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 07 14

    Description: Namespace file for cedar::aux::kernel.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_KERNEL_NAMESPACE_H
#define CEDAR_AUX_KERNEL_NAMESPACE_H

// CEDAR INCLUDES
#include "cedar/defines.h"

// FORWARD DECLARATIONS

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/smart_ptr.hpp>
#endif

#warning Do not include this header any more. Use the new forward declaration headers instead.

#include "cedar/auxiliaries/kernel/Box.fwd.h"
#include "cedar/auxiliaries/kernel/FactoryManager.fwd.h"
#include "cedar/auxiliaries/kernel/Gauss.fwd.h"
#include "cedar/auxiliaries/kernel/Kernel.fwd.h"
#include "cedar/auxiliaries/kernel/Separable.fwd.h"

#endif // CEDAR_AUX_KERNEL_NAMESPACE_H
