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

    File:        TCPReader.cpp

    Maintainer:  Jan Tekuelve
    Email:       jan.tekuelve@ini.ruhr-uni-bochum.de
    Date:        Wed 14 Apr 2021

    Description:

    Credits:

=============================================================================*/

#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "TCPReader.h"
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

#ifdef _WIN32
#include <winsock2.h>
#else

#include <arpa/inet.h>  /* definition of inet_ntoa */
#include <netdb.h>      /* definition of gethostbyname */
#include <netinet/in.h> /* definition of struct sockaddr_in */
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h> /* definition of close */

#endif

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
                    new ElementDeclarationTemplate<cedar::proc::sources::TCPReader>
                            (
                                    "Sources",
                                    "cedar.processing.sources.TCPReader"
                            )
            );
    input_declaration->setIconPath(":/steps/tcp_reader.svg");
    input_declaration->setDescription("Forwards a tensor via a TCP Socket to a destination."
                                      "See also the TCPReader step.");

    input_declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

QReadWriteLock *cedar::proc::sources::TCPReader::mpDataLock = NULL;

cedar::proc::sources::TCPReader::TCPReader()
        :
        cedar::proc::Step(true),
// outputs
        mOutput(new cedar::aux::MatData(cv::Mat())),
        isConnected(false),
        timeSinceLastConnectTrial(0),
        numberOfFailedReads(0),
        lastReadMatrix(cv::Mat::zeros(50, 50, CV_32F)),
        lastReadDimensions(cv::Mat(2, 1, CV_32S,50)),
        mOverflowBuffer(""),
        _mBufferLength(new cedar::aux::UIntParameter(this, "buffer size", 32768)),
        _mPort(new cedar::aux::UIntParameter(this, "port", 50000, 49152, 65535)), //ephemeral ports only
        _mTimeOutBetweenPackets(new cedar::aux::UIntParameter(this, "packet timeout (ms)", 500)),
        _mTimeStepsBetweenMessages(new cedar::aux::UIntParameter(this, "message timeout (ms)", 5)),
        _mTimeStepsBetweenAcceptTrials(new cedar::aux::UIntParameter(this, "accept interval (steps)", 30))
{
  cedar::proc::sources::TCPReader::mpDataLock = new QReadWriteLock();

  _mBufferLength->markAdvanced(true);
  _mTimeOutBetweenPackets->markAdvanced(true);
  _mTimeStepsBetweenMessages->markAdvanced(true);
  _mTimeStepsBetweenAcceptTrials->markAdvanced(true);


  this->declareOutput("output", mOutput);

  mOutput->setData(lastReadMatrix);
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sources::TCPReader::onStart()
{
  this->establishConnection();
}


void cedar::proc::sources::TCPReader::establishConnection()
{
  raw_socket_h = create_socket_server(_mPort->getValue());
  socket_h = accept_client(raw_socket_h);

}

void cedar::proc::sources::TCPReader::reconnect()
{
  closeSocket();
  establishConnection();
}

int cedar::proc::sources::TCPReader::create_socket_server(int port)
{
  int sfd, rc;
  struct sockaddr_in address;

#ifdef _WIN32
  /* initialize the socket api */
      WSADATA info;

      rc = WSAStartup(MAKEWORD(1, 1), &info); /* Winsock 1.1 */
      if (rc != 0)
      {
          printf("cannot initialize Winsock\n");
          return -1;
      }
#endif
  /* create the socket */
  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1)
  {
    std::cout << "cannot create socket" << std::endl;
    return -1;
  }


#ifdef _WIN32
  const char opt = 1;
          if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR,
                         &opt, sizeof(opt)))
          {
              std::cout << "Error Setting Socket Options" << std::endl;
          }

          unsigned long on = 1;
          ioctlsocket(sfd, FIONBIO, &on); //make socket Non-blockable in windows
#else
  int opt = 1;
  if (setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT,
                 &opt, sizeof(opt)))
  {
    std::cout << "Error Setting Socket Options" << std::endl;
  }
  fcntl(sfd, F_SETFL, O_NONBLOCK); //make socket Non-blockable
#endif


  /* fill in socket address */
  memset(&address, 0, sizeof(struct sockaddr_in));
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = INADDR_ANY;


  /* bind to port */
  rc = bind(sfd, (struct sockaddr *) &address, sizeof(struct sockaddr));
  if (rc == -1)
  {
    printf("cannot bind port %d\n", port);
#ifdef _WIN32
    closesocket(sfd);
#else
    close(sfd);
#endif
    return -1;
  }

  /* listen for connections */
  if (listen(sfd, 1) == -1)
  {
    printf("cannot listen for connections\n");
#ifdef _WIN32
    closesocket(sfd);
#else
    close(sfd);
#endif
    return -1;
  }
  printf("Waiting for a connection on port %d...\n", port);

  return sfd;

}

int cedar::proc::sources::TCPReader::accept_client(int server_fd)
{
  isConnected = false;
  int cfd;
  struct sockaddr_in client;

#ifndef _WIN32
  socklen_t asize;
#else
  int asize;
#endif

  struct hostent *client_info;

  asize = sizeof(struct sockaddr_in);

  cfd = accept(server_fd, (struct sockaddr *) &client, &asize);
  if (cfd == -1)
  {
    return -1;
  }

  client_info = gethostbyname((char *) inet_ntoa(client.sin_addr));
//  printf("Accepted connection from: %s \n", client_info->h_name);
  std::cout<< this->getName() << " accepted connection from: " << client_info->h_name << " on port: " << this->_mPort->getValue() << std::endl;

  isConnected = true;
  FD_ZERO(&rfds);
  FD_SET(cfd, &rfds);

  return cfd;
}

void cedar::proc::sources::TCPReader::receiveMatData()
{
//  std::cout<<this->getName() << " receiveMatData()" << std::endl;
  if (!isConnected)
  {
    timeSinceLastConnectTrial = timeSinceLastConnectTrial + 1;
    if (timeSinceLastConnectTrial > _mTimeStepsBetweenAcceptTrials->getValue())
    {
      timeSinceLastConnectTrial = 0;
      if (raw_socket_h != -1)
      {
        //Just wait for some connection!
        socket_h = accept_client(raw_socket_h);
      } else
      {
        reconnect();
      }
    }
    return;
  }

//  std::cout<<this->getName() << " before FD_SET" << std::endl;
//  FD_ZERO(&rfds);
//  FD_SET(socket_h, &rfds);


//  std::cout<<this->getName() << " before malloc" << std::endl;
  // Size may be varied. Just picked this number for now.
  char *buffer = (char *) malloc(_mBufferLength->getValue() * sizeof(char));
  std::string fullMessageString = "";

  if (mOverflowBuffer != "")
  {
    fullMessageString += mOverflowBuffer;
    mOverflowBuffer = "";
  }

  bool endFound = false;
  bool timeout = false;
  int msgLength;

  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

//  std::cout<<this->getName() << " before While" << std::endl;

  while (!endFound && !timeout)
  {

    end = std::chrono::steady_clock::now();
    timeout = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() >
              _mTimeOutBetweenPackets->getValue();

    memset(buffer, 0, _mBufferLength->getValue() * sizeof(char));

#ifdef _WIN32
    msgLength = recv(socket_h, buffer, _mBufferLength->getValue(), 0);
#else
    msgLength = recv(socket_h, buffer, _mBufferLength->getValue(), MSG_DONTWAIT);
#endif
    if (msgLength > 0)
    {
      numberOfFailedReads = 0;
      std::string concatString = std::string(buffer, msgLength);

      //Let's see, if we found the end of a Msg
      if (concatString.find(MATMSG_END) != std::string::npos)
      {
        endFound = true;
        std::vector<std::string> splitStrings;
        cedar::aux::split(concatString, MATMSG_END, splitStrings);
        if (splitStrings.size() == 2)
        {
          concatString = splitStrings[0];
          mOverflowBuffer = splitStrings[1];
          fullMessageString += concatString; // The final Matrix String
        } else if (splitStrings.size() > 2)
        { //"We have more than one Message in this one reading!
          fullMessageString = splitStrings[splitStrings.size() - 2]; //The last guaranteed full Matrix
          mOverflowBuffer = splitStrings[splitStrings.size() - 1];
        }

      } else
      { // We are not at the end! Let's continue concatenating.
        fullMessageString += concatString;
      }
    } else
    {
      numberOfFailedReads = numberOfFailedReads + 1;

      if (numberOfFailedReads > _mTimeStepsBetweenMessages->getValue())
      {
        std::cout<< this->getName()<< " failed reading for " << _mTimeStepsBetweenMessages->getValue() << " times"<<std::endl;
        numberOfFailedReads = 0;
        free(buffer);
        if (raw_socket_h != -1)
        {
          //Just wait for some connection!
//          std::cout<<this->getName()<<" tried to ACCEPT again because " << _mTimeStepsBetweenMessages->getValue() << " reading trials did fail!" << std::endl;
          socket_h = accept_client(raw_socket_h);
        } else
        {
//          std::cout<<this->getName()<<" tried to RECONNECT again because " << _mTimeStepsBetweenMessages->getValue() << " reading trials did fail!" << std::endl;
          reconnect();
        }
        return;
      }
	  //This should be freed again!
	  free(buffer);
      return;
    }

  }
  free(buffer); //Good old Malloc ...

  if (timeout)
  {
    reconnect();
    return;
  }


  std::vector<std::string> chkSumSplit;
  cedar::aux::split(fullMessageString, MATMSG_CHK, chkSumSplit);

  std::string matrixString = "";
  std::string sentChecksum = "";

  if (chkSumSplit.size() == 2)
  {
    matrixString = chkSumSplit[0];
    sentChecksum = chkSumSplit[1];
  } else
  {
    //Something went wrong. The message had more than one Checksum included. Abort
    return;
  }

  std::string genChecksum = std::to_string(cedar::aux::generateCR32Checksum(matrixString.c_str(), matrixString.size()));

  if (genChecksum != sentChecksum)
  {
    return;
  }


  std::stringstream inputStream; // In the future we might get rid of the stream here
  inputStream << fullMessageString;

  cedar::aux::MatData transformData;
  transformData.deserialize(inputStream, cedar::aux::SerializationFormat::Compact);
  lastReadMatrix = transformData.getData();
}

void cedar::proc::sources::TCPReader::onStop()
{
  _mPort->setConstant(false);
  closeSocket();
}

void cedar::proc::sources::TCPReader::closeSocket()
{
#ifdef _WIN32
  closesocket(socket_h);
  closesocket(raw_socket_h);
  WSACleanup();
#else
  close(socket_h);
  close(raw_socket_h);
#endif
  isConnected = false;
}

void cedar::proc::sources::TCPReader::reset()
{
  reconnect();
}

void cedar::proc::sources::TCPReader::compute(const cedar::proc::Arguments &)
{
  if (0 == _mPort->getValue())
  {
    this->setState(cedar::proc::Step::STATE_EXCEPTION, "Port must not be empty!");
  } else
  {
    receiveMatData();
    mOutput->setData(lastReadMatrix);

    if(lastReadMatrix.rows != lastReadDimensions.at<int>(0,0) || lastReadMatrix.cols != lastReadDimensions.at<int>(1,0))
    {
        this->emitOutputPropertiesChangedSignal("output");
        lastReadDimensions.at<int>(0,0) = lastReadMatrix.rows;
        lastReadDimensions.at<int>(1,0) = lastReadMatrix.cols;
    }


    confirmAliveStatus();

    if (isConnected)
    {
      this->setState(cedar::proc::Step::STATE_RUNNING, "Connected");
    } else
    {
      this->setState(cedar::proc::Step::STATE_INITIALIZING, "Waiting for Incoming Connections... ");
    }
  }
}

void cedar::proc::sources::TCPReader::confirmAliveStatus()
{
  std::string msgContent = this->getName() + " is alive!";
#ifdef _WIN32
  send(socket_h, msgContent.c_str(), msgContent.size(), 0);
#else
  send(socket_h, msgContent.c_str(), msgContent.size(), MSG_NOSIGNAL);
#endif
}

cedar::proc::DataSlot::VALIDITY cedar::proc::sources::TCPReader::determineInputValidity
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