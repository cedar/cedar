/*=============================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Writer.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Thu 18 Aug 2011 

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_NET_WRITER_H
#define CEDAR_NET_WRITER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/Writer.fwd.h"
#include "cedar/auxiliaries/net/exceptions.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/opencv/cvMatHelper.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/std/StringHelper.h"
#include "cedar/auxiliaries/net/detail/transport/simple/SimpleNetWriter.h"
#include "cedar/auxiliaries/net/detail/transport/collated/CollatedNetWriter.h"

// PROJECT INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/static_assert.hpp>
#endif

// SYSTEM INCLUDES
#include <string>

  //---------------------------------------------------------------------------
  // the unspecialized template class cannot be instantiated
  //---------------------------------------------------------------------------

/*!@brief Provides means to transport transparently (ie it is easy to use) a datatype over the net.
 * 
 * The class will assist you in sending a datatype over
 * the network (or locally) which you can read at the other end with Reader.
 *
 * This class is a template, so you need to initialize it with the
 * datatype you want to send as a template argument. 
 * It has to be the same datatype that you will configure the Reader with, obviously.
 *
 * Short example:
 * @code
 *   Writer<cv::Mat> myWriter("userchannel");
 *   cv::Mat mat= ...;
 *   myWriter.write(mat);
 * @endcode
 *
 * Please also refer to the <a href='../../cedar/auxiliaries/net/HOWTO.NetTransporter'>HOWTO</a> for full examples.
 *
 * See also the constructor and the write() function.
 *
 * This class follows the RAII paradigm: ressource acquisition is instantiation.
 * It therefore is not copyable!
 *
 * The outgoing data is buffered in the background, so the write() function
 * will return before the data has actually left the machine.
 *
 * Valid instantiations are: Writer<char>, Writer<unsigned char>, Writer<short>,
 * Writer<unsigned short>, Writer<int>, Writer<unsigned int>, Writer<long>,
 * Writer<unsigned long>, Writer<float>, Writer<double>, Writer<bool>,
 * Writer<cv::Mat>, Writer< cv::Mat_<float>, Writer< std::string >
 * @see: Reader, NetBlockingReader
 */
template <typename T>
class cedar::aux::net::Writer
{
   //!@brief The standard constructor will not load. Do not call it.
  Writer()
  {
   // this template cannot be instantiated if there is not a
   // valid specialization

   // this will be triggered if this type is instantiated
   // and not before (due to dependency on T in the statement)
   BOOST_STATIC_ASSERT(sizeof(T) == 0); 
  }

  /*! @brief Use this constructor
   * @param userchannel is the channel (or YARP port) you want to use.
   *       The channels should be the same for reading and writing,
   *       but unique in your local network (ie task/application specific)
   */
  Writer(std::string userchannel); // just declaration for doxygen

  /*! @brief write the datatype T to the network
   *
   * @param data: the data you want to send.
   */
  void write(T data); // just the declaration for doxygen
};



namespace cedar {
  namespace aux {
    namespace net {


  //---------------------------------------------------------------------------
  // template specialization for char 
  //---------------------------------------------------------------------------

//!@brief Char specialization, see Writer for details.
template <>
class Writer<char> : public cedar::aux::net::detail::SimpleNetWriter<char>
{ 
public:
  //!@brief use this constructor. See Writer::Writer for details.
  explicit Writer(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<char>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned char 
  //---------------------------------------------------------------------------
//!@cond SKIPPED_DOCUMENTATION

//!@brief unsigned char specialization, see Writer for details.
template <>
class Writer<unsigned char> : public cedar::aux::net::detail::SimpleNetWriter<unsigned char>
{ 
public:
  //!@brief use this constructor. See Writer for details.
  explicit Writer(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<unsigned char>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for short
  //---------------------------------------------------------------------------

//!@brief short specialization, see Writer for details.
template <>
class Writer<short> : public cedar::aux::net::detail::SimpleNetWriter<short>
{ 
public:
  //!@brief use this constructor. See Writer for details.
  explicit Writer(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<short>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned short
  //---------------------------------------------------------------------------

//!@brief unsigned short specialization, see Writer for details.
template <>
class Writer<unsigned short> : public cedar::aux::net::detail::SimpleNetWriter<unsigned short>
{ 
public:
  //!@brief use this constructor. See Writer for details.
  explicit Writer(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<unsigned short>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for int
  //---------------------------------------------------------------------------

//!@brief int specialization, see Writer for details.
template <>
class Writer<int> : public cedar::aux::net::detail::SimpleNetWriter<int>
{ 
public:
  //!@brief use this constructor. See Writer for details.
  explicit Writer(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<int>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned int
  //---------------------------------------------------------------------------

//!@brief unsigned int specialization, see Writer for details.
template <>
class Writer<unsigned int> : public cedar::aux::net::detail::SimpleNetWriter<unsigned int>
{ 
public:
  //!@brief use this constructor. See Writer for details.
  explicit Writer(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<unsigned int>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for long
  //---------------------------------------------------------------------------

//!@brief long specialization, see Writer for details.
template <>
class Writer<long> : public cedar::aux::net::detail::SimpleNetWriter<long>
{ 
public:
  //!@brief use this constructor. See Writer for details.
  explicit Writer(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<long>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned long
  //---------------------------------------------------------------------------

//!@brief unsigned long specialization, see Writer for details.
template <>
class Writer<unsigned long> : public cedar::aux::net::detail::SimpleNetWriter<unsigned long>
{ 
public:
  //!@brief use this constructor. See Writer for details.
  explicit Writer(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<unsigned long>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for bool
  //---------------------------------------------------------------------------

//!@brief bool specialization, see Writer for details.
template <>
class Writer<bool> : public cedar::aux::net::detail::SimpleNetWriter<bool>
{ 
public:
  //!@brief use this constructor. See Writer for details.
  explicit Writer(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<bool>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for float
  //---------------------------------------------------------------------------

//!@brief float specialization, see Writer for details.
template <>
class Writer<float> : public cedar::aux::net::detail::SimpleNetWriter<float>
{ 
public:
  //!@brief use this constructor. See Writer for details.
  explicit Writer(const std::string &s) 
                       : cedar::aux::net::detail::SimpleNetWriter<float>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for double
  //---------------------------------------------------------------------------

//!@brief double specialization, see Writer for details.
template <>
class Writer<double> : public cedar::aux::net::detail::SimpleNetWriter<double>
{ 
public:
  //!@brief use this constructor. See Writer for details.
  explicit Writer(const std::string &s) 
                       : cedar::aux::net::detail::SimpleNetWriter<double>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for std::string
  //---------------------------------------------------------------------------

//!@brief bool specialization, see Writer for details.
template <>
class Writer<std::string> : public cedar::aux::net::detail::StringHelper<std::string>,
                            public cedar::aux::net::detail::CollatedNetWriter<std::string>
{ 
public:
  //!@brief use this constructor. See Writer for details.
  explicit Writer(const std::string &s) 
                           : cedar::aux::net::detail::CollatedNetWriter<std::string>(s)
  {
  }

private:
  bool checkCollatedDataForWrite(const cedar::aux::net::detail::CollatedTraits<std::string>::DataType &data,
                                 cedar::aux::net::detail::CollatedTraits<std::string>::HeaderType &header)
  {
    return cedar::aux::net::detail::StringHelper<std::string>::checkCollatedDataForWrite(data,header);
  }

};

  //---------------------------------------------------------------------------
  // template specialization for cv::Mat
  //---------------------------------------------------------------------------

//!@brief cv::Mat specialization, see Writer for details.
template <>
class Writer<cv::Mat> : public cedar::aux::net::detail::cvMatHelper<cv::Mat>,
                                   public cedar::aux::net::detail::CollatedNetWriter<cv::Mat> 
{
public:
  //!@brief use this constructor. See Writer for details.
  explicit Writer(const std::string &myPortName) 
                         : cedar::aux::net::detail::cvMatHelper<cv::Mat>(), 
                           cedar::aux::net::detail::CollatedNetWriter<cv::Mat>(myPortName)
  {
  }

private:
  bool checkCollatedDataForWrite(const cedar::aux::net::detail::CollatedTraits<cv::Mat>::DataType &data,
                                 cedar::aux::net::detail::CollatedTraits<cv::Mat>::HeaderType &header)
  {
    return cedar::aux::net::detail::cvMatHelper<cv::Mat>::checkCollatedDataForWrite(data,header);
  }

};

  //---------------------------------------------------------------------------
  // template specialization for cv::Mat_<float>
  //---------------------------------------------------------------------------

//!@brief cv::Mat_<float> specialization, see Writer for details.
template <>
class Writer< cv::Mat_<float> > 
                         : public cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >,
                           public cedar::aux::net::detail::CollatedNetWriter< cv::Mat_<float> > 
{
public:
  //!@brief use this constructor. See Writer for details.
  explicit Writer(const std::string &myPortName) 
                         : cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >(), 
                           cedar::aux::net::detail::CollatedNetWriter< cv::Mat_<float> >(myPortName)
  {
  }

private:
  bool checkCollatedDataForWrite(const cedar::aux::net::detail::CollatedTraits<cv::Mat_<float> >::DataType &data,
                                 cedar::aux::net::detail::CollatedTraits<cv::Mat_<float> >::HeaderType &header)
  {
    return cedar::aux::net::detail::cvMatHelper<cv::Mat_<float> >::checkCollatedDataForWrite(data,header);
  }

};


//!@endcond

} } } // end namespaces

#endif
