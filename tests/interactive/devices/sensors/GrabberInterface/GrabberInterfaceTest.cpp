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
#include "TestGrabber.h"

// PROJECT INCLUDES
#include <auxiliaries/LogFile.h>
#include <opencv2/opencv.hpp>

// SYSTEM INCLUDES
#include <cstdlib>
#include <iostream>


using namespace std;
using namespace boost::posix_time;
using namespace cedar::dev::sensors::visual;
using namespace cedar::aux;

//constants

#define FPS_TEST_DURATION_IN_SEC 1
#define CHANNEL_0_NAME "CHANNEL_0_NAME"
#define CHANNEL_1_NAME "CHANNEL_1_NAME"

#define GRABBER_NAME_1 "TestGrabber"
#define CONFIG_FILE_NAME_1 "grabber1.config"

#define GRABBER_NAME_2 "NewTestGrabber"
#define CONFIG_FILE_NAME_2 "grabber2.config"


//for functions
namespace TEST_GRABBER
{
	
    //---------------------------------------------------------------
	  // simple function to test the framerate of the grabbing-thread
    //---------------------------------------------------------------

    bool  test_framrate(TestGrabber& grabber )
		{
			
			double fps = grabber.getFps();

			//timestamp for framerate-tests
			ptime start_test, end_test;
			start_test = microsec_clock::universal_time();
      std::cout << "Start thread . . . " << std::flush;
			grabber.start();
			sleep(FPS_TEST_DURATION_IN_SEC);
			grabber.stop();
      std::cout << "test finished - ";
			end_test = microsec_clock::universal_time();
			unsigned int counter = grabber.getCounter();
			
			//calculate real diff:
			//ptime real_test_duration = end_test-start_test;
			
			double theoretical_count = fps*FPS_TEST_DURATION_IN_SEC;
			if (theoretical_count == counter)
			{
			  std::cout << "passed\n";
				return true;
			}
			else
			{
			  std::cout << "error\ngot "<<counter<<" ticks, there should be "<< theoretical_count<<" ticks!\n";
				return false;
			}
			
		}
	


		
	
}


//-----------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------

// main test program
using namespace TEST_GRABBER;



int main(int , char **)
{

  std::cout << "Interactive test of Grabberinterface\n";


  //create a stereo-grabber (only a dummy grabber for testing)
  TestGrabber *grabber_1 = new TestGrabber(CONFIG_FILE_NAME_1,CHANNEL_0_NAME,CHANNEL_1_NAME);

  //-----------------------------------------------------------
  grabber_1->setName(GRABBER_NAME_1);
  std::cout << "\nGrabber created with name "<<grabber_1->getName()<<std::endl;


  //Test FPS
  std::cout << "\nTest FPS\nSet Fps to \"40\" and start thread\n";
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

  std::cout << "\nSet Fps to \"1000\" and start thread\n";
  grabber_1->setFps(1000);
  test_framrate(*grabber_1);

  std::cout << "\nSet Fps to \"10000\" and start thread\n";
  grabber_1->setFps(10000);
  test_framrate(*grabber_1);

  std::cout << "\n\n---------------------------\n";
  std::cout << "Test default parameter: load and save\n";
  std::cout << "grabber_1->getTestParam "<< grabber_1->getTestParam()<<std::endl;

  //do with another number
  grabber_1->setTestParam(456);
  grabber_1->setFps(50);
  std::cout << "Set TestParam to \"456\" and FPS to \"50\"\n";


  delete grabber_1;
  grabber_1 = NULL;

  //SEGFAULT WHEN CREATEING AN OTHER/A NEW INSTANCE

  usleep(1000);
  //std::cout << "\n\n---------------------------\n";
  TestGrabber *grabber_2 = new TestGrabber(CONFIG_FILE_NAME_1,CHANNEL_0_NAME,CHANNEL_1_NAME);
  //grabber_2->setName(GRABBER_NAME_2);
  std::cout << "\nA new grabber created with name "<<grabber_2->getName()<<std::endl;
  std::cout << "grabber_1->getTestParam "<< grabber_2->getTestParam()<<std::endl;
  std::cout << "grabber_1->getFps "<< grabber_2->getFps()<<std::endl;


  std::cout << "\n\n---------------------------\n";
  std::cout << "Test CTRL-C Handler\n";


  TestGrabber *grabber_3 = new TestGrabber(CONFIG_FILE_NAME_2,CHANNEL_0_NAME,CHANNEL_1_NAME);
  grabber_3->setName(GRABBER_NAME_2);
  std::cout << "\nA new grabber created with name "<<grabber_3->getName()<<std::endl;
  std::cout << "grabber_1->getTestParam "<< grabber_3->getTestParam()<<std::endl;
  std::cout << "grabber_1->getFps "<< grabber_3->getFps()<<std::endl;

  std::cout << "\n\nPress now CTRL-C\n";
  sleep(3*FPS_TEST_DURATION_IN_SEC);

  //no ctrl-c pressed
  std::cout << "\n\nERROR: No CTRL-C catched - do cleanup\n";

  delete grabber_2;
  grabber_2=NULL;

  delete grabber_3;
  grabber_3=NULL;

  std::cout << "Tests finished\n";


}

