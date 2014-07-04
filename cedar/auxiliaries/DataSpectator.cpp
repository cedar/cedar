/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/auxiliaries/DataSpectator.h"
#include "cedar/auxiliaries/Recorder.h"
#include "cedar/auxiliaries/GlobalClock.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#include <boost/algorithm/string/replace.hpp>

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------

cedar::aux::DataSpectator::DataSpectator
(
  cedar::aux::ConstDataPtr toSpectate,
  cedar::unit::Time recordIntervall,
  const std::string& name
)
:
mData(toSpectate),
mpOfstreamLock(new QReadWriteLock()),
mpQueueLock(new QReadWriteLock()),
mName(name)
{
  this->setStepSize(recordIntervall);

  this->connectToStartSignal(boost::bind(&cedar::aux::DataSpectator::prepareStart, this));
  this->connectToStopSignal(boost::bind(&cedar::aux::DataSpectator::processStop, this, _1));
}


cedar::aux::DataSpectator::~DataSpectator()
{
  {
    QWriteLocker locker(mpOfstreamLock);
    mOutputStream.close();
  }
  delete mpOfstreamLock;
  delete mpQueueLock;
}

void cedar::aux::DataSpectator::step(cedar::unit::Time)
{
  record();
}

void cedar::aux::DataSpectator::prepareStart()
{
  mOutputPath = cedar::aux::RecorderSingleton::getInstance()->getOutputDirectory() + "/" +
      boost::algorithm::replace_all_copy(mName," ","_") + ".csv";
  mOutputStream.open(mOutputPath, std::ios::out | std::ios::app);
  writeHeader();
}

void cedar::aux::DataSpectator::processStop(bool /* suppressWarning */)
{
  writeAllRecordData();
  
  {
    QWriteLocker locker(mpOfstreamLock);
    mOutputStream.close();
  }
}

void cedar::aux::DataSpectator::writeHeader()
{
  QWriteLocker locker(mpOfstreamLock);
  mData->serializeHeader(mOutputStream);
  mOutputStream << std::endl;
}

void cedar::aux::DataSpectator::record()
{
  // Copy the Data
  cedar::aux::DataPtr data = mData->clone();

  //Create new recordData
  RecordData rec;
  rec.mData = data;
  rec.mRecordTime = cedar::aux::GlobalClockSingleton::getInstance()->getTime();
  //Lock the Queue and push record Data
  QWriteLocker locker(mpQueueLock);
  mDataQueue.push_back(rec);
}

void cedar::aux::DataSpectator::writeFirstRecordData()
{
  // thread context: called from Recorder's thread.
  /* This function uses a lot of locks. It is important to don't lock the queue during the serialization (takes 25-30ms)
   * to not block the record function for pushing new RecordData into the queue.
   */
  unsigned int size;

  {
    QReadLocker locker(mpQueueLock);
    size = mDataQueue.size();
  }

  if (size > 0)
  {
    // Save data to disk.
    RecordData data;
    {
      QWriteLocker locker(mpQueueLock);
      data = mDataQueue.front();
      mDataQueue.pop_front();
    }

    {
      QWriteLocker locker(mpOfstreamLock);
      mOutputStream << data.mRecordTime << ",";
      data.mData->serializeData(mOutputStream);
      mOutputStream << std::endl;
    }
  }
}

void cedar::aux::DataSpectator::writeAllRecordData()
{
  // thread context: called from Recorder's thread.
  // here a single lock is enough. No new recordData will be created so the list can be blocked.

  QWriteLocker locker(mpQueueLock);

  while (mDataQueue.size() > 0)
  {
    RecordData data;
    data = mDataQueue.front();

    {
      QWriteLocker locker(mpOfstreamLock);
      mOutputStream << data.mRecordTime << ",";
      data.mData->serializeData(mOutputStream);
      mOutputStream << std::endl;
    }
    mDataQueue.pop_front();

  }
}


const std::string& cedar::aux::DataSpectator::getName() const
{
  return this->mName;
}


void cedar::aux::DataSpectator::setName(const std::string& name)
{
  this->mName = name;
}

cedar::aux::ConstDataPtr cedar::aux::DataSpectator::getData() const
{
  return this->mData;
}

cedar::unit::Time cedar::aux::DataSpectator::getRecordIntervalTime() const
{
  return this->getStepSize();
}

void cedar::aux::DataSpectator::makeSnapshot()
{
  this->prepareStart();
  this->record();
}
