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

    File:        LoopMode.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 06 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_LOOP_MODE_H
#define CEDAR_AUX_LOOP_MODE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumBase.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/LoopMode.fwd.h"
#include "cedar/auxiliaries/EnumType.fwd.h"

// SYSTEM INCLUDES


/*!@brief An enum class for the different modes of looping in LoopedThread.
 */
class cedar::aux::LoopMode
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! The type of the enum values.
  typedef cedar::aux::EnumId Id;

  //! The pointer type of the enum base object.
  typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Initializes the enum values.
   */
  static void construct();

  /*!@brief Returns a reference to the enum base object.
   */
  static const cedar::aux::EnumBase& type();

  /*!@brief Returns a pointer to the enum base object.
   */
  static const cedar::aux::LoopMode::TypePtr& typePtr();

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
  //! Thread runs as fast as possible, elapsed time is measured.
  static const Id RealTime = 0;

  /*! Thread calls step function in a fixed time step and waits for next iteration.
   *  If step takes too long, steps are skipped accordingly.
   */
  static const Id Fixed = 1;

  /*! Thread calls step function in a fixed time step and waits for next iteration.
   * If step takes too long, the next iteration is executed as soon as possible.
   */
  static const Id FixedAdaptive = 2;

  /*! Thread runs in arbitrary time but does not measure time. Instead, a fixed value is passed to the step function.
   */
  static const Id Simulated = 3;

  /*! Sleep for (unmodifyable) step time. Pass the real elapsed time to down to step())
   */
  static const Id RealDT = 4;


  /*! Sleep for (globally scaled) step time. Pass a fake time to down to step())
   */
  static const Id FakeDT = 6;

protected:
  // none yet

private:
  //! The enum object.
  static cedar::aux::EnumType<cedar::aux::LoopMode> mType;

}; // class cedar::aux::conv::LoopMode

#endif // CEDAR_AUX_LOOP_MODE_H

