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

    File:        NormalizationType.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 17

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROCESSING_STEPS_NORMALIZATION_TYPE_H
#define CEDAR_PROCESSING_STEPS_NORMALIZATION_TYPE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumType.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/NormalizationType.fwd.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>


/*!@brief An enum class for normalization types.
 */
class cedar::proc::steps::NormalizationType
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Type of the enum.
  typedef cedar::aux::EnumId Id;
public:
  //! Pointer to the enumeration type.
  typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Constructs the enumeration values.
  static void construct();

  //! Returns the enum base class.
  static const cedar::aux::EnumBase& type();

  //! Returns a pointer to the enum base class.
  static const cedar::proc::steps::NormalizationType::TypePtr& typePtr();

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
  //! No normalization.
  static const Id None = 0;

  //! Infinity norm.
  static const Id InfinityNorm = cv::NORM_INF;

  //! L1 norm.
  static const Id L1Norm = cv::NORM_L1;

  //! L2 norm.
  static const Id L2Norm = cv::NORM_L2;

protected:
  // none yet
private:
  static cedar::aux::EnumType<cedar::proc::steps::NormalizationType> mType;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::steps::NormalizationType

#endif // CEDAR_PROCESSING_STEPS_NORMALIZATION_TYPE_H

