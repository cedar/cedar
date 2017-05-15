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

    File:        CommandLineParser.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 10 23

    Description: Header file for the cedar::aux::CommandLineParser class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_COMMAND_LINE_PARSER_H
#define CEDAR_AUX_COMMAND_LINE_PARSER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Enum.h"
#include "cedar/auxiliaries/EnumBase.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/assert.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/Configurable.fwd.h"
#include "cedar/auxiliaries/CommandLineParser.fwd.h"
#include "cedar/auxiliaries/Path.fwd.h"
#include "cedar/auxiliaries/EnumBase.fwd.h"

// SYSTEM INCLUDES
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>

/*!@brief A class for conveniently parsing options from a command line.
 *
 *        Can also read/write parameters to a file.
 *
 * @todo  Type-checking for defined values.
 */
class cedar::aux::CommandLineParser
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  CommandLineParser();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Defines a command line flag, i.e., an option that does not expect a value.
   *
   * @param longName    Long name of the parameter. Should not contain spaces.
   * @param description Description to show to the user.
   * @param shortName   Short name of the command. Leave 0 to ignore.
   * @param group       Logical group of the parameter. Mainly used for the help function.
   */
  void defineFlag
  (
    const std::string& longName,
    const std::string& description,
    char shortName = 0,
    const std::string& group = std::string()
  );

  /*!@brief Defines a command line value, i.e., an option that must be given a value. If the option is not present, the
   *        parser will return the default value.
   *
   * @param longName     Long name of the option.
   * @param description  Description of the option.
   * @param defaultValue The default value to be used when the user does not specify anything.
   * @param shortName    Optional short name of the option.
   * @param group        Logical group of the parameter. Mainly used for the help function.
   */
  template <typename T>
  void defineValue
  (
    const std::string& longName,
    const std::string& description,
    const T& defaultValue,
    char shortName = 0,
    const std::string& group = std::string()
  )
  {
    this->defineValueInt(longName, description, cedar::aux::toString(defaultValue), shortName, group);
  }

  /*!@brief Defines a command line value, i.e., an option that must be given a value. If this option is missing, the
   *        parser throws an exception when it is queried by the application.
   *
   * @param longName    Long name of the option.
   * @param description Description of the option.
   * @param shortName   Optional short name of the option.
   * @param group       Logical group of the parameter. Mainly used for the help function.
   */
  void defineValue
  (
    const std::string& longName,
    const std::string& description,
    char shortName = 0,
    const std::string& group = std::string()
  );

  /*!@brief Defines a command line value that is determined from an enum class.
   *
   * @param longName     Long name of the option.
   * @param description  Description of the option.
   * @param enumType     Enum class to be used for translating values from and to strings.
   * @param defaultValue Default value for the parameter.
   * @param shortName    Optional short name of the option.
   * @param group        Group of the option; used for displaying the help text.
   */
  void defineEnum
  (
    const std::string& longName,
    const std::string& description,
    cedar::aux::EnumBasePtr enumType,
    cedar::aux::EnumId defaultValue,
    char shortName = 0,
    const std::string& group = std::string()
  );

  /*!@brief Defines a command line value that is determined from an enum class.
   *
   * @param longName    Long name of the option.
   * @param description Description of the option.
   * @param enumType    Enum class to be used for translating values from and to strings.
   * @param shortName   Optional short name of the option.
   * @param group       Group of the option; used for displaying the help text.
   */
  void defineEnum
  (
    const std::string& longName,
    const std::string& description,
    cedar::aux::EnumBasePtr enumType,
    char shortName = 0,
    const std::string& group = std::string()
  );

  //! Test if a flag is present in the parsed options.
  bool hasParsedFlag(const std::string& longName) const;

  //! Test if a value is present in the parsed options. Does not take default values into account.
  bool hasParsedValue(const std::string& longName) const;

  /*!@brief   Parse the given command line options
   *
   * @remarks If a command such as help is specified, this may terminate the program unless this is explicitly disabled
   *          with the @em terminationAllowed parameter.
   */
  void parse(int argc, char* argv[], bool terminationAllowed = true);

  /*! Writes the help message to the given stream.
   */
  void writeHelp(std::ostream& stream = std::cout) const;

  /*! Writes a summary of the read values to the given stream.
   */
  void writeSummary(std::ostream& stream = std::cout) const;

  /*! Writes all currently set configuration values to the given stream.
   */
  void writeConfigurationValues(std::ostream& stream = std::cout) const;

  /*!@brief Returns a value with a given type.
   */
  template <typename T>
  T getValue(const std::string& longName) const
  {
    return cedar::aux::fromString<T>(this->getValue(longName));
  }

  //! Adds a given option to a group.
  void addOptionToGroup(const std::string& groupName, const std::string& longName);

  /*!@brief Writes the currently parsed arguments to a file.
   *
   *        This method is called automatically if the --write-config-to-file argument is passed on the command line.
   *        The file written by this method can then later on be read with --read-config-from-file or the
   *        readConfigFromFile method.
   */
  void writeConfigToFile(const cedar::aux::Path& path) const;

  /*!@brief Reads the configuration to use from a file.
   *
   *        This method is called automatically if the --read-config-to-file argument is passed on the command line.
   *        The file read by this method should have the format written by --write-config-to-file or the
   *        writeConfigToFile method.
   *
   *        In addition, this function will parser include directives in the file being read. These take the form of a
   *        json-list with the files to include, relative to the configuration file being read. Example:
   *
           @code
     "include" : ["path1", "path2"]
           @endcode
   */
  void readConfigFromFile(const cedar::aux::Path& path);

  /*! @brief Returns a vector of unparsed values.
   *
   *         Unparsed values are those that do not belong to a value parameter. Exmaple: if --foo is a flag:
   *         ./programName --foo unparsed_value
   */
  const std::vector<std::string>& getUnparsedValues() const;

  /*! Sets the default value for a given command line option.
   */
  void setDefaultValue(const std::string& longName, const std::string& defaultValue);

  //! set a description of the executable displayed during "help"
  void setDescription(const std::string& description);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void defineValueInt
  (
    const std::string& longName,
    const std::string& description,
    const std::string& defaultValue,
    char shortName = 0,
    const std::string& group = std::string()
  );

  void defineOption
       (
         const std::string& longName,
         const std::string& description,
         char shortName = 0,
         const std::string& group = std::string()
       );

  bool isFlag(const std::string& longName) const;

  bool isEnum(const std::string& longName) const;

  const std::string& getLongName(char shortName);

  bool shortNameExists(char shortName) const;

  bool hasShortNameFor(const std::string& longName) const;

  char getShortNameFor(const std::string& longName) const;

  bool hasDefaultValue(const std::string& longName) const;

  const std::string& getDefaultValue(const std::string& longName) const;

  bool isDefined(const std::string& longName) const;

  /*!@brief Returns a value with a given type.
   */
  const std::string& getValue(const std::string& longName) const;

  void writeHeading(std::ostream& stream, const std::string& heading, char fill) const;

  void writeGroup(cedar::aux::ConfigurationNode& node, const std::set<std::string>& group) const;

  void readConfiguration(cedar::aux::ConfigurationNode& node);

  void setParsedValue(const std::string& longName, const std::string& value);

  void setParsedFlag(const std::string& longName, bool value);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! argc and argv in a more modern structure.
  std::vector<std::string> mArguments;

  //! Contains all defined commands and their associated descriptions.
  std::map<std::string, std::string> mDescriptions;

  //! Contains all defined commands and their associated default values.
  std::map<std::string, std::string> mDefaultValues;

  //! A map of all enum values and their associated enum types
  std::map<std::string, cedar::aux::EnumBasePtr> mEnumValues;

  //! Options stored in here are flags.
  std::set<std::string> mFlags;

  //! List of abbreviations
  std::map<char, std::string> mAbbreviations;

  //! Association of values found for parameters.
  std::map<std::string, std::string> mParsedValues;

  //! Long names of parsed flags.
  std::vector<std::string> mParsedFlags;

  //! List of unparsed values.
  std::vector<std::string> mUnparsedValues;

  //! Parameters in groups.
  std::map<std::string, std::set<std::string> > mGroups;

  // Static members
  static std::string M_STANDARD_OPTIONS_GROUP;

  static std::string M_WRITE_CONFIG_COMMAND;

  static std::string M_READ_CONFIG_COMMAND;

  static std::string M_UNGROUPED_OPTIONS_NAME;

  static std::string M_CFG_FILE_INCLUDE_TAG;

  //! a string containing a description of the executable
  std::string mDescription;

}; // class cedar::aux::CommandLineParser

namespace cedar
{
  namespace aux
  {
    //! Specialization of the getValue method for enums.
    template <> inline cedar::aux::Enum CommandLineParser::getValue(const std::string& longName) const
    {
      if (!this->isEnum(longName))
      {
        CEDAR_THROW(cedar::aux::TypeMismatchException, "The parameter \"" + longName + "\" is not an enum parameter.");
      }
      return this->mEnumValues.find(longName)->second->get(this->getValue(longName));
    }
  }
}

#endif // CEDAR_AUX_COMMAND_LINE_PARSER_H

