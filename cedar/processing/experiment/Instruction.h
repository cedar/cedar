/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Instruction.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2014 01 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_proc_EXPERIMENT_INSTRUCTION_H
#define CEDAR_proc_EXPERIMENT_INSTRUCTION_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/Instruction.fwd.h"

// SYSTEM INCLUDES


/*!@brief
 */
class cedar::proc::experiment::Instruction : public cedar::aux::Configurable
{
private:

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  Instruction();
  //!@brief Destructor
  ~Instruction();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:

}; // class cedar::proc::experiment::Instruction

#include "cedar/auxiliaries/FactoryManager.h"

CEDAR_AUX_EXPORT_SINGLETON(cedar::aux::FactoryManager<cedar::proc::experiment::InstructionPtr>);

namespace cedar
{
  namespace proc
  {
    namespace experiment
    {
      //!@brief The singleton instance of the kernel factory manager.
      typedef cedar::aux::Singleton< cedar::aux::FactoryManager<cedar::proc::experiment::InstructionPtr>>
              InstructionManagerSingleton;
    }
  }
}

#endif // CEDAR_proc_EXPERIMENT_INSTRUCTION_H

