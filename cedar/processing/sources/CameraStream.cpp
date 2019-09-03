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

    File:        CameraStream.cpp

    Maintainer:  Raul Grieben
    Email:       raul.grieben@ini.ruhr-uni-bochum.de
    Date:        2019 09 03

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/sources/CameraStream.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/Arguments.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration
    (
      new ElementDeclarationTemplate<cedar::proc::sources::CameraStream>
      (
        "Sources",
        "cedar.processing.sources.CameraStream"
      )
    );
    declaration->setIconPath(":/steps/camera_grabber.svg");
      declaration->setDescription
              (
                      "A source that reads images from a camera stream."
              );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::sources::CameraStream::CameraStream()
        :
        cedar::proc::Step(true),
        mStream(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
        _mURL(new cedar::aux::StringParameter(this, "URL", "http://192.168.25.1:8080/?action=stream&file=stream.mjpg"))
        //,_mBufferSize(new cedar::aux::IntParameter(this, "Buffer Size", 1))
{
    this->declareOutput("stream", this->mStream);
    QObject::connect(_mURL.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
    this->cap = new cv::VideoCapture(this->_mURL->getValue());
}

cedar::proc::sources::CameraStream::~CameraStream()
{
    this->cap->release();
    delete this->cap;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::proc::sources::CameraStream::compute(const cedar::proc::Arguments&)
{
    //cap->set(cv::CAP_PROP_BUFFERSIZE, _mBufferSize->getValue());
    if(!this->cap->isOpened()){
        this->setState(cedar::proc::Step::STATE_EXCEPTION, "Error opening video stream");
        return;
    }
    cv::Mat frame;
    if (cap->read(frame))
    {
        this->mStream->setData(frame);
    }
}

void cedar::proc::sources::CameraStream::recompute()
{
    this->cap->release();
    this->cap = new cv::VideoCapture(this->_mURL->getValue());
    this->onTrigger();
}
