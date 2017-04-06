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

    File:        DeclarationRegistry.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 11 18

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_DECLARATION_REGISTRY_H
#define CEDAR_PROC_DECLARATION_REGISTRY_H

// CEDAR INCLUDES
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/assert.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/DeclarationManagerTemplate.h"
#include "cedar/processing/Element.fwd.h"

// SYSTEM INCLUDES

namespace cedar
{
  namespace proc
  {
    //! A manager for element declarations
    typedef
      cedar::aux::DeclarationManagerTemplate<cedar::proc::ElementPtr>
      ElementDeclarationManager;

    //! Sinleton of the element manager.
    typedef
      cedar::aux::Singleton<cedar::proc::ElementDeclarationManager>
      ElementManagerSingleton;

    //! A manager for element factories
    typedef
      cedar::aux::FactoryManager<cedar::proc::ElementPtr>
      ElementFactoryManager;
  }
}

CEDAR_PROC_SINGLETON(ElementDeclarationManager);
CEDAR_PROC_SINGLETON(ElementFactoryManager);

#endif // CEDAR_PROC_DECLARATION_REGISTRY_H
