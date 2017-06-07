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

    File:        Mode.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 19

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_CONV_MODE_H
#define CEDAR_AUX_CONV_MODE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumBase.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/convolution/Mode.fwd.h"

// SYSTEM INCLUDES


/*!@brief Enum describing the convolution mode.
 *
 *        This parameter describes the mode of the convolution. It has two values:
 *        <ul>
 *          <li>@em Full: The result is the full convolution, i.e., nothing is cut away.</li>
 *          <li>@em Same: The result of the convolution has the same size as the input matrix.</li>
 *        </ul>
 */
class cedar::aux::conv::Mode
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! The enum id
  typedef cedar::aux::EnumId Id;

  //! Pointer type to the enum base object of this class.
  typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Initialization of the enum values.
  static void construct();

  //! Returns a reference to the base enum object.
  static const cedar::aux::EnumBase& type();

  //! Returns a pointer to the base enum object.
  static const cedar::aux::conv::Mode::TypePtr& typePtr();

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
  //! The output image has the same size as the input.
  static const Id Same = 0;

  //! The output image is large enough so that no values have to be discarded.
  static const Id Full = 1;

  //! Only use the results where no border handling was necessary.
  static const Id Valid = 2;

protected:
  // none yet
private:
  //! The type object for this enum class.
  static cedar::aux::EnumType<cedar::aux::conv::Mode> mType;

}; // class cedar::aux::conv::Mode

#endif // CEDAR_AUX_CONV_MODE_H

