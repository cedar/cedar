/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 03

    Description: Namespace file for cedar::dyn.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DYN_NAMESPACE_H
#define CEDAR_DYN_NAMESPACE_H

// CEDAR INCLUDES
#include "cedar/dynamics/lib.h"
#include "cedar/processing/PluginDeclaration.h"

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>
#include <opencv2/opencv.hpp>


namespace cedar
{
  /*!@brief Namespace for all dyn classes. */
  namespace dyn
  {
    //!@brief a template class tailored for use in all dynamics
    template <typename T> class Activation;
    //!@brief a specialization for double values
    typedef Activation<double> DoubleActivation;

    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_GENERATE_POINTER_TYPES(DoubleActivation);
    CEDAR_DECLARE_DYN_CLASS(Dynamics);
    CEDAR_DECLARE_DYN_CLASS(SpaceCode);
    CEDAR_DECLARE_DYN_CLASS(NeuralField);
    CEDAR_DECLARE_DYN_CLASS(MatActivation);
    CEDAR_DECLARE_DYN_CLASS(Preshape);
    CEDAR_DECLARE_DYN_CLASS(Noise);
    CEDAR_DECLARE_DYN_CLASS(RateMatrixToSpaceCode);
    CEDAR_DECLARE_DYN_CLASS(RateToSpaceCode);
    CEDAR_DECLARE_DYN_CLASS(SpaceToRateCode);
    //!@endcond

    //!@brief A function for loading the declarations of the dynamics namespace as plugin.
    CEDAR_DECLARE_DEPRECATED(CEDAR_DYN_LIB_EXPORT void getPluginDesciption(cedar::proc::PluginDeclarationPtr plugin));
  }
}

#endif // CEDAR_DYN_NAMESPACE_H
