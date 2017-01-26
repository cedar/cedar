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

    File:        NotLogger.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 01

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_NULL_LOGGER_H
#define CEDAR_AUX_NULL_LOGGER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/LogInterface.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/NullLogger.fwd.h"


// SYSTEM INCLUDES


/*!@brief  Doesn't log messages.
 *
 *         This logger can be used to filter out undesired messages. Any messages sent to this logger will not go
 *         anywhere.
 *
 * @remarks The logger, it does nothing!
 */
class cedar::aux::NullLogger : public cedar::aux::LogInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief drops all messages
  void message(cedar::aux::LOG_LEVEL, const std::string&, const std::string&);

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
}; // class cedar::aux::NullLogger

#endif // CEDAR_AUX_NULL_LOGGER_H

