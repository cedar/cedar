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

    File:        exceptions.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 17

    Description:

    Credits:

======================================================================================================================*/

#include "auxiliaries/exceptions.h"

cedar::aux::UnhandledTypeException::UnhandledTypeException()
{
  this->mType = "UnhandledTypeException";
}

cedar::aux::UnknownTypeException::UnknownTypeException()
{
  this->mType = "UnknownTypeException";
}

cedar::aux::ParameterNotFoundException::ParameterNotFoundException()
{
  this->mType = "ParameterNotFoundException";
}

cedar::aux::DuplicateNameException::DuplicateNameException()
{
  this->mType = "DuplicateNameException";
}

cedar::aux::UnknownNameException::UnknownNameException()
{
  this->mType = "UnknownNameException";
}

cedar::aux::UnhandledValueException::UnhandledValueException()
{
  this->mType = "UnhandledValueException";
}

cedar::aux::RangeException::RangeException()
{
  this->mType = "RangeException";
}

cedar::aux::NoDefaultException::NoDefaultException()
{
  this->mType = "NoDefaultException";
}