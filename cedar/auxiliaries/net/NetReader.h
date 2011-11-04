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

    File:        NetReader.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Thu 18 Aug 2011 11:38:01 AM CEST

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_NETREADER_H
#define CEDAR_NETREADER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/internals/namespace.h"
#include "cedar/auxiliaries/net/internals/transport/simple/SimpleNetReader.h"
#include "cedar/auxiliaries/net/internals/transport/collated/CollatedNetReader.h"
#include "cedar/auxiliaries/net/internals/datatypes/opencv/cvMatHelper.h"

// PROJECT INCLUDES
#include <opencv2/opencv.hpp>

// SYSTEM INCLUDES



namespace cedar {
  namespace aux {
    namespace net {


  //---------------------------------------------------------------------------
  // the unspecialized template class cannot be instantiated
  //---------------------------------------------------------------------------

/*!@brief Provides means to transport transparently (ie it is easy to use) a datatype over the net (non-blocking version)
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
 *   NetReader<cv::Mat> myReader("mychannel");
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
 * Valid instantiations are: NetReader<char>, NetReader<unsigned char>, NetReader<short>,
 * NetReader<unsigned short>, NetReader<int>, NetReader<unsigned int>, NetReader<long>,
 * NetReader<unsigned long>, NetReader<float>, NetReader<double>, NetReader<bool>,
 * NetReader<cv::Mat>, NetReader< cv::Mat_<float>  
 * @see: NetBlockingReader, NetWriter
 */
template <typename T>
class NetReader
{
   //!@brief The standard constructor will not load. Do not call it.
   NetReader()
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
   NetReader(std::string mychannel); // just declaration for doxygen

   /*! @brief read() the datatype T from the network
    *
    * Note that in the NetReader the read() operation will not(!) block
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


  //---------------------------------------------------------------------------
  // template specialization for char 
  //---------------------------------------------------------------------------

//!@brief Char specialization, see NetReader for details.
template <>
class NetReader<char> : public cedar::aux::net::detail::SimpleNetReader<char, false>
{ 
public:
  //!@brief use this constructor. See NetReader::NetReader for details.
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<char, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned char 
  //---------------------------------------------------------------------------

//!@brief unsigned char specialization, see NetReader for details.
template <>
class NetReader<unsigned char> : public cedar::aux::net::detail::SimpleNetReader<unsigned char, false>
{ 
public:
  //!@brief use this constructor. See NetReader for details.
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned char, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for short
  //---------------------------------------------------------------------------

//!@brief short specialization, see NetReader for details.
template <>
class NetReader<short> : public cedar::aux::net::detail::SimpleNetReader<short, false>
{ 
public:
  //!@brief use this constructor. See NetReader for details.
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<short, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned short
  //---------------------------------------------------------------------------

//!@brief unsigned short specialization, see NetReader for details.
template <>
class NetReader<unsigned short> : public cedar::aux::net::detail::SimpleNetReader<unsigned short, false>
{ 
public:
  //!@brief use this constructor. See NetReader for details.
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned short, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for int
  //---------------------------------------------------------------------------

//!@brief int specialization, see NetReader for details.
template <>
class NetReader<int> : public cedar::aux::net::detail::SimpleNetReader<int, false>
{ 
public:
  //!@brief use this constructor. See NetReader for details.
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<int, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned int
  //---------------------------------------------------------------------------

//!@brief unsigned int specialization, see NetReader for details.
template <>
class NetReader<unsigned int> : public cedar::aux::net::detail::SimpleNetReader<unsigned int, false>
{ 
public:
  //!@brief use this constructor. See NetReader for details.
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned int, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for long
  //---------------------------------------------------------------------------

//!@brief long specialization, see NetReader for details.
template <>
class NetReader<long> : public cedar::aux::net::detail::SimpleNetReader<long, false>
{ 
public:
  //!@brief use this constructor. See NetReader for details.
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<long, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned long
  //---------------------------------------------------------------------------

//!@brief unsigned long specialization, see NetReader for details.
template <>
class NetReader<unsigned long> : public cedar::aux::net::detail::SimpleNetReader<unsigned long, false>
{ 
public:
  //!@brief use this constructor. See NetReader for details.
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned long, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for bool
  //---------------------------------------------------------------------------

//!@brief bool specialization, see NetReader for details.
template <>
class NetReader<bool> : public cedar::aux::net::detail::SimpleNetReader<bool, false>
{ 
public:
  //!@brief use this constructor. See NetReader for details.
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<bool, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for float
  //---------------------------------------------------------------------------

//!@brief float specialization, see NetReader for details.
template <>
class NetReader<float> : public cedar::aux::net::detail::SimpleNetReader<float, false>
{ 
public:
  //!@brief use this constructor. See NetReader for details.
  explicit NetReader(const std::string &s) 
                       : cedar::aux::net::detail::SimpleNetReader<float, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for double
  //---------------------------------------------------------------------------

//!@brief double specialization, see NetReader for details.
template <>
class NetReader<double> : public cedar::aux::net::detail::SimpleNetReader<double, false>
{ 
public:
  //!@brief use this constructor. See NetReader for details.
  explicit NetReader(const std::string &s) 
                       : cedar::aux::net::detail::SimpleNetReader<double, false>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for cv::Mat
  //---------------------------------------------------------------------------

//!@brief cv::Mat specialization, see NetReader for details.
template <>
class NetReader<cv::Mat> : public cedar::aux::net::detail::cvMatHelper<cv::Mat>,
                                   public cedar::aux::net::detail::CollatedNetReader<cv::Mat, false> 
{
public:
  //!@brief use this constructor. See NetReader for details.
  explicit NetReader(const std::string &myPortName) 
                         : cedar::aux::net::detail::cvMatHelper<cv::Mat>(), 
                           cedar::aux::net::detail::CollatedNetReader<cv::Mat, false>(myPortName)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for cv::Mat_<float>
  //---------------------------------------------------------------------------

//!@brief cv::Mat_<float> specialization, see NetReader for details.
template <>
class NetReader< cv::Mat_<float> > 
                         : public cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >,
                           public cedar::aux::net::detail::CollatedNetReader< cv::Mat_<float> , false> 
{
public:
  //!@brief use this constructor. See NetReader for details.
  explicit NetReader(const std::string &myPortName) 
                         : cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >(), 
                           cedar::aux::net::detail::CollatedNetReader< cv::Mat_<float> , false>(myPortName)
  {
  }
};

} } } // end namespaces

#endif

