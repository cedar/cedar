#ifndef _COLLATED_NET_PORTABLE_H_
#define _COLLATED_NET_PORTABLE_H_

#include <boost/utility.hpp>

#include "../../namespace.h"
#include "../../datatypes/CollatedType.h"
#include "header/MatrixNetHeaderAccessor.h"
#include <yarp/os/Portable.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>

using namespace yarp::os;

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {




// this will be similar to yarp::BinPortable but with size of the data (matrix)
// only determinable at runtime

template<class T>
class CollatedNetPortable : public yarp::os::Portable,
                            boost::noncopyable
{
    // TODO: trennen in Reader und Writer-Teil
private:
  CollatedType <T>mNetType;
  char *pVals;  // size 1 byte
  size_t sizeVals;

  typedef collated_traits<T>                traits_type;
  typedef typename traits_type::data_type   data_type;
  typedef typename traits_type::header_type header_type;

public:
  
  CollatedNetPortable() : mNetType(), pVals(), sizeVals()
  { 
    pVals= NULL;
  }

  virtual ~CollatedNetPortable()
  {
//    if (pVals != NULL)
//      free(pVals);
  }

  data_type& content() 
  {
    return mNetType.data;
  }

  header_type& header()
  {
    return mNetType.header;
  }

  bool read(ConnectionReader& connection) 
  {
    int iHeaderSize, iDataSize, iNumElements, iElemSize;
    int i;

    iHeaderSize= sizeof(mNetType.header);

    connection.convertTextMode(); // if connection is text-mode, convert!

    // header lesen ...
    connection.expectBlock( (char*)(&mNetType.header),
                            iHeaderSize );

    //////////// Nun haben wir den Header-Teil ///////////

    iDataSize= MatrixNetHeaderAccessor::getDataSize( 
                 mNetType.header );
    iNumElements= MatrixNetHeaderAccessor::getTotalElements(
                    mNetType.header);
    iElemSize= MatrixNetHeaderAccessor::getElemSize( 
                 mNetType.header );

    // matrixdaten lesen ...

    if (pVals == NULL) // erstes Lesen? Speicher allokieren
    {
      // dies kann erst beim ersten Lesen geschehen, weil wir davor
      // nicht die zu verwendende Datengroesse haben
    
      pVals= (char*)malloc( iDataSize );

      if (pVals == NULL) // TODO: Allokfehler werfen und abfangen ...
        return false;


      // TODO: groesse checken

      // Daten-Matrix korrekt initialisieren 
      mNetType.data= mNetType.late_init_data_from_header();
    }

    connection.expectBlock( (char*)pVals,
                            iDataSize );

    //////////// Nun haben wir den Daten-Teil ///////////

    if (pVals == NULL) // TODO: assertion
      return false;
   
    // jetzt Content (mNetType.data) fuellen ...
    for (i= 0; i< iNumElements; i++)
    {
      // memcpy: da wir Speicherbereiche kopieren, die zur Laufzeit
      //         unbekannte Groesse haben
      // template qualifier: siehe Stroustrup C.13.6
      //                     (bewirkt, dass der Template-Aufruf richtig
      //                      geparsed wird)
      memcpy( mNetType.contentAt(i, iElemSize),
              pVals + (i*iElemSize),
              iElemSize );
    }

    // member NetType.data enthaelt jetzt die Daten
    return true;
  }

  // this function will be called (threaded) for every listener
  bool write(ConnectionWriter& connection) 
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

    // TODO: checken

    // header schreiben
    connection.appendBlock( (char*)(&mNetType.header),
                            sizeof(mNetType.header) );
       // TODO: auf externalBlock aendern?

    // matrixdaten schreiben ...
    if (pVals == NULL)
    {
//cout << " DATA SIZE (write) "  << iDataSize  << endl;
//cout << " ELEM SIZE (write) "  << iElemSize  << endl;
//cout << " ELEM NUMS (write) "  << iNumElements  << endl;
      pVals= (char*)malloc( iDataSize );
    }

    for (i= 0; i < iNumElements; i++)
    {
      memcpy( pVals + (i * iElemSize),
             mNetType.contentAt(i, iElemSize),
             iElemSize );
    } 
 
    connection.appendBlock( (char*)pVals,
                           iDataSize );
 
    connection.convertTextMode(); // if connection is text-mode, convert!
    return true;
  }
};

} } } } // end namespaces


#endif

