/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        DataPath.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2015 01 23

    Description: Header file for the class cedar::proc::DataPath.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_DATA_PATH_H
#define CEDAR_PROC_DATA_PATH_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/DataRole.h"
#include "cedar/processing/GroupPath.h"

// FORWARD DECLARATIONS
#include "cedar/processing/DataPath.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::DataPath
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  DataPath(cedar::proc::GroupPath pathToElement, cedar::proc::DataRole::Id role, const std::string& dataName);
  DataPath(const std::string& path);

  //!@brief Destructor
  virtual ~DataPath();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  cedar::proc::GroupPath getPathToElement() const;
  cedar::proc::DataRole::Id getDataRole() const;
  std::string getDataName() const;
  void parseFromString(const std::string& dataPath);
  void setDataPath(cedar::proc::GroupPath pathToElement, cedar::proc::DataRole::Id role, const std::string& dataName);
  std::string toString() const;

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
  cedar::proc::GroupPath mPathToElement;
  cedar::proc::DataRole::Id mDataRole;
  std::string mDataName;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::DataPath

#endif // CEDAR_PROC_DATA_PATH_H

