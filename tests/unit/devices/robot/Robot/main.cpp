/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 11 08

 ----- Description: Implements all unit tests for the @em cedar::dev::robot::Robot class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "TestRobot.h"

// PROJECT INCLUDES
#include "cedar/devices/robot/Robot.h"
#include "cedar/auxiliaries/LogFile.h"

// SYSTEM INCLUDES
#include <string>


using namespace cedar::tests::unit::dev::robot::Robot;

int main()
{
  cedar::aux::LogFile logFile("unitTest_robot.log");

  TestRobot robot;
  robot.getComponent("TestComponent1");
  robot.getComponent("TestComponent2");

  logFile.close();

  return 0;
}
