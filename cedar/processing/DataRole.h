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

    File:        DataRole.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 24

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_DATA_ROLE_H
#define CEDAR_PROC_DATA_ROLE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumType.h"

// FORWARD DECLARATIONS
#include "cedar/processing/DataRole.fwd.h"

// SYSTEM INCLUDES


/*!@brief This is an enum class for the roles data can have in a cedar::proc::Step.
 *
 * Currently, data can be input, output or a buffer, i.e., internal data that should be plottable nonetheless.
 */
class cedar::proc::DataRole
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! The base type of enum ids of this class.
  typedef cedar::aux::EnumId Id;

  //! Typedef of the shared pointer of enum values belonging to this class.
  typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! @brief Construct method that fills the enum.
   *  @see cedar::aux::EnumBase
   */
  static void construct();

  //! @returns A const reference to the base enum object.
  static const cedar::aux::EnumBase& type();

  //! @returns A pointer to the base enum object.
  static const cedar::proc::DataRole::TypePtr& typePtr();

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
  //! Identifier for the role input.
  static const Id INPUT = 0;
  //! Identifier for the role output.
  static const Id OUTPUT = 1;
  //! Identifier for the role buffer.
  static const Id BUFFER = 2;

private:
  //! The base enum object.
  static cedar::aux::EnumType<cedar::proc::DataRole> mType;
}; // class cedar::proc::DataRole

#endif // CEDAR_PROC_DATA_ROLE_H
