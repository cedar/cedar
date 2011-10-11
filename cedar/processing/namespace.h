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

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.rub.de
    Date:        2011 05 23

    Description: Namespace file for cedar::aux::comp.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_PROC_NAMESPACE_H
#define CEDAR_PROC_NAMESPACE_H

// LOCAL INCLUDES
#include "processing/lib.h"
#include "auxiliaries/AbstractFactory.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>


namespace cedar
{
  /*!@brief Namespace for all processing classes. */
  namespace proc
  {
    CEDAR_DECLARE_PROC_CLASS(Arguments);
    CEDAR_DECLARE_PROC_CLASS(DataRole);
    CEDAR_DECLARE_PROC_CLASS(DataSlot);
    CEDAR_DECLARE_PROC_CLASS(LoopArguments);
    CEDAR_DECLARE_PROC_CLASS(LoopMode);
    CEDAR_DECLARE_PROC_CLASS(FrameworkSettings);
    CEDAR_DECLARE_PROC_CLASS(LoopedTrigger);
    CEDAR_DECLARE_PROC_CLASS(MultiTrigger);
    CEDAR_DECLARE_PROC_CLASS(Step);
    CEDAR_DECLARE_PROC_CLASS(StepTime);
    CEDAR_DECLARE_PROC_CLASS(Trigger);
    CEDAR_DECLARE_PROC_CLASS(Group);
    
    CEDAR_DECLARE_PROC_CLASS(Manager);
    CEDAR_DECLARE_PROC_CLASS(Network);
    class Connection;
    typedef boost::shared_ptr<Connection> ConnectionPtr;

    template <class BaseClass, class FactoryType> class DeclarationBase;
    
    CEDAR_DECLARE_PROC_CLASS(StepDeclaration);
    template <class DerivedClass> class StepDeclarationT;

    CEDAR_DECLARE_PROC_CLASS(TriggerDeclaration);
    template <class DerivedClass> class TriggerDeclarationT;

    typedef boost::shared_ptr<cedar::aux::AbstractFactory<Step> > StepFactoryPtr;
    typedef boost::shared_ptr<cedar::aux::Factory<Trigger, cedar::proc::TriggerPtr> > TriggerFactoryPtr;

    CEDAR_DECLARE_PROC_CLASS(PluginProxy);
    CEDAR_DECLARE_PROC_CLASS(PluginDeclaration);

    template <class T, class T_Declaration> class Registry;
    typedef cedar::proc::Registry<Step, StepDeclaration> StepRegistry;
    CEDAR_GENERATE_POINTER_TYPES(StepRegistry);
    typedef cedar::proc::Registry<Trigger, TriggerDeclaration> TriggerRegistry;
    CEDAR_GENERATE_POINTER_TYPES(TriggerRegistry);

    /* Exceptions */
    class CEDAR_PROC_LIB_EXPORT ConnectionNotFoundException;
    class CEDAR_PROC_LIB_EXPORT DuplicateNameException;
    class CEDAR_PROC_LIB_EXPORT InvalidNameException;
    class CEDAR_PROC_LIB_EXPORT InvalidObjectException;
    class CEDAR_PROC_LIB_EXPORT InvalidRoleException;
    class CEDAR_PROC_LIB_EXPORT InvalidArgumentsException;
    class CEDAR_PROC_LIB_EXPORT InvalidCategoryException;
    class CEDAR_PROC_LIB_EXPORT MissingConnectionException;
    class CEDAR_PROC_LIB_EXPORT MissingDeclarationException;
    class CEDAR_PROC_LIB_EXPORT ParseException;
    class CEDAR_PROC_LIB_EXPORT PluginException;
  }
}

#endif // CEDAR_PROC_NAMESPACE_H
