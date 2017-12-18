/*=============================================================================

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

===============================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        cvMatHelper.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011

    Description: Helper class for openCV matrices. Implements some checks 

    Credits:

=============================================================================*/

#ifndef CEDAR_CVMAT_HELPER_H
#define CEDAR_CVMAT_HELPER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/datatypesupport/CollatedTraits.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/opencv/cvMatNetHeader.h"

// PROJECT INCLUDES
#include <opencv2/opencv.hpp>

// SYSTEM INCLUDES




namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {
//!@cond SKIPPED_DOCUMENTATION

//!@brief a helper class which implements some checks for opencv types
template <typename CVT>
class cvMatHelper
{

  //---------------------------------------------------------------------------
  // members and typedefs
  //---------------------------------------------------------------------------
public:
  typedef cvMatNetHeader HeaderType;

private:
  HeaderType mLocalHeader; // local info about matrix-header, to compare
                           // with future user inputs

  //!@brief prepare (init) the external data, which will be sent
  void initExternalHeader(const CVT &mat, HeaderType &extheader);
  //!@brief prepare (init) the local copy of the header for comparisons
  void initLocalHeader(const HeaderType &extheader);


  //---------------------------------------------------------------------------
  // protected methods
  //---------------------------------------------------------------------------
protected:
  //!@brief check the data before writing it (opencv implementation)
  bool checkCollatedDataForWrite(const CVT &mat, HeaderType &extheader);
  //!@brief check the data before reading it (opencv implementation)
  bool checkCollatedDataForRead(const HeaderType &extheader);

  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  cvMatHelper();
  //!@brief Destructor
  virtual ~cvMatHelper();
};

//!@endcond
} } } } // end namespaces

#endif
