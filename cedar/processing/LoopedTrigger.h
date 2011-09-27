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

// LOCAL INCLUDES
#include "processing/namespace.h"
#include "processing/Trigger.h"
#include "auxiliaries/LoopedThread.h"
#include "auxiliaries/NumericParameter.h"
#include "auxiliaries/EnumParameter.h"


// PROJECT INCLUDES

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
  LoopedTrigger(double stepSize = 1.0);

  //!@brief Destructor
  virtual ~LoopedTrigger();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void step(double time);

  //!@todo Make the start/stop methods in LoopedThread virtual?
  void startTrigger();
  void stopTrigger();

public slots:
  void loopModeChanged();
  void loopTimeChanged();

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
public:
  // none yet (hopefully never!)
protected:

private:


  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::aux::EnumParameterPtr mLoopType;
  cedar::aux::DoubleParameterPtr mLoopTime;

}; // class cedar::proc::LoopedTrigger

#endif // CEDAR_PROC_LOOPED_TRIGGER_H

