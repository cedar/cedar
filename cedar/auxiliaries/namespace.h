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
#include <boost/smart_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <opencv2/opencv.hpp>


namespace cedar
{
  /*!@brief Namespace for all aux classes. */
  namespace aux
  {
    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_DECLARE_AUX_CLASS(Configurable);
    CEDAR_DECLARE_AUX_CLASS(NamedConfigurable);
    CEDAR_DECLARE_AUX_CLASS(Lockable);
    CEDAR_DECLARE_AUX_CLASS(LoopedThread);
    CEDAR_DECLARE_AUX_CLASS(ConfigurationInterface);
    CEDAR_DECLARE_AUX_CLASS(UserData);
    CEDAR_DECLARE_AUX_CLASS_INTRUSIVE(IntrusivePtrBase);
    //!@endcond

    template <class T> class Singleton;

    template <typename T> class MovingAverage;

    template <class ReturnedT> class CloneableBase;

    template <class ClonedT, class ReturnedT> class Cloneable;

    //!@todo This class may be obsolete once the ConfigurationInterface is removed.
    template <typename T> class IntervalData;
    //!@brief smart pointer for IntervalData<double>
    typedef boost::shared_ptr<IntervalData<double> > DoubleIntervalPtr;
    //!@brief smart pointer for IntervalData<int>
    typedef boost::shared_ptr<IntervalData<int> > IntIntervalPtr;

    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_DECLARE_AUX_CLASS(LogFile);
    CEDAR_DECLARE_AUX_CLASS(MatrixIterator);
    CEDAR_DECLARE_AUX_CLASS(LocalCoordinateFrame);
    CEDAR_DECLARE_DEPRECATED(typedef LocalCoordinateFrame Object);
    CEDAR_DECLARE_DEPRECATED(typedef LocalCoordinateFrame RigidBody);
    CEDAR_DECLARE_AUX_CLASS(System);
    //!@endcond

    //!@brief A type for identifying enum entries. Corresponds to the C++ enum int values for each enum entry.
    typedef unsigned int EnumId;
    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_DECLARE_AUX_CLASS(Enum);
    CEDAR_DECLARE_AUX_CLASS(EnumBase);
    //!@endcond
    template <class T> class EnumType;

    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_DECLARE_AUX_CLASS(Arguments);
    //!@endcond

    template <class BaseTypePtr> class Factory;
    template <class BaseTypePtr, class DerivedTypePtr> class FactoryDerived;
    template <class BaseTypePtr> class FactoryManager;

    template <typename KeyBasePtr, typename ValueBasePtr> class TypeBasedFactory;

    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_DECLARE_AUX_CLASS_INTRUSIVE(Parameter);
    CEDAR_DECLARE_AUX_CLASS_INTRUSIVE(ObjectParameter);
    CEDAR_DECLARE_AUX_CLASS_INTRUSIVE(ObjectListParameter);
    //!@endcond

    //!@brief a template class for parameters
    template <typename T> class ParameterTemplate;
    //!@brief a template class for numeric parameters, having a minimum and maximum value
    template <typename T> class NumericParameter;
    //!@brief a template class for vector parameters (of same type)
    template <typename T> class VectorParameter;
    //!@brief a template class for vector parameters (of numeric type)
    template <typename T> class NumericVectorParameter;
    //!@brief a template class for storing objects that are allocated dynamically.
    template <typename T> class ObjectParameterTemplate;
    //!@brief a template class for lists of objects of arbitrary type
    template <typename T> class ObjectListParameterTemplate;
    //!@brief a template class for maps of objects of arbitrary type
    template <typename T> class ObjectMapParameterTemplate;
    //!@brief A concretization of NumericParameter for double values.
    typedef NumericParameter<double> DoubleParameter;
    //!@brief A concretization of NumericParameter for unsigned int values.
    typedef NumericParameter<unsigned int> UIntParameter;
    //!@brief A concretization of NumericParameter for int values.
    typedef NumericParameter<int> IntParameter;
    //!@brief A concretization of ParameterTemplate for strings.
    typedef ParameterTemplate<std::string> StringParameter;
    //!@brief A concretization of ParameterTemplate for booleans.
    typedef ParameterTemplate<bool> BoolParameter;
    //!@brief A concretization of VectorParameter for booleans.
    typedef VectorParameter<bool> BoolVectorParameter;
    //!@brief A concretization of VectorParameter for strings.
    typedef VectorParameter<std::string> StringVectorParameter;
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
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(BoolParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(BoolVectorParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(DoubleParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(DoubleVectorParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(IntParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(IntVectorParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(StringParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(StringVectorParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(UIntParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(UIntVectorParameter);
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

    //!@brief a better name for boost's property tree
    typedef boost::property_tree::ptree ConfigurationNode;

    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_DECLARE_AUX_CLASS(Data);
    //!@endcond

    //!@brief a templated version of cedar::aux::Data
    template <typename T> class DataTemplate;

    //!@brief A concretization of DataTemplate for simple points (cv::Point).
    typedef DataTemplate<cv::Point> CvPointData;

    //!@brief A concretization of DataTemplate for double values.
    typedef DataTemplate<double> DoubleData;

    //!@brief A concretization of DataTemplate for a set of simple matrices (std::vector<cv::Mat>).
    typedef DataTemplate<std::vector<cv::Mat> > ImageSetData;

    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_DECLARE_AUX_CLASS(MatData);
    CEDAR_GENERATE_POINTER_TYPES(CvPointData);
    CEDAR_GENERATE_POINTER_TYPES(DoubleData);
    CEDAR_GENERATE_POINTER_TYPES(ImageSetData);
    CEDAR_DECLARE_AUX_CLASS(ImageData);
    CEDAR_DECLARE_AUX_CLASS(StereoImageData);
    /* exceptions */
    CEDAR_DECLARE_AUX_CLASS(BadConnectionException);
    CEDAR_DECLARE_AUX_CLASS(ConversionFailedException);
    CEDAR_DECLARE_AUX_CLASS(DeadReferenceException);
    CEDAR_DECLARE_AUX_CLASS(DuplicateIdException);
    CEDAR_DECLARE_AUX_CLASS(DuplicateNameException);
    CEDAR_DECLARE_AUX_CLASS(ExceptionBase);
    CEDAR_DECLARE_AUX_CLASS(FileNotFoundException);
    CEDAR_DECLARE_AUX_CLASS(FailedAssertionException);
    CEDAR_DECLARE_AUX_CLASS(IndexOutOfRangeException);
    CEDAR_DECLARE_AUX_CLASS(InitializationException);
    CEDAR_DECLARE_AUX_CLASS(InvalidNameException);
    CEDAR_DECLARE_AUX_CLASS(MatrixMismatchException);
    CEDAR_DECLARE_AUX_CLASS(NoDefaultException);
    CEDAR_DECLARE_AUX_CLASS(NotFoundException);
    CEDAR_DECLARE_AUX_CLASS(NullPointerException);
    CEDAR_DECLARE_AUX_CLASS(ParameterNotFoundException);
    CEDAR_DECLARE_AUX_CLASS(RangeException);
    CEDAR_DECLARE_AUX_CLASS(ResourceNotFoundException);
    CEDAR_DECLARE_AUX_CLASS(TypeMismatchException);
    CEDAR_DECLARE_AUX_CLASS(UnhandledTypeException);
    CEDAR_DECLARE_AUX_CLASS(UnhandledValueException);
    CEDAR_DECLARE_AUX_CLASS(UnknownNameException);
    CEDAR_DECLARE_AUX_CLASS(UnknownTypeException);
    CEDAR_DECLARE_AUX_CLASS(UnmanglingFailedException);
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
