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


#ifndef CEDAR_AUX_COMP_NAMESPACE_H
#define CEDAR_AUX_COMP_NAMESPACE_H

// LOCAL INCLUDES

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>


namespace cedar
{
  namespace aux
  {
    /*!@brief Namespace for all math classes. */
    namespace comp
    {
      class Arguments;
      typedef boost::shared_ptr<Arguments> ArgumentsPtr;

      class StepTime;
      typedef boost::shared_ptr<StepTime> StepTimePtr;

      class ProcessingStep;
      typedef boost::shared_ptr<ProcessingStep> ProcessingStepPtr;

      class LoopArguments;

      class MultiTrigger;
      typedef boost::shared_ptr<MultiTrigger> MultiTriggerPtr;

      class Trigger;
      typedef boost::shared_ptr<Trigger> TriggerPtr;

      class LoopedTrigger;
      typedef boost::shared_ptr<LoopedTrigger> LoopedTriggerPtr;
    }
  }
}

#endif // CEDAR_AUX_COMP_NAMESPACE_H
