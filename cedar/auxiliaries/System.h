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

    File:        System.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 26

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_SYSTEM_H
#define CEDAR_AUX_SYSTEM_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <string>
#include <QReadWriteLock>
#include <fstream>


/*!@brief Wrapper for some functions that depend on the operating system.
 */
class cedar::aux::System
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief try to get the home directory from standard environment variables
  static std::string getUserHomeDirectory();
  //!@brief try to get the directory in which application data of cedar is stored
  static std::string getUserApplicationDataDirectory();

  /*!@brief This function opens a crash report file in a standardized location.
   */
  static void openCrashFile(std::ofstream& stream, std::string& fileName);


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
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief For debugging only.
  static QReadWriteLock mCOutLock;
protected:
  // none yet

private:

}; // class cedar::aux::System

#endif // CEDAR_AUX_SYSTEM_H

