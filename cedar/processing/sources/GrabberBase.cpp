/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        GrabberBase.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.ruhr-uni-bochum.d
    Date:        2012 05 23

    Description: The implementation of the GrabberBase class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/sources/GrabberBase.h"


// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::sources::GrabberBase::GrabberBase()
:
cedar::proc::Step(false, true),
mImage(new cedar::aux::ImageData(cv::Mat::zeros(1, 1, CV_32F))),
mRecording(new cedar::aux::BoolParameter(this, "record", false)),
mRecordName(new cedar::aux::FileParameter(this, "recordName", cedar::aux::FileParameter::WRITE, "")),
mSnapshotName(new cedar::aux::FileParameter(this, "snapshotName", cedar::aux::FileParameter::WRITE, "")),
_mConfigurationFileName(new cedar::aux::FileParameter(this, "config",cedar::aux::FileParameter::READ,""))
{
  mRecordName->setValue("./video.avi");
  mSnapshotName->setValue("./picture.png");
  mGrabber.reset();
  QObject::connect(mRecording.get(), SIGNAL(valueChanged()), this, SLOT(setRecording()));
  QObject::connect(_mConfigurationFileName.get(), SIGNAL(valueChanged()), this, SLOT(setConfigurationFileName()));

  //Snapshot as an action
  this->registerFunction("save snapshot", boost::bind(&cedar::proc::sources::GrabberBase::saveSnapshot, this));
}

cedar::proc::sources::GrabberBase::~GrabberBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::GrabberBase::setRecording()
{
  if (mGrabber)
  {
    //check if already recording or not
    std::string info;
    bool rec = this->mRecording->getValue();
    if (rec)
    {
      //info = "Starting record from grabber";
      //cedar::aux::LogSingleton::getInstance()->message(info,"cedar::proc::sources::GrabberSource::setRecording()");
      //!@todo: change to right values read from config, parameter or what ever. Add changeable FourCC

      //set recordname
      std::string record_name = this->mRecordName->getPath();
      if (record_name != "")
      {
        this->mGrabber->setRecordName(record_name);
      }

      //record
      this->mGrabber->startRecording(15,0,true,false);
      info = "Recording ON";
    }
    else
    {
      //info = "Stopping record from grabber";
      //cedar::aux::LogSingleton::getInstance()->message(info,"cedar::proc::sources::GrabberSource::setRecording()");
      this->mGrabber->stopRecording();
      info = "Recording OFF";
    }
    cedar::aux::LogSingleton::getInstance()->message(info,"cedar::proc::sources::GrabberSource::setRecording()");

    // check if record is running
    if (rec && !(this->mGrabber->isRecording()))
    {
      info = "Error while starting recording!";
      cedar::aux::LogSingleton::getInstance()->error(info,"cedar::proc::sources::GrabberSource::setRecording()");
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::GrabberBase::saveSnapshot()
{
  //bool save_snapshot = this->mSaveSnapshot->getValue();

  if (mGrabber)
  {
    std::string snapshot_name = this->mSnapshotName->getPath();
    if (snapshot_name != "")
    {
      this->mGrabber->setSnapshotName(snapshot_name);
    }
    this->mGrabber->saveSnapshotAllCams();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::GrabberBase::createGrabber()
{
  // destroy the old grabber (if any), in order to save the configuration
  if (mGrabber)
  {
    const std::string message = "Old grabber deleted";
    cedar::aux::LogSingleton::getInstance()->debugMessage(message,"cedar::proc::sources::GrabberBase::createGrabber()");
  }
  mGrabber.reset();

  onCreateGrabber();

  if (mGrabber)
  {
    //@!todo Values doesn't change after re-selection of the grabber
    this->mSnapshotName->setValue(this->mGrabber->getSnapshotName());
    this->mRecordName->setValue(this->mGrabber->getRecordName());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::GrabberBase::setConfigurationFileName()
{
  if (mGrabber)
  {
    //@!todo: ask user if grabber should be recreated, using new configfile
  }
}