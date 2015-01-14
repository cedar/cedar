/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        TestObject.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 02 15

    Description: Header for the @em cedar::tests::unit::dev::TestKinematicChain class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_TESTS_UNIT_DEV_KINEMATIC_CHAIN_H
#define CEDAR_TESTS_UNIT_DEV_KINEMATIC_CHAIN_H

// CEDAR INCLUDES
#include "cedar/devices/KinematicChain.h"

// SYSTEM INCLUDES

/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class TestKinematicChain : public cedar::dev::KinematicChain
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor that gets a configuration file name.
  TestKinematicChain();

  //!@brief Destructor
  virtual ~TestKinematicChain();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief get current state of a single joint angle
   *
   * @param index    specifies the joint
   * @return    joint angle value
   */
  double getJointAngle(unsigned int index) const;

  /*!@brief set current state of a single joint angle
   *
   * @param index    specifies the joint
   * @param angle    new joint angle value
   */
  virtual void setJointAngle(unsigned int index, double angle);

  /*!@brief check whether the kinematic chain is currently responsive to movement commands
   *
   * @return    state
   */
  virtual bool isMovable() const;


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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! test member bool

private:
  // none yet

}; // class TestKinematicChain

#endif // CEDAR_TESTS_UNIT_DEV_KINEMATIC_CHAIN_H

