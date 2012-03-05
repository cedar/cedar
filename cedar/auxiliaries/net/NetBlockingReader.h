/*=============================================================================

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

===============================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        NetBlockingReader.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Thu 18 Aug 2011 11:39:23 AM CEST

    Description: the blocking version of the NetReader

    Credits:

=============================================================================*/

#ifndef CEDAR_NETBLOCKINGREADER_H
#define CEDAR_NETBLOCKINGREADER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/namespace.h"
#include "cedar/auxiliaries/net/detail/transport/simple/SimpleNetReader.h"
#include "cedar/auxiliaries/net/detail/transport/collated/CollatedNetReader.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/opencv/cvMatHelper.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


namespace cedar {
  namespace aux {
    namespace net {

  //---------------------------------------------------------------------------
  // the unspecialized template class cannot be instantiated
  //---------------------------------------------------------------------------

/*!@brief Provides means to transport transparently (ie it is easy to use) a datatype over the net (blocking version)
 * 
 * The class will assist you in receiving a datatype that you sent over
 * the network (or locally) using the NetWriter.
 *
 * This class is a template, so you need to initialize it with the
 * datatype you want to receive as a template argument. 
 * It has to be the same datatype that you originally sent, obviously.
 *
 * Short example:
 * @code
 *   NetBlockingReader<cv::Mat> myReader("mychannel");
 *   cv::Mat mat;
 *   mat= myReader.read(); // this will block
 * @endcode
 *
 * Please also refer to the <a href='../../cedar/auxiliaries/net/HOWTO'>HOWTO</a> for full examples.
 *
 * See also the constructor and the read() function.
 *
 * @attention This version of the NetReader is blocking, which means that the
 * read() operation will only return when there actually was
 * valid transmission received. For the non-blocking version, see NetReader.
 * If data was already waiting (see below) the read() will not block and
 * return immediately.
 *
 * This class follows the RAII paradigm: ressource acquisition is instantiation.
 * It therefore is not copyable!
 *
 * The incoming data is buffered in the background between reads, but the
 * buffer will only always contain one element - the newest one.
 *
 * Valid instantiations are: NetBlockingReader<char>, NetBlockingReader<unsigned char>, NetBlockingReader<short>,
 * NetBlockingReader<unsigned short>, NetBlockingReader<int>, NetBlockingReader<unsigned int>, NetBlockingReader<long>,
 * NetBlockingReader<unsigned long>, NetBlockingReader<float>, NetBlockingReader<double>, NetBlockingReader<bool>,
 * NetBlockingReader<cv::Mat>, NetBlockingReader< cv::Mat_<float>  
 * @see: NetReader, NetWriter
 */
template <typename T>
class NetBlockingReader
{
   //!@brief The standard constructor will not load. Do not call it.
   NetBlockingReader()
   {
     // this template cannot be instantiated if there is not a
     // valid specialization

     // this will be triggered if this type is instantiated
     BOOST_STATIC_ASSERT(sizeof(T) == 0); 
   }

   /*! @brief Use this constructor
   * @param mychannel is the channel (or YARP port) you want to use.
   *       The channels should be the same for reading and writing,
   *       but unique in your local network (ie task/application specific)
   */
   NetBlockingReader(std::string mychannel); // just declaration for doxygen

   /*! @brief read() the datatype T from the network
    *
    * Note that in the NetBlockingReader the read() operation *will* block,
    * which means it will only return, when it has received valid
    * data over the network.
    *
    * @return returns an instance of the specified datatype T, which you
    *         sent over the network.
   */
   T read(); // just the declaration for doxygen
   
};


  //---------------------------------------------------------------------------
  // template specialization for char 
  //---------------------------------------------------------------------------

//!@brief Char specialization, see NetBlockingReader for details.
template <>
class NetBlockingReader<char> : public cedar::aux::net::detail::SimpleNetReader<char, true>
{ 
public:
  //!@brief use this constructor. See NetBlockingReader::NetBlockingReader for details.
  explicit NetBlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<char, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned char 
  //---------------------------------------------------------------------------

//!@brief unsigned char specialization, see NetBlockingReader for details.
template <>
class NetBlockingReader<unsigned char> : public cedar::aux::net::detail::SimpleNetReader<unsigned char, true>
{ 
public:
  //!@brief use this constructor. See NetBlockingReader for details.
  explicit NetBlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned char, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for short
  //---------------------------------------------------------------------------

//!@brief short specialization, see NetBlockingReader for details.
template <>
class NetBlockingReader<short> : public cedar::aux::net::detail::SimpleNetReader<short, true>
{ 
public:
  //!@brief use this constructor. See NetBlockingReader for details.
  explicit NetBlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<short, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned short
  //---------------------------------------------------------------------------

//!@brief unsigned short specialization, see NetBlockingReader for details.
template <>
class NetBlockingReader<unsigned short> : public cedar::aux::net::detail::SimpleNetReader<unsigned short, true>
{ 
public:
  //!@brief use this constructor. See NetBlockingReader for details.
  explicit NetBlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned short, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for int
  //---------------------------------------------------------------------------

//!@brief int specialization, see NetBlockingReader for details.
template <>
class NetBlockingReader<int> : public cedar::aux::net::detail::SimpleNetReader<int, true>
{ 
public:
  //!@brief use this constructor. See NetBlockingReader for details.
  explicit NetBlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<int, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned int
  //---------------------------------------------------------------------------

//!@brief unsigned int specialization, see NetBlockingReader for details.
template <>
class NetBlockingReader<unsigned int> : public cedar::aux::net::detail::SimpleNetReader<unsigned int, true>
{ 
public:
  //!@brief use this constructor. See NetBlockingReader for details.
  explicit NetBlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned int, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for long
  //---------------------------------------------------------------------------

//!@brief long specialization, see NetBlockingReader for details.
template <>
class NetBlockingReader<long> : public cedar::aux::net::detail::SimpleNetReader<long, true>
{ 
public:
  //!@brief use this constructor. See NetBlockingReader for details.
  explicit NetBlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<long, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned long
  //---------------------------------------------------------------------------

//!@brief unsigned long specialization, see NetBlockingReader for details.
template <>
class NetBlockingReader<unsigned long> : public cedar::aux::net::detail::SimpleNetReader<unsigned long, true>
{ 
public:
  //!@brief use this constructor. See NetBlockingReader for details.
  explicit NetBlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned long, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for bool
  //---------------------------------------------------------------------------

//!@brief bool specialization, see NetBlockingReader for details.
template <>
class NetBlockingReader<bool> : public cedar::aux::net::detail::SimpleNetReader<bool, true>
{ 
public:
  //!@brief use this constructor. See NetBlockingReader for details.
  explicit NetBlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<bool, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for float
  //---------------------------------------------------------------------------

//!@brief float specialization, see NetBlockingReader for details.
template <>
class NetBlockingReader<float> : public cedar::aux::net::detail::SimpleNetReader<float, true>
{ 
public:
  //!@brief use this constructor. See NetBlockingReader for details.
  explicit NetBlockingReader(const std::string &s) 
                       : cedar::aux::net::detail::SimpleNetReader<float, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for double
  //---------------------------------------------------------------------------

//!@brief double specialization, see NetBlockingReader for details.
template <>
class NetBlockingReader<double> : public cedar::aux::net::detail::SimpleNetReader<double, true>
{ 
public:
  //!@brief use this constructor. See NetBlockingReader for details.
  explicit NetBlockingReader(const std::string &s) 
                       : cedar::aux::net::detail::SimpleNetReader<double, true>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for cv::Mat
  //---------------------------------------------------------------------------

//!@brief cv::Mat specialization, see NetBlockingReader for details.
template <>
class NetBlockingReader<cv::Mat> : public cedar::aux::net::detail::cvMatHelper<cv::Mat>,
                                   public cedar::aux::net::detail::CollatedNetReader<cv::Mat, true> 
{
public:
  //!@brief use this constructor. See NetBlockingReader for details.
  explicit NetBlockingReader(const std::string &myPortName) 
                         : cedar::aux::net::detail::cvMatHelper<cv::Mat>(), 
                           cedar::aux::net::detail::CollatedNetReader<cv::Mat, true>(myPortName)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for cv::Mat_<float>
  //---------------------------------------------------------------------------

//!@brief cv::Mat_<float> specialization, see NetBlockingReader for details.
template <>
class NetBlockingReader< cv::Mat_<float> > 
                         : public cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >,
                           public cedar::aux::net::detail::CollatedNetReader< cv::Mat_<float> , true> 
{
public:
  //!@brief use this constructor. See NetBlockingReader for details.
  explicit NetBlockingReader(const std::string &myPortName) 
                         : cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >(), 
                           cedar::aux::net::detail::CollatedNetReader< cv::Mat_<float> , true>(myPortName)
  {
  }
};

} } } // end namespaces

#endif
