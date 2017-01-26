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

    File:        LoopedElementInNonLoopedGroup.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2014 07 18

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_LOPPED_ELEMENT_IN_NON_LOOPED_GROUP_H
#define CEDAR_PROC_LOPPED_ELEMENT_IN_NON_LOOPED_GROUP_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/consistency/ConsistencyIssue.h"

// FORWARD DECLARATIONS
#include "cedar/processing/consistency/LoopedElementInNonLoopedGroup.fwd.h"

// FORWARD DECLARATIONS
#include "cedar/processing/Element.fwd.h"

// SYSTEM INCLUDES
#include <string>


/*!@brief A consistency issue that indicates a looped step that is not connected to a looped trigger.
 */
class cedar::proc::LoopedElementInNonLoopedGroup : public cedar::proc::ConsistencyIssue
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  LoopedElementInNonLoopedGroup(cedar::proc::ElementPtr unconnectedElement);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Returns the looped step that is not connected to a looped trigger.
  cedar::proc::ElementPtr getUnconnectedElement() const
  {
    return this->mUnconnectedElement;
  }

  std::string getDescription() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! The unconnected element.
  cedar::proc::ElementPtr mUnconnectedElement;

}; // class cedar::proc::LoopedElementInNonLoopedGroup

#endif // CEDAR_PROC_LOPPED_ELEMENT_IN_NON_LOOPED_GROUP_H

