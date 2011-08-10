/*=============================================================================

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

===============================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        cvMatHelper.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011 05:10:21 PM CEST

    Description: Helper class for openCV matrices. Implements some checks 
                 we need to conform to the InterfaceCollatedData 

    Credits:

=============================================================================*/

#ifndef CEDAR_CVMATHELPER_H
#define CEDAR_CVMATHELPER_H

// LOCAL INCLUDES
#include "../../namespace.h"
#include "../interfaces/InterfaceCollatedData.h"
#include "../CollatedTraits.h"
#include "cvMatNetHeader.h"

// PROJECT INCLUDES
#include <opencv/cv.h>

// SYSTEM INCLUDES




namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {

template <typename CVT>
class cvMatHelper  : virtual protected InterfaceCollatedData<CVT>
{

  //---------------------------------------------------------------------------
  // members and typedefs
  //---------------------------------------------------------------------------
public:
  typedef cvMatNetHeader HeaderType;

private:
  HeaderType mCheckHeader; // local info about matrix-header, to compare
                           // with future user inputs

  void init_externalheader(const CVT &mat, HeaderType &extheader);
  void init_checkheader(const HeaderType &extheader);


  //---------------------------------------------------------------------------
  // protected methods
  //---------------------------------------------------------------------------
protected:
  bool check_collateddata_for_write(const CVT &mat, HeaderType &extheader);
  bool check_collateddata_for_read(const HeaderType &extheader);

  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  cvMatHelper();
  //!@brief Destructor
  virtual ~cvMatHelper();
};

} } } } // end namespaces

#endif
