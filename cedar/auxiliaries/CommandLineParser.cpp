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

    File:        CommandLineParser.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 10 23

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/CommandLineParser.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::CommandLineParser::CommandLineParser()
{
  // default commands
  std::string standard_group = "common options";
  this->defineFlag("help", "Displays help.", 'h', standard_group);
  this->defineValue("read-config-from-file", "Reads configuration options from a file.", 0, standard_group);
  this->defineValue("write-config-to-file", "Writes the chosen configuration options to a file.", 0, standard_group);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::CommandLineParser::defineFlag
     (
       const std::string& longName,
       const std::string& description,
       char shortName,
       const std::string& group
     )
{
  this->defineOption(longName, description, shortName, group);
  this->mFlags.insert(longName);
}

void cedar::aux::CommandLineParser::defineValue
     (
       const std::string& longName,
       const std::string& description,
       char shortName,
       const std::string& group
     )
{
  this->defineOption(longName, description, shortName, group);
}

void cedar::aux::CommandLineParser::defineValue
     (
       const std::string& longName,
       const std::string& description,
       const std::string& defaultValue,
       char shortName,
       const std::string& group
     )
{
  this->defineValue(longName, description, shortName, group);

  CEDAR_ASSERT(this->mDefaultValues.find(longName) == this->mDefaultValues.end());
  this->mDefaultValues[longName] = defaultValue;
}

void cedar::aux::CommandLineParser::defineOption
     (
       const std::string& longName,
       const std::string& description,
       char shortName,
       const std::string& group
     )
{
  //!@todo Proper exception
  CEDAR_ASSERT(mDescriptions.find(longName) == mDescriptions.end());

  this->mDescriptions[longName] = description;

  CEDAR_ASSERT(this->mAbbreviations.find(shortName) == this->mAbbreviations.end());

  if (shortName != 0)
  {
    this->mAbbreviations[shortName] = longName;
  }

  this->addOptionToGroup(group, longName);
}

void cedar::aux::CommandLineParser::addOptionToGroup(const std::string& groupName, const std::string& longName)
{
  // remove option from all other groups
  for (auto iter = this->mGroups.begin(); iter != this->mGroups.end(); ++iter)
  {
    auto set = iter->second;
    auto option_it = set.find(longName);
    if (option_it != set.end())
    {
      set.erase(option_it);
    }
  }

  auto iter = this->mGroups.find(groupName);
  if (iter == this->mGroups.end())
  {
    this->mGroups[groupName] = std::set<std::string>();
    iter = this->mGroups.find(groupName);
  }

  iter->second.insert(longName);
}

bool cedar::aux::CommandLineParser::isFlag(const std::string& longName) const
{
  return this->mFlags.find(longName) != this->mFlags.end();
}

bool cedar::aux::CommandLineParser::hasFlag(const std::string& longName) const
{
  auto iter = std::find(this->mParsedFlags.begin(), this->mParsedFlags.end(), longName);
  return iter != this->mParsedFlags.end();
}

bool cedar::aux::CommandLineParser::hasDefaultValue(const std::string& longName) const
{
  return this->mDefaultValues.find(longName) != this->mDefaultValues.end();
}

const std::string& cedar::aux::CommandLineParser::getDefaultValue(const std::string& longName) const
{
  auto iter = this->mDefaultValues.find(longName);
  if (iter != this->mDefaultValues.end())
  {
    return iter->second;
  }
  else
  {
    CEDAR_THROW(cedar::aux::NoDefaultException, "No default value given for --" + longName);
  }
}

void cedar::aux::CommandLineParser::parse(int argc, char* argv[], bool terminationAllowed)
{
  enum STATE
  {
    STATE_PLAIN,
    STATE_EXPECTING_VALUE
  };

  this->mArguments.clear();
  this->mParsedValues.clear();
  this->mParsedFlags.clear();

  for (int i = 0; i < argc; ++i)
  {
    this->mArguments.push_back(std::string(argv[i]));
  }

  STATE state = STATE_PLAIN;
  std::string current_option;
  for (size_t i = 1; i < this->mArguments.size(); ++i)
  {
    auto string = this->mArguments.at(i);

    switch (state)
    {
      case STATE_PLAIN:
      {
        if (string.size() > 0 && string.at(0) == '-')
        {
          if (string.size() > 1 && string.at(1) == '-')
          {
            std::string command = string.substr(2);

            if (!this->isDefined(command))
            {
              std::cout << "Error: unknown command --" << command << std::endl;

              if (terminationAllowed)
              {
                exit(-1);
              }
            }
            else
            {
              // long name
              if (this->isFlag(command))
              {
                this->mParsedFlags.push_back(command);
              }
              else
              {
                current_option = command;
                state = STATE_EXPECTING_VALUE;
              }
            }
          }
          else
          {
            // series of short names
            //!@todo Only accept series if there's no value-expecting one in there
            for (size_t i = 1; i < string.size(); ++i)
            {
              char short_name = string.at(i);
              try
              {
                const std::string& long_name = this->getLongName(short_name);

                if (!this->isDefined(long_name))
                {
                  std::cout << "Error: unknown command --" << long_name << std::endl;

                  if (terminationAllowed)
                  {
                    exit(-1);
                  }
                }
                else
                {
                  if (this->isFlag(long_name))
                  {
                    this->mParsedFlags.push_back(long_name);
                  }
                  else
                  {
                    current_option = long_name;
                    state = STATE_EXPECTING_VALUE;
                  }
                }
              }
              catch (const cedar::aux::UnknownNameException& e)
              {
                std::cout << "Error: " << e.getMessage() << std::endl;

                if (terminationAllowed)
                {
                  exit(-1);
                }
              }
            }
          }
        }
        break;
      }

      case STATE_EXPECTING_VALUE:
      {
        //!@todo Proper exception: expecting a value.
        CEDAR_ASSERT(!current_option.empty());
        this->mParsedValues[current_option] = string;
        state = STATE_PLAIN;
        break;
      }
    }
  }

  //!@todo Proper exception: should not end while looking for value.
  CEDAR_ASSERT(state == STATE_PLAIN);

  if (this->hasFlag("help"))
  {
    this->writeHelp();
    if (terminationAllowed)
    {
      exit(0);
    }
  }
}

bool cedar::aux::CommandLineParser::isDefined(const std::string& longName) const
{
  return this->mDescriptions.find(longName) != this->mDescriptions.end();
}

const std::string& cedar::aux::CommandLineParser::getLongName(char shortName)
{
  auto iter = this->mAbbreviations.find(shortName);
  if (iter != this->mAbbreviations.end())
  {
    return iter->second;
  }
  else
  {
    std::string name;
    name += shortName;
    CEDAR_THROW(cedar::aux::UnknownNameException, "Unknown option: -" + name);
  }
}

bool cedar::aux::CommandLineParser::shortNameExists(char shortName) const
{
  return this->mAbbreviations.find(shortName) != this->mAbbreviations.end();
}

bool cedar::aux::CommandLineParser::hasShortNameFor(const std::string& longName) const
{
  for (auto iter = this->mAbbreviations.begin(); iter != this->mAbbreviations.end(); ++iter)
  {
    if (iter->second == longName)
    {
      return true;
    }
  }
  return false;
}

const std::string& cedar::aux::CommandLineParser::getValue(const std::string& longName) const
{
  auto iter = this->mParsedValues.find(longName);
  if(iter != this->mParsedValues.end())
  {
    return iter->second;
  }
  else
  {
    if (this->hasDefaultValue(longName))
    {
      return this->getDefaultValue(longName);
    }
    else
    {
      CEDAR_THROW(cedar::aux::NotFoundException, "Could not find value for parameter --" + longName);
    }
  }
}

char cedar::aux::CommandLineParser::getShortNameFor(const std::string& longName) const
{
  for (auto iter = this->mAbbreviations.begin(); iter != this->mAbbreviations.end(); ++iter)
  {
    if (iter->second == longName)
    {
      return iter->first;
    }
  }
  CEDAR_THROW(cedar::aux::UnknownNameException, "Cannot find short name for --" + longName);
}

void cedar::aux::CommandLineParser::writeHeading(std::ostream& stream, const std::string& heading, char fill) const
{
  std::string separator;

  separator.assign(heading.size() + 2, fill);

  stream << separator << std::endl;
  stream << " " << heading << std::endl;
  stream << separator << std::endl;
}

void cedar::aux::CommandLineParser::writeHelp(std::ostream& stream) const
{
  if (this->mArguments.size() > 0)
  {
    //!@todo also append free-floating values, if allowed.
    stream << "Usage: " << this->mArguments.front() << " [options]" << std::endl << std::endl;
  }

  this->writeHeading(stream, "Options", '=');
  stream << std::endl;

  for (auto group_iter = this->mGroups.begin(); group_iter != this->mGroups.end(); ++group_iter)
  {
    auto group = group_iter->first;
    auto options = group_iter->second;

    if (!group.empty())
    {
      this->writeHeading(stream, group, '-');
      stream << std::endl;
    }

    for (auto iter = options.begin(); iter != options.end(); ++iter)
    {
      auto long_name = *iter;
      auto desc_iter = this->mDescriptions.find(long_name);
      CEDAR_DEBUG_ASSERT(desc_iter != this->mDescriptions.end());
      auto description = desc_iter->second;

      stream << "--" << long_name;
      if (this->hasShortNameFor(long_name))
      {
        stream << ", -" << this->getShortNameFor(long_name);
      }

      if (!this->isFlag(long_name))
      {
        stream << " VALUE" << std::endl;
      }

      stream << std::endl;

      stream << "    " << description << std::endl;

      if (this->hasDefaultValue(long_name))
      {
        stream << "    " << "Default value: " << this->getDefaultValue(long_name) << std::endl;
      }

      stream << std::endl;
    } // option iter

    stream << "-----" << std::endl << std::endl;
  } // group iter
}

void cedar::aux::CommandLineParser::writeSummary(std::ostream& stream) const
{
  stream << "------------------------------------------" << std::endl;
  stream << " Read the following command line options:" << std::endl;
  stream << "------------------------------------------" << std::endl;
  stream << std::endl;
  if (!this->mParsedFlags.empty())
  {
    stream << "The following flags were parsed: ";

    for (auto iter = this->mParsedFlags.begin(); iter != this->mParsedFlags.end(); ++iter)
    {
      if (iter != this->mParsedFlags.begin())
      {
        stream << ", ";
      }
      stream << *iter;
    }
    stream << std::endl << std::endl;
  }

  if (!this->mParsedValues.empty())
  {
    stream << "The following values were parsed:" << std::endl;

    for (auto iter = this->mParsedValues.begin(); iter != this->mParsedValues.end(); ++iter)
    {
      stream << "--" << iter->first << ": " << iter->second << std::endl;
    }
    stream << std::endl;
  }
}
