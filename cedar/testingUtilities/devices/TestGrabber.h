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

    File:        TestGrabber.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 04 23

    Description: Header for the @em cedar::testing::dev::TestGrabber class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_TESTING_DEV_TEST_GRABBER_H
#define CEDAR_TESTING_DEV_TEST_GRABBER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/testingUtilities/devices/TestChannel.h"
#include "cedar/devices/sensors/visual/Grabber.h"
#include "cedar/auxiliaries/StringParameter.h"

// FORWARD DECLARATIONS
#include "cedar/testingUtilities/devices/TestGrabber.fwd.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <string>

/*! @class cedar::test::dev::TestGrabber
 *
 *  @brief A simple Grabber class for testing the Grabber interface
 *
 *   This grabber class is used to test the grabber interface. It
 *   creates a Grabber with a TestParam (default-value 123) and FPS set to 15
 *
 *  @remarks For grabber developers<br>
 *    This class can also be used as a template to create other classes derived from GrabberInstance
 *
 */
class cedar::test::dev::TestGrabber
:
public cedar::dev::sensors::visual::Grabber
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief  Constructor for a single channel grabber
   *  @param sourceFileName Filename to grab from (here only for demonstration and testing)
   */
  TestGrabber
  (
    const std::string& sourceFileName = "./default_parameter_value_for_code_instantiation"
  );

  /*! @brief Constructor for a stereo channel grabber
   *  @param sourceFileName0 Filename to grab from for channel 0 (here only for demonstration and testing)
   *  @param sourceFileName1 Filename to grab from for channel 1 (here only for demonstration and testing)
   */
  TestGrabber
  (
    const std::string& sourceFileName0,
    const std::string& sourceFileName1
  );

  //!@brief Destructor
  ~TestGrabber();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief Read the counter and reset it to zero
   *  @remarks The counter counts the framerate (i.e. the call of the grab() method) of LoopedThread.
   */
  unsigned int getCounter();

  /*! @brief Get the filename of the channel (example)
   *
   */
  std::string getSourceFileName(unsigned int channel) const;

  /*! @brief Simple get-function for the test parameter
   *
   */
  int getTestParam() const;


  /*! @brief Simple set-function for the test parameter
   *
   */
  void setTestParam(int testParameter);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  // derived from Grabber
  void onCreateGrabber();
  void onCloseGrabber();
  void onCleanUp();
  void onGrab(unsigned int channel);
  std::string onGetSourceInfo(unsigned int channel);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class TestChannelPtr
  inline TestChannelPtr getTestChannel(unsigned int channel)
  {
    return boost::static_pointer_cast<TestChannel>
           (
             cedar::dev::sensors::visual::Grabber::_mChannels->at(channel)
           );
  }

  //! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class TestChannelPtr
  inline ConstTestChannelPtr getTestChannel(unsigned int channel) const
  {
    return boost::static_pointer_cast<const TestChannel>
       (
         cedar::dev::sensors::visual::Grabber::_mChannels->at(channel)
       );
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:

  //! @brief The counter
  unsigned int mCounter;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! @brief The test parameter
  cedar::aux::IntParameterPtr _mTestParam;

private:
  // none yet

}; // class cedar::test::dev::TestGrabber

#endif // CEDAR_TESTING_DEV_TEST_GRABBER_H
