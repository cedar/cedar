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

    File:        StringFunctions.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 21

    Description: Advanced string functions.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifndef CEDAR_AUX_STRING_FUNCTIONS_H
#define CEDAR_AUX_STRING_FUNCTIONS_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/lib.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/lexical_cast.hpp>
  #include <boost/regex.hpp>
#endif
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <limits.h>

namespace cedar
{
  namespace aux
  {
    /*!@brief Formats a time duration to a human-readable string.
     */
    CEDAR_AUX_LIB_EXPORT std::string formatDuration(const cedar::unit::Time& time);

    /*!@brief Template method that converts simple data types to a string.
     *
     * @param value The data value that will be converted to a string.
     */
    template<typename T>
    std::string toString(const T &value)
    {
      std::ostringstream streamOut;
      streamOut << value;
      return streamOut.str();
    }

    /*!@brief Generates a string that contains a human-readable version number from a version made with the
     *        CEDAR_MAKE_VERSION macro.
     *
     * @param version Integer generated with CEDAR_MAKE_VERSION.
     */
    CEDAR_AUX_LIB_EXPORT std::string versionNumberToString(unsigned int version);

    /*!@brief  Splits a string based on the given separator.
     *
     *         For example, the string "This is an example" is split into the list ["This", "is", "an", "example"] when
     *         the separator " " is used.
     *
     * @param  str The string to split.
     * @param  separator The separator.
     * @param  parts Result vector into which the separated parts of the string are written.
     *
     * @remark If the separator is not found in \em str (e.g., if \em str is empty), the function will return a list
     *         with the input string as the only element.
     */
    inline void split(const std::string& str,
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
        last_index = index + separator.size();
        index = str.find(separator, last_index);
      }
      std::string chunk = str.substr(last_index, index - last_index);
      parts.push_back(chunk);
    }

    /*!@brief  Splits a string in two parts, up to the first occurrence of the separator.
     *
     *         For example, the string "This is an example" is split into the parts "This" and "is an example"] when
     *         the separator " " is used.
     *
     * @param  str The string to split.
     * @param  separator The separator.
     * @param  first First part of the string.
     * @param  rest Second part of the string - may contain separators.
     *
     * @remark If the separator is not found in \em str (e.g., if \em str is empty), the function will return the full
     *         string in first and "" in rest.
     */
    inline void splitFirst
                (
                  const std::string& str,
                  const std::string& separator,
                  std::string& first,
                  std::string& rest
                )
    {
      size_t index = str.find(separator);
      if (index != std::string::npos)
      {
        first = str.substr(0, index);
        rest = str.substr(index + 1, std::string::npos);
      }
      else
      {
        first = str;
        rest = "";
      }
    }

    /*!@brief  Splits a string in two parts, up to the last occurrence of the separator.
     *
     *         For example, the string "This is an example" is split into the parts "This is an" and "example"] when
     *         the separator " " is used.
     *
     * @param  str The string to split.
     * @param  separator The separator.
     * @param  rest First part of the string - may contain separators.
     * @param  last Second part of the string.
     *
     * @remark If the separator is not found in \em str (e.g., if \em str is empty), the function will return the full
     *         string in last and "" in rest.
     */
    inline void splitLast
                (
                  const std::string& str,
                  const std::string& separator,
                  std::string& rest,
                  std::string& last
                )
    {
      size_t index = str.rfind(separator);
      if (index != std::string::npos)
      {
        rest = str.substr(0, index);
        last = str.substr(index + 1, std::string::npos);
      }
      else
      {
        last = str;
        rest = "";
      }
    }

    /*!@brief  Joins an iterable list of strings together.
     *
     * @tparam T               type of the container. This container should have a const_iterator where ++const_iterator
     *                         is possible and *const_iterator is a std::string. The container should also have a begin
     *                         and an end method for returning const_iterators to the begin and end, respectively.
     *
     * @param  stringList      A list of strings that are joined by the function.
     * @param  separator       Separator that is inserted between the items in the list.
     * @param  outJoinedString String to which the result is written.
     */
    template <typename T>
    inline void join(const T& stringList,
                     const std::string& separator,
                     std::string& outJoinedString
                     )
    {
      outJoinedString = "";
      for (typename T::const_iterator iter = stringList.begin(); iter != stringList.end(); ++iter)
      {
        if (iter != stringList.begin())
        {
          outJoinedString += separator;
        }
        outJoinedString += *iter;
      }
    }

    /*!@brief Joins an iterable list of strings together.
     *
     *        This method works like cedar::aux::join, however, it returns the string instead of writing to a reference.
     * @see   cedar::aux::join for a detailed description.
     */
    template <typename T>
    inline std::string join(const T& stringList,
                            const std::string& separator
                            )
    {
      std::string result;
      cedar::aux::join(stringList, separator, result);
      return result;
    }

    /*!@brief Function that checks whether a string starts with a given one.
     *
     * @returns True, if string starts with startsWith, false otherwise.
     */
    inline bool startsWith(const std::string& string, const std::string& startsWith)
    {
      if (string.length() < startsWith.length())
      {
        return false;
      }

      for (size_t i = 0; i < startsWith.size(); ++i)
      {
        if (string.at(i) != startsWith.at(i))
        {
          return false;
        }
      }

      return true;
    }

    /*!@brief Function that checks whether a string ends with a given one.
     *
     * @returns True, if string ends with endsWith, false otherwise.
     */
    inline bool endsWith(const std::string& string, const std::string& endsWith)
    {
      if (string.length() < endsWith.length())
      {
        return false;
      }

      for (size_t i = 0; i < endsWith.size(); ++i)
      {
        if (string.at(string.length() - endsWith.length() + i) != endsWith.at(i))
        {
          return false;
        }
      }

      return true;
    }

    /*!@brief Replaces all occurrences of needle in a given string with a specified replacement.
     *
     */
    inline std::string replace(const std::string& haystack, const std::string& needle, const std::string& replacement)
    {
      std::string result = haystack;

      if (haystack.empty())
      {
        return haystack;
      }

      size_t index = haystack.find(needle);
      while (index != std::string::npos)
      {
        result = result.replace(index, needle.length(), replacement);
        index = result.find(needle, index + replacement.length());
      }
      
      return result;
    }

    /*! @brief Returns a string with all occurrences of needle removed from the given string haystack.
     */
    inline std::string erase(const std::string& haystack, const std::string& needle)
    {
      return replace(haystack, needle, "");
    }

    /*! Returns a copy of the given string that has no more white spaces
     */
    std::string removeWhiteSpaces(const std::string& stringFromWhichToRemoveWhiteSpaces);

    /*! Removes all white space at the beginning and end of the string.
     */
    std::string removeLeadingAndTrailingWhiteSpaces(const std::string& stringFromWhichToRemoveWhiteSpaces);

    /*!@brief Template function that converts an STL string to a simple data type.
     *
     * @param value The data value that will be converted to a string.
     * @param string The string the value will be extracted from.
     * @param encoding The encoding of the string (e.g., the base of the number (i.e., decimal, hexadecimal, etc.))
     *        (see http://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-3.3/classstd_1_1ios__base.html)
     */
    template <class T>
    bool fromString(T& value, const std::string& string, std::ios_base& (*encoding)(std::ios_base&))
    {
      std::istringstream stream(string);
      return !(stream >> encoding >> value).fail();
    }

    /*!@brief Overloaded method, provided for convenience.
     *
     * @see cedar::aux::fromString
     */
    template <class T>
    T fromString(const std::string& string, std::ios_base& (*encoding)(std::ios_base&))
    {
      T result;
      if (!cedar::aux::fromString(result, string, encoding))
      {
        CEDAR_THROW
        (
          cedar::aux::ConversionFailedException,
          "Could not convert the string \"" + string + "\" to the requested type."
        );
      }
      return result;
    }

    /*!@brief Template function that converts a string to a simple data type.
     *
     * @param string The string the value will be extracted from.
     */
    template <class T>
    inline T fromString(const std::string& string)
    {
      try
      {
        return boost::lexical_cast<T>(string);
      }
      catch (boost::bad_lexical_cast)
      {
        CEDAR_THROW
        (
          cedar::aux::ConversionFailedException,
          "Could not convert the string \"" + string + "\" to the requested type."
        );
      }
    }

    //! Template specialization for double values.
    template <>
    inline double fromString<double>(const std::string& string)
    {
      double result;
      std::istringstream stream(string);
      if((stream >> result).fail()|| string.find("#") != std::string::npos)
      {
        if (string == "inf" || string == "1.#INF")
        {
          //!@todo num_limits<>::has_infinity could help make this more generic/remove the need for double, float specialization
          return std::numeric_limits<double>::infinity();
        }
        else if (string == "-inf" || string == "-1.#INF")
        {
          return -std::numeric_limits<double>::infinity();
        }
        else if (string == "nan" || string == "-nan")
        {
          if (std::numeric_limits<double>::has_quiet_NaN)
          {
            return std::numeric_limits<double>::quiet_NaN();
          }
          else
          {
            return std::numeric_limits<double>::signaling_NaN();
          }
        }
        else
        {
          CEDAR_THROW
          (
            cedar::aux::ConversionFailedException,
            "Could not convert the string \"" + string + "\" to the requested type."
          );
        }
      }

      return result;
    }

    //! Template specialization for float values.
    template <>
    inline float fromString<float>(const std::string& string)
    {
      float result;
      std::istringstream stream(string);

      if((stream >> result).fail() || string.find("#") != std::string::npos)
      {
        if (string == "inf" || string == "1.#INF")
        {
          return std::numeric_limits<float>::infinity();
        }
        else if (string == "-inf" || string == "-1.#INF" )
        {
          return -std::numeric_limits<float>::infinity();
        }
        else if (string == "nan" || string == "-nan")
        {
          if (std::numeric_limits<float>::has_quiet_NaN)
          {
            return std::numeric_limits<float>::quiet_NaN();
          }
          else
          {
            return std::numeric_limits<float>::signaling_NaN();
          }
        }
        else
        {
          CEDAR_THROW
          (
            cedar::aux::ConversionFailedException,
            "Could not convert the string \"" + string + "\" to the requested type."
          );
        }
      }

      return result;
    }

    /*!@brief a regular expressions based replace inside a string
     * @param input the input string
     * @param regex a regular expression (syntax can be found in boost doc)
     * @param replaceText the text that replaces all matching parts of the input
     *
     * based on http://www.boost.org/doc/libs/1_49_0/libs/regex/doc/html/boost_regex/ref/regex_replace.html
     */
    inline std::string regexReplace(const std::string& input, const std::string& regex, const std::string& replaceText)
    {
      // based on http://www.boost.org/doc/libs/1_49_0/libs/regex/doc/html/boost_regex/ref/regex_replace.html
      std::ostringstream t(std::ios::out | std::ios::binary);
      std::ostream_iterator<char, char> oi(t);
      boost::regex exp;
      exp.assign(regex);
      boost::regex_replace
      (
        oi, // output iterator
        input.begin(), // input begin iterator
        input.end(), // input end iterator
        exp, // regex
        replaceText, // formatter
        boost::match_default | boost::format_all // parameter
      );
      return t.str();
    }

    //! Returns a lower-case version of the given string.
    inline std::string toLower(const std::string& in)
    {
      std::string lower_case_str = in;
      std::transform(lower_case_str.begin(), lower_case_str.end(), lower_case_str.begin(), ::tolower);
      return lower_case_str;
    }

    //! Converts camel-case instances to spaces.
	CEDAR_AUX_LIB_EXPORT std::string camelCaseToSpaces(const std::string& camelCasedString);
  }
}

#endif // CEDAR_AUX_STRING_FUNCTIONS_H
