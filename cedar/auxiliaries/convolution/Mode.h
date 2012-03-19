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
#include "cedar/auxiliaries/convolution/namespace.h"
#include "cedar/auxiliaries/EnumBase.h"

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
    typedef cedar::aux::EnumId Id;
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
    static void construct();

    static const cedar::aux::EnumBase& type();

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
    static const Id Same = 0;
    static const Id Full = 1;

protected:
  // none yet
private:
  static cedar::aux::EnumType<cedar::aux::conv::Mode> mType;

}; // class cedar::aux::conv::Mode

#endif // CEDAR_AUX_CONV_MODE_H

