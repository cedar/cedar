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
#include "auxiliaries/AbstractFactory.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>
#include <opencv2/opencv.hpp>


namespace cedar
{
  /*!@brief Namespace for all processing classes. */
  namespace proc
  {
    class Arguments;
    typedef boost::shared_ptr<Arguments> ArgumentsPtr;

    class Data;
    typedef boost::shared_ptr<Data> DataPtr;

    template <typename T> class DataT;

    typedef DataT<cv::Mat> MatData;

    class DataRole;

    class LoopArguments;

    class LoopedTrigger;
    typedef boost::shared_ptr<LoopedTrigger> LoopedTriggerPtr;

    class MultiTrigger;
    typedef boost::shared_ptr<MultiTrigger> MultiTriggerPtr;

    class Step;
    typedef boost::shared_ptr<Step> StepPtr;

    class StepTime;
    typedef boost::shared_ptr<StepTime> StepTimePtr;

    class Trigger;
    typedef boost::shared_ptr<Trigger> TriggerPtr;

    class Group;
    typedef boost::shared_ptr<Group> GroupPtr;

    class Manager;
    template <class T, class T_Declaration> class Registry;

    class Network;
    typedef boost::shared_ptr<Network> NetworkPtr;

    template <class BaseClass, class FactoryType> class DeclarationBase;

    class StepDeclaration;
    typedef boost::shared_ptr<StepDeclaration> StepDeclarationPtr;
    template <class DerivedClass> class StepDeclarationT;

    class TriggerDeclaration;
    typedef boost::shared_ptr<TriggerDeclaration> TriggerDeclarationPtr;
    template <class DerivedClass> class TriggerDeclarationT;

    typedef boost::shared_ptr<cedar::aux::AbstractFactory<Step> > StepFactoryPtr;
    typedef boost::shared_ptr<cedar::aux::Factory<Trigger> > TriggerFactoryPtr;

    class PluginProxy;
    typedef boost::shared_ptr<PluginProxy> PluginProxyPtr;

    class PluginDeclaration;
    typedef boost::shared_ptr<PluginDeclaration> PluginDeclarationPtr;

    /* Exceptions */
    class InvalidNameException;
    class InvalidRoleException;
    class InvalidArgumentsException;
    class DuplicateNameException;
    class MissingConnectionException;
    class PluginException;
  }
}

#endif // CEDAR_PROC_NAMESPACE_H
