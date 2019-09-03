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

    File:        PythonScriptLooped.cpp

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2019 04 01

    Description: Source file for the class PythonScriptLooped.

    Credits:

======================================================================================================================*/

/*
The MIT License (MIT)

Copyright (c) 2014 Yati Sagade

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


// CEDAR CONFIGURATION
#include "cedar/configuration.h"
#ifdef CEDAR_USE_PYTHON

// CLASS HEADER
#include "cedar/processing/steps/PythonScriptLooped.h"

#include "cedar/processing/ElementDeclaration.h"

// SYSTEM INCLUDES
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>


//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration
    (
      new ElementDeclarationTemplate<cedar::proc::steps::PythonScriptLooped>
      (
        "Programming",
        "cedar.processing.steps.PythonScriptLooped"
      )
    );
    declaration->setIconPath(":/steps/python_script.svg");
    declaration->setDescription
    (
      "In-Runtime Python Script-editor to develop new steps from scratch. (Looped)"
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

cedar::proc::steps::PythonScriptLooped::PythonScriptLooped()
:
cedar::proc::steps::PythonScript(true)
{
  
}

cedar::proc::steps::PythonScriptLooped::~PythonScriptLooped()
{
  
}

#endif // CEDAR_USE_PYTHON
