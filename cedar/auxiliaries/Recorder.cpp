/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#include <QWriteLocker>
#include <QReadLocker>
#include <QReadWriteLock>
#include <QDateTime>
#ifndef Q_MOC_RUN
  #include <boost/filesystem.hpp>
  #include <boost/date_time/posix_time/posix_time.hpp>
  #include <boost/pointer_cast.hpp>
#endif
#include <list>
#include <limits>

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------

cedar::aux::Recorder::Recorder()
:
mSubFolder("recording_#T#")
{
  mProjectName = "Unnamed";

  this->connectToStartSignal(boost::bind(&cedar::aux::Recorder::prepareStart, this));
  this->connectToQuitSignal(boost::bind(&cedar::aux::Recorder::processQuit, this));
}

cedar::aux::Recorder::~Recorder()
{
  this->clear();
}

void cedar::aux::Recorder::step(cedar::unit::Time)
{
  // Writing the first value of every DataSpectator queue.
  for (auto data_spectator : mDataSpectatorCollection.getCollection())
  {
    boost::static_pointer_cast<cedar::aux::DataSpectator>(data_spectator.second)->writeFirstRecordData();
  }
}

void cedar::aux::Recorder::registerData(cedar::aux::ConstDataPtr toSpectate, cedar::unit::Time recordInterval, const std::string& name)
{
  // check if Name is not already in use
  if (isRegistered(name))
  {
    CEDAR_THROW(cedar::aux::DuplicateNameException, "The data with name " + name + " is already registered");
  }

  // create new DataSpectaor and push it to the DataSpectator list
  cedar::aux::DataSpectatorPtr spec
    = cedar::aux::DataSpectatorPtr(new cedar::aux::DataSpectator(toSpectate, recordInterval, name));
  mDataSpectatorCollection.addThread(name, spec);

  // If Recorder is already running, also start the new DataSpectator
  if (this->isRunningNolocking())
  {
    spec->start();
  }
}

void cedar::aux::Recorder::unregisterData(const std::string& name)
{
  // throw exception if running
  if (isRunningNolocking())
  {
    CEDAR_THROW(cedar::aux::RecorderException, "Cannot unregister data while Recorder is running");
  }
  mDataSpectatorCollection.remove(name);
}

void cedar::aux::Recorder::unregisterData(cedar::aux::ConstDataPtr data)
{
  for (auto data_spectator : mDataSpectatorCollection.getCollection())
  {
    auto casted = boost::static_pointer_cast<cedar::aux::DataSpectator>(data_spectator.second);
    if (casted->getData() == data)
    {
      mDataSpectatorCollection.remove(data_spectator.first);
      break;
    }
  }
}

void cedar::aux::Recorder::createOutputDirectory()
{
  mOutputDirectory = cedar::aux::SettingsSingleton::getInstance()->getRecorderOutputDirectory()
                           + "/"+mProjectName+"/"+ mSubFolder ;
  std::string time_stamp = this->getTimeStamp();
  boost::replace_all(mOutputDirectory, "#T#", time_stamp);
  boost::filesystem::create_directories(mOutputDirectory);
}

void cedar::aux::Recorder::prepareStart()
{
  if (mDataSpectatorCollection.size() > 0)
  {
    // Generate output directory
    this->createOutputDirectory();
  }

  // find the minimal time to write to file. This should the smallest stepTime in the DataSpectator threads.
  cedar::unit::Time min(1000.0 * cedar::unit::milli * cedar::unit::seconds);
  for (auto data_spectator : mDataSpectatorCollection.getCollection())
  {
    auto casted = boost::static_pointer_cast<cedar::aux::DataSpectator>(data_spectator.second);
    if (casted->getStepSize() < min)
    {
      min = casted->getStepSize();
    }
  }

  setStepSize(min);

  // start all DataSpectators.
  mDataSpectatorCollection.startAll();
}

void cedar::aux::Recorder::processQuit()
{
  // Resets the subdirectory if any was setted
  mSubFolder = "recording_#T#";

  // Stop all DataSpectators.They will automatically write all containing data to file.
  mDataSpectatorCollection.stopAll();
}

void cedar::aux::Recorder::clear()
{
  // throw exception if running
  if (isRunningNolocking())
  {
    CEDAR_THROW(cedar::aux::RecorderException, "Cannot unregister data while Recorder is Running");
  }

  stop();
  // remove all data.
  mDataSpectatorCollection.removeAll();
}


void cedar::aux::Recorder::setRecordedProjectName(const std::string& name)
{
  //throw exception if running
  if (isRunningNolocking())
  {
    CEDAR_THROW(cedar::aux::RecorderException, "Cannot change output directory while recorder is running");
  }

  this->mProjectName = boost::filesystem::path(name).stem().string();
}

const std::string& cedar::aux::Recorder::getOutputDirectory() const
{
  return this->mOutputDirectory;
}

void cedar::aux::Recorder::setRecordIntervalTime(const std::string& name, cedar::unit::Time recordInterval)
{
  // throw exception if running
  if (isRunningNolocking())
  {
    CEDAR_THROW(cedar::aux::RecorderException,"Cannot change record interval while recorder is running");
  }
  auto data_spectator = mDataSpectatorCollection.get<cedar::aux::DataSpectator>(name);
  data_spectator->setStepSize(recordInterval);
}

cedar::unit::Time cedar::aux::Recorder::getRecordIntervalTime(const std::string& name) const
{
  auto data_spectator = mDataSpectatorCollection.get<cedar::aux::DataSpectator>(name);
  return data_spectator->getRecordIntervalTime();
}

bool cedar::aux::Recorder::isRegistered(const std::string& name) const
{
  return this->mDataSpectatorCollection.isRegistered(name);
}

void cedar::aux::Recorder::renameRegisteredData(cedar::aux::ConstDataPtr data, const std::string& newName)
{
  // throw exception if running
  if (isRunningNolocking())
  {
    CEDAR_THROW(cedar::aux::RecorderException, "Cannot rename data while recorder is running");
  }
  for (auto data_spectator : mDataSpectatorCollection.getCollection())
  {
    auto casted = boost::static_pointer_cast<cedar::aux::DataSpectator>(data_spectator.second);
    if (casted->getData() == data)
    {
      std::string oldName = data_spectator.first;
      casted->setName(newName);
      mDataSpectatorCollection.remove(oldName);
      mDataSpectatorCollection.addThread(newName, casted);
    }
  }
}

void cedar::aux::Recorder::takeSnapshot()
{
  for (auto data_spectator : mDataSpectatorCollection.getCollection())
  {
    auto casted = boost::static_pointer_cast<cedar::aux::DataSpectator>(data_spectator.second);
    casted->makeSnapshot();
  }
}

std::map<std::string, cedar::unit::Time> cedar::aux::Recorder::getRegisteredData() const
{
  std::map<std::string, cedar::unit::Time> registeredData;

  for (auto data_spectator : mDataSpectatorCollection.getCollection())
  {
    auto casted = boost::static_pointer_cast<cedar::aux::DataSpectator>(data_spectator.second);
    registeredData[data_spectator.first] = casted->getRecordIntervalTime();
  }
  return registeredData;
}


const std::string& cedar::aux::Recorder::getRecorderProjectName()
{
  return mProjectName;
}


void cedar::aux::Recorder::setSubfolder(const std::string& subfolderName)
{
  // throw exception if running
  if (isRunningNolocking())
  {
    CEDAR_THROW(cedar::aux::RecorderException, "Cannot set output directory while recorder is running");
  }

  this->mSubFolder = subfolderName;
}
std::string cedar::aux::Recorder::getTimeStamp()
{
  return cedar::aux::Path::getTimestampForFileName();
}
