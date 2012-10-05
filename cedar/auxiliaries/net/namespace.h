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

    File:        namespace.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 25 Apr 2012

    Description: the classes in the ::net:: namespace

    Credits:

=============================================================================*/

#ifndef CEDAR_NAMESPACE_NET_H
#define CEDAR_NAMESPACE_NET_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/namespace.h"

// SYSTEM INCLUDES

// CEDAR INCLUDES
#include "cedar/auxiliaries/lib.h"
#include "cedar/defines.h"

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>
#include <opencv2/opencv.hpp>


namespace cedar
{

  namespace aux
  {

    /*!@brief Namespace for all network communication related classes. */
    namespace net
    {
      template <typename T>
      class Writer;

      template <typename T>
      class BlockingReader;

      template <typename T>
      class BlockingReader;

      //!@cond SKIPPED_DOCUMENTATION

      /* exceptions */
      CEDAR_DECLARE_AUX_CLASS(NetMissingRessourceException);
      CEDAR_DECLARE_AUX_CLASS(NetUnexpectedDataException);
      CEDAR_DECLARE_AUX_CLASS(NetUnhandledDataException);
      CEDAR_DECLARE_AUX_CLASS(NetWaitingForWriterException);
      //!@endcond

    }
  }
}; // end namespaces

#endif // CEDAR_XXX_XXX_H
