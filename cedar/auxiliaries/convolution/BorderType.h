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

    File:        BorderType.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 14

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_CONV_BORDER_TYPE_H
#define CEDAR_AUX_CONV_BORDER_TYPE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/namespace.h"
#include "cedar/auxiliaries/EnumBase.h"

// SYSTEM INCLUDES


/*!@brief An enum class for the different kinds of border handling in convolution methods.
 */
class cedar::aux::conv::BorderType
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
    /*!@brief Initializes the enum values.
     */
    static void construct();

    /*!@brief Returns a reference to the enum base object.
     */
    static const cedar::aux::EnumBase& type();

    /*!@brief Returns a pointer to the enum base object.
     */
    static const cedar::aux::conv::BorderType::TypePtr& typePtr();

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
    //! Borders are considered cyclic.
    static const Id Cyclic = 0;

    //! Values at the borders are considered zero.
    static const Id Zero = 1;

    //! The values at the borders are the same as those of the closest point in the image.
    static const Id Replicate = 2;

    //! Borders are mirror-images of the actual image.
    static const Id Reflect = 3;

  protected:
    // none yet

  private:
    //! The enum object.
    static cedar::aux::EnumType<cedar::aux::conv::BorderType> mType;

}; // class cedar::aux::conv::BorderType

#endif // CEDAR_AUX_CONV_BORDER_TYPE_H

