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

    File:        PythonScriptLooped.h

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2019 04 01

    Description: Header file for the class PythonScriptLooped.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_PYTHON_SCRIPT_LOOPED_H
#define CEDAR_PROC_STEPS_PYTHON_SCRIPT_LOOPED_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"
#ifdef CEDAR_USE_PYTHON

// CEDAR INCLUDES
#include "cedar/processing/steps/PythonScript.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/PythonScriptLooped.fwd.h"


/*!@brief Looped (Triggered) version of the PythonScript step
 */
class cedar::proc::steps::PythonScriptLooped : public cedar::proc::steps::PythonScript
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------


  Q_OBJECT

  
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  PythonScriptLooped();

  //!@brief Destructor
  ~PythonScriptLooped();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  
public slots:
  
signals:

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------

public:
  
protected:
  // none yet
private:

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  
protected:
  // none yet

private:

};

#endif // CEDAR_USE_PYTHON

#endif // CEDAR_PROC_STEPS_PYTHON_SCRIPT_LOOPED_H

