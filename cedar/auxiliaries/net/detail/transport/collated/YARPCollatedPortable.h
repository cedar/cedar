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

    File:        YARPCollatedPortable.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011 

    Description: This class is deeply intertwined with the way YARP
                 uses its yarp::os::BufferedReader/Writer classes.
                 This works analogously to yarp::os::BinPortable -
                 we inherit from yarp::os::Portable.
                 YARPCollatedPortable is templetized around the data type
                 you want to transport, these read() and write() functions
                 will be called by YARP.
                 Here we prepare the data for its network transport.

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATED_NET_YARP_PORTABLE_H
#define CEDAR_COLLATED_NET_YARP_PORTABLE_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/transport/collated/YARPCollatedPortable.fwd.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/MatrixTypeWrapper.h"
#include "cedar/auxiliaries/net/exceptions.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/utility.hpp>
#endif
#include <yarp/os/Portable.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/conf/version.h>
#include <vector>

//!@cond SKIPPED_DOCUMENTATION


/*!@brief Wrapper around the transported matrix type. handles sending/receiving
 *
 * this will be similar to yarp::BinPortable but with size of the data (matrix)
 * only determinable at runtime
 * 
 */
template<class T>
class cedar::aux::net::detail::YARPCollatedPortable : public yarp::os::Portable,boost::noncopyable
{
    // TODO: seperate into Reader and Writer parts

  //---------------------------------------------------------------------------
  // members
  //---------------------------------------------------------------------------
private:
#if (YARP_VERSION_MAJOR > 2)
  mutable MatrixTypeWrapper<T> mNetType;
  mutable char                 *mpVals;  // size 1 byte
#else
   MatrixTypeWrapper<T> mNetType;
   char                 *mpVals;  // size 1 byte
#endif //  YARP_VERSION_MAJOR > 2
  size_t               mSizeVals;

  typedef CollatedTraits<T>               TraitsType;
  typedef typename TraitsType::DataType   DataType;
  typedef typename TraitsType::HeaderType HeaderType;

  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  YARPCollatedPortable() 
    : mNetType(), mpVals(), mSizeVals()
  { 
    mpVals= NULL;
  }


  //!@brief Destructor, virtual to be sure
  virtual ~YARPCollatedPortable()
  {
    if (mpVals != NULL)
      free(mpVals);
  }


  //---------------------------------------------------------------------------
  // public methods
  //---------------------------------------------------------------------------
public:
  //!@brief direct reference to the actual data (a copy of this will be transported)
  DataType& content() 
  {
    return mNetType.mData;
  }

  //!@brief The generated header that will be transported, reference
  HeaderType& header()
  {
    return mNetType.mHeader;
  }

  //!@brief YARP will call this read() and we will prepare the matrix and send it
  //
  // the content has to be placed into the reference given by content()
  bool read(yarp::os::ConnectionReader& connection) 
  {
    int fixed_header_size, variable_header_size, data_size;

    fixed_header_size= mNetType.mHeader.getFixedSerializationSize();

    connection.convertTextMode(); // if connection is text-mode, convert!

    ////////////////////////
    // read out the fixed part of the header ...
    std::vector<char> fixed_part;
    fixed_part.resize(fixed_header_size);
    connection.expectBlock( &fixed_part.front(), fixed_header_size);
    mNetType.mHeader.deserializeFixed(&fixed_part.front());

    // read out the variable part of the header ...
    variable_header_size = mNetType.mHeader.getVariableSerializationSize();
    std::vector<char> variable_part;
    variable_part.resize(variable_header_size);
    connection.expectBlock( &variable_part.front(), variable_header_size);
    mNetType.mHeader.deserializeVariable(&variable_part.front());

    data_size= mNetType.getDataSize();

    ///////////////////////
    // read out the matrix content ...
    if (mpVals == NULL) // first time?
    {
      // we only have the data sizes at runtime for some variable types
      // (i.e. cv::Mat) so we can only do this at the time of the first
      // read
    
      mpVals= static_cast<char*>(malloc(data_size));

      if (mpVals == NULL)
      {
        CEDAR_THROW( cedar::aux::net::NetUnexpectedDataException,
                     "out of memory - cannot alloc transfered data" );
        return false;
      }

      // we will validify the data in CollatedNetReader::read()

      // generate header if not yet initialized
      mNetType.mData= mNetType.lateInitDataFromHeader();
    }

    connection.expectBlock(static_cast<char*>(mpVals), data_size );

    //////////// now we have the data-part ///////////

    if (mpVals == NULL) // paranoid
      return false;

    mNetType.readFromMemory(mpVals); // specialized depending on T

    // NetType.mData now holds the matrix data
    return true;
  }

  //!@brief YARP will call this write() and we will write the receive data to content
  //
  // note: this function will be called (threaded) for every listener by YARP
  //
  // the values of the (prepared) header where already checked in 
  // CollatedNetWriter::write()
#if (YARP_VERSION_MAJOR > 2)
  bool write(yarp::os::ConnectionWriter& connection) const
  {
    int fixed_header_size, variable_header_size, data_size, blocks_array[3];

    fixed_header_size = mNetType.mHeader.getFixedSerializationSize();
    variable_header_size = mNetType.mHeader.getVariableSerializationSize();
    data_size= mNetType.getDataSize();
 
    blocks_array[0]= fixed_header_size;
    blocks_array[1]= variable_header_size;
    blocks_array[2]= data_size;

    connection.declareSizes( 3, blocks_array );
      // yarp: This may improve efficiency in some situations.

    //////////////////////////////////////
    //!@todo Make this a member so it only has to be (de)allocated once
    std::vector<char> bytes_fixed, bytes_variable;
    bytes_fixed.resize(fixed_header_size);
    bytes_variable.resize(variable_header_size);
    mNetType.mHeader.serializeFixed(&bytes_fixed.front());
    mNetType.mHeader.serializeVariable(&bytes_variable.front());
    // write the generated matrix header to network ...
    connection.appendBlock(&bytes_fixed.front(), fixed_header_size);
    connection.appendBlock(&bytes_variable.front(), variable_header_size);

    //////////////////////////////////////
    // write matrix content to network ...
    if (mpVals == nullptr)
    {
      mpVals= static_cast<char*>(malloc(data_size));
    }

    mNetType.writeToMemory(mpVals); // specialized depending on T
 
    connection.appendBlock(static_cast<char*>(mpVals), data_size );
      // YARP will copy this block internally TODO: ist that necessary?
 
    connection.convertTextMode(); // if connection is text-mode, convert!

    return true;
  }
#else
  bool write(yarp::os::ConnectionWriter& connection)
  {
    int fixed_header_size, variable_header_size, data_size, blocks_array[3];

    fixed_header_size = mNetType.mHeader.getFixedSerializationSize();
    variable_header_size = mNetType.mHeader.getVariableSerializationSize();
    data_size= mNetType.getDataSize();

    blocks_array[0]= fixed_header_size;
    blocks_array[1]= variable_header_size;
    blocks_array[2]= data_size;

    connection.declareSizes( 3, blocks_array );
    // yarp: This may improve efficiency in some situations.

    //////////////////////////////////////
    //!@todo Make this a member so it only has to be (de)allocated once
    std::vector<char> bytes_fixed, bytes_variable;
    bytes_fixed.resize(fixed_header_size);
    bytes_variable.resize(variable_header_size);
    mNetType.mHeader.serializeFixed(&bytes_fixed.front());
    mNetType.mHeader.serializeVariable(&bytes_variable.front());
    // write the generated matrix header to network ...
    connection.appendBlock(&bytes_fixed.front(), fixed_header_size);
    connection.appendBlock(&bytes_variable.front(), variable_header_size);

    //////////////////////////////////////
    // write matrix content to network ...
    if (mpVals == nullptr)
    {
      mpVals= static_cast<char*>(malloc(data_size));
    }

    mNetType.writeToMemory(mpVals); // specialized depending on T

    connection.appendBlock(static_cast<char*>(mpVals), data_size );
    // YARP will copy this block internally TODO: ist that necessary?

    connection.convertTextMode(); // if connection is text-mode, convert!

    return true;
  }

#endif  //YARP_VERSION_MAJOR > 2
};

//!@endcond
#endif
