/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        exceptions.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 21

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_EXCEPTIONS_H
#define CEDAR_PROC_GUI_EXCEPTIONS_H

#include "cedar/processing/gui/namespace.h"
#include "cedar/auxiliaries/ExceptionBase.h"

/*!@brief TODO
 */
class cedar::proc::gui::InvalidStepNameException : public cedar::aux::ExceptionBase
{
public:
  InvalidStepNameException()
  {
    this->mType = "InvalidStepNameException";
  }
}; // class cedar::proc::InvalidStepNameException

/*!@brief TODO
 */
class cedar::proc::gui::InvalidTriggerNameException : public cedar::aux::ExceptionBase
{
public:
  InvalidTriggerNameException()
  {
    this->mType = "InvalidTriggerNameException";
  }
}; // class cedar::proc::InvalidTriggerNameException

/*!@brief This exception is thrown whenever a mode is not known to cedar.
 */
class cedar::proc::gui::InvalidModeException : public cedar::aux::ExceptionBase
{
public:
  InvalidModeException()
  {
    this->mType = "InvalidModeException";
  }
}; // class cedar::proc::InvalidModeException

#endif // CEDAR_PROC_GUI_EXCEPTIONS_H
