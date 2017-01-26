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

    File:        VideoSink.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2014 06 24

    Description: Source file for the class cedar::proc::sinks::VideoSink.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/sinks/VideoSink.h"
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/StepTime.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/opencv_helper.h"

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
      new ElementDeclarationTemplate<cedar::proc::sinks::VideoSink>
      (
        "Sinks",
        "cedar.processing.VideoSink"
      )
    );
    declaration->setIconPath(":/steps/video_sink.svg");
    declaration->setDescription
    (
      "Writes its input to a video file."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::sinks::VideoSink::VideoSink()
:
cedar::proc::Step(true),
mCurrentFrameDuration(0.0),
_mOutputFileName(new cedar::aux::FileParameter(this, "output file name", cedar::aux::FileParameter::WRITE)),
_mFrameRate(new cedar::aux::DoubleParameter(this, "frame rate", 30.0))
{
  auto input_slot = this->declareInput("input");

  cedar::proc::typecheck::Matrix matrix_check;
  matrix_check.addAcceptedDimensionality(2);
  matrix_check.addAcceptedNumberOfChannels(3);
  matrix_check.addAcceptedType(CV_8UC3);
  input_slot->setCheck(matrix_check);
}

cedar::proc::sinks::VideoSink::~VideoSink()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sinks::VideoSink::onStart()
{
  mVideoWriter.open
               (
                 _mOutputFileName->getValue().absolutePath().toStdString(),
#if CEDAR_OPENCV_MAJOR_VERSION >= 3
                 cv::VideoWriter::fourcc('P','I','M','1')
#else
                 static_cast<unsigned int>(CV_FOURCC('P','I','M','1'))
#endif
                 ,
                 _mFrameRate->getValue(),
                 this->getInput("input")->getData<cv::Mat>().size(),
                 true
               );
}

void cedar::proc::sinks::VideoSink::onStop()
{
  mVideoWriter = cv::VideoWriter();
}

void cedar::proc::sinks::VideoSink::compute(const cedar::proc::Arguments& arguments)
{
  try
  {
    if (mVideoWriter.isOpened())
    {
      const cedar::proc::StepTime& step_time = dynamic_cast<const cedar::proc::StepTime&>(arguments);
      const cedar::unit::Time& time = step_time.getStepTime();
      const double& frame_duration = 1.0/_mFrameRate->getValue();
      double elapsed_time = time / (1.0 * cedar::unit::second);
      mCurrentFrameDuration += elapsed_time;
      if (mCurrentFrameDuration > frame_duration)
      {
        mVideoWriter << this->getInput("input")->getData<cv::Mat>();
        mCurrentFrameDuration -= frame_duration;
      }
    }
  }
  catch (const std::bad_cast& e)
  {
    CEDAR_THROW(cedar::proc::InvalidArgumentsException, "Bad arguments passed to video writer. Expected StepTime.");
  }
}
