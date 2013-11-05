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

#include "cedar/auxiliaries/Recorder.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/ThreadWrapper.h"
#include "cedar/auxiliaries/Settings.h"

// SYSTEM INCLUDES
#include <list>
#include <QWriteLocker>
#include <QReadLocker>
#include <QReadWriteLock>
#include <QDateTime>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/pointer_cast.hpp>
#include <limits>

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------

cedar::aux::Recorder::Recorder()
{
  mProjectName = "Unnamed";
}

cedar::aux::Recorder::~Recorder()
{
  this->clear();
}

void cedar::aux::Recorder::step(double)
{
  //Writing the first value of every DataSpectator queue.
  for (unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    mDataSpectatorCollection.get<cedar::aux::DataSpectator>(i)->writeFirstRecordData();
  }
}

void cedar::aux::Recorder::registerData(cedar::aux::ConstDataPtr toSpectate, unsigned int recordIntv, const std::string& name)
{
  //check if Name is not already in use
  if (isRegistered(name))
  {
    CEDAR_THROW(cedar::aux::DuplicateNameException,"The data with name " + name + " is already registered");
  }

  // create new DataSpectaor and push it to the DataSpectator list
  cedar::aux::DataSpectatorPtr spec
    = cedar::aux::DataSpectatorPtr(new cedar::aux::DataSpectator(toSpectate, recordIntv, name));
  mDataSpectatorCollection.addThread(spec);

  // If Recorder is already running, also start the new DataSpectator
  if (this->isRunning())
  {
    spec->start();
  }

}

void cedar::aux::Recorder::unregisterData(const std::string& name)
{
  //throw exception if running
  if (isRunning())
  {
    CEDAR_THROW(cedar::aux::ThreadRunningExeption, "Cannot unregister data while Recorder is running");
  }
    
  //!@todo This is redundant with the code below (unregisterData(cedar::aux::ConstDataPtr data)); please remove these
  //!      redundancies as far as possible (make this function call the one below, or the other way around)
  for(unsigned int i = 0; i < mDataSpectatorCollection.size(); ++i)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<DataSpectator>(i);
    if (spec->getName() == name)
    {
      mDataSpectatorCollection.remove(i);
      break;
    }
  }
}

void cedar::aux::Recorder::unregisterData(cedar::aux::ConstDataPtr data)
{
  for (unsigned int i = 0; i < mDataSpectatorCollection.size(); ++i)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<DataSpectator>(i);
    if (spec->getData() == data)
    {
      mDataSpectatorCollection.remove(i);
      break;
    }
  }
}


void cedar::aux::Recorder::createOutputDirectory()
{
  mOutputDirectory = cedar::aux::SettingsSingleton::getInstance()->getRecorderOutputDirectory()
                       + "/"+mProjectName+"/recording_" + QDateTime::currentDateTime().toString("yyyy.MM.dd_hh:mm:ss").toStdString();
  boost::filesystem::create_directories(mOutputDirectory);
}

void cedar::aux::Recorder::applyStart()
{
  if (mDataSpectatorCollection.size() > 0)
  {
    //Generate output directory
    this->createOutputDirectory();
  }

  //start the timer.
  mStartTime.restart();

  //find the minimal time to write to file. This should the smallest stepTime in the DataSpectator threads.
  //!@todo std::numeric_limits<int>::max() will lock the GUI!!!!
  int min = 1000;// std::numeric_limits<int>::max() ;
  for (unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
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
  //throw exception if running
  if (isRunning())
  {
    CEDAR_THROW(cedar::aux::ThreadRunningExeption,"Cannot unregister data while Recorder is Running");
  }

  stop();
  //remove all data.
  mDataSpectatorCollection.removeAll();
}


void cedar::aux::Recorder::setRecordedProjectName(const std::string& name)
{
  //throw exception if running
  if (isRunning())
  {
    CEDAR_THROW(cedar::aux::ThreadRunningExeption,"Cannot change output directory while recorder is running");
  }

  this->mProjectName = boost::filesystem::path(name).stem().string();
}

const std::string& cedar::aux::Recorder::getOutputDirectory() const
{
  return this->mOutputDirectory;
}

int cedar::aux::Recorder::getTimeStamp() const
{
  return mStartTime.elapsed();
}

void cedar::aux::Recorder::setRecordIntervalTime(const std::string& name, unsigned int recordIntv)
{
  //throw exception if running
  if (isRunning())
  {
    CEDAR_THROW(cedar::aux::ThreadRunningExeption,"Cannot change record inerval while recorder is running");
  }

  for (unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<cedar::aux::DataSpectator>(i);
    if (spec->getName() == name)
    {
      spec->setStepSize(recordIntv);
      return;
    }
  }
  CEDAR_THROW(cedar::aux::UnknownNameException, "The data named \"" + name + "\" is not registered.");
}

unsigned int cedar::aux::Recorder::getRecordIntervalTime(const std::string& name) const
{
  for (unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<DataSpectator>(i);
    if (spec->getName() == name)
    {
      return spec->getRecordIntervalTime();
    }
  }
  CEDAR_THROW(cedar::aux::UnknownNameException, "The data named \"" + name + "\" is not registered.");
}

unsigned int cedar::aux::Recorder::getRecordIntervalTime(cedar::aux::ConstDataPtr data) const
{
  for (unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<cedar::aux::DataSpectator>(i);
    if (spec->getData() == data)
    {
      return spec->getRecordIntervalTime();
    }
  }
  CEDAR_THROW(cedar::aux::UnknownNameException, "The given data pointer is not registered.");
}

bool cedar::aux::Recorder::isRegistered(const std::string& name) const
{
  for (unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<DataSpectator>(i);
    if (spec->getName() == name)
    {
      return true;
    }
  }
  return false;
}

bool cedar::aux::Recorder::isRegistered(cedar::aux::ConstDataPtr data) const
{
  for (unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<DataSpectator>(i);
    if (spec->getData() == data)
    {
      return true;
    }
  }
  return false;
}

void cedar::aux::Recorder::renameRegisteredData(cedar::aux::ConstDataPtr data, const std::string& newName)
{
  //throw exception if running
  if(isRunning())
  {
    CEDAR_THROW(cedar::aux::ThreadRunningExeption,"Cannot rename data while recorder is running");
  }

  for (unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<DataSpectator>(i);
    if (spec->getData() == data)
    {
      spec->setName(newName);
    }
  }
}

void cedar::aux::Recorder::takeSnapshot()
{
  std::string oldName = this->mProjectName;
  this->mProjectName = oldName+"/Snapshots";
  this->createOutputDirectory();
  for (unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<DataSpectator>(i);
    spec->makeSnapshot();
  }
  for (unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<DataSpectator>(i);
    spec->applyStop(true);
  }
  this->mProjectName = oldName;
}

std::map<std::string, int> cedar::aux::Recorder::getRegisteredData() const
{
  std::map<std::string, int> registeredData;

  for (unsigned int i = 0; i < mDataSpectatorCollection.size(); i++)
  {
    cedar::aux::DataSpectatorPtr spec = mDataSpectatorCollection.get<DataSpectator>(i);
    registeredData[spec->getName()] = spec->getRecordIntervalTime();
  }
  return registeredData;
}
