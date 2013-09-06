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

    File:        Recorder.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2013 07 01

    Description: Implementation of the @em cedar::aux::Recorder class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES

#include "Recorder.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/ThreadWrapper.h"

// SYSTEM INCLUDES
#include <list>
#include <QWriteLocker>
#include <QReadLocker>
#include <QReadWriteLock>
#include <QDateTime>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/pointer_cast.hpp>
//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------
cedar::aux::Recorder::Recorder()
{
  mOutputDirectory = "Unknown";
}

cedar::aux::Recorder:: ~Recorder()
{
  clear();
}

void cedar::aux::Recorder::step(double)
{
  //Writing the first value of every DataSpectator queue.
  for(unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    mDataSpectatorCollection.get<DataSpectator>(i)->writeFirstRecordData();
  }
}

void cedar::aux::Recorder::registerData(cedar::aux::ConstDataPtr toSpectate, unsigned int recordIntv, const std::string& name)
{

  //check if Name is not already in use
  if(isRegistered(name))
  {
    CEDAR_THROW(cedar::aux::DuplicateNameException,"The data with name "+name+" is already registered");
    return;
  }

  // create new DataSpectaor and push it to the DataSpectator list
  cedar::aux::DataSpectatorPtr spec
    = cedar::aux::DataSpectatorPtr(new cedar::aux::DataSpectator(toSpectate, recordIntv, name));
  mDataSpectatorCollection.addThread(spec);

  // If Recorder is already running, also start the new DataSpectator
  if(this->isRunning())
  {
    spec->start();
  }

}

void cedar::aux::Recorder::unregisterData(const std::string& name)
{
  for(unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<DataSpectator>(i);
    if(spec->getName() == name)
    {
      mDataSpectatorCollection.remove(i);
      break;
    }
  }
}

void cedar::aux::Recorder::unregisterData(cedar::aux::ConstDataPtr data)
{
  for(unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<DataSpectator>(i);
    if(spec->getData() == data)
    {
      mDataSpectatorCollection.remove(i);
      break;
    }
  }
}


void cedar::aux::Recorder::createOutputDirectory()
{
  mOutputDirectory = QDateTime::currentDateTime().toString("yy.MM.dd hh-mm-ss").toStdString();
  boost::filesystem::create_directories(mOutputDirectory);
}

void cedar::aux::Recorder::applyStart()
{

  //Generate output directory
  this->createOutputDirectory();

  //start the timer.
  mStartTime.start();

  //find the minimal time to write to file. This should the smallest stepTime in the DataSpectator Threads.
  int min = 1000;
  for(unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<DataSpectator>(i);
    if(spec->getStepSize()<min)
    {
      min = spec->getStepSize();
    }
  }
  setStepSize(min);

  //start all DataSpectators.
  mDataSpectatorCollection.startAll();
}

void cedar::aux::Recorder::applyStop(bool /*suppressWarning*/)
{
  //Stop all DataSpectators.They will automatically write all containing data to file.
  mDataSpectatorCollection.stopAll();
}

void cedar::aux::Recorder::clear()
{
  stop();
  //remove all data.
  mDataSpectatorCollection.removeAll();
}


void cedar::aux::Recorder::setOutputDirectory(const std::string& path)
{
  this->mOutputDirectory = path;
}

std::string cedar::aux::Recorder::getOutputDirectory()
{
  return this->mOutputDirectory;
}

int cedar::aux::Recorder::getTimeStamp()
{
  return mStartTime.elapsed();
}

void cedar::aux::Recorder::setRecordIntervalTime(const std::string& name, unsigned int recordIntv)
{
  bool found = false;
  for(unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<DataSpectator>(i);
    if(spec->getName() == name)
    {
      spec->setStepSize(recordIntv);
      found = true;
      break;
    }
  }
  if(!found)
    CEDAR_THROW(cedar::aux::UnknownNameException,"The DatPtr named "+name+" is not registered.");
}

int cedar::aux::Recorder::getRecordIntervalTime(const std::string& name)
{
  for(unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<DataSpectator>(i);
    if(spec->getName() == name)
    {
      return spec->getRecordIntervalTime();
    }
  }
  return -1;
}

int cedar::aux::Recorder::getRecordIntervalTime(cedar::aux::ConstDataPtr data)
{
  for(unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<DataSpectator>(i);
    if(spec->getData() == data)
    {
      return spec->getRecordIntervalTime();
    }
  }
  return -1;
}

bool cedar::aux::Recorder::isRegistered(const std::string& name)
{
  for(unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<DataSpectator>(i);
    if(spec->getName() == name)
    {
      return true;
    }
  }
  return false;
}

bool cedar::aux::Recorder::isRegistered(cedar::aux::ConstDataPtr data)
{
  for(unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<DataSpectator>(i);
    if(spec->getData() == data)
    {
      return true;
    }
  }
  return false;
}

void cedar::aux::Recorder::renameRegisteredData(cedar::aux::ConstDataPtr data, const std::string& newName)
{
  for(unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<DataSpectator>(i);
    if(spec->getData() == data)
    {
      spec->setName(newName);
    }
  }
}
