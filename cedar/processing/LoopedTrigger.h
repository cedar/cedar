/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/processing/Trigger.h"
#include "cedar/auxiliaries/LoopedThread.h"

// FORWARD DECLARATIONS
#include "cedar/processing/LoopedTrigger.fwd.h"

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
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  LoopedTrigger(
                 cedar::unit::Time stepSize = cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::second),
                 const std::string& name = ""
               );

  //!@brief Destructor
  virtual ~LoopedTrigger();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Step method implemented from the superclass.
   */
  void step(cedar::unit::Time time);

  /*!@brief Starts the trigger loop.
   *
   * @deprecated Use the ThreadWrapper::start() method instead.
   */
  CEDAR_DECLARE_DEPRECATED(void startTrigger())
  {
    start();
  }

  /*!@brief Stops the trigger loop.
   *
   * @deprecated Use the ThreadWrapper::stop() method instead.
   */
  CEDAR_DECLARE_DEPRECATED(void stopTrigger())
  {
    stop();
  }

public slots:
  //!@brief This slot is called when the step's name is changed.
  void onNameChanged();

signals:
  //!@brief Signal that is emitted whenever the step's name is changed.
  void nameChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

signals:
  //! Emitted before the trigger is started.
  void triggerStarting();

  //! Emitted whenever the trigger is started.
  void triggerStarted();

  //! Emitted before the trigger is stopped.
  void triggerStopping();

  //! Emitted whenever the trigger is stopped.
  void triggerStopped();

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

  //! Called when the trigger is started.
  void prepareStart();

  //! Called when the trigger is started.
  void processStop(bool);

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
  //! Used to prevent multiple start calls to the trigger.
  bool mStarted;

  //! Used to prevent multiple start calls to the trigger.
  QMutex mStartedMutex;

}; // class cedar::proc::LoopedTrigger

#endif // CEDAR_PROC_LOOPED_TRIGGER_H
