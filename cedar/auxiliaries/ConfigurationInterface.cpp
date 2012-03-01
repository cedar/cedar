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

    File:        ConfigurationInterface.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@rub.de
    Date:        2010 01 07

    Description: Interface for classes with configuration parameters.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/ConfigurationInterface.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <boost/property_tree/ini_parser.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

#ifndef CEDAR_LIBCONFIG_LEGACY_MODE

// Constructors
cedar::aux::ConfigurationInterface::ConfigurationInterface()
:
mConfigFileName("")
{
  mParameterInfos.clear();
  readConfigurationFile();
}

cedar::aux::ConfigurationInterface::ConfigurationInterface(const std::string& configFileName)
:
mConfigFileName(configFileName)
{
  mParameterInfos.clear();
  readConfigurationFile();
}

cedar::aux::ConfigurationInterface::ConfigurationInterface(const char* pConfigFileName)
:
mConfigFileName(std::string(pConfigFileName))
{
  mParameterInfos.clear();
  readConfigurationFile();
}

// Destructor
cedar::aux::ConfigurationInterface::~ConfigurationInterface()
{
  for (ParameterInfoVector::iterator iter = this->mParameterInfos.begin();
       iter != this->mParameterInfos.end();
       ++iter)
  {
    if (iter->mpUserData != NULL)
    {
      delete iter->mpUserData;
      iter->mpUserData = NULL;
    }
  }
}

// Destructor for ParameterInfo
cedar::aux::ConfigurationInterface::ParameterInfo::ParameterInfo()
:
mpUserData(NULL)
{
}

// Destructor for ParameterInfo
cedar::aux::ConfigurationInterface::ParameterInfo::~ParameterInfo()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

const std::string& cedar::aux::ConfigurationInterface::getName() const
{
  return _mName;
}

void cedar::aux::ConfigurationInterface::setName(const std::string& rName)
{
  _mName = rName;
}

int cedar::aux::ConfigurationInterface::addParameter(
                                          bool* pMember,
                                          const std::string& name,
                                          const bool& defaultValue,
                                          UserData *pUserData
                                        )
{
  new BoolLegacyParameter(this, pMember, name, defaultValue);

  mParameterInfos.push_back(ParameterInfo());
  ParameterInfo& parameter_info = mParameterInfos.back();
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = cedar::aux::ConfigurationInterface::toString(defaultValue);
  parameter_info.mMembersType = libconfig::Setting::TypeBoolean;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = libconfig::Setting::TypeNone;
  parameter_info.mpUserData = pUserData;
  return CONFIG_SUCCESS;
}

int cedar::aux::ConfigurationInterface::addParameter(
                                          int* pMember,
                                          const std::string& name,
                                          const int& defaultValue,
                                          UserData *pUserData
                                        )
{
  new IntLegacyParameter(this, pMember, name, defaultValue);

  mParameterInfos.push_back(ParameterInfo());
  ParameterInfo& parameter_info = mParameterInfos.back();
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = this->toString(defaultValue);
  parameter_info.mMembersType = libconfig::Setting::TypeInt;
  parameter_info.mIsUnsigned = false;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = libconfig::Setting::TypeNone;
  parameter_info.mpUserData = pUserData;
  return CONFIG_SUCCESS;
}

int cedar::aux::ConfigurationInterface::addParameter(
                                          unsigned int* pMember,
                                          const std::string& name,
                                          const unsigned int& defaultValue,
                                          UserData *pUserData
                                        )
{
  new UIntLegacyParameter(this, pMember, name, defaultValue);

  mParameterInfos.push_back(ParameterInfo());
  ParameterInfo& parameter_info = mParameterInfos.back();
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = this->toString(defaultValue);
  parameter_info.mMembersType = libconfig::Setting::TypeInt;
  parameter_info.mIsUnsigned = true;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = libconfig::Setting::TypeNone;
  parameter_info.mpUserData = pUserData;
  return CONFIG_SUCCESS;
}

int cedar::aux::ConfigurationInterface::addParameter(
                                          double* pMember,
                                          const std::string& name,
                                          const double& defaultValue,
                                          UserData *pUserData
                                        )
{
  new DoubleLegacyParameter(this, pMember, name, defaultValue);

  mParameterInfos.push_back(ParameterInfo());
  ParameterInfo& parameter_info = mParameterInfos.back();
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = this->toString(defaultValue);
  parameter_info.mMembersType = libconfig::Setting::TypeFloat;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = libconfig::Setting::TypeNone;
  parameter_info.mpUserData = pUserData;
  return CONFIG_SUCCESS;
}

int cedar::aux::ConfigurationInterface::addParameter(
                                          std::string* pMember,
                                          const std::string& name,
                                          const std::string& defaultValue,
                                          UserData*
                                        )
{
  new StringLegacyParameter(this, pMember, name, defaultValue);

  mParameterInfos.push_back(ParameterInfo());
  ParameterInfo& parameter_info = mParameterInfos.back();
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = defaultValue;
  parameter_info.mMembersType = libconfig::Setting::TypeString;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = libconfig::Setting::TypeNone;
  return CONFIG_SUCCESS;
}

int cedar::aux::ConfigurationInterface::addParameter(
                                          std::vector<bool>* pMember,
                                          const std::string& name,
                                          const bool& defaultValue
                                        )
{
  new BoolLegacyVectorParameter(this, pMember, name, defaultValue);

  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = cedar::aux::ConfigurationInterface::toString(defaultValue);
  parameter_info.mMembersType = libconfig::Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = libconfig::Setting::TypeBoolean;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int cedar::aux::ConfigurationInterface::addParameter(
                                          std::vector<bool>* pMember,
                                          const std::string& name,
                                          const std::vector<bool>& defaultValues
                                        )
{
  new BoolLegacyVectorParameter(this, pMember, name, defaultValues);

  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.clear();
  for(unsigned int i = 0; i < defaultValues.size(); i++)
  {
    parameter_info.mDefaultValues.push_back(ConfigurationInterface::toString(defaultValues.at(i)));
  }
  parameter_info.mMembersType = libconfig::Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = libconfig::Setting::TypeBoolean;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int cedar::aux::ConfigurationInterface::addParameter(
                                          std::vector<int>* pMember,
                                          const std::string& name,
                                          const int& defaultValue
                                        )
{
  new IntLegacyVectorParameter(this, pMember, name, defaultValue);

  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = cedar::aux::ConfigurationInterface::toString(defaultValue);
  parameter_info.mMembersType = libconfig::Setting::TypeArray;
  parameter_info.mIsUnsigned = false;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = libconfig::Setting::TypeInt;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int cedar::aux::ConfigurationInterface::addParameter(
                                          std::vector<unsigned int>* pMember,
                                          const std::string& name,
                                          const unsigned int& defaultValue
                                        )
{
  new UIntLegacyVectorParameter(this, pMember, name, defaultValue);

  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = cedar::aux::ConfigurationInterface::toString(defaultValue);
  parameter_info.mMembersType = libconfig::Setting::TypeArray;
  parameter_info.mIsUnsigned = true;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = libconfig::Setting::TypeInt;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int cedar::aux::ConfigurationInterface::addParameter(
                                          std::vector<int>* pMember,
                                          const std::string& name,
                                          const std::vector<int>& defaultValues
                                        )
{
  new IntLegacyVectorParameter(this, pMember, name, defaultValues);

  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.clear();
  for(unsigned int i = 0; i < defaultValues.size(); i++)
  {
    parameter_info.mDefaultValues.push_back(ConfigurationInterface::toString(defaultValues.at(i)));
  }
  parameter_info.mMembersType = libconfig::Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = libconfig::Setting::TypeInt;
  parameter_info.mIsUnsigned = false;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int cedar::aux::ConfigurationInterface::addParameter(
                                          std::vector<unsigned int>* pMember,
                                          const std::string& name,
                                          const std::vector<unsigned int>& defaultValues
                                        )
{
  new UIntLegacyVectorParameter(this, pMember, name, defaultValues);

  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.clear();
  for(unsigned int i = 0; i < defaultValues.size(); i++)
  {
    parameter_info.mDefaultValues.push_back(ConfigurationInterface::toString(defaultValues.at(i)));
  }
  parameter_info.mMembersType = libconfig::Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = libconfig::Setting::TypeInt;
  parameter_info.mIsUnsigned = true;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int cedar::aux::ConfigurationInterface::addParameter(
                                          std::vector<double>* pMember,
                                          const std::string& name,
                                          const double& defaultValue
                                        )
{
  new DoubleLegacyVectorParameter(this, pMember, name, defaultValue);

  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = cedar::aux::ConfigurationInterface::toString(defaultValue);
  parameter_info.mMembersType = libconfig::Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = libconfig::Setting::TypeFloat;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int cedar::aux::ConfigurationInterface::addParameter(
                                          std::vector<double>* pMember,
                                          const std::string& name,
                                          const std::vector<double>& defaultValues
                                        )
{
  new DoubleLegacyVectorParameter(this, pMember, name, defaultValues);

  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.clear();
  for(unsigned int i = 0; i < defaultValues.size(); i++)
  {
    parameter_info.mDefaultValues.push_back(ConfigurationInterface::toString(defaultValues.at(i)));
  }
  parameter_info.mMembersType = libconfig::Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = libconfig::Setting::TypeFloat;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int cedar::aux::ConfigurationInterface::addParameter(
                                          std::vector<std::string>* pMember,
                                          const std::string& name,
                                          const std::string& defaultValue
                                        )
{
  new StringLegacyVectorParameter(this, pMember, name, defaultValue);

  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = defaultValue;
  parameter_info.mMembersType = libconfig::Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = libconfig::Setting::TypeString;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int cedar::aux::ConfigurationInterface::addParameter(
                                          std::vector<std::string>* pMember,
                                          const std::string& name,
                                          const std::vector<std::string>& defaultValues
                                        )
{
  new StringLegacyVectorParameter(this, pMember, name, defaultValues);

  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.clear();
  for(unsigned int i = 0; i < defaultValues.size(); i++)
  {
    parameter_info.mDefaultValues.push_back(defaultValues.at(i));
  }
  parameter_info.mMembersType = libconfig::Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = libconfig::Setting::TypeString;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

void cedar::aux::ConfigurationInterface::addParameter(int* pMember,
                                  const std::string& name,
                                  int defaultValue,
                                  int min,
                                  int max,
                                  int step,
                                  bool readOnly
                                  )
{
  this->addParameter(pMember, name, defaultValue, new cedar::aux::IntervalData<int>(min, max, step, readOnly));
}

void cedar::aux::ConfigurationInterface::addParameter(double* pMember,
                                  const std::string& name,
                                  double defaultValue,
                                  double min,
                                  double max,
                                  double step,
                                  bool readOnly
                                  )
{
  this->addParameter(pMember, name, defaultValue, new cedar::aux::IntervalData<double>(min, max, step, readOnly));
}

int cedar::aux::ConfigurationInterface::readConfigurationFile()
{
  if (this->mConfigFileName.empty())
  {
    this->mConfigurationErrors.push_back("Tried to read with an empty file name.");
    return CONFIG_FILE_ERROR;
  }

  try
  {
    this->readOldConfig(this->mConfigFileName);
    return CONFIG_SUCCESS;
  }
  catch (boost::property_tree::ini_parser::ini_parser_error& e)
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "INI parser error:\nmessage: " + e.message()
        + "\nfile: " + e.filename()
        + "\nline: " + cedar::aux::toString(e.line()),
      "cedar::aux::ConfigurationInterface::readConfigurationFile()"
    );
    this->mConfigurationErrors.push_back("The INI parser threw an error.");
    return CONFIG_FILE_ERROR;
  }
  catch (cedar::aux::FileNotFoundException&)
  {
    this->defaultAll();
    this->mConfigurationErrors.push_back("The specified configuration file does not exist.");
    return CONFIG_FILE_ERROR;
  }
}

int cedar::aux::ConfigurationInterface::readConfiguration()
{
  return this->readConfigurationFile();
}

int cedar::aux::ConfigurationInterface::handleTypeException(const cedar::aux::ConfigurationInterface::ParameterInfo&)
{
  return CONFIG_SUCCESS;
}

int cedar::aux::ConfigurationInterface::handleSettingNotFoundException(const cedar::aux::ConfigurationInterface::ParameterInfo&)
{
  return CONFIG_SUCCESS;
}

int cedar::aux::ConfigurationInterface::writeConfiguration()
{
  if (!this->mConfigFileName.empty())
  {
    try
    {
      this->writeOldConfig(this->mConfigFileName);
      return CONFIG_SUCCESS;
    }
    catch (boost::property_tree::ini_parser::ini_parser_error& e)
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "INI parser error:\nmessage: " + e.message()
          + "\nfile: " + e.filename()
          + "\nline: " + cedar::aux::toString(e.line()),
        "cedar::aux::ConfigurationInterface::writeConfiguration()"
      );
      this->mConfigurationErrors.push_back("The INI parser threw an error.");
      return CONFIG_FILE_ERROR;
    }
  }
  else
  {
    return CONFIG_FILE_ERROR;
  }
}

void cedar::aux::ConfigurationInterface::setParameterToDefault(
                                                    void* /*pParameter*/,
                                                    std::vector<std::string> /*defaultValues*/,
                                                    int /*type*/,
                                                    int /*arrayType*/
                                                  )
{
}

void cedar::aux::ConfigurationInterface::setParameterToDefault(const ParameterInfo& /*info*/)
{
//  this->setParameterToDefault(info.mpMember, info.mDefaultValues, info.mMembersType, info.mIsVectorOfType);
}

const cedar::aux::ConfigurationInterface::ParameterInfoVector& cedar::aux::ConfigurationInterface::getParameterList() const
{
  return this->mParameterInfos;
}

const std::string& cedar::aux::ConfigurationInterface::getConfigFileName() const
{
  return this->mConfigFileName;
}

void cedar::aux::ConfigurationInterface::adjustVectorSize(
                                               void* /*pVector*/,
                                               const libconfig::Setting::Type /*type*/,
                                               libconfig::Setting& /*element*/
                                             )
{
}

void cedar::aux::ConfigurationInterface::readOrDefaultConfiguration()
{
  if (this->readConfigurationFile() != cedar::aux::ConfigurationInterface::CONFIG_SUCCESS)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "error (" + mConfigFileName + "): Some parameters missing or corrupt, writing additional parameters to file.",
      "cedar::aux::ConfigurationInterface::readOrDefaultConfiguration()"
    );

    if (this->writeConfiguration() != cedar::aux::ConfigurationInterface::CONFIG_SUCCESS)
    {
      cedar::aux::LogSingleton::getInstance()->error
      (
        "error (" + mConfigFileName + "): Could not write config file, please check file permissions.",
        "cedar::aux::ConfigurationInterface::readOrDefaultConfiguration()"
      );
    }
  }
}

/*!Recursively creates a new directory for a new configuration file.
 * First, the given path is cut off after the last slash. If the function
 * reached root, nothing is done. If not, createConfigPath checks if the current path
 * already exists. If not, it is called again (recursively) with
 * the cut-off path. When this call returns, the current path is created with
 * system("mkdir " + cut_off_pathname).
 */
void cedar::aux::ConfigurationInterface::createConfigPath(std::string /*path*/)
{
}

//!Recursively add groups to the setting tree until we reach a leaf. The leaf is created with
void cedar::aux::ConfigurationInterface::createNewGroupEntry(
                                                  const std::string& /*name*/,
                                                  const libconfig::Setting::Type /*type*/,
                                                  libconfig::Setting& /*tree*/
                                                )
{
}

template<typename T>
void cedar::aux::ConfigurationInterface::writeArray(
                                         void* /*pVector*/,
                                         const std::string& /*name*/,
                                         const libconfig::Setting::Type /*vectorType*/
                                       )
{
}

void cedar::aux::ConfigurationInterface::writeArrayUInt(
                                             void* /*pVector*/,
                                             const std::string& /*name*/,
                                             const libconfig::Setting::Type /*vectorType*/
                                           )
{
}

template<typename T>
void cedar::aux::ConfigurationInterface::adjustVector(
                                           void* /*pVector*/,
                                           const libconfig::Setting::Type& /*type*/,
                                           libconfig::Setting& /*element*/
                                         )
{
}

template<typename T>
int cedar::aux::ConfigurationInterface::readArray(ParameterInfo& /*info*/)
{
  return CONFIG_TYPE_ERROR;
}

void cedar::aux::ConfigurationInterface::removeItem(const std::string& /*name*/)
{
}

const std::vector<std::string>& cedar::aux::ConfigurationInterface::getErrors()
{
  return mConfigurationErrors;
}

#endif // CEDAR_LIBCONFIG_LEGACY_MODE
