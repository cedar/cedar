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

    File:        CollatedNetPortable.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011 02:49:00 PM CEST

    Description: This class is deeply intertwined with the way YARP
                 uses its yarp::os::BufferedReader/Writer classes.
                 This works analogously to yarp::os::BinPortable -
                 we inherit from yarp::os::Portable.
                 CollatedNetPortable is templetized around the data type
                 you want to transport, these read() and write() functions
                 will be called by YARP.
                 Here we prepare the data for its network transport.

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATEDNETPORTABLE_H
#define CEDAR_COLLATEDNETPORTABLE_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/namespace.h"
#include "cedar/auxiliaries/net/detail/datatypes/CollatedType.h"
#include "cedar/auxiliaries/net/detail/transport/collated/header/MatrixNetHeaderAccessor.h"
#include "cedar/auxiliaries/net/exceptions/NetException.h"

// PROJECT INCLUDES
#include <boost/utility.hpp>
#include <yarp/os/Portable.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>

// SYSTEM INCLUDES





namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {


/*!@brief Wrapper around the transported matrix type. handles sending/receiving
 *
 * this will be similar to yarp::BinPortable but with size of the data (matrix)
 * only determinable at runtime
 * 
 */
template<class T>
class CollatedNetPortable : public yarp::os::Portable,
                            boost::noncopyable
{
    // TODO: trennen in Reader und Writer-Teil

  //---------------------------------------------------------------------------
  // members
  //---------------------------------------------------------------------------
private:
  CollatedType <T>mNetType;
  char *pVals;  // size 1 byte
  size_t sizeVals;

  typedef collated_traits<T>                traits_type;
  typedef typename traits_type::data_type   data_type;
  typedef typename traits_type::header_type header_type;

  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  CollatedNetPortable() : mNetType(), pVals(), sizeVals()
  { 
    pVals= NULL;
  }


  //!@brief Destructor, virtual to be sure
  virtual ~CollatedNetPortable()
  {
    if (pVals != NULL)
      free(pVals);
  }


  //---------------------------------------------------------------------------
  // public methods
  //---------------------------------------------------------------------------
public:
  //!@brief direct reference to the actual data (a copy of this will be transported)
  data_type& content() 
  {
    return mNetType.data;
  }

  //!@brief The generated header that will be transported, reference
  header_type& header()
  {
    return mNetType.header;
  }

  //!@brief YARP will call this read() and we will prepare the matrix and send it
  //
  // the content has to be placed into the reference given by content()
  bool read(yarp::os::ConnectionReader& connection) 
  {
    int iHeaderSize, iDataSize, iNumElements, iElemSize;
    int i;

    iHeaderSize= sizeof(mNetType.header);

    connection.convertTextMode(); // if connection is text-mode, convert!

    ////////////////////////
    // read out the header ...
    connection.expectBlock( (char*)(&mNetType.header),
                            iHeaderSize );

    iDataSize= MatrixNetHeaderAccessor::getDataSize( 
                 mNetType.header );
    iNumElements= MatrixNetHeaderAccessor::getTotalElements(
                    mNetType.header);
    iElemSize= MatrixNetHeaderAccessor::getElemSize( 
                 mNetType.header );

    ///////////////////////
    // read out the matrix content ...
    if (pVals == NULL) // first time?
    {
      // we only have the data sizes at runtime for some variable types
      // (i.e. cv::Mat) so we can only do this at the time of the first
      // read
    
      pVals= (char*)malloc( iDataSize );

      if (pVals == NULL)
      {
        CEDAR_THROW( cedar::aux::exc::NetUnexpectedDataException,
                     "out of memory - cannot alloc transfered data" );
        return false;
      }

      // we will validify the data in CollatedNetReader::read()

      // generate header if not yet initialized
      mNetType.data= mNetType.late_init_data_from_header();
    }

    connection.expectBlock( (char*)pVals,
                            iDataSize );

    //////////// Nun haben wir den Daten-Teil ///////////

    if (pVals == NULL) // paranoid
      return false;
   
    for (i= 0; i< iNumElements; i++)
    {
      // we use memcpy as we only know the sizes of the data at runtime
      memcpy( mNetType.contentAt(i, iElemSize), // see datatype/ directory
              pVals + (i*iElemSize),
              iElemSize );
    }

    // NetType.data now holds the matrix data
    return true;
  }

  //!@brief YARP will call this write() and we will write the receive data to content
  //
  // note: this function will be called (threaded) for every listener by YARP
  //
  // the values of the (prepared) header where already checked in 
  // CollatedNetWriter::write()
  bool write(yarp::os::ConnectionWriter& connection) 
  {
    int iHeaderSize, iDataSize, aiBlocks[2], iNumElements, iElemSize;
    int i;
 
    iHeaderSize= sizeof(mNetType.header);
    iDataSize= MatrixNetHeaderAccessor::getDataSize( 
                 mNetType.header );
    iNumElements= MatrixNetHeaderAccessor::getTotalElements(
                    mNetType.header);
    iElemSize= MatrixNetHeaderAccessor::getElemSize( 
                 mNetType.header );
 
    aiBlocks[0]= iHeaderSize;
    aiBlocks[1]= iDataSize;

    connection.declareSizes( 2, aiBlocks );
      // yarp: This may improve efficiency in some situations.

    //////////////////////////////////////
    // write the generated matrix header to network ...
    connection.appendBlock( (char*)(&mNetType.header),
                            sizeof(mNetType.header) );
      // note: YARP will copy this block - TODO: is that necessary?

    //////////////////////////////////////
    // write matrix content to network ...
    if (pVals == NULL)
    {
      pVals= (char*)malloc( iDataSize );
    }

    for (i= 0; i < iNumElements; i++)
    {
      // copy each element.
      // we access it with contentAt() which will be specialized for each
      // transportet data type (see datatype/ directory)
      memcpy( pVals + (i * iElemSize),
             mNetType.contentAt(i, iElemSize),
             iElemSize );
    } 
 
    connection.appendBlock( (char*)pVals,
                           iDataSize );
      // YARP will copy this block internally TODO: ist that necessary?
 
    connection.convertTextMode(); // if connection is text-mode, convert!
    return true;
  }

};

} } } } // end namespaces


#endif

