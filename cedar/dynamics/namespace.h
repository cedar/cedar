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

// LOCAL INCLUDES
#include "dynamics/lib.h"
#ifdef WINDOWS
#include "processing/PluginDeclaration.h"
#endif // WINDOWS

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>
#include <opencv2/opencv.hpp>


namespace cedar
{
  /*!@brief Namespace for all dyn classes. */
  namespace dyn
  {
    class CEDAR_DYN_LIB_EXPORT Dynamics;
    typedef boost::shared_ptr<Dynamics> DynamicsPtr;

    template <typename T> class Activation;
    typedef Activation<double> DoubleActivation;
    typedef boost::shared_ptr<DoubleActivation> DoubleActivationPtr;
    typedef Activation<cv::Mat> MatActivation;
    typedef boost::shared_ptr<MatActivation> MatActivationPtr;

    class CEDAR_DYN_LIB_EXPORT SpaceCode;
    typedef boost::shared_ptr<SpaceCode> SpaceCodePtr;

    class CEDAR_DYN_LIB_EXPORT NeuralField;
    typedef boost::shared_ptr<NeuralField> NeuralFieldPtr;

#ifdef GCC
    // for msvc, this is replaced by the plugin stuff below, currently as a workaround.
    CEDAR_DYN_LIB_EXPORT void initialize();
#endif // GCC
  }
}

#ifdef MSVC // workaround for circular linking
CEDAR_BEGIN_PLUGIN_DECLARATION

cedar::proc::PluginDeclarationPtr pluginDeclaration();

CEDAR_END_PLUGIN_DECLARATION
#endif // MSVC

#endif // CEDAR_DYN_NAMESPACE_H
