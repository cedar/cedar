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

    File:        TestChannel.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 09 28

    Description: Class TestChannel

    Credits:

======================================================================================================================*/

#ifndef CEDAR_TESTING_DEV_TEST_CHANNEL_H
#define CEDAR_TESTING_DEV_TEST_CHANNEL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/GrabberChannel.h"
#include "cedar/auxiliaries/IntParameter.h"

// FORWARD DECLARATIONS
#include "cedar/testingUtilities/devices/TestChannel.fwd.h"
#include "cedar/testingUtilities/devices/TestGrabber.fwd.h"

// SYSTEM INCLUDES


//!@brief TestChannel contains additional data of a picture grabbing channel
class cedar::test::dev::TestChannel
:
public cedar::dev::sensors::visual::GrabberChannel
{
  //!@brief friend class of TestGrabber for direct access to the members
  friend class cedar::test::dev::TestGrabber;
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  TestChannel(const std::string& fileName = "")
  :
  cedar::dev::sensors::visual::GrabberChannel(),
  _mSourceFileName(new cedar::aux::FileParameter(this, "filename", cedar::aux::FileParameter::READ, fileName))
  {
  };

  //!@brief Destructor
  virtual ~TestChannel()
  {
  };

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
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The test parameter
  cedar::aux::FileParameterPtr _mSourceFileName;

private:
  // none yet

}; // class cedar::testing::dev::TestChannel

#endif // CEDAR_TESTING_DEV_TEST_CHANNEL_H


