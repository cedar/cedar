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

    File:        TestVisualization.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 19

    Description: Header for the @em cedar::tests::unit::aux::gl::TestVisualization class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_TESTS_UNIT_AUX_OBJECT_VISUALIZATION_TEST_VISUALIZATION_H
#define CEDAR_TESTS_UNIT_AUX_OBJECT_VISUALIZATION_TEST_VISUALIZATION_H

// LOCAL INCLUDES
#include "namespace.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/gl/ObjectVisualization.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.h"

// SYSTEM INCLUDES

/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::tests::unit::aux::gl::TestVisualization : public cedar::aux::gl::ObjectVisualization
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor that gets a configuration file name.
  TestVisualization(cedar::aux::LocalCoordinateFramePtr pLocalCoordinateFrame);

  //!@brief Destructor
  virtual ~TestVisualization();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void draw();


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
  // none yet (hopefully never!)
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
  //! test member bool

private:
  // none yet

}; // class cedar::tests::unit::aux::gl::Object::TestObject

#endif // CEDAR_TESTS_UNIT_AUX_CONFIGURATION_INTERFACE_TEST_CLASS_H

