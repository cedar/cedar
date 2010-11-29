/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        TestRobot.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 11 08

 ----- Description: Header for the @em cedar::tests::unit::dev::robot::Robot::TestRobot class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_TESTS_UNIT_DEV_ROBOT_ROBOT_TEST_ROBOT_H
#define CEDAR_TESTS_UNIT_DEV_ROBOT_ROBOT_TEST_ROBOT_H

// LOCAL INCLUDES
#include "namespace.h"

// PROJECT INCLUDES
#include "cedar/devices/robot/Robot.h"
#include "cedar/devices/robot/Component.h"

// SYSTEM INCLUDES
#include <string>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::tests::unit::dev::robot::Robot::TestRobot : public cedar::dev::robot::Robot
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor that gets a configuration file name.
  TestRobot(void);

  //!@brief Destructor
  virtual ~TestRobot(void);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

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
public:
  virtual cedar::dev::robot::ComponentPtr createComponent(const std::string& rComponentName);

protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::tests::unit::aux::dev::robot::Robot::TestRobot

#endif // CEDAR_TESTS_UNIT_DEV_ROBOT_ROBOT_TEST_ROBOT_H

