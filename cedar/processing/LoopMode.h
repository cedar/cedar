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

    File:        LoopMode.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 09 20

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_LOOP_MODE_H
#define CEDAR_PROC_LOOP_MODE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/processing/namespace.h"

// SYSTEM INCLUDES

/*!@brief   Enum class for the loop mode.
 *
 * @see     @ref EnumClasses for an explanation of the concepts.
 * @remarks This is used as a parameter for cedar::proc::LoopedTrigger.
 */
class cedar::proc::LoopMode
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Type to be used when storing enum ids of this enum class.
  typedef cedar::aux::EnumId Id;

  //!@brief Pointer to this enum's enum base type.
  typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief A static construct method that gets called by @em TypePtr.
   */
  static void construct();

  /*!@brief Returns a const reference to the enum base object.
   */
  static const cedar::aux::EnumBase& type();

  /*!@brief Returns a pointer to the enum base object.
   */
  static const cedar::proc::LoopMode::TypePtr& typePtr();

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
  //!@brief Step time that gets bigger when the calculations attached to the loop are too slow.
  static const Id FIXED_ADAPTIVE = 0;

  //!@brief Fixed step time.
  static const Id FIXED = 1;

  //!@brief Step time is determined by measuring.
  static const Id REALTIME = 2;

protected:
  // none yet
private:
  //!@brief The type pointer of the enum.
  static cedar::aux::EnumType<cedar::proc::LoopMode> mType;

}; // class cedar::proc::DataRole

#endif // CEDAR_PROC_DATA_ROLE_H

