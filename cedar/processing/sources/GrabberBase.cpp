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
mImage(new cedar::aux::ImageData(cv::Mat::zeros(1, 1, CV_8UC3))),
mRecording(new cedar::aux::BoolParameter(this, "record", false))
{
  //this->getGrabber().reset();

  QObject::connect(mRecording.get(), SIGNAL(valueChanged()), this, SLOT(setRecording()));

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
  if (mpGrabber->isCreated())
  {

    bool want_recording = this->mRecording->getValue();
    bool is_recording = this->mpGrabber->isRecording();

    if (want_recording == is_recording)
    {
      return;
    }

    //check if already recording or not
    std::string info;

    if (want_recording)
    {
      //record
      //!@todo:change to speed from looped trigger or grab via grabbing-thread
      this->mpGrabber->startRecording(15,0,true,false);
      info = "Recording ON";
      cedar::aux::LogSingleton::getInstance()->message(info,"cedar::proc::sources::GrabberSource::setRecording()");

      // check if record is running
      if ( !(this->mpGrabber->isRecording()))
      {
        info = "Error while start recording!";
        cedar::aux::LogSingleton::getInstance()->error(info,"cedar::proc::sources::GrabberSource::setRecording()");
        this->mRecording->setValue(false);
      }
    }
    else
    {
      this->mpGrabber->stopRecording();
      info = "Recording OFF";
      cedar::aux::LogSingleton::getInstance()->message(info,"cedar::proc::sources::GrabberSource::setRecording()");
    }

  }
}

//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::GrabberBase::saveSnapshot()
{
  if (this->getGrabber()->isCreated())
  {
//    std::string snapshot_name = this->mSnapshotName->getPath();
//    if (snapshot_name != "")
//    {
//      this->mpGrabber->setSnapshotName(snapshot_name);
//    }
    this->getGrabber()->saveSnapshotAllCams();
  }
  else
  {
    const std::string message = "Can't save a snapshot: No valid channels";
    cedar::aux::LogSingleton::getInstance()->warning(message,"cedar::proc::sources::GrabberBase::saveSnapshot()");
  }
}
