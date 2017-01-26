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

    File:        TestComponent.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 11 08

    Description: Header for the @em TestComponent class.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_TESTS_UNIT_DEV_ROBOT_TEST_COMPONENT_H
#define CEDAR_TESTS_UNIT_DEV_ROBOT_TEST_COMPONENT_H

// LOCAL INCLUDES
#include "unit/devices/Robot/namespace.h"

// PROJECT INCLUDES
#include "cedar/devices/Robot.h"
#include "cedar/devices/Component.h"
#include "cedar/auxiliaries/UIntParameter.h"

// SYSTEM INCLUDES
#include <string>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::tests::unit::dev::Robot::TestComponent : public cedar::dev::Component
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor.
  TestComponent();

  //!@brief Destructor
  virtual ~TestComponent();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  bool applyBrakeNowController();
  bool applyBrakeSlowlyController();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

protected:
  // none yet
private:

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  cedar::aux::UIntParameterPtr _mParameter1;
  cedar::aux::UIntParameterPtr _mParameter2;
protected:
  // none yet
private:
  // none yet

}; // class cedar::tests::unit::aux::dev::Robot::TestComponent

#endif // CEDAR_TESTS_UNIT_DEV_ROBOT_TEST_COMPONENT_H

