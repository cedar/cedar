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

    File:        Reader.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Thu 18 Aug 2011 

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_NET_READER_H
#define CEDAR_NET_READER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/Reader.fwd.h"
#include "cedar/auxiliaries/net/exceptions.h"
#include "cedar/auxiliaries/net/detail/transport/simple/SimpleNetReader.h"
#include "cedar/auxiliaries/net/detail/transport/collated/CollatedNetReader.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/opencv/cvMatHelper.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/opencv/cvMatHelper.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/std/StringHelper.h"

// PROJECT INCLUDES
#include <opencv2/opencv.hpp>

// SYSTEM INCLUDES
#include <string>



  //---------------------------------------------------------------------------
  // the unspecialized template class cannot be instantiated
  //---------------------------------------------------------------------------

/*!@brief Provides means to transport transparently (ie it is easy to use) a datatype over the net (non-blocking version)
 * 
 * The class will assist you in receiving a datatype that you sent over
 * the network (or locally) using the Writer.
 *
 * This class is a template, so you need to initialize it with the
 * datatype you want to receive as a template argument. 
 * It has to be the same datatype that you originally sent, obviously.
 *
 * Short example:
 * @code
 *   Reader<cv::Mat> myReader("userchannel");
 *   cv::Mat mat;
 *   mat= myReader.read(); // this will not block and will throw
 *                         // an exception if no data was waiting, see below
 * @endcode
 *
 * Please also refer to the <a href='../../cedar/auxiliaries/net/HOWTO'>HOWTO</a> for full examples.
 *
 * See also the constructor and the read() function.
 *
 * @attention This reader does not block but will throw the exception
 * cedar::aux::exc::NetUnexpectedDataException if no data was waiting
 * in the network buffer. This allows you to decide if you want to
 * abort or re-use older data in such a case. See the HOWTO for this.
 *
 * This class follows the RAII paradigm: ressource acquisition is instantiation.
 * It therefore is not copyable!
 *
 * The incoming data is buffered in the background between reads, but the
 * buffer will only always contain one element - the newest one.
 *
 * Valid instantiations are: Reader<char>, Reader<unsigned char>, Reader<short>,
 * Reader<unsigned short>, Reader<int>, Reader<unsigned int>, Reader<long>,
 * Reader<unsigned long>, Reader<float>, Reader<double>, Reader<bool>,
 * Reader<cv::Mat>, Reader< cv::Mat_<float>, Reader< std::string >
 * @see: BlockingReader, Writer
 */
template <typename T>
class cedar::aux::net::Reader
{
   //!@brief The standard constructor will not load. Do not call it.
   Reader()
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
   Reader(std::string userchannel); // just declaration for doxygen

   /*! @brief read() the datatype T from the network
    *
    * Note that in the Reader the read() operation will not(!) block
    * when no data was waiting in the network buffer, which means that
    * your program will not suffer from additional delays.
    * In that case read() will throw a cedar::aux::exc::NetUnexpectedDataException that you can catch and react to
    * accordingly.
    *
    * @return returns an instance of the specified datatype T, which you
    *         sent over the network.
   */
   T read(); // just the declaration for doxygen
   
};

namespace cedar {
  namespace aux {
    namespace net {

  //---------------------------------------------------------------------------
  // template specialization for char 
  //---------------------------------------------------------------------------
//!@cond SKIPPED_DOCUMENTATION


//!@brief Char specialization, see Reader for details.
template <>
class Reader<char> : public cedar::aux::net::detail::SimpleNetReader<char, false>
{ 
public:
  //!@brief use this constructor. See Reader::Reader for details.
  explicit Reader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<char, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned char 
  //---------------------------------------------------------------------------

//!@brief unsigned char specialization, see Reader for details.
template <>
class Reader<unsigned char> : public cedar::aux::net::detail::SimpleNetReader<unsigned char, false>
{ 
public:
  //!@brief use this constructor. See Reader for details.
  explicit Reader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned char, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for short
  //---------------------------------------------------------------------------

//!@brief short specialization, see Reader for details.
template <>
class Reader<short> : public cedar::aux::net::detail::SimpleNetReader<short, false>
{ 
public:
  //!@brief use this constructor. See Reader for details.
  explicit Reader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<short, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned short
  //---------------------------------------------------------------------------

//!@brief unsigned short specialization, see Reader for details.
template <>
class Reader<unsigned short> : public cedar::aux::net::detail::SimpleNetReader<unsigned short, false>
{ 
public:
  //!@brief use this constructor. See Reader for details.
  explicit Reader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned short, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for int
  //---------------------------------------------------------------------------

//!@brief int specialization, see Reader for details.
template <>
class Reader<int> : public cedar::aux::net::detail::SimpleNetReader<int, false>
{ 
public:
  //!@brief use this constructor. See Reader for details.
  explicit Reader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<int, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned int
  //---------------------------------------------------------------------------

//!@brief unsigned int specialization, see Reader for details.
template <>
class Reader<unsigned int> : public cedar::aux::net::detail::SimpleNetReader<unsigned int, false>
{ 
public:
  //!@brief use this constructor. See Reader for details.
  explicit Reader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned int, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for long
  //---------------------------------------------------------------------------

//!@brief long specialization, see Reader for details.
template <>
class Reader<long> : public cedar::aux::net::detail::SimpleNetReader<long, false>
{ 
public:
  //!@brief use this constructor. See Reader for details.
  explicit Reader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<long, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned long
  //---------------------------------------------------------------------------

//!@brief unsigned long specialization, see Reader for details.
template <>
class Reader<unsigned long> : public cedar::aux::net::detail::SimpleNetReader<unsigned long, false>
{ 
public:
  //!@brief use this constructor. See Reader for details.
  explicit Reader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned long, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for bool
  //---------------------------------------------------------------------------

//!@brief bool specialization, see Reader for details.
template <>
class Reader<bool> : public cedar::aux::net::detail::SimpleNetReader<bool, false>
{ 
public:
  //!@brief use this constructor. See Reader for details.
  explicit Reader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<bool, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for float
  //---------------------------------------------------------------------------

//!@brief float specialization, see Reader for details.
template <>
class Reader<float> : public cedar::aux::net::detail::SimpleNetReader<float, false>
{ 
public:
  //!@brief use this constructor. See Reader for details.
  explicit Reader(const std::string &s) 
                       : cedar::aux::net::detail::SimpleNetReader<float, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for double
  //---------------------------------------------------------------------------

//!@brief double specialization, see Reader for details.
template <>
class Reader<double> : public cedar::aux::net::detail::SimpleNetReader<double, false>
{ 
public:
  //!@brief use this constructor. See Reader for details.
  explicit Reader(const std::string &s) 
                       : cedar::aux::net::detail::SimpleNetReader<double, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for std::string
  //---------------------------------------------------------------------------

//!@brief double specialization, see Reader for details.
template <>
class Reader<std::string> : public cedar::aux::net::detail::StringHelper<std::string>,
                            public cedar::aux::net::detail::CollatedNetReader<std::string>
{ 
public:
  //!@brief use this constructor. See Reader for details.
  explicit Reader(const std::string &s) 
                       : cedar::aux::net::detail::CollatedNetReader<std::string, false>(s)
  {
  }

private:
  bool checkCollatedDataForRead(const cedar::aux::net::detail::CollatedTraits<std::string>::HeaderType &header)
  {
    return cedar::aux::net::detail::StringHelper<std::string>::checkCollatedDataForRead(header);
  }

};

  //---------------------------------------------------------------------------
  // template specialization for cv::Mat
  //---------------------------------------------------------------------------

//!@brief cv::Mat specialization, see Reader for details.
template <>
class Reader<cv::Mat> : public cedar::aux::net::detail::cvMatHelper<cv::Mat>,
                                   public cedar::aux::net::detail::CollatedNetReader<cv::Mat, false> 
{
public:
  //!@brief use this constructor. See Reader for details.
  explicit Reader(const std::string &myPortName) 
                         : cedar::aux::net::detail::cvMatHelper<cv::Mat>(), 
                           cedar::aux::net::detail::CollatedNetReader<cv::Mat, false>(myPortName)
  {
  }

private:
  bool checkCollatedDataForRead(const cedar::aux::net::detail::CollatedTraits<cv::Mat>::HeaderType &header)
  {
    return cedar::aux::net::detail::cvMatHelper<cv::Mat>::checkCollatedDataForRead(header);
  }

};

  //---------------------------------------------------------------------------
  // template specialization for cv::Mat_<float>
  //---------------------------------------------------------------------------

//!@brief cv::Mat_<float> specialization, see Reader for details.
template <>
class Reader< cv::Mat_<float> > 
                         : public cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >,
                           public cedar::aux::net::detail::CollatedNetReader< cv::Mat_<float> , false> 
{
public:
  //!@brief use this constructor. See Reader for details.
  explicit Reader(const std::string &myPortName) 
                         : cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >(), 
                           cedar::aux::net::detail::CollatedNetReader< cv::Mat_<float> , false>(myPortName)
  {
  }

private:
  bool checkCollatedDataForRead(const cedar::aux::net::detail::CollatedTraits<cv::Mat_<float> >::HeaderType &header)
  {
    return cedar::aux::net::detail::cvMatHelper<cv::Mat_<float> >::checkCollatedDataForRead(header);
  }

};

//!@endcond
} } } // end namespaces

#endif

