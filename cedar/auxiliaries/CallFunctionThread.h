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

    File:        CallFunctionThread.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2010 12 02

    Description: Header for the @em cedar::aux::CallFunctionThread class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_CALL_FUN_THREAD_H
#define CEDAR_AUX_CALL_FUN_THREAD_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/LoopMode.h"
#include "cedar/auxiliaries/ThreadWrapper.h"
#include "cedar/auxiliaries/CallFunctionThreadWorker.h"

// SYSTEM INCLUDES
#include <string>
#include <iostream>
#include <QThread>
#include <QMutex>
#include <QReadWriteLock>
#include <boost/date_time/posix_time/posix_time_types.hpp>


class cedar::aux::CallFunctionThread : public cedar::aux::ThreadWrapper
{
  //----------------------------------------------------------------------------
  // friends
  //----------------------------------------------------------------------------

  friend class cedar::aux::detail::CallFunctionThreadWorker;

public:
  //----------------------------------------------------------------------------
  // typedefs
  //----------------------------------------------------------------------------

  typedef std::function< void(void) > FunctionType;

  //----------------------------------------------------------------------------
  // constructors and destructor
  //----------------------------------------------------------------------------
public:

  CallFunctionThread(FunctionType fun);

  //!@brief Destructor
  virtual ~CallFunctionThread();

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------
public:


  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------
protected:

  //----------------------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------------------
private:
  void call();

  cedar::aux::detail::ThreadWorker* resetWorker();


  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
protected:

private:
  FunctionType mFunction;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::aux::detail::CallFunctionThreadWorker* mpWorker;
}; // class cedar::aux::CallFunctionThread

#endif // CEDAR_AUX_LOOPED_THREAD_H
