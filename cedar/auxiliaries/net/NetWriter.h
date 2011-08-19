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

    File:        NetWriter.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Thu 18 Aug 2011 11:38:32 AM CEST

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_NETWRITER_H
#define CEDAR_NETWRITER_H

// LOCAL INCLUDES
#include "internals/namespace.h"
#include "internals/transport/simple/SimpleNetWriter.h"
#include "internals/transport/collated/CollatedNetWriter.h"
#include "internals/datatypes/opencv/cvMatHelper.h"

// PROJECT INCLUDES
#include <boost/static_assert.hpp>

// SYSTEM INCLUDES


namespace cedar {
  namespace aux {
    namespace net {

  //---------------------------------------------------------------------------
  // the unspecialized template class cannot be instantiated
  //---------------------------------------------------------------------------

/*!@brief Provides means to transport transparently (ie it is easy to use) a datatype over the net.
 * 
 * The class will assist you in sending a datatype over
 * the network (or locally) which you can read at the other end with NetReader.
 *
 * This class is a template, so you need to initialize it with the
 * datatype you want to send as a template argument. 
 * It has to be the same datatype that you will configure the NetReader with, obviously.
 *
 * Short example:
 * @code
 *   NetWriter<cv::Mat> myWriter("mychannel");
 *   cv::Mat mat= ...;
 *   myWriter.write(mat);
 * @endcode
 *
 * Please also refer to the <a href='../../cedar/auxiliaries/net/HOWTO'>HOWTO</a> for full examples.
 *
 * See also the constructor and the write() function.
 *
 * This class follows the RAII paradigm: ressource acquisition is instantiation.
 * It therefore is not copyable!
 *
 * The outgoing data is buffered in the background, so the write() function
 * will return before the data has actually left the machine.
 *
 * Valid instantiations are: NetWriter<char>, NetWriter<unsigned char>, NetWriter<short>,
 * NetWriter<unsigned short>, NetWriter<int>, NetWriter<unsigned int>, NetWriter<long>,
 * NetWriter<unsigned long>, NetWriter<float>, NetWriter<double>, NetWriter<bool>,
 * NetWriter<cv::Mat>, NetWriter< cv::Mat_<float>  
 * @see: NetReader, NetBlockingReader
 */
template <typename T>
class NetWriter
{
   //!@brief The standard constructor will not load. Do not call it.
  NetWriter()
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
  NetWriter(std::string mychannel); // just declaration for doxygen

  /*! @brief write the datatype T to the network
   *
   * @param data: the data you want to send.
   */
  void write(T data); // just the declaration for doxygen
};


  //---------------------------------------------------------------------------
  // template specialization for char 
  //---------------------------------------------------------------------------

//!@brief Char specialization, see NetWriter for details.
template <>
class NetWriter<char> : public cedar::aux::net::detail::SimpleNetWriter<char>
{ 
public:
  //!@brief use this constructor. See NetWriter::NetWriter for details.
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<char>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned char 
  //---------------------------------------------------------------------------

//!@brief unsigned char specialization, see NetWriter for details.
template <>
class NetWriter<unsigned char> : public cedar::aux::net::detail::SimpleNetWriter<unsigned char>
{ 
public:
  //!@brief use this constructor. See NetWriter for details.
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<unsigned char>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for short
  //---------------------------------------------------------------------------

//!@brief short specialization, see NetWriter for details.
template <>
class NetWriter<short> : public cedar::aux::net::detail::SimpleNetWriter<short>
{ 
public:
  //!@brief use this constructor. See NetWriter for details.
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<short>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned short
  //---------------------------------------------------------------------------

//!@brief unsigned short specialization, see NetWriter for details.
template <>
class NetWriter<unsigned short> : public cedar::aux::net::detail::SimpleNetWriter<unsigned short>
{ 
public:
  //!@brief use this constructor. See NetWriter for details.
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<unsigned short>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for int
  //---------------------------------------------------------------------------

//!@brief int specialization, see NetWriter for details.
template <>
class NetWriter<int> : public cedar::aux::net::detail::SimpleNetWriter<int>
{ 
public:
  //!@brief use this constructor. See NetWriter for details.
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<int>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned int
  //---------------------------------------------------------------------------

//!@brief unsigned int specialization, see NetWriter for details.
template <>
class NetWriter<unsigned int> : public cedar::aux::net::detail::SimpleNetWriter<unsigned int>
{ 
public:
  //!@brief use this constructor. See NetWriter for details.
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<unsigned int>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for long
  //---------------------------------------------------------------------------

//!@brief long specialization, see NetWriter for details.
template <>
class NetWriter<long> : public cedar::aux::net::detail::SimpleNetWriter<long>
{ 
public:
  //!@brief use this constructor. See NetWriter for details.
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<long>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for unsigned long
  //---------------------------------------------------------------------------

//!@brief unsigned long specialization, see NetWriter for details.
template <>
class NetWriter<unsigned long> : public cedar::aux::net::detail::SimpleNetWriter<unsigned long>
{ 
public:
  //!@brief use this constructor. See NetWriter for details.
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<unsigned long>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for bool
  //---------------------------------------------------------------------------

//!@brief bool specialization, see NetWriter for details.
template <>
class NetWriter<bool> : public cedar::aux::net::detail::SimpleNetWriter<bool>
{ 
public:
  //!@brief use this constructor. See NetWriter for details.
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<bool>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for float
  //---------------------------------------------------------------------------

//!@brief float specialization, see NetWriter for details.
template <>
class NetWriter<float> : public cedar::aux::net::detail::SimpleNetWriter<float>
{ 
public:
  //!@brief use this constructor. See NetWriter for details.
  explicit NetWriter(const std::string &s) 
                       : cedar::aux::net::detail::SimpleNetWriter<float>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for double
  //---------------------------------------------------------------------------

//!@brief double specialization, see NetWriter for details.
template <>
class NetWriter<double> : public cedar::aux::net::detail::SimpleNetWriter<double>
{ 
public:
  //!@brief use this constructor. See NetWriter for details.
  explicit NetWriter(const std::string &s) 
                       : cedar::aux::net::detail::SimpleNetWriter<double>(s)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for cv::Mat
  //---------------------------------------------------------------------------

//!@brief cv::Mat specialization, see NetWriter for details.
template <>
class NetWriter<cv::Mat> : public cedar::aux::net::detail::cvMatHelper<cv::Mat>,
                                   public cedar::aux::net::detail::CollatedNetWriter<cv::Mat> 
{
public:
  //!@brief use this constructor. See NetWriter for details.
  explicit NetWriter(const std::string &myPortName) 
                         : cedar::aux::net::detail::cvMatHelper<cv::Mat>(), 
                           cedar::aux::net::detail::CollatedNetWriter<cv::Mat>(myPortName)
  {
  }
};

  //---------------------------------------------------------------------------
  // template specialization for cv::Mat_<float>
  //---------------------------------------------------------------------------

//!@brief cv::Mat_<float> specialization, see NetWriter for details.
template <>
class NetWriter< cv::Mat_<float> > 
                         : public cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >,
                           public cedar::aux::net::detail::CollatedNetWriter< cv::Mat_<float> > 
{
public:
  //!@brief use this constructor. See NetWriter for details.
  explicit NetWriter(const std::string &myPortName) 
                         : cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >(), 
                           cedar::aux::net::detail::CollatedNetWriter< cv::Mat_<float> >(myPortName)
  {
  }
};


} } } // end namespaces

#endif

