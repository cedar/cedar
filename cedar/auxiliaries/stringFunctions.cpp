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

    File:        utilities.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 01 16

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/version.h"

// SYSTEM INCLUDES
#include <boost/optional.hpp>
#include <iomanip>
#include <sstream>

std::string cedar::aux::versionNumberToString(unsigned int version)
{
  std::string res = cedar::aux::toString(CEDAR_GET_VERSION_MAJOR(version));
  res += "." + cedar::aux::toString(CEDAR_GET_VERSION_MINOR(version));
  res += "." + cedar::aux::toString(CEDAR_GET_VERSION_BUGFIX(version));
  return res;
}

std::string cedar::aux::removeWhiteSpaces(const std::string& stringFromWhichToRemoveWhiteSpaces)
{
  std::string result = cedar::aux::erase(stringFromWhichToRemoveWhiteSpaces, " ");
  result = cedar::aux::erase(result, "\n");
  result = cedar::aux::erase(result, "\r");
  return result;
}

std::string cedar::aux::removeLeadingAndTrailingWhiteSpaces(const std::string& stringFromWhichToRemoveWhiteSpaces)
{
  size_t leading = stringFromWhichToRemoveWhiteSpaces.find_first_not_of(" \r\n\t");
  size_t trailing = stringFromWhichToRemoveWhiteSpaces.find_last_not_of(" \r\n\t");
  return stringFromWhichToRemoveWhiteSpaces.substr(leading, (trailing - leading + 1));
}

std::string cedar::aux::formatDuration(const cedar::unit::Time& time)
{
  struct TimeSubdiv
  {
    TimeSubdiv(double max, const std::string& shortHandle, unsigned int precision)
    :
    max(max),
    precision(precision),
    short_handle(shortHandle)
    {
    }

    TimeSubdiv(double max, const std::string& shortHandle)
    :
    max(max),
    short_handle(shortHandle)
    {
    }

    TimeSubdiv(const std::string& shortHandle)
    :
    short_handle(shortHandle)
    {
    }

    boost::optional<double> max;
    boost::optional<unsigned int> precision;
    std::string short_handle;
  };

  double time_in_seconds = time / (1.0 * cedar::unit::seconds);

  static std::vector<TimeSubdiv> time_subdivs;
  if (time_subdivs.empty())
  {
    time_subdivs.push_back(TimeSubdiv(60.0, "s", 2));
    time_subdivs.push_back(TimeSubdiv(60.0, "m", 2));
    time_subdivs.push_back(TimeSubdiv(24.0, "h", 2));
    time_subdivs.push_back(TimeSubdiv(365, "d"));
    time_subdivs.push_back(TimeSubdiv("y"));
  }

  std::string time_str;

  double current_max = 1.0;
  double time_remaining = time_in_seconds;
  bool first = true;
  for (const auto& subdiv : time_subdivs)
  {
    double subtime;
    double old_max = current_max;
    if (subdiv.max)
    {
      current_max *= subdiv.max.get();
      subtime = std::fmod(time_remaining, current_max);
      time_remaining -= subtime;
    }
    else
    {
      subtime = time_remaining;
      time_remaining = 0.0;
    }
    std::stringstream stream;
    // make the conversion zero-padded
    unsigned int padding = 0;
    stream << std::fixed;
    if (first)
    {
      stream << std::setprecision(1);
      padding = 2;
      first = false;
    }
    else
    {
      stream << std::setprecision(0);
    }

    if (time_remaining > 0.0 && subdiv.precision)
    {
      stream << std::setw(subdiv.precision.get() + padding) << std::setfill('0');
    }

    stream << (subtime / old_max);
    stream << subdiv.short_handle;
    std::string subdiv_str = stream.str();
    if (!time_str.empty())
    {
      subdiv_str += " ";
    }
    time_str = subdiv_str + time_str;

    if (time_remaining <= 0.0)
    {
      break;
    }
  }

  return time_str;
}

std::string cedar::aux::camelCaseToSpaces(const std::string& camelCasedString)
{
  if (camelCasedString.empty())
  {
    return camelCasedString;
  }

  std::string spaced;

  for (size_t i = 0; i < camelCasedString.size(); ++i)
  {
    bool in_upper = isupper(camelCasedString.at(i));
    bool in_lower = islower(camelCasedString.at(i));
    if (in_upper)
    {
      if (i > 0 && (i + 1) < camelCasedString.size() && islower(camelCasedString.at(i + 1)) && isupper(camelCasedString.at(i - 1)))
      {
        spaced += " ";
      }
      spaced += camelCasedString.at(i);
    }
    else if (in_lower)
    {
      spaced += camelCasedString.at(i);
      if (i > 0 && (i + 1) < camelCasedString.size() && isupper(camelCasedString.at(i + 1)))
      {
        spaced += " ";
      }
    }
    else
    {
      // other case: character is not an alphabetical one
      spaced += camelCasedString.at(i);
    }
  }

  return spaced;
}
