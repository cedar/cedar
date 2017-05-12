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

    File:        LoopFunctionInThread.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.rub.de
    Date:        2013 07 01

    Description: Header for the @em cedar::aux::LoopFunctionInThread class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_CALL_FUN_LOOP_THREAD_H
#define CEDAR_AUX_CALL_FUN_LOOP_THREAD_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/LoopedThread.h"
#include "cedar/units/Time.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/LoopFunctionInThread.fwd.h"

// SYSTEM INCLUDES
#include <functional>

/*!@brief This class calls a function of your choice in a (threaded) loop
 *
 * This class allows you to execute a function (accessed via a pointer
 * to the function) in a thread. It is called repeatedly in
 * setStepSize() intervals.
 * The thread ends when requestStop() is called.
 *
 * The function needs to return void and have no parameters.
 *
 * The current implementation of LoopFunctionInThread uses QT threads and needs a
 * running main event loop, i.e. you must start a Q(Core)Application's exec().
 * 
 * Pass a pointer to the function callback in LoopFunctionInThread's constructor. When you start() the LoopFunctionInThread object, a new thread will spawn and execute your function callback in a loop.
 * @see: CallFunctionInThread
 */
class cedar::aux::LoopFunctionInThread : public cedar::aux::LoopedThread
{
public:
  //----------------------------------------------------------------------------
  // typedefs
  //----------------------------------------------------------------------------

  //! the type we use to store function calls
  typedef std::function<void(cedar::unit::Time)> FunctionType;

  //----------------------------------------------------------------------------
  // constructors and destructor
  //----------------------------------------------------------------------------
public:

  /*! Construct a new LoopFunctionInThread.
   *
   * Start the thread via start().
   *
   *@param fun is a std::function<void(void)> and thus can be a function pointer or function object.
   */
  LoopFunctionInThread(FunctionType fun);

  //!@brief Destructor
  virtual ~LoopFunctionInThread();

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------
public:

  //----------------------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------------------
private:
  void step(cedar::unit::Time time);

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
private:
  //! the call-back
  FunctionType mFunction;

}
; // class cedar::aux::LoopFunctionInThread

#endif // CEDAR_AUX_FUN_LOOP_THREAD_H
