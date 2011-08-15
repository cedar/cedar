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
#include "auxiliaries/macros.h"

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

    template <class T> class Factory;
    template <class T, class T2> class FactoryDerived;

    template <class T> class AbstractFactory;
    template <class T, class T2> class AbstractFactoryDerived;

    template <class KeyBaseType, class ValueBaseType> class TypeBasedFactory;

    class CEDAR_AUX_LIB_EXPORT ParameterBase;
    typedef boost::shared_ptr<ParameterBase> ParameterBasePtr;

    template <typename T> class Parameter;
    template <typename T> class NumericParameter;
    template <typename T> class VectorParameter;
    template <typename T> class NumericVectorParameter;
    typedef NumericParameter<double> DoubleParameter;
    typedef boost::shared_ptr<DoubleParameter> DoubleParameterPtr;
    typedef NumericParameter<unsigned int> UIntParameter;
    typedef boost::shared_ptr<UIntParameter> UIntParameterPtr;
    typedef Parameter<std::string> StringParameter;
    typedef boost::shared_ptr<StringParameter> StringParameterPtr;
    typedef Parameter<bool> BoolParameter;
    typedef boost::shared_ptr<BoolParameter> BoolParameterPtr;
    typedef VectorParameter<bool> BoolVectorParameter;
    typedef boost::shared_ptr<BoolVectorParameter> BoolVectorParameterPtr;
    typedef VectorParameter<std::string> StringVectorParameter;
    typedef boost::shared_ptr<StringVectorParameter> StringVectorParameterPtr;
    typedef NumericVectorParameter<double> DoubleVectorParameter;
    typedef boost::shared_ptr<DoubleVectorParameter> DoubleVectorParameterPtr;
    typedef NumericVectorParameter<unsigned int> UIntVectorParameter;
    typedef boost::shared_ptr<UIntVectorParameter> UIntVectorParameterPtr;
    class DirectoryParameter;
    typedef boost::shared_ptr<DirectoryParameter> DirectoryParameterPtr;
    CEDAR_DECLARE_AUX_CLASS(EnumParameter);

    template <typename T> class SetParameter;
    typedef SetParameter<std::string> StringSetParameter;
    typedef boost::shared_ptr<StringSetParameter> StringSetParameterPtr;

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

    template <typename T> class DataT;

    typedef DataT<cv::Mat> MatData;
    CEDAR_GENERATE_POINTER_TYPES(MatData);

    CEDAR_DECLARE_AUX_CLASS(ImageData);

    /* exceptions */
    CEDAR_DECLARE_AUX_CLASS(DuplicateNameException);
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
