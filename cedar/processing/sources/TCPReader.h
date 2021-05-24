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

    File:        TCPReader.h

    Maintainer:  Jan Tekuelve
    Email:       jan.tekuelve@ini.ruhr-uni-bochum.de
    Date:        Wed 14 Apr 2021

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_TCP_READER_SOURCE_H
#define CEDAR_TCP_READER_SOURCE_H

#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/NumericParameter.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/utilities.h"

// FORWARD DECLARATIONS
#include "TCPReader.fwd.h"

// SYSTEM INCLUDES
#ifdef _WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <arpa/inet.h>  /* definition of inet_ntoa */
#include <netdb.h>      /* definition of gethostbyname */
#include <netinet/in.h> /* definition of struct sockaddr_in */
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h> /* definition of close */
#endif



#include <cedar/auxiliaries/UIntParameter.h>

#define MATMSG_END "E-N-D!"
#define MATMSG_CHK "CHK-SM"

/*!@brief A step which sends matrices locally  */
class cedar::proc::sources::TCPReader : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  TCPReader();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Updates the output matrix.
  void compute(const cedar::proc::Arguments& arguments);
  void onStart();
  void onStop();


  /*! Returns the name of the port.
   */
  inline  unsigned int getPort() const
  {
    return this->_mPort->getValue();
  }

  /*! Sets the name of the port.
   */
  inline void setPort(const unsigned int port)
  {
    this->_mPort->setValue(port);
  }

public slots:
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Determines whether the data item can be connected to the slot.
  cedar::proc::DataSlot::VALIDITY determineInputValidity
                                  (
                                    cedar::proc::ConstDataSlotPtr slot,
                                    cedar::aux::ConstDataPtr data
                                  ) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Resets the step and recreates the yarp connection.
  void reset();
  void establishConnection();
  void receiveMatData();
  int accept_client(int server_fd);
  int create_socket_server(int port);
  void confirmAliveStatus();
  void closeSocket();
  void reconnect();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief MatrixData representing the input. Storing it like this saves time during computation.
  cedar::aux::ConstMatDataPtr mInput;
  cedar::aux::MatDataPtr mOutput;
  std::string oldName = "";
  std::string debugString = "";
private:
    int socket_h; //socket handle
    int raw_socket_h; //raw_handle
    struct sockaddr_in serv_addr; //Server Adress Object
    bool isConnected;
    fd_set rfds;
    unsigned int timeSinceLastConnectTrial;
    cv::Mat lastReadMatrix;
    std::string mOverflowBuffer;

public:
  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
private:
  cedar::aux::UIntParameterPtr _mBufferLength;
  cedar::aux::UIntParameterPtr _mPort;
  cedar::aux::UIntParameterPtr _mTimeOutBetweenPackets;
  cedar::aux::UIntParameterPtr _mTimeStepsBetweenAcceptTrials;

  // locking for thread safety
  static QReadWriteLock *mpDataLock;

  static std::map<std::string, cv::Mat>& accessData()
  {
    static std::map<std::string, cv::Mat> mData;
    return mData; // returns a reference!
  }

}; // class cedar::proc::sources::TcpReader

#endif // CEDAR_TCP_READER_SOURCE_H
