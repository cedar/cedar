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

    File:        TCPWriter.cpp

    Maintainer:  Jan Tekuelve
    Email:       jan.tekuelve@ini.ruhr-uni-bochum.de
    Date:        Wed 14 Apr 2021

    Description:

    Credits:

=============================================================================*/

#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/sinks/TCPWriter.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/version.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES
#include <QReadLocker>
#include <QWriteLocker>
#include <iostream>
#include <vector>
#include <string>

//Socket Tutorial Includes
#include <stdio.h>

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


#include <sys/types.h>
#include <string.h>
#include <cedar/auxiliaries/UIntParameter.h>

//Non-Blocking Includes
//#include <bits/fcntl.h>
#include <fcntl.h>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr input_declaration
            (
                    new ElementDeclarationTemplate<cedar::proc::sinks::TCPWriter>
                            (
                                    "Sinks",
                                    "cedar.processing.sinks.TCPWriter"
                            )
            );
    input_declaration->setIconPath(":/steps/tcp_writer.svg");
    input_declaration->setDescription("Forwards a tensor via a TCP Socket to a destination."
                                      "See also the TCPReader step.");

    input_declaration->declare();

    return true;
  }

  bool declared = declare();
}

QReadWriteLock *cedar::proc::sinks::TCPWriter::mpDataLock = NULL;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
cedar::proc::sinks::TCPWriter::TCPWriter()
        :
        cedar::proc::Step(true),
// outputs
        mInput(new cedar::aux::MatData(cv::Mat())),
        socket_h(0),
        isConnected(false),
        stepCounter(0),
        numConsecutiveTimeOuts(0),
        startMeasurement(std::chrono::steady_clock::now()),
        _mPort(new cedar::aux::UIntParameter(this, "port", 50000, 49152, 65535)), //ephemeral ports only
        _mIpAdress(new cedar::aux::StringParameter(this, "ip_adress", "127.0.0.1")),
        _mReconnectionTimeOut(new cedar::aux::UIntParameter(this, "timeout (ms)", 3000)),
        _mMaxTimeouts(new cedar::aux::UIntParameter(this, "max timeouts", 50)),
        _mSendInterval(new cedar::aux::UIntParameter(this, "send interval (timesteps)", 1))
{
  cedar::proc::sinks::TCPWriter::mpDataLock = new QReadWriteLock();

  _mReconnectionTimeOut->markAdvanced(true);
  _mMaxTimeouts->markAdvanced(true);
  _mSendInterval->markAdvanced(true);


  // declare all data
  this->declareInput("input");
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sinks::TCPWriter::onStart()
{
  _mPort->setConstant(true);
  _mIpAdress->setConstant(true);
  this->establishConnection();
}

void cedar::proc::sinks::TCPWriter::reconnect()
{
  closeConnection();
  establishConnection();
}


void cedar::proc::sinks::TCPWriter::establishConnection()
{
  socket_h = 0;
#ifdef _WIN32
  /* initialize the socket api */
      WSADATA info;
      int rc = WSAStartup(MAKEWORD(1, 1), &info); /* Winsock 1.1 */
      if (rc != 0)
      {
          printf("cannot initialize Winsock\n");
          return;
      }
#endif

  if ((socket_h = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    std::cout << "Socket creation error" << std::endl;

  } else
  {

#ifdef _WIN32
    const char opt = 1;
          if (setsockopt(socket_h, SOL_SOCKET, SO_REUSEADDR,
                         &opt, sizeof(opt)))
          {
              std::cout << "Error Setting Socket Options" << std::endl;
          }

          unsigned long on = 1;
          ioctlsocket(socket_h, FIONBIO, &on); //make socket Non-blockable in windows
#else
    int opt = 1;
    if (setsockopt(socket_h, SOL_SOCKET, SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
      std::cout << "Error Setting Socket Options" << std::endl;
    }
    fcntl(socket_h, F_SETFL, O_NONBLOCK); //make socket Non-blockable
#endif

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(_mPort->getValue());

    if (inet_pton(AF_INET, this->_mIpAdress->getValue().c_str(), &serv_addr.sin_addr) <= 0)
    {
      std::cout << "Invalid address/ Address not supported" << std::endl;
    } else
    {
      ::connect(socket_h, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    }
  }

}


void cedar::proc::sinks::TCPWriter::sendMatData(cedar::aux::ConstMatDataPtr data)
{
  std::ostringstream stream;
  data->serialize(stream, cedar::aux::SerializationFormat::Compact);

  unsigned int checkSum = cedar::aux::generateCR32Checksum(stream.str().c_str(), stream.str().size());
  std::string streamContent = stream.str() + MATMSG_CHK + std::to_string(checkSum) + MATMSG_END;

#ifdef _WIN32
  send(socket_h, streamContent.c_str(), streamContent.size(), 0);
#else
  send(socket_h, streamContent.c_str(), streamContent.size(), MSG_NOSIGNAL);
#endif

}

void cedar::proc::sinks::TCPWriter::onStop()
{
  _mPort->setConstant(false);
  _mIpAdress->setConstant(false);
  closeConnection();
}

void cedar::proc::sinks::TCPWriter::reset()
{
  this->reconnect();
  this->onTrigger();
}

void cedar::proc::sinks::TCPWriter::compute(const cedar::proc::Arguments &)
{
  if (0 == _mPort->getValue() || _mIpAdress->getValue() == "")
  {
    this->setState(cedar::proc::Step::STATE_EXCEPTION, "Port and Ip Address must not be empty!");
  } else
  {
    stepCounter = stepCounter + 1;
    if (stepCounter >= this->_mSendInterval->getValue())
    {
      stepCounter = 0;
      sendMatData(this->mInput);
      bool isReaderAlive = checkReaderStatus();

      if (!isReaderAlive)
      {
        reconnect();
      }
    }
  }
}

cedar::proc::DataSlot::VALIDITY cedar::proc::sinks::TCPWriter::determineInputValidity
        (
                cedar::proc::ConstDataSlotPtr CEDAR_DEBUG_ONLY(slot),
                cedar::aux::ConstDataPtr data
        ) const
{
  // First, let's make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(slot->getName() == "input")

  if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
  {
    return cedar::proc::DataSlot::VALIDITY_VALID;
  } else
  {
    return cedar::proc::DataSlot::VALIDITY_ERROR;
  }
}

void cedar::proc::sinks::TCPWriter::inputConnectionChanged(const std::string &inputName)
{
  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "input");

  // Assign the input to the member. This saves us from casting in every computation step.
  this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));

  if (!this->mInput)
  {
    return;
  }
}


bool cedar::proc::sinks::TCPWriter::checkReaderStatus()
{
  const int mBufferLength = 32768;
  char *readBuffer = (char *) malloc(mBufferLength * sizeof(char));
  memset(readBuffer, 0, mBufferLength);

#ifdef _WIN32
  int msgLength = recv(socket_h, readBuffer, mBufferLength, 0);
#else
  int msgLength = recv(socket_h, readBuffer, mBufferLength, MSG_DONTWAIT);
#endif


  if (msgLength > 0)
  {
    std::string message = std::string(readBuffer, msgLength);
    free(readBuffer);

    this->setState(cedar::proc::Step::STATE_RUNNING, "Connected");
    startMeasurement = std::chrono::steady_clock::now();
    numConsecutiveTimeOuts = 0;
    return true;
  } else
  {
    free(readBuffer);
    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
    unsigned int elapsedMilliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime - startMeasurement).count();
    //I could not read! The original socket is  not there anymore
    if (elapsedMilliSeconds > _mReconnectionTimeOut->getValue())
    {
      startMeasurement = std::chrono::steady_clock::now();
      numConsecutiveTimeOuts = numConsecutiveTimeOuts + 1;
      if (numConsecutiveTimeOuts > _mMaxTimeouts->getValue())
      {
        this->setState(cedar::proc::Step::STATE_EXCEPTION,
                       "More than " + std::to_string(_mMaxTimeouts->getValue()) +
                       " consecutive timeouts. Press Reset to start Reconnecting again.");
      } else
      {
        this->setState(cedar::proc::Step::STATE_INITIALIZING,
                       "Timeout of at least " + std::to_string(_mReconnectionTimeOut->getValue()) + " ms.");
      }


      return false;
    }
  }

  return true; //Technically here the Status is unknown, but we will continue to try
}

void cedar::proc::sinks::TCPWriter::closeConnection()
{
#ifdef _WIN32
  closesocket(socket_h);
  WSACleanup();
#else
  close(socket_h);
#endif

  numConsecutiveTimeOuts = 0;
}