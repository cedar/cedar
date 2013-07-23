/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        DataSpectator.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2011 12 09

    Description: Implementation of cedar::aux::DataSpectator.

    Credits:

======================================================================================================================*/


// CEDAR INCLUDES
#include "DataSpectator.h"
#include "Recorder.h"

// SYSTEM INCLUDES

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------

cedar::aux::DataSpectator::DataSpectator(cedar::aux::DataPtr toSpectate, int recordIntv,std::string name)
{
  this->mpQueueLock = new QReadWriteLock();
  this->mpOfstreamLock = new QReadWriteLock();
  this->setStepSize(recordIntv);
  mName = name;
  mData = toSpectate;
  QWriteLocker locker(mpOfstreamLock);
  mpOutputStream = boost::shared_ptr<std::ofstream>(new std::ofstream());
}

cedar::aux::DataPtr cedar::aux::DataSpectator::data()
{
  return this->mData;
}

cedar::aux::DataSpectator::~DataSpectator()
{
  {
    QWriteLocker locker(mpOfstreamLock);
    mpOutputStream->close();
  }
  delete mpOfstreamLock;
  delete mpQueueLock;
}

void cedar::aux::DataSpectator::step(double)
{
  record();
}

void cedar::aux::DataSpectator::start()
{
  mOutputPath = cedar::aux::RecorderSingleton::getInstance()->getOutputDirectory() + "/" + mName;
  mpOutputStream->open(mOutputPath,std::ios::out | std::ios::app);
  writeHeader();
  cedar::aux::LoopedThread::start();
}

void cedar::aux::DataSpectator::stop(unsigned int timeout /*= UINT_MAX*/, bool suppressWarning /*= false*/)
{
  writeAllRecordData();
  {
    QWriteLocker locker(mpOfstreamLock);
    mpOutputStream->close();
  }
  cedar::aux::LoopedThread::stop(timeout, suppressWarning);
}

void cedar::aux::DataSpectator::writeHeader()
{
  QWriteLocker locker(mpOfstreamLock);
  (*mpOutputStream) << mData->serializeHeader() << std::endl;
}

void cedar::aux::DataSpectator::record()
{
  // Copy the Data
  cedar::aux::DataPtr d = mData->clone();

  //Create new recordData
  recordData* rec = new recordData();
  rec->mData = d;
  rec->mRecordTime = cedar::aux::RecorderSingleton::getInstance()->getTimeStamp();

  //Lock the Queue and push record Data
  QWriteLocker locker(mpQueueLock);
  mDataQueue.push_back(boost::shared_ptr<recordData>(rec));
}

void cedar::aux::DataSpectator::writeOneRecordData()
{
  // thread context: called from Recorder's thread.
  /* This function uses a lot of locks. It is important to dont lock the queue during the serialization(takes 25-30ms)
   * to not block the record function for pushing new RecordData into the queue.
   */
  unsigned int size;

  {
    QReadLocker locker(mpQueueLock);
    size = mDataQueue.size();
  }

  if(size > 0)
  {
    // Save data to disk.
    recordData* data;
    {
      QReadLocker locker(mpQueueLock);
      data = mDataQueue.front().get();
    }

    {
      QWriteLocker locker(mpOfstreamLock);
      (*mpOutputStream) <<data->mRecordTime << "," << data->mData->serializeData() << std::endl;
    }

    {
      QWriteLocker locker(mpQueueLock);
      mDataQueue.pop_front();
    }
  }
}

void cedar::aux::DataSpectator::writeAllRecordData()
{
  // thread context: called from Recorder's thread.
  // here a single lock is enough. No new recordData will be created so the list can be blocked.

   QWriteLocker locker(mpQueueLock);

  while(mDataQueue.size() > 0)
  {
    recordData* data;
    data = mDataQueue.front().get();

    {
      QWriteLocker locker(mpOfstreamLock);
      (*mpOutputStream) <<data->mRecordTime << "," << data->mData->serializeData() << std::endl;
    }
    mDataQueue.pop_front();

  }
}


