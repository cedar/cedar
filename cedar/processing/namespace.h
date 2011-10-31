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
  /*!
   *  @brief Namespace containing main elements of cedar's processing framework.
   *
   *  @see   @ref ProcessingIntroduction for an introduction and code tutorials.
   */
  namespace proc
  {
    //!@cond SKIPPED_DOCUMENTATION
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
    CEDAR_DECLARE_PROC_CLASS(PluginProxy);
    CEDAR_DECLARE_PROC_CLASS(PluginDeclaration);
    CEDAR_DECLARE_PROC_CLASS(StepDeclaration);
    CEDAR_DECLARE_PROC_CLASS(TriggerDeclaration);
    CEDAR_DECLARE_PROC_CLASS(Connection);
    //!@endcond

    template <class BaseClass, class FactoryType> class DeclarationBase;
    
    template <class DerivedClass> class StepDeclarationT;

    template <class DerivedClass> class TriggerDeclarationT;

    typedef boost::shared_ptr<cedar::aux::AbstractFactory<Step> > StepFactoryPtr;
    typedef boost::shared_ptr<cedar::aux::Factory<Trigger, cedar::proc::TriggerPtr> > TriggerFactoryPtr;


    template <class T, class T_Declaration> class Registry;
    typedef cedar::proc::Registry<Step, StepDeclaration> StepRegistry;
    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_GENERATE_POINTER_TYPES(StepRegistry);
    //!@endcond
    typedef cedar::proc::Registry<Trigger, TriggerDeclaration> TriggerRegistry;
    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_GENERATE_POINTER_TYPES(TriggerRegistry);
    //!@endcond

    /* Exceptions */
    //!@cond SKIPPED_DOCUMENTATION
    class CEDAR_PROC_LIB_EXPORT ConnectionMemberDeletedException;
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
    //!@endcond
  }
}

#endif // CEDAR_PROC_NAMESPACE_H
