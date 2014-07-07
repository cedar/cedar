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

    File:        LogFile.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 10 27

    Description: Header for the \em cedar::aux::LogFile class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_LOG_FILE_H
#define CEDAR_AUX_LOG_FILE_H

// CEDAR INCLUDES
#include "cedar/namespace.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/LogFile.fwd.h"

// SYSTEM INCLUDES
#include <fstream>
#include <string>

/*!@brief A class for logging messages in a file.
 *
 * @todo Port this to the new logging framework or remove it.
 */
class cedar::aux::LogFile : public std::ofstream
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The constructor.
  LogFile(const std::string& logFileName);

  //!@brief Destructor
  virtual ~LogFile();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Adds a separator line to the log file.
  void addSeparatorLine();
  //!@brief Adds a time stamp to the log file, without linefeed.
  void addTimeStamp();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

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
}; // class cedar::aux::LogFile

#endif // CEDAR_AUX_LOG_FILE_H
