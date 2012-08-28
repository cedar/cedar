/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        LoopedTrigger.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 06

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_LOOPED_TRIGGER_H
#define CEDAR_PROC_LOOPED_TRIGGER_H

// CEDAR INCLUDES
#include "cedar/processing/namespace.h"
#include "cedar/processing/Trigger.h"
#include "cedar/auxiliaries/LoopedThread.h"
#include "cedar/auxiliaries/namespace.h"

// SYSTEM INCLUDES
#include <vector>
#include <QObject>

/*!@brief A Trigger that sends trigger events in a constant loop.
 *
 *        This class is a translation of the cedar::aux::LoopedThread concept into the processing framework.
 */
class cedar::proc::LoopedTrigger : public cedar::aux::LoopedThread,
                                   public cedar::proc::Trigger
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  LoopedTrigger(double stepSize = 1.0, const std::string& name = "");

  //!@brief Destructor
  virtual ~LoopedTrigger();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Step method implemented from the superclass.
   */
  void step(double time);

  /*!@brief Starts the trigger loop.
   * @todo Make the start/stop methods in LoopedThread virtual and overload them in LoopedTrigger instead?
   */
  void startTrigger();

  /*!@brief Stops the trigger loop.
   */
  void stopTrigger();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Removes the triggerable from the list of listeners of this trigger.
   */
  void removeListener(cedar::proc::TriggerablePtr triggerable);

  /*!@brief Adds the triggerable to the listeners of this of this trigger.
   */
  void addListener(cedar::proc::TriggerablePtr triggerable);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief Whether the looped trigger waits for all its listeners to finish their processing.
  cedar::aux::BoolParameterPtr mWait;

}; // class cedar::proc::LoopedTrigger

#endif // CEDAR_PROC_LOOPED_TRIGGER_H
