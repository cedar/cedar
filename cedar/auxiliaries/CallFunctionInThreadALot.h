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

    File:        CallFunctionInThreadALot.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2010 12 02

    Description: Header for the @em cedar::aux::CallFunctionInThreadALot class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_CALL_FUN_THREAD_A_LOT_H
#define CEDAR_AUX_CALL_FUN_THREAD_A_LOT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/detail/CallFunctionInThreadALotWorker.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/LoopMode.h"
#include "cedar/auxiliaries/ThreadWrapper.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/CallFunctionInThreadALot.fwd.h"

// SYSTEM INCLUDES
#include <string>
#include <iostream>
#include <QThread>
#include <QMutex>
#include <QReadWriteLock>
#ifndef Q_MOC_RUN
  #include <boost/date_time/posix_time/posix_time_types.hpp>
#endif
#include <functional>

/*!@brief This class wraps a function call into a thread.
 *
 * This class allows you to execute a function (accessed via a pointer
 * to the function) in a thread.
 * The thread ends when the function returns.
 *
 * The function needs to return void and have no parameters.
 *
 * The current implementation of CallFunctionInThreadALot uses QT threads and needs a
 * running main event loop, i.e. you must start a Q(Core)Application's exec().
 * 
 * Pass a pointer to the function callback in CallFunctionInThreadALot's constructor. When you start() the CallFunctionInThread object, a new thread will spawn and execute your function callback.
 * @see: LoopFunctionInThread
 */
class cedar::aux::CallFunctionInThreadALot : public cedar::aux::ThreadWrapper
{
  Q_OBJECT

  //----------------------------------------------------------------------------
  // friends
  //----------------------------------------------------------------------------

  friend class cedar::aux::detail::CallFunctionInThreadALotWorker;

public:
  //----------------------------------------------------------------------------
  // typedefs
  //----------------------------------------------------------------------------

  //! the type we use to store function calls
  typedef std::function< void(void) > FunctionType;

  //----------------------------------------------------------------------------
  // constructors and destructor
  //----------------------------------------------------------------------------
public:

  /*! Construct a new CallFunctionInThreadALot.
   *
   * Start the thread via start(). Execute the function via execute()
   *
   *@param fun is a std::function< void(void) > and thus can be a function pointer or function object.
   */
  CallFunctionInThreadALot(FunctionType fun);

  //!@brief Destructor
  virtual ~CallFunctionInThreadALot();

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------
public:
  //! Execute the function in its own thread.
  void execute();

  //! is the step currently executing?
  bool isExecuting();

  void finishedWorkSlot();

  //----------------------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------------------
private:
  //! function that executes the stored function pointer
  void call();

  //! overwritten method. return a new worker pointer
  cedar::aux::detail::ThreadWorker* resetWorker();

  //! called when stop() is called
  void processStop();

signals:
  //! Signals, that the function should be executed.
  void executeSignal(); 

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
private:
  //! the call-back
  FunctionType mFunction;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! the worker pointer
  cedar::aux::detail::CallFunctionInThreadALotWorker* mpWorker;

  bool mExecuting;
  mutable QReadWriteLock mExecutingLock;

}; // class cedar::aux::CallFunctionInThreadALot

#endif // CEDAR_AUX_LOOPED_THREAD_H
