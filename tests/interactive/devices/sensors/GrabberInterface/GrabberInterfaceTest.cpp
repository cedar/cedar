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

    File:        main.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Unit test for the behavior of the GrabberInterface.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/devices/sensors/visual/TestGrabber.h"
#include "cedar/auxiliaries/LogFile.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>


//constants
#define FPS_TEST_DURATION_IN_SEC 1
#define CHANNEL_0_NAME "CHANNEL_0_NAME"
#define CHANNEL_1_NAME "CHANNEL_1_NAME"

#define GRABBER_NAME_1 "TestGrabber"
#define CONFIG_FILE_NAME_1 "grabber1.config"

#define GRABBER_NAME_2 "TestGrabber_2"
#define CONFIG_FILE_NAME_2 "grabber2.config"

//--------------------------------------------------------------------------------------------------------------------
//additional namespace to hide global functions
namespace TEST_GRABBER
{
    using namespace boost::posix_time;

    //---------------------------------------------------------------
    // simple function to test the framerate of the grabbing-thread
    //---------------------------------------------------------------

    bool  test_framrate(cedar::dev::sensors::visual::TestGrabber& grabber )
    {

      double fps = grabber.getFps();

      //timestamp for framerate-tests
      ptime test_start = microsec_clock::local_time();
      std::cout << "Start thread . . . " << std::flush;
      grabber.start();
      sleep(FPS_TEST_DURATION_IN_SEC);
      grabber.stop();
      std::cout << "test finished - ";
      ptime test_end = microsec_clock::local_time();

      //calculate real diff:
      time_duration test_duration = test_end - test_start;

      unsigned int real_count = grabber.getCounter();
      double theoretical_count = fps*test_duration.seconds();

      if (theoretical_count == real_count)
      {
        std::cout << "PASSED"<<std::endl;
        return true;
      }
      else
      {
        std::cout << "ERROR\ngot "<<real_count<<" fps, there should be "
                  << theoretical_count<<" fps!"<<std::endl;
        return false;
      }
    } // test_framrate
} // namespace TEST_GRABBER
//--------------------------------------------------------------------------------------------------------------------

// main test program
using namespace TEST_GRABBER;

int main(int , char **)
{

  std::cout << "\n\nInteractive test of GrabberInterface\n";
  std::cout << "------------------------------------\n\n";
  std::cout << "Create a TestGrabber:";

  //create a stereo-grabber (only a dummy grabber for testing)
  cedar::dev::sensors::visual::TestGrabber *grabber_1
        = new cedar::dev::sensors::visual::TestGrabber(CONFIG_FILE_NAME_1,CHANNEL_0_NAME,CHANNEL_1_NAME);

  grabber_1->setName(GRABBER_NAME_1);
  std::cout << "\nGrabber created with name \""<<grabber_1->getName()<<"\"\n";

  //-----------------------------------------------------------
  //Test FPS
  std::cout << "\n\n---------------------------\n";
  std::cout << "Test FPS\n";
  std::cout << "\nSet Fps to \"40\" and start thread\n";
  grabber_1->setFps(40);
  test_framrate(*grabber_1);

  std::cout << "\nSet Fps to \"100\" and start thread\n";
  grabber_1->setFps(100);
  test_framrate(*grabber_1);

  std::cout << "\nSet Fps to \"200\" and start thread\n";
  grabber_1->setFps(200);
  test_framrate(*grabber_1);

  std::cout << "\nSet Fps to \"500\" and start thread\n";
  grabber_1->setFps(500);
  test_framrate(*grabber_1);

  std::cout << "\nSet Fps to \"50000\" and start thread\n";
  grabber_1->setFps(50000);
  test_framrate(*grabber_1);


  //-----------------------------------------------------------
  //Test parameter save and restore
  std::cout << "\n\n---------------------------\n";
  std::cout << "Test default parameter: load and save\n";
  std::cout << "grabber_1->getTestParam "<< grabber_1->getTestParam()<<std::endl;

  //do with another number
  grabber_1->setTestParam(456);
  grabber_1->setFps(50);
  std::cout << "Set TestParam to \"456\" and FPS to \"50\"\n";


  delete grabber_1;
  grabber_1 = NULL;

  usleep(1000);
  //std::cout << "\n\n---------------------------\n";
  cedar::dev::sensors::visual::TestGrabber *grabber_2
    = new cedar::dev::sensors::visual::TestGrabber(CONFIG_FILE_NAME_1,CHANNEL_0_NAME,CHANNEL_1_NAME);

  //grabber_2->setName(GRABBER_NAME_2);
  std::cout << "\nA new grabber created with name "<<grabber_2->getName()<<std::endl;
  std::cout << "grabber_1->getTestParam "<< grabber_2->getTestParam()<<std::endl;
  std::cout << "grabber_1->getFps "<< grabber_2->getFps()<<std::endl;


  //-----------------------------------------------------------
  //Test CTRL-C handler
  std::cout << "\n\n---------------------------\n";
  std::cout << "Test CTRL-C Handler\n\n";

  std::cout << "Creating 3 additional grabbers"<<std::endl;

  std::cout << "\n1.\n";
  cedar::dev::sensors::visual::TestGrabber *grabber_3
    = new cedar::dev::sensors::visual::TestGrabber(CONFIG_FILE_NAME_2,CHANNEL_0_NAME,CHANNEL_1_NAME);
  grabber_3->setName(GRABBER_NAME_2);

  std::cout << "-> A new grabber created with name \""<<grabber_3->getName()<<"\"\n";
  std::cout << "-> grabber_1->getTestParam "<< grabber_3->getTestParam()<<std::endl;
  std::cout << "-> grabber_1->getFps "<< grabber_3->getFps()<<std::endl;

  //create another grabber (a static one)
  std::cout << "\n2.\n";
  cedar::dev::sensors::visual::TestGrabber grabber(CONFIG_FILE_NAME_1,CHANNEL_0_NAME);
  grabber.setName("static grabber");
  std::cout << "-> A new grabber created with name \""<<grabber.getName()<<"\"\n";

  //create another grabber (with a shared pointer)
  std::cout << "\n3.\n";
  cedar::dev::sensors::visual::TestGrabberPtr
        grabber_4(new cedar::dev::sensors::visual::TestGrabber(CONFIG_FILE_NAME_2,CHANNEL_1_NAME ));
  grabber_4->setName("shared_ptr grabber");
  std::cout << "-> A new grabber created with name \""<<grabber_4->getName()<<"\"\n";


  std::cout << "\n\nPress now CTRL-C\n";
  sleep(3*FPS_TEST_DURATION_IN_SEC);

  //no ctrl-c pressed
  std::cout << "\n\nWARNING: No CTRL-C catched - do normal cleanup\n";

  delete grabber_2;
  grabber_2=NULL;

  delete grabber_3;
  grabber_3=NULL;

  //grabber is static
  //grabber_4 is shared pointer

  std::cout << "Tests finished\n";

} //main

