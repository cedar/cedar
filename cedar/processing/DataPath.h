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
#include <string>


/*!@brief A class that encapsulates functionality for addressing data in an architecture.
 *
 * This class is responsible for parsing and creating paths to data in architectures. These paths have the form
 * path.to.Element[ROLE].data name
 * where path.to.Element is a cedar::proc::GroupPath, ROLE is one of either INPUT, BUFFER or OUTPUT
 * (cf. cedar::proc::DataRole), and data name is the name of a data slot in the element referenced by path.to.Element.
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
  //!@brief Constructor that takes a path to an element, a role and a data name.
  DataPath(cedar::proc::GroupPath pathToElement, cedar::proc::DataRole::Id role, const std::string& dataName);

  //! Constructs the path from a string.
  DataPath(const std::string& path);

  //!@brief Destructor
  virtual ~DataPath();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Returns the path to the element stored in this data path.
  cedar::proc::GroupPath getPathToElement() const;

  //! Returns the data role stored in this path.
  cedar::proc::DataRole::Id getDataRole() const;

  //! Returns the name of the data object referenced by this path.
  std::string getDataName() const;

  //! Sets the path by parsing a string.
  void parseFromString(const std::string& dataPath);

  //! Sets the data path with a given group path to an element, data role and data name.
  void setDataPath(cedar::proc::GroupPath pathToElement, cedar::proc::DataRole::Id role, const std::string& dataName);

  //! Converts the path to a string.
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

