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

    File:        exceptions.cpp

    Maintainer:  Oliver Lomp

    Email:       oliver.lomp@ini.ruhr-uni-bochum.de

    Date:        2013 09 18

    Description: Source file for exceptions in the cedar::proc namespace.

    Credits:

======================================================================================================================*/

#include "cedar/processing/exceptions.h"
#include "cedar/processing/Triggerable.h"
#include "cedar/auxiliaries/NamedConfigurable.h"

cedar::proc::TriggerCycleException::TriggerCycleException(const std::vector<std::set<cedar::proc::TriggerablePtr> >& cycles)
{
  std::string message = "One or more cycles in the trigger connections were detected. They are:";
  for (size_t i = 0; i < cycles.size(); ++i)
  {
    message += "\n";
    for (auto iter = cycles.at(i).begin(); iter != cycles.at(i).end(); ++iter)
    {
      cedar::proc::TriggerablePtr triggerable = *iter;

      if (iter != cycles.at(i).begin())
      {
        message += " -> ";
      }

      if (auto named = boost::dynamic_pointer_cast<cedar::aux::NamedConfigurable>(triggerable))
      {
        message += named->getName();
      }
      else
      {
        message += "(unnamed)";
      }
    }
  }

  this->setMessage(message);
}
