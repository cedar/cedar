/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 10 12

    Description: Namespace file for cedar::aux.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_NAMESPACE_H
#define CEDAR_AUX_NAMESPACE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/lib.h"
#include "cedar/defines.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/smart_ptr.hpp>
  #include <boost/property_tree/ptree.hpp>
#endif
#include <opencv2/opencv.hpp>

#warning Do not include this header any more. Use the new forward declaration headers instead!

#include "cedar/auxiliaries/BoolParameter.fwd.h"
#include "cedar/auxiliaries/BoolVectorParameter.fwd.h"
#include "cedar/auxiliaries/CallFunctionInThread.fwd.h"
#include "cedar/auxiliaries/Cloneable.fwd.h"
#include "cedar/auxiliaries/CommandLineParser.fwd.h"
#include "cedar/auxiliaries/Configurable.fwd.h"
#include "cedar/auxiliaries/ConsoleLog.fwd.h"
#include "cedar/auxiliaries/Data.fwd.h"
#include "cedar/auxiliaries/DataTemplate.fwd.h"
#include "cedar/auxiliaries/DataSpectator.fwd.h"
#include "cedar/auxiliaries/DeclarationManagerTemplate.fwd.h"
#include "cedar/auxiliaries/DirectoryParameter.fwd.h"
#include "cedar/auxiliaries/DoubleData.fwd.h"
#include "cedar/auxiliaries/DoubleParameter.fwd.h"
#include "cedar/auxiliaries/DoubleVectorParameter.fwd.h"
#include "cedar/auxiliaries/Enum.fwd.h"
#include "cedar/auxiliaries/EnumBase.fwd.h"
#include "cedar/auxiliaries/EnumParameter.fwd.h"
#include "cedar/auxiliaries/EnumType.fwd.h"
#include "cedar/auxiliaries/exceptions.fwd.h"
#include "cedar/auxiliaries/GlobalClock.fwd.h"
#include "cedar/auxiliaries/Grabbable.fwd.h"
#include "cedar/auxiliaries/GraphTemplate.fwd.h"
#include "cedar/auxiliaries/Factory.fwd.h"
#include "cedar/auxiliaries/FactoryDerived.fwd.h"
#include "cedar/auxiliaries/FactoryManager.fwd.h"
#include "cedar/auxiliaries/FileParameter.fwd.h"
#include "cedar/auxiliaries/ImageData.fwd.h"
#include "cedar/auxiliaries/IntParameter.fwd.h"
#include "cedar/auxiliaries/IntrusivePtrBase.fwd.h"
#include "cedar/auxiliaries/IntVectorParameter.fwd.h"
#include "cedar/auxiliaries/ImageSetData.fwd.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.fwd.h"
#include "cedar/auxiliaries/Lockable.fwd.h"
#include "cedar/auxiliaries/Log.fwd.h"
#include "cedar/auxiliaries/LogFile.fwd.h"
#include "cedar/auxiliaries/LogFilter.fwd.h"
#include "cedar/auxiliaries/LogInterface.fwd.h"
#include "cedar/auxiliaries/LoopedThread.fwd.h"
#include "cedar/auxiliaries/LoopFunctionInThread.fwd.h"
#include "cedar/auxiliaries/LoopMode.fwd.h"
#include "cedar/auxiliaries/MapParameter.fwd.h"
#include "cedar/auxiliaries/MatrixIterator.fwd.h"
#include "cedar/auxiliaries/NamedConfigurable.fwd.h"
#include "cedar/auxiliaries/NullLogger.fwd.h"
#include "cedar/auxiliaries/NumericParameter.fwd.h"
#include "cedar/auxiliaries/NumericVectorParameter.fwd.h"
#include "cedar/auxiliaries/ObjectListParameter.fwd.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.fwd.h"
#include "cedar/auxiliaries/ObjectMapParameterTemplate.fwd.h"
#include "cedar/auxiliaries/ObjectParameter.fwd.h"
#include "cedar/auxiliaries/ObjectParameterTemplate.fwd.h"
#include "cedar/auxiliaries/Parameter.fwd.h"
#include "cedar/auxiliaries/ParameterTemplate.fwd.h"
#include "cedar/auxiliaries/Path.fwd.h"
#include "cedar/auxiliaries/PluginDeclaration.fwd.h"
#include "cedar/auxiliaries/PluginDeclarationList.fwd.h"
#include "cedar/auxiliaries/PluginDeclarationTemplate.fwd.h"
#include "cedar/auxiliaries/PluginProxy.fwd.h"
#include "cedar/auxiliaries/Recorder.fwd.h"
#include "cedar/auxiliaries/SetParameter.fwd.h"
#include "cedar/auxiliaries/Settings.fwd.h"
#include "cedar/auxiliaries/Singleton.fwd.h"
#include "cedar/auxiliaries/StereoImageData.fwd.h"
#include "cedar/auxiliaries/StringMapParameter.fwd.h"
#include "cedar/auxiliaries/StringParameter.fwd.h"
#include "cedar/auxiliaries/StringSetParameter.fwd.h"
#include "cedar/auxiliaries/StringVectorParameter.fwd.h"
#include "cedar/auxiliaries/ThreadCollection.fwd.h"
#include "cedar/auxiliaries/ThreadWrapper.fwd.h"
#include "cedar/auxiliaries/TypeBasedFactory.fwd.h"
#include "cedar/auxiliaries/TypeHierarchyMap.fwd.h"
#include "cedar/auxiliaries/UIntParameter.fwd.h"
#include "cedar/auxiliaries/UIntVectorParameter.fwd.h"
#include "cedar/auxiliaries/UnitData.fwd.h"
#include "cedar/auxiliaries/UnitDataTemplate.fwd.h"
#include "cedar/auxiliaries/UnitParameterTemplate.fwd.h"
#include "cedar/auxiliaries/detail/CallFunctionInThreadWorker.fwd.h"
#include "cedar/auxiliaries/detail/LoopedThreadWorker.fwd.h"
#include "cedar/auxiliaries/detail/ThreadWorker.fwd.h"

#endif // CEDAR_AUX_NAMESPACE_H
