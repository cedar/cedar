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
#include "cedar/auxiliaries/EnumBase.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/convolution/BorderType.fwd.h"

// SYSTEM INCLUDES
#include <opencv2/imgproc/imgproc.hpp>

/*!@brief An enum class for the different kinds of border handling in convolution methods.
 */
class cedar::aux::conv::BorderType
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! The type of the enum values.
  typedef cedar::aux::EnumId Id;

  //! The pointer type of the enum base object.
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

  /*!@brief Returns the border type translated to one of the opencv constants.
   */
  inline static int toCvConstant(Id id)
  {
    switch (id)
    {
      case cedar::aux::conv::BorderType::Cyclic:
        return cv::BORDER_WRAP;

      case cedar::aux::conv::BorderType::Reflect:
        return cv::BORDER_REFLECT;

      case cedar::aux::conv::BorderType::Replicate:
        return cv::BORDER_REPLICATE;

      case cedar::aux::conv::BorderType::Zero:
        return cv::BORDER_CONSTANT;

      default:
        return cv::BORDER_DEFAULT;
    }
  }

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

