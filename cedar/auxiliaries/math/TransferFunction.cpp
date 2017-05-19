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

    File:        TransferFunction.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 07 19

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/TransferFunction.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::math::TransferFunction::~TransferFunction()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

float cedar::aux::math::TransferFunction::compute(float value) const
{
  return static_cast<float>(compute(static_cast<double>(value)));
}

cv::Mat cedar::aux::math::TransferFunction::compute(const cv::Mat& values) const
{
  cv::Mat result = values.clone();
  if (values.type() == CV_32F)
  {
    cv::MatConstIterator_<float> iter_src = values.begin<float>();
    cv::MatIterator_<float> iter_dest = result.begin<float>();
    auto end = values.end<float>();
    for ( ; iter_src != end; ++iter_src, ++iter_dest)
    {
      *iter_dest = static_cast<float>(compute(static_cast<double>(*iter_src)));
    }
  }
  else if (values.type() == CV_64F)
  {
    cv::MatConstIterator_<double> iter_src = values.begin<double>();
    cv::MatIterator_<double> iter_dest = result.begin<double>();
    auto end = values.end<double>();
    for ( ; iter_src != end; ++iter_src, ++iter_dest)
    {
      *iter_dest = compute(*iter_src);
    }
  }
  else
  {
    CEDAR_THROW
    (
      cedar::aux::NotImplementedException,
      "This transfer function is not implemented for non-floating data types."
    );
  }
  return result;
}

