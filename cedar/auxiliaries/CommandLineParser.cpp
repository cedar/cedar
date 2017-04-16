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

    File:        CommandLineParser.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 10 23

    Description: Source file for the cedar::aux::CommandLineParser class.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/CommandLineParser.h"
#include "cedar/auxiliaries/Path.h"
#include "cedar/auxiliaries/EnumBase.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <boost/property_tree/json_parser.hpp>

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

std::string CEDAR_AUX_LIB_EXPORT cedar::aux::CommandLineParser::M_STANDARD_OPTIONS_GROUP = "common options";
std::string CEDAR_AUX_LIB_EXPORT cedar::aux::CommandLineParser::M_WRITE_CONFIG_COMMAND = "write-config-to-file";
std::string CEDAR_AUX_LIB_EXPORT cedar::aux::CommandLineParser::M_READ_CONFIG_COMMAND = "read-config-from-file";
std::string CEDAR_AUX_LIB_EXPORT cedar::aux::CommandLineParser::M_UNGROUPED_OPTIONS_NAME = "ungrouped";
std::string CEDAR_AUX_LIB_EXPORT cedar::aux::CommandLineParser::M_CFG_FILE_INCLUDE_TAG = "include";

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::CommandLineParser::CommandLineParser()
{
  // default commands
  this->defineFlag
        (
          "help",
          "Displays help.",
          'h',
          M_STANDARD_OPTIONS_GROUP
        );

  this->defineValue
        (
          M_READ_CONFIG_COMMAND,
          "Reads configuration options from a file.",
          0,
          M_STANDARD_OPTIONS_GROUP
        );

  this->defineValue
        (
          M_WRITE_CONFIG_COMMAND,
          "Writes the chosen configuration options to a file.",
          0,
          M_STANDARD_OPTIONS_GROUP
        );
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::CommandLineParser::setParsedValue(const std::string& longName, const std::string& value)
{
  auto enum_iter = this->mEnumValues.find(longName);
  if (enum_iter != this->mEnumValues.end())
  {
    auto enum_base_ptr = enum_iter->second;
    if (enum_base_ptr->get(value) == cedar::aux::Enum::UNDEFINED)
    {
      CEDAR_THROW(cedar::aux::InvalidValueException, "Invalid value \"" + value + "\"specified for command line parameter \"" + longName + "\". See --help for more details.");
    }
  }
  this->mParsedValues[longName] = value;

  // execute command action: read configuration
  if (longName == M_READ_CONFIG_COMMAND)
  {
    this->readConfigFromFile(value);
  }
}

void cedar::aux::CommandLineParser::setParsedFlag(const std::string& longName, bool value)
{
  if (value)
  {
    this->mParsedFlags.push_back(longName);
  }
  else
  {
    // erase all occurrences of longName
    for (auto iter = this->mParsedFlags.begin(); iter != this->mParsedFlags.end();)
    {
      if (*iter == longName)
      {
        this->mParsedFlags.erase(iter);
      }
      else
      {
        ++iter;
      }
    }
  }
}

void cedar::aux::CommandLineParser::readConfigFromFile(const cedar::aux::Path& path)
{
  cedar::aux::ConfigurationNode root;
  boost::property_tree::read_json(path.absolute().toString(), root);

  auto include_iter = root.find(M_CFG_FILE_INCLUDE_TAG);
  if (include_iter != root.not_found())
  {
    auto includes = include_iter->second;
    for (auto include_iter = includes.begin(); include_iter != includes.end(); ++include_iter)
    {
      const std::string& include = include_iter->second.get_value<std::string>();
      cedar::aux::Path dir = path.getDirectory();
      this->readConfigFromFile(dir + include);
    }
  }

  this->readConfiguration(root);
}

void cedar::aux::CommandLineParser::readConfiguration(cedar::aux::ConfigurationNode& root)
{
  for (auto node_iter = root.begin(); node_iter != root.end(); ++node_iter)
  {
    const std::string& group_name = node_iter->first;

    // skip includes; they are processed by readConfigFromFile
    if (group_name == M_CFG_FILE_INCLUDE_TAG)
    {
      continue;
    }

    const cedar::aux::ConfigurationNode& group = node_iter->second;

    auto flags_iter = group.find("flags");
    if (flags_iter != group.not_found())
    {
      auto flags_node = flags_iter->second;
      for (auto iter = flags_node.begin(); iter != flags_node.end(); ++iter)
      {
        auto long_name = iter->first;
        bool value;
        try
        {
          value = iter->second.get_value<bool>();
        }
        catch(boost::property_tree::ptree_bad_data)
        {
          cedar::aux::LogSingleton::getInstance()->error
          (
            "Could not parse command-line value for flag \"" + long_name
            + "\" from file. Did you use the right file format?",
            "void cedar::aux::CommandLineParser::readConfiguration(cedar::aux::ConfigurationNode&)"
          );
          continue;
        }

        this->setParsedFlag(long_name, value);
      }
    }

    auto values_iter = group.find("values");
    if (values_iter != group.not_found())
    {
      auto values_node = values_iter->second;
      for (auto iter = values_node.begin(); iter != values_node.end(); ++iter)
      {
        auto long_name = iter->first;
        auto value = iter->second.get_value<std::string>();
        this->setParsedValue(long_name, value);
      }
    }
  }
}

void cedar::aux::CommandLineParser::writeConfigToFile(const cedar::aux::Path& path) const
{
  cedar::aux::ConfigurationNode root;
  for (auto iter = this->mGroups.begin(); iter != this->mGroups.end(); ++iter)
  {
    std::string group_name = iter->first;
    auto group = iter->second;

    // standard options are not written out
    if (group_name == M_STANDARD_OPTIONS_GROUP)
    {
      continue;
    }

    cedar::aux::ConfigurationNode group_node;
    this->writeGroup(group_node, group);

    if (!group_node.empty())
    {
      if (group_name.empty())
      {
        group_name = M_UNGROUPED_OPTIONS_NAME;
      }
      root.push_back(cedar::aux::ConfigurationNode::value_type(group_name, group_node));
    }
  }

  boost::property_tree::write_json(path.absolute().toString(), root);
}

void cedar::aux::CommandLineParser::writeGroup
     (
       cedar::aux::ConfigurationNode& node,
       const std::set<std::string>& group
     )
     const
{
  cedar::aux::ConfigurationNode flags_node;
  cedar::aux::ConfigurationNode values_node;

  for (auto iter = group.begin(); iter != group.end(); ++iter)
  {
    auto option_name = *iter;
    if (this->isFlag(option_name))
    {
      flags_node.put(option_name, this->hasParsedFlag(option_name));
    }
    else
    {
      try
      {
        auto value = this->getValue(option_name);
        values_node.put(option_name, value);

      }
      catch (cedar::aux::NotFoundException)
      {
        // ok -- no value given for parameter
      }
    }
  }

  if (!flags_node.empty())
  {
    node.push_back(cedar::aux::ConfigurationNode::value_type("flags", flags_node));
  }

  if (!values_node.empty())
  {
    node.push_back(cedar::aux::ConfigurationNode::value_type("values", values_node));
  }
}


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

void cedar::aux::CommandLineParser::defineEnum
     (
       const std::string& longName,
       const std::string& description,
       cedar::aux::EnumBasePtr enumType,
       char shortName,
       const std::string& group
     )
{
  this->defineOption(longName, description, shortName, group);
  this->mEnumValues[longName] = enumType;
}

void cedar::aux::CommandLineParser::defineEnum
(
  const std::string& longName,
  const std::string& description,
  cedar::aux::EnumBasePtr enumType,
  cedar::aux::EnumId defaultValue,
  char shortName,
  const std::string& group
)
{
  this->defineEnum(longName, description, enumType, shortName, group);
  this->setDefaultValue(longName, enumType->get(defaultValue).name());
}

bool cedar::aux::CommandLineParser::isEnum(const std::string& longName) const
{
  return this->mEnumValues.find(longName) != this->mEnumValues.end();
}

void cedar::aux::CommandLineParser::defineValueInt
     (
       const std::string& longName,
       const std::string& description,
       const std::string& defaultValue,
       char shortName,
       const std::string& group
     )
{
  this->defineValue(longName, description, shortName, group);

  this->setDefaultValue(longName, defaultValue);
}

void cedar::aux::CommandLineParser::setDefaultValue(const std::string& longName, const std::string& defaultValue)
{
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
  if (mDescriptions.find(longName) != mDescriptions.end())
  {
    CEDAR_THROW(cedar::aux::DuplicateNameException, "Cannot add command line option \"" + longName + "\": already defined.");
  }

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

bool cedar::aux::CommandLineParser::hasParsedFlag(const std::string& longName) const
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

const std::vector<std::string>& cedar::aux::CommandLineParser::getUnparsedValues() const
{
  return this->mUnparsedValues;
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
                this->setParsedFlag(command, true);
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
        else // string does not begin with "-" or "--", thus, it is an unparsed value
        {
          this->mUnparsedValues.push_back(string);
        }
        break;
      }

      case STATE_EXPECTING_VALUE:
      {
        if (current_option.empty())
        {
          CEDAR_THROW(cedar::aux::InvalidValueException, "Option " + current_option + " expects a value, none was given.");
        }
        this->setParsedValue(current_option, string);
        state = STATE_PLAIN;
        current_option = std::string();
        break;
      }
    }
  }

  if (state != STATE_PLAIN)
  {
    CEDAR_THROW(cedar::aux::InvalidValueException, "Error parsing options: still expecting a value at end of arguments.");
  }

  if (this->hasParsedFlag("help"))
  {
    this->writeHelp();
    if (terminationAllowed)
    {
      exit(0);
    }
  }

  if (this->hasParsedValue(M_WRITE_CONFIG_COMMAND))
  {
    const std::string& path = this->getValue(M_WRITE_CONFIG_COMMAND);
    this->writeConfigToFile(path);
  }
}

bool cedar::aux::CommandLineParser::hasParsedValue(const std::string& longName) const
{
  return this->mParsedValues.find(longName) != this->mParsedValues.end();
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
    stream << "Usage: " << this->mArguments.front() << " [options]" << std::endl << std::endl;
  }

  if (!this->mDescription.empty())
  {
    stream << this->mDescription << std::endl << std::endl;
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

      if (this->isEnum(long_name))
      {
        stream << "    Possible Values are: ";
        bool first = true;
        for (auto value : this->mEnumValues.find(long_name)->second->list())
        {
          if (first)
          {
            first = false;
          }
          else
          {
            stream << ", ";
          }
          stream << "\"" << value.name() << "\"";
        }
        stream << std::endl;
      }

      stream << std::endl;
    } // option iter

    stream << "-----" << std::endl << std::endl;
  } // group iter
}

void cedar::aux::CommandLineParser::writeConfigurationValues(std::ostream& stream) const
{
  stream << "--------------------------------------" << std::endl;
  stream << " The following parameters are chosen:" << std::endl;
  stream << "--------------------------------------" << std::endl;

  for (auto option_it = this->mDescriptions.begin(); option_it != this->mDescriptions.end(); ++option_it)
  {
    const std::string& long_name = option_it->first;
    stream << long_name << ": ";

    if (this->isFlag(long_name))
    {
      if (this->hasParsedFlag(long_name))
      {
        stream << "true";
      }
      else
      {
        stream << "false";
      }
    }
    else
    {
      try
      {
        std::string value = this->getValue(long_name);
        stream << this->getValue(long_name);
      }
      catch (const cedar::aux::NotFoundException&)
      {
        stream << "(not set)";
      }
    }

    stream << std::endl;
  }
}

void cedar::aux::CommandLineParser::writeSummary(std::ostream& stream) const
{
  // if there's nothing to print, don't bother
  if (this->mParsedFlags.empty() && this->mParsedValues.empty())
  {
    return;
  }

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

  if (!this->mUnparsedValues.empty())
  {
    stream << "The following unparsed values were found:" << std::endl;
    for (const auto& unparsed : this->mUnparsedValues)
    {
      stream << unparsed << std::endl;
    }
    stream << std::endl;
  }
}

void cedar::aux::CommandLineParser::setDescription(const std::string& description)
{
  this->mDescription = description;
}
