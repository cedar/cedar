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

    File:        StringFunctions.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 21

    Description: Advanced string functions.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_STRING_FUNCTIONS_H
#define CEDAR_AUX_STRING_FUNCTIONS_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/lib.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>

namespace cedar
{
  namespace aux
  {
    CEDAR_AUX_LIB_EXPORT void split(const std::string& str,
                                    const std::string& separator,
                                    std::vector<std::string>& parts)
    {
      parts.clear();

      size_t last_index = 0;
      size_t index = str.find(separator);
      while (index != std::string::npos)
      {
        std::string chunk = str.substr(last_index, index - last_index);
        parts.push_back(chunk);
        last_index = index + 1;
        index = str.find(separator, last_index);
      }
      std::string chunk = str.substr(last_index, index - last_index);
      parts.push_back(chunk);
    }
  }
}


#endif // CEDAR_AUX_STRING_FUNCTIONS_H

