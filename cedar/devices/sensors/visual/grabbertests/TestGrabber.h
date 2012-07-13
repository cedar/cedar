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
    Date:        2012 04 23

    Description: Header for the @em @em cedar::dev::sensors::visual::TestGrabber class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_TEST_GRABBER_H
#define CEDAR_DEV_SENSORS_VISUAL_TEST_GRABBER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/Grabber.h"
#include "cedar/auxiliaries/StringParameter.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>

/*! @brief A simple Grabber class for testing the Grabber interface
 *
 *  This grabber class is used to test the grabber interface. It
 *  creates a Grabber with a TestParam (default-value 123) and FPS set to 15
 *
 *  @remarks For grabber developers<br>
 *    This class can also be used as a template to create other classes derived from GrabberInstance
 *
 */
class cedar::dev::sensors::visual::TestGrabber
:
public cedar::dev::sensors::visual::Grabber
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //!@cond SKIPPED_DOCUMENTATION
public:

  /*! @struct TestChannel
   *  @brief Additional data of a grabbing channel
   *  @remarks For grabber developers<br>
   *    You don't have to create an extended channel structure, until you need more channel data.
   *    But when, then you have to implement the onAddChannel() member function as well
   */
  struct TestChannel
  :
  cedar::dev::sensors::visual::Grabber::Channel
  {
  public:
    TestChannel(const std::string& defaultFileName = "./default_parameter_value_for_processingGui_instantiation")
    :
    cedar::dev::sensors::visual::Grabber::Channel(),
    _mpSourceFileName(new cedar::aux::StringParameter(this, "name_of_parameter", defaultFileName))
    {
    }

    //! @brief The filenames
    cedar::aux::StringParameterPtr _mpSourceFileName;
  };

  CEDAR_GENERATE_POINTER_TYPES(TestChannel);

  //!@endcond

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief  Constructor for a single channel grabber
   *  @param grabberName  Name of the grabber
   *  @param sourceFileName Filename to grab from (here only for demonstration and testing)
   */
  TestGrabber
  (
    const std::string& sourceFileName = "./default_parameter_value_for_code_instantiation",
    const std::string& grabberName = "TestGrabber"
  );

  /*! @brief Constructor for a stereo channel grabber
   *  @param grabberName  Name of the grabber
   *  @param sourceFileName0 Filename to grab from for channel 0 (here only for demonstration and testing)
   *  @param sourceFileName1 Filename to grab from for channel 1 (here only for demonstration and testing)
   */
  TestGrabber
  (
    const std::string& sourceFileName0 = "./default_parameter_value_for_code_instantiation_channel0",
    const std::string& sourceFileName1 = "./default_parameter_value_for_code_instantiation_channel1",
    const std::string& grabberName = "StereoTestGrabber"
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

  /*! @brief Simple get-function for the test parameter
   *
   */
  int getTestParam();

  /*! @brief Simple set-function for the test parameter
   *
   */
  void setTestParam(int mTest);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //derived from Grabber
  bool onInit();
  void onCleanUp();
  void onUpdateSourceInfo(unsigned int channel);
  bool onGrab();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  ///! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class TestChannelPtr
  inline TestChannelPtr getTestChannel(unsigned int channel)
  {
    return boost::static_pointer_cast<TestChannel>
           (
             cedar::dev::sensors::visual::Grabber::_mChannels->at(channel)
           );
  }

  ///! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class TestChannelPtr
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

  //!@brief The counter
  unsigned int mCounter;

  //!@brief The test parameter
  int _mTest;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

  //! @brief The filename of the picture you want to grab from
  //cedar::aux::FileParameterPtr _mSourceFileName;

}; // class cedar::dev::sensors::visual::TestGrabber

#endif //CEDAR_DEV_SENSORS_VISUAL_TEST_GRABBER_H
