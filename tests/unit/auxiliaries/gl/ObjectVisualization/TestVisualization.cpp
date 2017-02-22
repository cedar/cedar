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

    File:        TestVisualization.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 19

    Description: Implementation of the @em cedar::tests::unit::aux::gl::TestVisualization class.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "TestVisualization.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
cedar::tests::unit::aux::gl::TestVisualization::TestVisualization(cedar::aux::LocalCoordinateFramePtr pLocalCoordinateFrame)
:
cedar::aux::gl::ObjectVisualization(pLocalCoordinateFrame)
{
  
}

//! destructor
cedar::tests::unit::aux::gl::TestVisualization::~TestVisualization()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::tests::unit::aux::gl::TestVisualization::draw()
{

}
