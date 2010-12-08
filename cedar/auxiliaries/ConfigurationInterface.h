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

    File:        ConfigurationInterface.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@rub.de
    Date:        2010 01 07

    Description: Interface for classes with configuration parameters.

    Credits:

======================================================================================================================*/

//!@todo built-in check, if addParameter adds a parameter that is already stored
//!@todo remove the "throw 20" lines, replace with real exception or something from CONFIG_READ_WRITE_STATE

#ifndef CEDAR_AUX_CONFIGURATION_INTERFACE_H
#define CEDAR_AUX_CONFIGURATION_INTERFACE_H

// LOCAL INCLUDES
#include "namespace.h"
#include "Base.h"
#include "UserData.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>

/*!@brief Interface for classes with configuration parameters.
 */
class cedar::aux::ConfigurationInterface : public cedar::aux::Base
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros, structs, enums
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Struct that holds all information about a parameter
  struct ParameterInfo
  {
    ParameterInfo();
    ~ParameterInfo();

    //! a pointer to a configuration member
    void* mpMember;
    //! list of default values stored as strings
    std::vector<std::string> mDefaultValues;
    //! lists the type of a member from libconfig's type enum
    libconfig::Setting::Type mMembersType;
    //! the name under which a parameter is stored in the configuration file
    std::string mName;
    //! flag if this parameter is a vector of a certain type
    libconfig::Setting::Type mIsVectorOfType;
    //! flag if a stored integer is unsigned (cannot extend Setting::Type)
    bool mIsUnsigned;

    /*!@brief Contains user defined data. To use this, inherit from UserData and add
     * your user-data in the derived class.
     * @remarks The user data will be deleted automatically!
     */
    UserData *mpUserData;
  };

  /*!@brief The type used to store the parameters.
   * @remark When iterating, use this to get ParameterInfoVector::iterator.
   */
  typedef std::vector<ParameterInfo> ParameterInfoVector;

  /*!@brief Enum that defines the state of a read or write process.
   *
   */
  enum CONFIG_READ_WRITE_STATE
  {
    //! a successful operation
    CONFIG_SUCCESS,
    //! an error in writing or reading a file - permission conflict or non-existing file
    CONFIG_FILE_ERROR,
    //! error related to the path of the configuration file - may not exist or permission conflict
    CONFIG_DIRECTORY_ERROR,
    //! error caused by a mismatch in expected and found type
    CONFIG_TYPE_ERROR,
    //! error caused by a missing configuration item
    CONFIG_MISSING_ITEM_ERROR
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------

  /*!@brief The standard constructor.
   */
  ConfigurationInterface();

  /*!@brief Constructor with name of configuration file.
   * @param configFileName    the name of a configuration file
   */
  ConfigurationInterface(const std::string& configFileName);

  /*!@brief Constructor with name of configuration file.
   * @param pConfigFileName    the name of a configuration file
   */
  ConfigurationInterface(const char* pConfigFileName);

  /*!@brief Destructor. */
  virtual ~ConfigurationInterface();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------

  /*!@brief Adds a parameter to the parameter list.
   * @param pMember    pointer to member variable
   * @param name    name of variable in the scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue    default value for the parameter
   * @param pUserData    multi-purpose data structure for further extensions
   */
  virtual int addParameter(
                            bool* pMember,
                            const std::string& name,
                            const bool& defaultValue,
                            UserData *pUserData = NULL
                          );

  /*!@brief Adds a parameter to the parameter list.
   * @param pMember    pointer to member variable
   * @param name    name of variable in the scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue    default value for the parameter
   * @param pUserData    multi-purpose data structure for further extensions
   */
  virtual int addParameter(
                            int* pMember,
                            const std::string& name,
                            const int& defaultValue,
                            UserData *pUserData = NULL
                          );

  /*!@brief Adds a parameter to the parameter list.
   * @param pMember    pointer to member variable
   * @param name    name of variable in the scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue    default value for the parameter
   * @param pUserData    multi-purpose data structure for further extensions
   */
  virtual int addParameter(
                            unsigned int* pMember,
                            const std::string& name,
                            const unsigned int& defaultValue,
                            UserData *pUserData = NULL
                          );

  /*!@brief Adds a parameter to the parameter list.
   * @param pMember    pointer to member variable
   * @param name    name of variable in the scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue    default value for the parameter
   * @param pUserData    multi-purpose data structure for further extensions
   */
  virtual int addParameter(
                            double* pMember,
                            const std::string& name,
                            const double& defaultValue,
                            UserData *pUserData = NULL
                          );

  /*!@brief Adds a parameter to the parameter list.
   * @param pMember    pointer to member variable
   * @param name    name of variable in the scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue    default value for the parameter
   * @param pUserData    multi-purpose data structure for further extensions
   */
  virtual int addParameter(
                            std::string* pMember,
                            const std::string& name,
                            const std::string& defaultValue,
                            UserData* pUserData = NULL
                          );

  /*!@brief Adds a parameter to the parameter list. Default value for a vector can only be a single element...
   * @param pMember    pointer to member variable
   * @param name    name of variable in th scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue    default value for the parameter
   */
  virtual int addParameter(std::vector<bool>* pMember, const std::string& name, const bool& defaultValue);

  /*!@brief Adds a parameter to the parameter list. Default value for a vector can only be a single element...
   * @param pMember    pointer to member variable
   * @param name    name of variable in th scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue    default value for the parameter
   */
  virtual int addParameter(std::vector<int>* pMember, const std::string& name, const int& defaultValue);

  /*!@brief Adds a parameter to the parameter list. Default value for a vector can only be a single element...
   * @param pMember    pointer to member variable
   * @param name    name of variable in th scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue    default value for the parameter
   */
  virtual int addParameter(
                            std::vector<unsigned int>* pMember,
                            const std::string& name,
                            const unsigned int& defaultValue
                          );

  /*!@brief Adds a parameter to the parameter list. Default value for a vector can only be a single element...
   * @param pMember    pointer to member variable
   * @param name    name of variable in th scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue    default value for the parameter
   */
  virtual int addParameter(std::vector<double>* pMember, const std::string& name, const double& defaultValue);

  /*!@brief Adds a parameter to the parameter list. Default value for a vector can only be a single element...
   * @param pMember    pointer to member variable
   * @param name    name of variable in th scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue    default value for the parameter
   */
  virtual int addParameter(std::vector<std::string>* pMember, const std::string& name, const std::string& defaultValue);

  /*!@brief Adds a parameter to the parameter list. Default value for a vector can only be a single element...
   * @param pMember    pointer to member variable
   * @param name    name of variable in th scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValues    vector of default values for the parameter
   */
  virtual int addParameter(std::vector<bool>* pMember, const std::string& name, const std::vector<bool>& defaultValues);

  /*!@brief Adds a parameter to the parameter list. Default value for a vector can only be a single element...
   * @param pMember    pointer to member variable
   * @param name    name of variable in th scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValues    vector of default values for the parameter
   */
  virtual int addParameter(std::vector<int>* pMember, const std::string& name, const std::vector<int>& defaultValues);

  /*!@brief Adds a parameter to the parameter list. Default value for a vector can only be a single element...
   * @param pMember    pointer to member variable
   * @param name    name of variable in th scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValues    vector of default values for the parameter
   */
  virtual int addParameter(
                            std::vector<unsigned int>* pMember,
                            const std::string& name,
                            const std::vector<unsigned int>& defaultValues
                          );

  /*!@brief Adds a parameter to the parameter list. Default value for a vector can only be a single element...
   * @param pMember    pointer to member variable
   * @param name    name of variable in th scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValues    vector of default value for the parameter
   */
  virtual int addParameter(
                            std::vector<double>* pMember,
                            const std::string& name,
                            const std::vector<double>& defaultValues
                          );

  /*!@brief Adds a parameter to the parameter list. Default value for a vector can only be a single element...
   * @param pMember    pointer to member variable
   * @param name    name of variable in th scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValues    vector of default values for the parameter
   */
  virtual int addParameter(
                            std::vector<std::string>* pMember,
                            const std::string& name,
                            const std::vector<std::string>& defaultValues
                          );

  /*!@brief Reads in the given set of parameters.
   * @return CONFIG_SUCCESS if successful, an error of the same enum otherwise
   */
  int readConfiguration();

  /*!@brief Reads in the given set of parameters.
   * @return CONFIG_SUCCESS if successful, an error of the same enum otherwise
   */
  int writeConfiguration();

  /*!@brief Translates a templated numeric value into a string.
   * @param value    value that should be translated into a string
   * @return the generated string
   */
  template <typename T>
  static std::string toString(const T& value)
  {
    std::stringstream input_stream;
    input_stream << std::setprecision(16) << value;
    return input_stream.str();
  }

  /*!@brief Translates a string into a templated numeric value.
   * @param value    string that should be translated back into a numeric value
   * @return numeric value extracted from string
   */
  template<typename T>
  static T to(const std::string& value)
  {
    T out;
    std::istringstream in_stream(value);
    if (!(in_stream >> out))
    {
      out = 0;
    }
    return out;
  }

  /*!@brief Returns an iteratable reference to the internal storage.
   * @return a vector of stored parameters
   */
  const ParameterInfoVector& getParameterList() const;

  /*!@brief Returns the stored configuration file name.
   * @return configuration file name
   */
  const std::string& getConfigFileName() const;

  /*!@brief Tries to read the configuration file.
   *
   * If the configuration file cannot be read the parameter values are set to their defaults.
   */
  void readOrDefaultConfiguration();

  /*!@brief Returns a vector of error messages that occured so far.
   */
  const std::vector<std::string>& getErrors();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Sets a given parameter to its default value.
   *
   * @param pParameter    void pointer to data
   * @param defaultValues    string containing the default value
   * @param type    first type info - either basic data type or array from libconfig::Setting::Type enum
   * @param arrayType    second type info - one of the basic data types from libconfig::Setting::Type enum
   */
  void setParameterToDefault(void* pParameter, std::vector<std::string> defaultValues, int type, int arrayType);

  void setParameterToDefault(const cedar::aux::ConfigurationInterface::ParameterInfo& info);

  /*!@brief Adjusts the size of the config file array, if a vector's size changed during run time.
   *
   * @param pVector    pointer to a (generic) vector
   * @param type    type of vector, taken from libconfig::Setting::Type enum
   * @param element    reference to config file array element (read off size and adjust size)
   */
  void adjustVectorSize(void* pVector, const libconfig::Setting::Type type, libconfig::Setting& element);

  int handleTypeException(const ParameterInfo& info);

  int handleSettingNotFoundException(const ParameterInfo& info);

  /*!@brief Recursively creates a new directory for a new configuration file.
   *
   * @param path    path to file - createConfigPath will cut off the top level
   */
  void createConfigPath(std::string path);

  /*!@brief Recursively creates a new group tree for a missing entry.
   * @param group    group names
   */
  void createNewGroupEntry(const std::string& name, const libconfig::Setting::Type type, libconfig::Setting& tree);

  /*!@brief This template function writes a vector of the given type to the Setting structure.
   * @param pVector    void pointer to std::vector<T>
   * @param name    name of the configuration
   * @param vectorType    type of the vector
   */
  template<typename T>
  void writeArray(void* pVector, const std::string& name, const libconfig::Setting::Type vectorType);

  /*!@brief This function writes a vector of the type unsigned int to the Setting structure.
   * @param pVector    void pointer to std::vector<T>
   * @param name    name of the configuration
   * @param vectorType    type of the vector
   */
  void writeArrayUInt(void* pVector, const std::string& name, const libconfig::Setting::Type vectorType);

  /*!@brief This template function adjusts the size of a Type::Array to match pVector.
   * @param pVector    void pointer to std::vector<T>
   * @param type    type of the array/vector, taken from Setting::Type
   * @param element    Setting object
   */
  template<typename T>
  void adjustVector(void* pVector, const libconfig::Setting::Type& type, libconfig::Setting& element);

  /*!@brief Template function that reads in an array from the configuration file with necessary exception handling.
   * @param info    a parameter info, which is used to read in an array from the configuration tree
   * @return    success or fail flag
   */
  template<typename T>
  int readArray(ParameterInfo& info);

  /*!@brief Function that removes the leaf with the given name without deleting the groups it's in.
   * @param name    full name of the configuration item, including the group path
   */
  void removeItem(const std::string& name);

  int readConfigurationFile();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none
protected:
  //! the Config object, which holds a tree of stored configuration entries
  libconfig::Config mConfig;
private:

  //! the name of the configuration file
  std::string mConfigFileName;
  //! a vector of parameters, interfacing between the configuration tree and the child class' parameter pointers
  ParameterInfoVector mParameterInfos;
  //! a vector holding various error messages
  std::vector<std::string> mConfigurationErrors;
}; // class cedar::aux::ConfigurationInterface

/*!@example ConfigurationInterfaceTest.cpp
 *this is an example of how to use the template
*/

#endif // CEDAR_AUX_CONFIGURATION_INTERFACE_H
