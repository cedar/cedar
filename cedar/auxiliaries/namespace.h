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

// LOCAL INCLUDES
#include "cedar/auxiliaries/lib.h"

// PROJECT INCLUDES
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
    CEDAR_DECLARE_AUX_CLASS(Base);
    CEDAR_DECLARE_AUX_CLASS(Configurable);
    CEDAR_DECLARE_AUX_CLASS(NamedConfigurable);
    CEDAR_DECLARE_AUX_CLASS(LoopedThread);
    CEDAR_DECLARE_AUX_CLASS(ConfigurationInterface);
    CEDAR_DECLARE_AUX_CLASS(UserData);
    //!@endcond

    template<class T> class Singleton;

    //!@todo This class may be obsolete once the ConfigurationInterface is removed.
    template <typename T> class IntervalData;
    //!@brief smart pointer for IntervalData<double>
    typedef boost::shared_ptr<IntervalData<double> > DoubleIntervalPtr;
    //!@brief smart pointer for IntervalData<int>
    typedef boost::shared_ptr<IntervalData<int> > IntIntervalPtr;

    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_DECLARE_AUX_CLASS(LogFile);
    CEDAR_DECLARE_AUX_CLASS(MatrixIterator);
    CEDAR_DECLARE_AUX_CLASS(Object);
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

    template <class T, typename SmartPointerType> class Factory;
    template <class T, class T2, typename SmartPointerType> class FactoryDerived;

    template <class T> class AbstractFactory;
    template <class T, class T2> class AbstractFactoryDerived;

    template
    <
      class KeyBaseType,
      class ValueBaseType,
      typename KeySmartPointerType = boost::shared_ptr<KeyBaseType>,
      typename ValueSmartPointerType = boost::shared_ptr<ValueBaseType>
    >
    class TypeBasedFactory;

    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_DECLARE_AUX_CLASS_INTRUSIVE(Parameter);
    //!@endcond

    //!@brief a template class for parameters
    template <typename T> class ParameterTemplate;
    //!@brief a template class for numeric parameters, having a minimum and maximum value
    template <typename T> class NumericParameter;
    //!@brief a template class for vector parameters (of same type)
    template <typename T> class VectorParameter;
    //!@brief a template class for vector parameters (of numeric type)
    template <typename T> class NumericVectorParameter;
    //!@brief A concretization of NumericParameter for double values.
    typedef NumericParameter<double> DoubleParameter;
    //!@brief A concretization of NumericParameter for unsigned int values.
    typedef NumericParameter<unsigned int> UIntParameter;
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
    // all intrusive smart pointers
    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(DoubleParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(UIntParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(StringParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(BoolParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(BoolVectorParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(StringVectorParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(DoubleVectorParameter);
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(UIntVectorParameter);
    //!@endcond

    //!@brief a parameter storing a valid directory
    class DirectoryParameter;
    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(DirectoryParameter);
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

    //!@brief A concretization of DataTemplate for simple matrices (cv::Mat).
    typedef DataTemplate<cv::Mat> MatData;

    //!@brief A concretization of DataTemplate for simple points (cv::Point).
    typedef DataTemplate<cv::Point> CvPointData;

    //!@brief A concretization of DataTemplate for double values.
    typedef DataTemplate<double> DoubleData;

    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_GENERATE_POINTER_TYPES(MatData);
    CEDAR_GENERATE_POINTER_TYPES(CvPointData);
    CEDAR_GENERATE_POINTER_TYPES(DoubleData);
    CEDAR_DECLARE_AUX_CLASS(ImageData);
    /* exceptions */
    CEDAR_DECLARE_AUX_CLASS(ExceptionBase);
    CEDAR_DECLARE_AUX_CLASS(BadConnectionException);
    CEDAR_DECLARE_AUX_CLASS(DeadReferenceException);
    CEDAR_DECLARE_AUX_CLASS(DuplicateIdException);
    CEDAR_DECLARE_AUX_CLASS(DuplicateNameException);
    CEDAR_DECLARE_AUX_CLASS(FailedAssertionException);
    CEDAR_DECLARE_AUX_CLASS(IndexOutOfRangeException);
    CEDAR_DECLARE_AUX_CLASS(InitializationException);
    CEDAR_DECLARE_AUX_CLASS(InvalidNameException);
    CEDAR_DECLARE_AUX_CLASS(NoDefaultException);
    CEDAR_DECLARE_AUX_CLASS(NullPointerException);
    CEDAR_DECLARE_AUX_CLASS(ParameterNotFoundException);
    CEDAR_DECLARE_AUX_CLASS(RangeException);
    CEDAR_DECLARE_AUX_CLASS(TypeMismatchException);
    CEDAR_DECLARE_AUX_CLASS(UnhandledTypeException);
    CEDAR_DECLARE_AUX_CLASS(UnhandledValueException);
    CEDAR_DECLARE_AUX_CLASS(UnknownNameException);
    CEDAR_DECLARE_AUX_CLASS(UnknownTypeException);
    CEDAR_DECLARE_AUX_CLASS(UnmanglingFailedException);
    //!@endcond
  }
}

// Functions for boost intrusive pointer.
extern CEDAR_AUX_LIB_EXPORT void intrusive_ptr_add_ref(cedar::aux::Parameter *pObject);
extern CEDAR_AUX_LIB_EXPORT void intrusive_ptr_release(cedar::aux::Parameter *pObject);

#endif // CEDAR_AUX_NAMESPACE_H
