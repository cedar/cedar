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

    File:        namespace.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 11 28

    Description: Namespace file for cedar::aux::conv.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_CONV_NAMESPACE_H
#define CEDAR_AUX_CONV_NAMESPACE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/defines.h"

// SYSTEM INCLUDES

namespace cedar
{
  namespace aux
  {
    /*!@brief Namespace for all classes related to cedar's convolution framework.
     */
    namespace conv
    {
      //!@cond SKIPPED_DOCUMENTATION
      CEDAR_DECLARE_AUX_CLASS(BorderType);
      CEDAR_DECLARE_AUX_CLASS(Convolution);
      CEDAR_DECLARE_AUX_CLASS(Engine);
      CEDAR_DECLARE_AUX_CLASS(KernelList);
      CEDAR_DECLARE_AUX_CLASS(Mode);

      // convolution engines
      CEDAR_DECLARE_AUX_CLASS(FFTW);
      CEDAR_DECLARE_AUX_CLASS(OpenCV);
      //!@endcond

      /*!@brief The manager of all convolution classes.
       */
      typedef cedar::aux::FactoryManager<cedar::aux::conv::EnginePtr> EngineManager;

      /*!@brief The manager of all convolution classes.
       */
      typedef cedar::aux::Singleton<EngineManager> EngineManagerSingleton;

      /*!@brief A parameter for dynamically storing convolution engine objects.
       */
      typedef cedar::aux::ObjectParameterTemplate<cedar::aux::conv::Engine> EngineParameter;

      //!@cond SKIPPED_DOCUMENTATION
      CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(EngineParameter);
      //!@endcond
    }
  }
}

#endif // CEDAR_AUX_CONV_NAMESPACE_H
