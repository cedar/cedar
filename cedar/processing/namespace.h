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

// CEDAR INCLUDES
#include "cedar/processing/lib.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/Factory.h"

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>

namespace cedar
{
  /*!
   *  @brief Namespace containing main elements of cedar's processing framework.
   *
   *  @see   @ref ProcessingFrameworkConcept for an introduction into the concepts using within the processing framework.
   */
  namespace proc
  {
    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_DECLARE_PROC_CLASS(Arguments);
    CEDAR_DECLARE_PROC_CLASS(Connectable);
    CEDAR_DECLARE_PROC_CLASS(DataConnection);
    CEDAR_DECLARE_PROC_CLASS(DataRole);
    CEDAR_DECLARE_PROC_CLASS(DataSlot);
    CEDAR_DECLARE_PROC_CLASS(DeclarationRegistry);
    CEDAR_DECLARE_PROC_CLASS(Element);
    CEDAR_DECLARE_PROC_CLASS(ElementDeclaration);
    CEDAR_DECLARE_PROC_CLASS(ExternalData);
    CEDAR_DECLARE_PROC_CLASS(LoopArguments);
    CEDAR_DECLARE_PROC_CLASS(FrameworkSettings);
    CEDAR_DECLARE_PROC_CLASS(LoopedTrigger);
    CEDAR_DECLARE_PROC_CLASS(MultiTrigger);
    CEDAR_DECLARE_PROC_CLASS(Step);
    CEDAR_DECLARE_PROC_CLASS(StepTime);
    CEDAR_DECLARE_PROC_CLASS(Trigger);
    CEDAR_DECLARE_PROC_CLASS(Manager);
    CEDAR_DECLARE_PROC_CLASS(Network);
    CEDAR_DECLARE_PROC_CLASS(OwnedData);
    CEDAR_DECLARE_PROC_CLASS(PluginProxy);
    CEDAR_DECLARE_PROC_CLASS(PluginDeclaration);
    CEDAR_DECLARE_PROC_CLASS(PromotedExternalData);
    CEDAR_DECLARE_PROC_CLASS(PromotedOwnedData);
    CEDAR_DECLARE_PROC_CLASS(Triggerable);
    CEDAR_DECLARE_PROC_CLASS(TriggerConnection);
    CEDAR_DECLARE_PROC_CLASS(ProjectionMapping);
    CEDAR_DECLARE_PROC_CLASS_INTRUSIVE(ProjectionMappingParameter);
    //!@endcond

    //!@cond SKIPPED_DOCUMENTATION
    template <class BaseClass, class FactoryType> class DeclarationBase;
    template <class DerivedClass> class ElementDeclarationTemplate;
    template <class DerivedClass> class StepDeclarationT;
    template <class DerivedClass> class TriggerDeclarationT;
    //!@endcond

    //!@brief a factory for elements
    typedef boost::shared_ptr<cedar::aux::Factory<ElementPtr> > ElementFactoryPtr;

    /* Exceptions */
    //!@cond SKIPPED_DOCUMENTATION
    class CEDAR_PROC_LIB_EXPORT ArchitectureLoadingException;
    class CEDAR_PROC_LIB_EXPORT ConnectionMemberDeletedException;
    class CEDAR_PROC_LIB_EXPORT ConnectionNotFoundException;
    class CEDAR_PROC_LIB_EXPORT DeadlockException;
    class CEDAR_PROC_LIB_EXPORT DuplicateConnectionException;
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
    class CEDAR_PROC_LIB_EXPORT NoMappingException;
    //!@endcond
  }
}

#endif // CEDAR_PROC_NAMESPACE_H
