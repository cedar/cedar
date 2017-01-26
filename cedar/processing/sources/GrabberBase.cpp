/*======================================================================================================================

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
#include "cedar/auxiliaries/annotation/ColorSpace.h"

// SYSTEM INCLUDES
#include <string>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::sources::GrabberBase::GrabberBase(bool looped)
:
cedar::proc::Step(looped),
mImage(new cedar::aux::MatData(cv::Mat())),
mRecording(new cedar::aux::BoolParameter(this, "record", false))
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  // Recording
  QObject::connect(mRecording.get(), SIGNAL(valueChanged()), this, SLOT(setRecording()));

  // Snapshot as an action
  this->registerFunction("save snapshot", boost::bind(&cedar::proc::sources::GrabberBase::saveSnapshot, this));

  this->annotateImage();
}

cedar::proc::sources::GrabberBase::~GrabberBase()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sources::GrabberBase::annotateImage()
{
  cedar::aux::annotation::ColorSpacePtr color_space;
  switch (this->mImage->getData().channels())
  {
    case 4:
      color_space = cedar::aux::annotation::ColorSpace::bgra();
      break;

    case 3:
      color_space = cedar::aux::annotation::ColorSpace::bgr();
      break;

    case 1:
      color_space = cedar::aux::annotation::ColorSpace::gray();
      break;

    default:
      // this should not happen.
      CEDAR_ASSERT(false);
  } // switch

  this->mImage->setAnnotation(color_space);
}

//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::GrabberBase::setRecording()
{
  if (mpGrabber->isCreated())
  {

    bool want_recording = this->mRecording->getValue();
    bool is_recording = this->mpGrabber->isRecording();

    //check if already recording or not
    if (want_recording == is_recording)
    {
      return;
    }

    std::string info;
    if (want_recording)
    {
      //record

      //!@todo: change to speed from looped trigger (not possible to get this) or grab via grabbing-thread
      //!@todo: record with given encoding from enum RecordType

      // try to get measured fps (but this could only be calculated if setIsGrabbing is set to true)
      double fps = this->mpGrabber->getMeasuredFramerate();
      if (fps < 1)
      {
        fps = 15;
      }

      this->mpGrabber->startRecording(fps,0,true,false);
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
    this->getGrabber()->saveSnapshotAllCams();
  }
  else
  {
    const std::string message = "Can't save a snapshot: No valid channels";
    cedar::aux::LogSingleton::getInstance()->warning(message,"cedar::proc::sources::GrabberBase::saveSnapshot()");
  }
}
