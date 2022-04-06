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
        _mReconnectionTimeOut(new cedar::aux::UIntParameter(this, "timeout (ms)", 1000)),
        _mMaxTimeouts(new cedar::aux::UIntParameter(this, "max timeouts", 50)),
        _mSendInterval(new cedar::aux::UIntParameter(this, "send interval (timesteps)", 1))
{
  cedar::proc::sinks::TCPWriter::mpDataLock = new QReadWriteLock();

  _mReconnectionTimeOut->markAdvanced(true);
  _mMaxTimeouts->markAdvanced(true);
  _mSendInterval->markAdvanced(true);

  mSendMatrixWasSet.store(false);

  QObject::connect(this,SIGNAL(stateChanged()),this,SLOT(updateState()));

  // declare all data
  this->declareInput("input");
}

cedar::proc::sinks::TCPWriter::~TCPWriter()
{
  if(mRunning.load())
  {
    abortAndJoin();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sinks::TCPWriter::onStart()
{
  _mPort->setConstant(true);
  _mIpAdress->setConstant(true);
  this->startCommunicationThread();
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

//          unsigned long on = 1;
//          ioctlsocket(socket_h, FIONBIO, &on); //make socket Non-blockable in windows
#else
    int opt = 1;
    if (setsockopt(socket_h, SOL_SOCKET, SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
      std::cout << "Error Setting Socket Options" << std::endl;
    }
//    fcntl(socket_h, F_SETFL, O_NONBLOCK); //make socket Non-blockable
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

  if(!data->getData().isContinuous())
  {
    std::cout<<this->getName() << ">>Could not send data, because it was not continuous!"<<std::endl;
    std::cout<<"Data: " << data->getData() << std::endl;
    return;
  }

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
  this->abortAndJoin();
}

void cedar::proc::sinks::TCPWriter::reset()
{
  this->abortAndJoin();
  this->startCommunicationThread();
  this->onTrigger();
}

void cedar::proc::sinks::TCPWriter::compute(const cedar::proc::Arguments &)
{
  if (0 == _mPort->getValue() || _mIpAdress->getValue() == "")
  {
    this->setState(cedar::proc::Step::STATE_EXCEPTION, "Port and Ip Address must not be empty!");
  } else
  {
    this->setMatrixToSend(this->mInput->getData());
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

  int msgLength = -1;

#ifdef _WIN32
  int msgLength = recv(socket_h, readBuffer, mBufferLength, 0);
#else
  //Old Versions. MSG_DONTWAIT LAGS SOMEHOW and 0 FLAGS is smooth, but may block thread completely
//  int msgLength = recv(socket_h, readBuffer, mBufferLength, MSG_DONTWAIT);
//  int msgLength = recv(socket_h, readBuffer, mBufferLength, 0);

// Use select for compromise
  fd_set set;
  struct timeval timeout;
  FD_ZERO(&set); /* clear the set */
  FD_SET(socket_h, &set); /* add our file descriptor to the set */
  timeout.tv_sec = 0;
  timeout.tv_usec = 100000;

  int rv = select(socket_h + 1, &set, NULL, NULL, &timeout);
  if (rv > 0)
  {
    // socket has something to read
    msgLength = recv(socket_h, readBuffer, mBufferLength, 0);
  }

#endif


  if (msgLength > 0)
  {
    std::string message = std::string(readBuffer, msgLength);
    free(readBuffer);

//    this->setState(cedar::proc::Step::STATE_RUNNING, "Connected");
    this->setCurrentStateAndAnnotation(cedar::proc::Step::STATE_RUNNING, "Connected");
    startMeasurement = std::chrono::steady_clock::now();
    numConsecutiveTimeOuts = 0;
    return true;
  }
  else
  {
    free(readBuffer);
    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
    unsigned int elapsedMilliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime - startMeasurement).count();
    //I could not read! The original socket is  not there anymore
//    std::cout<<"\t" << this->getName() << " elapsedMilli: " << elapsedMilliSeconds <<std::endl;
    if (elapsedMilliSeconds > _mReconnectionTimeOut->getValue())
    {
      startMeasurement = std::chrono::steady_clock::now();
      numConsecutiveTimeOuts = numConsecutiveTimeOuts + 1;
      if (numConsecutiveTimeOuts > _mMaxTimeouts->getValue())
      {
//        this->setState(cedar::proc::Step::STATE_EXCEPTION,
//                       "More than " + std::to_string(_mMaxTimeouts->getValue()) +
//                       " consecutive timeouts. Press Reset to start Reconnecting again.");
        this->setCurrentStateAndAnnotation(cedar::proc::Step::STATE_EXCEPTION,
                       "More than " + std::to_string(_mMaxTimeouts->getValue()) +
                       " consecutive timeouts. Press Reset to start Reconnecting again.");
      } else
      {
        this->setCurrentStateAndAnnotation(cedar::proc::Step::STATE_INITIALIZING,
                       "Timeout of at least " + std::to_string(_mReconnectionTimeOut->getValue()) + " ms.");
//        this->setState(cedar::proc::Step::STATE_INITIALIZING,
//                       "Timeout of at least " + std::to_string(_mReconnectionTimeOut->getValue()) + " ms.");
      }
//      std::cout<<this->getName() << " elapsedMilliseconds " << elapsedMilliSeconds <<" > " << _mReconnectionTimeOut->getValue() << std::endl;
      return false;
    }
  }

//  std::cout<< this->getName() <<" checkReaderStatus no msg read, but continue to try before reconnect" <<std::endl;
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


void cedar::proc::sinks::TCPWriter::startCommunicationThread()
{
  mAbortRequested.store(false);
  mSendMatrixWasSet.store(false);
  try
  {
    mCommunicationThread = std::thread(&cedar::proc::sinks::TCPWriter::communicationLoop, this);
  }
  catch (...)
  {
    std::cout<<" Starting the CommunicationThread of " << this->getName() << " did somehow not work!" <<std::endl;
  }
}

void cedar::proc::sinks::TCPWriter::abortAndJoin()
{
//  std::cout<< this->getName() << " abortAndJoin()" <<std::endl;
  mAbortRequested.store(true);
  if (mCommunicationThread.joinable())
  {
    mCommunicationThread.join();
  }
  this->closeConnection();
}


void cedar::proc::sinks::TCPWriter::communicationLoop()
{
  mRunning.store(true);

  this->establishConnection();

  while (false == mAbortRequested.load())
  {
    try
    {
      stepCounter = stepCounter + 1;
      if (stepCounter >= this->_mSendInterval->getValue())
      {
        stepCounter = 0;
        if (mSendMatrixWasSet.load())
        {
          cedar::aux::ConstMatDataPtr mySendData(new cedar::aux::MatData(this->getMatrixToSend()));
          sendMatData(mySendData);
          bool isReaderAlive = checkReaderStatus();
          if (!isReaderAlive)
          {
            reconnect();
          }
        }
      }
    }
    catch (std::runtime_error &e)
    {
      std::cout<<"Caught an error: " << e.what()  << " in " << this->getName() <<std::endl;
    }
    catch(const std::exception& ex)
    {
      // speciffic handling for all exceptions extending std::exception, except
      // std::runtime_error which is handled explicitly
      std::cout << "Error occurred:  " << ex.what() << " while running: " << this->getName() <<  std::endl;
    }
    catch (...)
    {
      // Make sure that nothing leaves the thread for now...
      std::cout<<"Caught something else while running " << this->getName()  <<std::endl;
    }
  }

  mRunning.store(false);
}

void cedar::proc::sinks::TCPWriter::setMatrixToSend(cv::Mat sendMatrix)
{
  std::lock_guard<std::shared_timed_mutex> guard(writeMatrixMutex);
  mMatrixToSend = sendMatrix;
  mSendMatrixWasSet.store(true);
}

cv::Mat cedar::proc::sinks::TCPWriter::getMatrixToSend()
{
  std::lock_guard<std::shared_timed_mutex> guard(writeMatrixMutex);
  return mMatrixToSend.clone();
}

cedar::proc::Triggerable::State cedar::proc::sinks::TCPWriter::getCurrentState()
{
  std::lock_guard<std::shared_timed_mutex> guard(stateMutex);
  return mCurState;
}

std::string cedar::proc::sinks::TCPWriter::getCurrentAnnotation()
{
  std::lock_guard<std::shared_timed_mutex> guard(stateMutex);
  return mCurStateAnnotation;
}

void cedar::proc::sinks::TCPWriter::setCurrentStateAndAnnotation(Triggerable::State state, std::string annotation)
{
  std::lock_guard<std::shared_timed_mutex> guard(stateMutex);
  mCurState = state;
  mCurStateAnnotation = annotation;

  emit stateChanged();
}

void cedar::proc::sinks::TCPWriter::updateState()
{
  cedar::proc::Triggerable::State curState = getCurrentState();
  std::string curAnno = getCurrentAnnotation();

  this->setState(curState,curAnno);
}