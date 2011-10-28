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
#include "auxiliaries/lib.h"

// PROJECT INCLUDES
#include "defines.h"

// SYSTEM INCLUDES
#include <sstream>
#include <boost/smart_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <opencv2/opencv.hpp>


namespace cedar
{
  /*!@brief Namespace for all aux classes. */
  namespace aux
  {
    CEDAR_DECLARE_AUX_CLASS(Base);
    CEDAR_DECLARE_AUX_CLASS(Configurable);
    CEDAR_DECLARE_AUX_CLASS(NamedConfigurable);
    CEDAR_DECLARE_AUX_CLASS(LoopedThread);
    CEDAR_DECLARE_AUX_CLASS(ConfigurationInterface);
    CEDAR_DECLARE_AUX_CLASS(UserData);

    template <typename T> class IntervalData;
    //!@brief smart pointer for IntervalData<double>
    typedef boost::shared_ptr<IntervalData<double> > DoubleIntervalPtr;
    //!@brief smart pointer for IntervalData<int>
    typedef boost::shared_ptr<IntervalData<int> > IntIntervalPtr;

    CEDAR_DECLARE_AUX_CLASS(LogFile);
    CEDAR_DECLARE_AUX_CLASS(Object);
    CEDAR_DECLARE_AUX_CLASS(System);

    typedef unsigned int EnumId;
    CEDAR_DECLARE_AUX_CLASS(Enum);
    CEDAR_DECLARE_AUX_CLASS(EnumBase);
    template <class T> class EnumType;

    CEDAR_DECLARE_AUX_CLASS(Arguments);

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

    CEDAR_DECLARE_AUX_CLASS_INTRUSIVE(Parameter);

    template <typename T> class ParameterTemplate;
    template <typename T> class NumericParameter;
    template <typename T> class VectorParameter;
    template <typename T> class NumericVectorParameter;
    typedef NumericParameter<double> DoubleParameter;
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(DoubleParameter);
    typedef NumericParameter<unsigned int> UIntParameter;
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(UIntParameter);
    typedef ParameterTemplate<std::string> StringParameter;
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(StringParameter);
    typedef ParameterTemplate<bool> BoolParameter;
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(BoolParameter);
    typedef VectorParameter<bool> BoolVectorParameter;
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(BoolVectorParameter);
    typedef VectorParameter<std::string> StringVectorParameter;
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(StringVectorParameter);
    typedef NumericVectorParameter<double> DoubleVectorParameter;
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(DoubleVectorParameter);
    typedef NumericVectorParameter<unsigned int> UIntVectorParameter;
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(UIntVectorParameter);
    class DirectoryParameter;
    CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(DirectoryParameter);
    CEDAR_DECLARE_AUX_CLASS_INTRUSIVE(EnumParameter);

    template <typename T> class SetParameter;
    typedef SetParameter<std::string> StringSetParameter;
    typedef boost::intrusive_ptr<StringSetParameter> StringSetParameterPtr;

    typedef boost::property_tree::ptree ConfigurationNode;


    /*!@brief Template method that converts simple data types to a string.
     *
     * @param[in] value The data value that will be converted to a string.
     */
    template<typename T>
    std::string toString(const T &value)
    {
      std::ostringstream streamOut;
      streamOut << value;
      return streamOut.str();
    }
    //!@brief class declaration of Data
    CEDAR_DECLARE_AUX_CLASS(Data);

    template <typename T> class DataTemplate;

    typedef DataTemplate<cv::Mat> MatData;
    CEDAR_GENERATE_POINTER_TYPES(MatData);

    typedef DataTemplate<cv::Point> CvPointData;
    CEDAR_GENERATE_POINTER_TYPES(CvPointData);

    typedef DataTemplate<double> DoubleData;
    CEDAR_GENERATE_POINTER_TYPES(DoubleData);

    CEDAR_DECLARE_AUX_CLASS(ImageData);

    /* exceptions */
    CEDAR_DECLARE_AUX_CLASS(DuplicateIdException);
    CEDAR_DECLARE_AUX_CLASS(DuplicateNameException);
    CEDAR_DECLARE_AUX_CLASS(InvalidNameException);
    CEDAR_DECLARE_AUX_CLASS(UnknownTypeException);
    CEDAR_DECLARE_AUX_CLASS(UnhandledTypeException);
    CEDAR_DECLARE_AUX_CLASS(UnhandledValueException);
    CEDAR_DECLARE_AUX_CLASS(UnknownTypeException);
    CEDAR_DECLARE_AUX_CLASS(ParameterNotFoundException);
    CEDAR_DECLARE_AUX_CLASS(UnknownNameException);
    CEDAR_DECLARE_AUX_CLASS(RangeException);
    CEDAR_DECLARE_AUX_CLASS(NoDefaultException);
  }
}

#endif // CEDAR_AUX_NAMESPACE_H
