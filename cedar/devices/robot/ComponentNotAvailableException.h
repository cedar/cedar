/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        ComponentNotAvailableException.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 11 08

    Description: Exception that can be thrown when a component is not available within a certain robot configuration.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_COMPONENT_NOT_AVAILABLE_EXCEPTION_BASE_H
#define CEDAR_DEV_ROBOT_COMPONENT_NOT_AVAILABLE_EXCEPTION_BASE_H

// CEDAR INCLUDES
#include "cedar/devices/robot/namespace.h"
#include "cedar/auxiliaries/ExceptionBase.h"

// SYSTEM INCLUDES

/*!@brief Class for an exception, that is thrown when a non-existing component is requested from a robot. */
class cedar::dev::robot::ComponentNotAvailableException : public cedar::aux::ExceptionBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ComponentNotAvailableException(void);

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
  // none yet (hopefully never!)
protected:
  // none yet
private:
  // none yet
}; // class cedar::dev::robot::ComponentNotAvailableException
#endif // CEDAR_DEV_ROBOT_COMPONENT_NOT_AVAILABLE_EXCEPTION_BASE_H
