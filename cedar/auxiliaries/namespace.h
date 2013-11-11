/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/units/AngularVelocity.h"
#include "cedar/units/Velocity.h"
#include "cedar/defines.h"

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>
#include <opencv2/opencv.hpp>

//#warning Do not include this header any more. Use the new forward declaration headers instead!

//#include "cedar/auxiliaries/BoolParameter.fwd.h"
//#include "cedar/auxiliaries/BoolVectorParameter.fwd.h"
//#include "cedar/auxiliaries/CallFunctionInThread.fwd.h"
//#include "cedar/auxiliaries/Cloneable.fwd.h"
//#include "cedar/auxiliaries/CommandLineParser.fwd.h"
//#include "cedar/auxiliaries/Configurable.fwd.h"
//#include "cedar/auxiliaries/Data.fwd.h"
//#include "cedar/auxiliaries/DataTemplate.fwd.h"
//#include "cedar/auxiliaries/DataSpectator.fwd.h"
//#include "cedar/auxiliaries/DeclarationManagerTemplate.fwd.h"
//#include "cedar/auxiliaries/Enum.fwd.h"
//#include "cedar/auxiliaries/EnumBase.fwd.h"
//#include "cedar/auxiliaries/EnumType.fwd.h"
//#include "cedar/auxiliaries/GraphTemplate.fwd.h"
//#include "cedar/auxiliaries/Factory.fwd.h"
//#include "cedar/auxiliaries/FactoryDerived.fwd.h"
//#include "cedar/auxiliaries/FactoryManager.fwd.h"
//#include "cedar/auxiliaries/IntrusivePtrBase.fwd.h"
//#include "cedar/auxiliaries/ImageSetData.fwd.h"
//#include "cedar/auxiliaries/LocalCoordinateFrame.fwd.h"
//#include "cedar/auxiliaries/Lockable.fwd.h"
//#include "cedar/auxiliaries/LogFile.fwd.h"
//#include "cedar/auxiliaries/LoopedThread.fwd.h"
//#include "cedar/auxiliaries/LoopFunctionInThread.fwd.h"
//#include "cedar/auxiliaries/LoopMode.fwd.h"
//#include "cedar/auxiliaries/MatrixIterator.fwd.h"
//#include "cedar/auxiliaries/NamedConfigurable.fwd.h"
//#include "cedar/auxiliaries/ObjectListParameter.fwd.h"
//#include "cedar/auxiliaries/ObjectMapParameterTemplate.fwd.h"
//#include "cedar/auxiliaries/ObjectParameter.fwd.h"
//#include "cedar/auxiliaries/ObjectParameterTemplate.fwd.h"
//#include "cedar/auxiliaries/Parameter.fwd.h"
//#include "cedar/auxiliaries/ParameterTemplate.fwd.h"
//#include "cedar/auxiliaries/PluginDeclaration.fwd.h"
//#include "cedar/auxiliaries/PluginDeclarationList.fwd.h"
//#include "cedar/auxiliaries/PluginDeclarationTemplate.fwd.h"
//#include "cedar/auxiliaries/PluginProxy.fwd.h"
//#include "cedar/auxiliaries/Recorder.fwd.h"
//#include "cedar/auxiliaries/Settings.fwd.h"
//#include "cedar/auxiliaries/Singleton.fwd.h"
//#include "cedar/auxiliaries/StringParameter.fwd.h"
//#include "cedar/auxiliaries/StringVectorParameter.fwd.h"
//#include "cedar/auxiliaries/ThreadCollection.fwd.h"
//#include "cedar/auxiliaries/ThreadWrapper.fwd.h"
//#include "cedar/auxiliaries/TypeBasedFactory.fwd.h"
//#include "cedar/auxiliaries/detail/CallFunctionInThreadWorker.fwd.h"
//#include "cedar/auxiliaries/detail/LoopedThreadWorker.fwd.h"
//#include "cedar/auxiliaries/detail/ThreadWorker.fwd.h"

namespace cedar
{
  /*!@brief Namespace for all aux classes. */
  namespace aux
  {
    //!@brief a template class for numeric parameters, having a minimum and maximum value
    template <typename T> class NumericParameter;
    //!@brief A template class for parameters holding boost::units.
    template <typename T> class UnitParameterTemplate;
    //!@brief a template class for vector parameters (of numeric type)
    template <typename T> class NumericVectorParameter;
    //!@brief a template class for map parameters (of primitive type)
    template <typename T> class MapParameter;
    //!@brief a template class for lists of objects of arbitrary type
    template <typename T> class ObjectListParameterTemplate;
    //!@brief A concretization of NumericParameter for double values.
    typedef NumericParameter<double> DoubleParameter;
    //!@brief A concretization of NumericParameter for unsigned int values.
    typedef NumericParameter<unsigned int> UIntParameter;
    //!@brief A concretization of NumericParameter for int values.
    typedef NumericParameter<int> IntParameter;
    //!@brief A concretization of NumericVectorParameter for double values.
    typedef NumericVectorParameter<double> DoubleVectorParameter;
    //!@brief A concretization of NumericVectorParameter for unsigned int values.
    typedef NumericVectorParameter<unsigned int> UIntVectorParameter;
    //!@brief A concretization of NumericVectorParameter for unsigned int values.
    typedef NumericVectorParameter<int> IntVectorParameter;

    //!@brief A class for associating data with types and mapping the type hierarchy as well.
    template <typename DataType, typename RootType> class TypeHierarchyMap;

    // all intrusive smart pointers
    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(DoubleParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(DoubleVectorParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(IntParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(IntVectorParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(UIntParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(UIntVectorParameter);

    typedef MapParameter<std::string> StringMapParameter;
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(StringMapParameter);
    //!@endcond

    //!@brief a parameter storing a valid directory
    class DirectoryParameter;
    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(DirectoryParameter);
    CEDAR_DECLARE_AUX_CLASS_INTRUSIVE(FileParameter);
    CEDAR_DECLARE_AUX_CLASS_INTRUSIVE(EnumParameter);
    //!@endcond

    //!@brief a parameter containing a set of type T
    template <typename T> class SetParameter;
    //!@brief a parameter containing a set of type string
    typedef SetParameter<std::string> StringSetParameter;
    //!@brief an intrusive pointer to a parameter containing a set of type string
    typedef boost::intrusive_ptr<StringSetParameter> StringSetParameterPtr;

    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_DECLARE_AUX_CLASS(UnitData);
    //!@endcond

    //!@brief a templated version of cedar::aux::UnitData
    template <typename T> class UnitDataTemplate;

    typedef cedar::aux::UnitDataTemplate<cedar::unit::AngularVelocity> AngularVelocityData;
    CEDAR_GENERATE_POINTER_TYPES(AngularVelocityData);

    typedef cedar::aux::UnitDataTemplate<cedar::unit::Velocity> VelocityData;
    CEDAR_GENERATE_POINTER_TYPES(VelocityData);

    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_DECLARE_AUX_CLASS(DoubleData);
    CEDAR_DECLARE_AUX_CLASS(ImageData);
    CEDAR_DECLARE_AUX_CLASS(StereoImageData);
    /* exceptions */
    CEDAR_DECLARE_AUX_CLASS(AnnotationNotFoundException);
    CEDAR_DECLARE_AUX_CLASS(BadConnectionException);
    CEDAR_DECLARE_AUX_CLASS(ConversionFailedException);
    CEDAR_DECLARE_AUX_CLASS(DeadReferenceException);
    CEDAR_DECLARE_AUX_CLASS(DimensionalityMismatchException);
    CEDAR_DECLARE_AUX_CLASS(DuplicateIdException);
    CEDAR_DECLARE_AUX_CLASS(DuplicateNameException);
    CEDAR_DECLARE_AUX_CLASS(DuplicateChannelNameException);
    CEDAR_DECLARE_AUX_CLASS(ExceptionBase);
    CEDAR_DECLARE_AUX_CLASS(FileNotFoundException);
    CEDAR_DECLARE_AUX_CLASS(FailedAssertionException);
    CEDAR_DECLARE_AUX_CLASS(IndexOutOfRangeException);
    CEDAR_DECLARE_AUX_CLASS(InitializationException);
    CEDAR_DECLARE_AUX_CLASS(InvalidNameException);
    CEDAR_DECLARE_AUX_CLASS(InvalidPathException);
    CEDAR_DECLARE_AUX_CLASS(MalformedConfigurationTreeException);
    CEDAR_DECLARE_AUX_CLASS(MatrixMismatchException);
    CEDAR_DECLARE_AUX_CLASS(NoDefaultException);
    CEDAR_DECLARE_AUX_CLASS(NotFoundException);
    CEDAR_DECLARE_AUX_CLASS(NotImplementedException);
    CEDAR_DECLARE_AUX_CLASS(NullPointerException);
    CEDAR_DECLARE_AUX_CLASS(ParameterNotFoundException);
    CEDAR_DECLARE_AUX_CLASS(ParseException);
    CEDAR_DECLARE_AUX_CLASS(PluginException);
    CEDAR_DECLARE_AUX_CLASS(PluginNotFoundException);
    CEDAR_DECLARE_AUX_CLASS(RangeException);
    CEDAR_DECLARE_AUX_CLASS(ResourceNotFoundException);
    CEDAR_DECLARE_AUX_CLASS(TypeMismatchException);
    CEDAR_DECLARE_AUX_CLASS(UnhandledTypeException);
    CEDAR_DECLARE_AUX_CLASS(UnhandledValueException);
    CEDAR_DECLARE_AUX_CLASS(UnknownNameException);
    CEDAR_DECLARE_AUX_CLASS(UnknownTypeException);
    CEDAR_DECLARE_AUX_CLASS(UnknownUnitSuffixException);
    CEDAR_DECLARE_AUX_CLASS(UnmanglingFailedException);
    CEDAR_DECLARE_AUX_CLASS(ValidationFailedException);
    CEDAR_DECLARE_AUX_CLASS(ThreadingErrorException);
    CEDAR_DECLARE_AUX_CLASS(NotImplementedException);
    CEDAR_DECLARE_AUX_CLASS(ThreadRunningExeption);
    //!@endcond
    
    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_DECLARE_AUX_CLASS(Grabbable);
    CEDAR_DECLARE_AUX_CLASS(Path);
    //!@endcond

    // Log related classes --------------------------------------------------------------------------------------------
    
    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_DECLARE_AUX_CLASS(Log);
    CEDAR_DECLARE_AUX_CLASS(LogInterface);
    CEDAR_DECLARE_AUX_CLASS(LogFilter);
    CEDAR_DECLARE_AUX_CLASS(ConsoleLog);
    CEDAR_DECLARE_AUX_CLASS(NullLogger);
    //!@endcond
    
    //!@brief Enumeration that defines different log levels.
    enum LOG_LEVEL
    {
      //! System information. For example, this could be to inform the user of automatically determined constants/values.
      LOG_LEVEL_SYSTEM_INFO,
      //! A normal message.
      LOG_LEVEL_MESSAGE,
      //! A warning.
      LOG_LEVEL_WARNING,
      //! An error.
      LOG_LEVEL_ERROR,
      //! A debug message. Will only be sent in debug builds!
      LOG_LEVEL_DEBUG,
      //! A debug message concerned with memory allocation. Will only be sent in debug builds!
      LOG_LEVEL_MEM_DEBUG
    };
  }
}

#endif // CEDAR_AUX_NAMESPACE_H
