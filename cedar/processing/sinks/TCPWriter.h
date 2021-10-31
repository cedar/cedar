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

    File:        TCPWriter.h

    Maintainer:  Jan Tekuelve
    Email:       jan.tekuelve@ini.ruhr-uni-bochum.de
    Date:        Wed 14 Apr 2021

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_TCP_WRITER_SINK_H
#define CEDAR_TCP_WRITER_SINK_H

#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/NumericParameter.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/utilities.h"

// FORWARD DECLARATIONS
#include "cedar/processing/sinks/TCPWriter.fwd.h"

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
#include <chrono>

#define MATMSG_END "E-N-D!"
#define MATMSG_CHK "CHK-SM"

/*!@brief A step which sends matrices locally  */
class cedar::proc::sinks::TCPWriter : public cedar::proc::Step
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
  TCPWriter();

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
  inline unsigned int getPort() const
  {
    return this->_mPort->getValue();
  }

  /*! Sets the name of the port.
   */
  inline void setPort(const unsigned int port)
  {
    this->_mPort->setValue(port);
  }

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
  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);
  //!@brief Resets the step and recreates the yarp connection.
  void reset();
  void establishConnection();
  void sendMatData(cedar::aux::ConstMatDataPtr data);
  bool checkReaderStatus();
  void closeConnection();
  void reconnect();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief MatrixData representing the input. Storing it like this saves time during computation.
  cedar::aux::ConstMatDataPtr mInput;
  std::string oldName = "";
  std::string debugString = "";
private:
    int socket_h; //socket handle
    struct sockaddr_in serv_addr; //Server Adress Object
    bool isConnected;
    unsigned int stepCounter;
    unsigned int numConsecutiveTimeOuts;
    std::chrono::steady_clock::time_point startMeasurement;

public:
  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
private:

  cedar::aux::UIntParameterPtr _mPort;
  cedar::aux::StringParameterPtr _mIpAdress;
  cedar::aux::UIntParameterPtr _mReconnectionTimeOut;
  cedar::aux::UIntParameterPtr _mMaxTimeouts;
  cedar::aux::UIntParameterPtr _mSendInterval;

  // locking for thread safety
  static QReadWriteLock *mpDataLock;

  static std::map<std::string, cv::Mat>& accessData()
  {
    static std::map<std::string, cv::Mat> mData;
    return mData; // returns a reference!
  }


}; // class cedar::proc::sinks::TcpWriter

#endif // CEDAR_TCP_WRITER_SINK_H
