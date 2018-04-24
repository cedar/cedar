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

    File:        ConsoleLog.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 02 14

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_CONSOLE_LOG_H
#define CEDAR_AUX_CONSOLE_LOG_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/LogInterface.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/ConsoleLog.fwd.h"

// SYSTEM INCLUDES
#include <string>


/*!@brief An interface for classes that redirect log outputs to the console.
 *
 *        This class represents cedar's default logger, i.e., if nothing else is set, all log messages will be printed
 *        on the console.
 *
 *        Depending on the operating system, these messages will also appear in different colors.
 */
class cedar::aux::ConsoleLog : public cedar::aux::LogInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet.

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief logs a message to a terminal
  void message(cedar::aux::LOG_LEVEL level, const std::string& message, const std::string& title);

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
  // none yet
}; // class cedar::aux::ConsoleLog

#endif // CEDAR_AUX_CONSOLE_LOG_H

