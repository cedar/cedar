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

    File:        BlockingReader.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Thu 18 Aug 2011

    Description: the blocking version of the Reader

    Credits:

=============================================================================*/

#ifndef CEDAR_NET_BLOCKING_READER_H
#define CEDAR_NET_BLOCKING_READER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/BlockingReader.fwd.h"
#include "cedar/auxiliaries/net/exceptions.h"
#include "cedar/auxiliaries/net/detail/transport/simple/SimpleNetReader.h"
#include "cedar/auxiliaries/net/detail/transport/collated/CollatedNetReader.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/opencv/cvMatHelper.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/opencv/cvMatHelper.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/std/StringHelper.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <string>


  //---------------------------------------------------------------------------
  // the unspecialized template class cannot be instantiated
  //---------------------------------------------------------------------------

/*!@brief Provides means to transport transparently (ie it is easy to use) a datatype over the net (blocking version)
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
 *   BlockingReader<cv::Mat> myReader("userchannel");
 *   cv::Mat mat;
 *   mat= myReader.read(); // this will block
 * @endcode
 *
 * Please also refer to the <a href='../../cedar/auxiliaries/net/HOWTO'>HOWTO</a> for full examples.
 *
 * See also the constructor and the read() function.
 *
 * @attention This version of the Reader is blocking, which means that the
 * read() operation will only return when there actually was
 * valid transmission received. For the non-blocking version, see Reader.
 * If data was already waiting (see below) the read() will not block and
 * return immediately.
 *
 * This class follows the RAII paradigm: ressource acquisition is instantiation.
 * It therefore is not copyable!
 *
 * The incoming data is buffered in the background between reads, but the
 * buffer will only always contain one element - the newest one.
 *
 * Valid instantiations are: BlockingReader<char>, BlockingReader<unsigned char>, BlockingReader<short>,
 * BlockingReader<unsigned short>, BlockingReader<int>, BlockingReader<unsigned int>, BlockingReader<long>,
 * BlockingReader<unsigned long>, BlockingReader<float>, BlockingReader<double>, BlockingReader<bool>,
 * BlockingReader<cv::Mat>, BlockingReader< cv::Mat_<float>, BlockingReader< std::string >
 * @see: Reader, Writer
 */
template <typename T>
class cedar::aux::net::BlockingReader
{
   //!@brief The standard constructor will not load. Do not call it.
   BlockingReader()
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
   BlockingReader(std::string userchannel); // just declaration for doxygen

   /*! @brief read() the datatype T from the network
    *
    * Note that in the BlockingReader the read() operation *will* block,
    * which means it will only return, when it has received valid
    * data over the network.
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

//!@brief Char specialization, see BlockingReader for details.
template <>
class BlockingReader<char> : public cedar::aux::net::detail::SimpleNetReader<char, true>
{ 
public:
  //!@brief use this constructor. See BlockingReader::BlockingReader for details.
  explicit BlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<char, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned char 
  //---------------------------------------------------------------------------

//!@brief unsigned char specialization, see BlockingReader for details.
template <>
class BlockingReader<unsigned char> : public cedar::aux::net::detail::SimpleNetReader<unsigned char, true>
{ 
public:
  //!@brief use this constructor. See BlockingReader for details.
  explicit BlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned char, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for short
  //---------------------------------------------------------------------------

//!@brief short specialization, see BlockingReader for details.
template <>
class BlockingReader<short> : public cedar::aux::net::detail::SimpleNetReader<short, true>
{ 
public:
  //!@brief use this constructor. See BlockingReader for details.
  explicit BlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<short, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned short
  //---------------------------------------------------------------------------

//!@brief unsigned short specialization, see BlockingReader for details.
template <>
class BlockingReader<unsigned short> : public cedar::aux::net::detail::SimpleNetReader<unsigned short, true>
{ 
public:
  //!@brief use this constructor. See BlockingReader for details.
  explicit BlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned short, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for int
  //---------------------------------------------------------------------------

//!@brief int specialization, see BlockingReader for details.
template <>
class BlockingReader<int> : public cedar::aux::net::detail::SimpleNetReader<int, true>
{ 
public:
  //!@brief use this constructor. See BlockingReader for details.
  explicit BlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<int, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned int
  //---------------------------------------------------------------------------

//!@brief unsigned int specialization, see BlockingReader for details.
template <>
class BlockingReader<unsigned int> : public cedar::aux::net::detail::SimpleNetReader<unsigned int, true>
{ 
public:
  //!@brief use this constructor. See BlockingReader for details.
  explicit BlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned int, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for long
  //---------------------------------------------------------------------------

//!@brief long specialization, see BlockingReader for details.
template <>
class BlockingReader<long> : public cedar::aux::net::detail::SimpleNetReader<long, true>
{ 
public:
  //!@brief use this constructor. See BlockingReader for details.
  explicit BlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<long, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned long
  //---------------------------------------------------------------------------

//!@brief unsigned long specialization, see BlockingReader for details.
template <>
class BlockingReader<unsigned long> : public cedar::aux::net::detail::SimpleNetReader<unsigned long, true>
{ 
public:
  //!@brief use this constructor. See BlockingReader for details.
  explicit BlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned long, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for bool
  //---------------------------------------------------------------------------

//!@brief bool specialization, see BlockingReader for details.
template <>
class BlockingReader<bool> : public cedar::aux::net::detail::SimpleNetReader<bool, true>
{ 
public:
  //!@brief use this constructor. See BlockingReader for details.
  explicit BlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<bool, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for float
  //---------------------------------------------------------------------------

//!@brief float specialization, see BlockingReader for details.
template <>
class BlockingReader<float> : public cedar::aux::net::detail::SimpleNetReader<float, true>
{ 
public:
  //!@brief use this constructor. See BlockingReader for details.
  explicit BlockingReader(const std::string &s) 
                       : cedar::aux::net::detail::SimpleNetReader<float, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for std::string
  //---------------------------------------------------------------------------

//!@brief std::string specialization, see BlockingReader for details.
template <>
class BlockingReader<std::string> : public cedar::aux::net::detail::StringHelper<std::string>,
                            public cedar::aux::net::detail::CollatedNetReader<std::string, true>
{ 
public:
  //!@brief use this constructor. See BlockingReader for details.
  explicit BlockingReader(const std::string &s) 
                       : cedar::aux::net::detail::CollatedNetReader<std::string, true>(s)
  {
  }

private:
  bool checkCollatedDataForRead(const cedar::aux::net::detail::CollatedTraits<std::string>::HeaderType &header)
  {
    return cedar::aux::net::detail::StringHelper<std::string>::checkCollatedDataForRead(header);
  }

};

  //---------------------------------------------------------------------------
  // template specialization for double
  //---------------------------------------------------------------------------

//!@brief double specialization, see BlockingReader for details.
template <>
class BlockingReader<double> : public cedar::aux::net::detail::SimpleNetReader<double, true>
{ 
public:
  //!@brief use this constructor. See BlockingReader for details.
  explicit BlockingReader(const std::string &s) 
                       : cedar::aux::net::detail::SimpleNetReader<double, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for cv::Mat
  //---------------------------------------------------------------------------

//!@brief cv::Mat specialization, see BlockingReader for details.
template <>
class BlockingReader<cv::Mat> : public cedar::aux::net::detail::cvMatHelper<cv::Mat>,
                                   public cedar::aux::net::detail::CollatedNetReader<cv::Mat, true> 
{
public:
  //!@brief use this constructor. See BlockingReader for details.
  explicit BlockingReader(const std::string &myPortName) 
                         : cedar::aux::net::detail::cvMatHelper<cv::Mat>(), 
                           cedar::aux::net::detail::CollatedNetReader<cv::Mat, true>(myPortName)
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

//!@brief cv::Mat_<float> specialization, see BlockingReader for details.
template <>
class BlockingReader< cv::Mat_<float> > 
                         : public cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >,
                           public cedar::aux::net::detail::CollatedNetReader< cv::Mat_<float> , true>
{
public:
  //!@brief use this constructor. See BlockingReader for details.
  explicit BlockingReader(const std::string &myPortName) 
                         : cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >(), 
                           cedar::aux::net::detail::CollatedNetReader< cv::Mat_<float> , true>(myPortName)
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
