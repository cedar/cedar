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

    File:        TestGrabber.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Header for the @em @em cedar::dev::sensors::visual::TestGrabber class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_TEST_GRABBER_H
#define CEDAR_DEV_SENSORS_VISUAL_TEST_GRABBER_H

// LOCAL INCLUDES
#include "cedar/devices/sensors/visual/GrabberInterface.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>


//definition of the namespace for the testgrabber
namespace cedar
{
  namespace dev
  {
    namespace sensors
      {
      namespace visual
      {
        class TestGrabber;
        //!@brief smart pointer for the TestGrabber class
        typedef boost::shared_ptr<TestGrabber> TestGrabberPtr;
      }
    }
  }
}





/*! \brief A simple Grabber class for testing the Grabber interface
 *
 *  This grabber class is used to test the grabber interface. It
 *  creates a Grabber with a TestParam (default-value 123) and FPS set to 15
 *
 *  \remarks
 *    This class can also be used as a template to create other classes derived from GrabberInstance
 *
 */
class cedar::dev::sensors::visual::TestGrabber
:
public cedar::dev::sensors::visual::GrabberInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! \brief The constructor for a single channel grabber.
   *  \param configFileName Filename for a file, where the configuration parameters should be stored
   *  \param ChannelName  Channel to grab from
   */
  TestGrabber(
               std::string configFileName,
               std::string ChannelName
             );

  /*! \brief The constructor for a stereo grabber.
   *  \param configFileName Filename for a file, where the configuration parameters should be stored
   *  \param ChannelName0  Channel one to grab from
   *  \param ChannelName1  Channel two to grab from
   */
  TestGrabber(
               std::string configFileName,
               std::string ChannelName0,
               std::string ChannelName1
             );

  //!@brief Destructor
  ~TestGrabber();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! \brief Read the counter and reset it to zero
   *  \remarks The counter counts the framerate (i.e. the call of the grab() method) of LoopedThread.
   */
  unsigned int getCounter();

  /*! \brief Simple get-function for the test parameter
   *
   */
  int getTestParam();

  /*! \brief Simple set-function for the test parameter
   *
   */
  void setTestParam(int mTest);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //derived from GrabberInterface
  bool onInit();
  void onCleanUp();
  bool onDeclareParameters();
  std::string onGetSourceInfo(unsigned int channel) const;
  bool onGrab();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  //!@brief The vector where the channelnames stored in
  std::vector<std::string> mChannelVector;

  //!@brief The counter
  unsigned int mCounter;

  //!@brief The test parameter
  int _mTest;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::sensors::visual::TestGrabber

#endif //CEDAR_DEV_SENSORS_VISUAL_TEST_GRABBER_H
