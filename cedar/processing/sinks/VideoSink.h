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

    File:        VideoSink.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2014 06 24

    Description: Header file for the class cedar::proc::sinks::VideoSink.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_SINKS_VIDEO_SINK_H
#define CEDAR_PROC_SINKS_VIDEO_SINK_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/FileParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/sinks/VideoSink.fwd.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>


/*!@brief Writes its input into a video file.
 */
class cedar::proc::sinks::VideoSink : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  VideoSink();

  //!@brief Destructor
  virtual ~VideoSink();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Writes the output matrix to a video file.
  void compute(const cedar::proc::Arguments& arguments);
  void onStart();
  void onStop();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cv::VideoWriter mVideoWriter;
  double mCurrentFrameDuration;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::aux::FileParameterPtr _mOutputFileName;
  cedar::aux::DoubleParameterPtr _mFrameRate;

}; // class cedar::proc::sinks::VideoSink

#endif // CEDAR_PROC_SINKS_VIDEO_SINK_H

