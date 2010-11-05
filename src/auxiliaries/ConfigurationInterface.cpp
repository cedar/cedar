/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        ConfigurationInterface.cpp

 ----- Author:      Stephan Zibner
 ----- Email:       stephan.zibner@rub.de
 ----- Date:        2010 01 07

 ----- Description: Interface for classes with configuration parameters.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "ConfigurationInterface.h"
#include "UserData.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace std;
using namespace libconfig;
using namespace cedar::aux;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

ConfigurationInterface::ConfigurationInterface()
:
mConfigFileName("newConfiguration")
{
  mParameterInfos.clear();
}

ConfigurationInterface::ConfigurationInterface(const std::string& configFileName)
:
mConfigFileName(configFileName)
{
  mParameterInfos.clear();
}

ConfigurationInterface::ConfigurationInterface(const char* pConfigFileName)
:
mConfigFileName(std::string(pConfigFileName))
{
  mParameterInfos.clear();
}

ConfigurationInterface::~ConfigurationInterface()
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
ConfigurationInterface::ParameterInfo::ParameterInfo()
{
  this->mpUserData = NULL;
}

// Destructor for ParameterInfo
ConfigurationInterface::ParameterInfo::~ParameterInfo()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

int ConfigurationInterface::addParameter(
                                          bool* pMember,
                                          std::string name,
                                          bool defaultValue,
                                          UserData *pUserData
                                        )
{
  mParameterInfos.push_back(ParameterInfo());
  ParameterInfo& parameter_info = mParameterInfos.back();
  parameter_info.mpMember = static_cast<void*> (pMember);
  std::stringstream input_stream;
  if (defaultValue)
  {
    input_stream << std::setprecision(16) << 1;
  } else
  {
    input_stream << std::setprecision(16) << 0;
  }
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = input_stream.str();
  parameter_info.mMembersType = Setting::TypeBoolean;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeNone;
  parameter_info.mpUserData = pUserData;
  return CONFIG_SUCCESS;
}

int ConfigurationInterface::addParameter(
                                          int* pMember,
                                          std::string name,
                                          int defaultValue,
                                          UserData *pUserData
                                        )
{
  mParameterInfos.push_back(ParameterInfo());
  ParameterInfo& parameter_info = mParameterInfos.back();
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = this->toString(defaultValue);
  parameter_info.mMembersType = Setting::TypeInt;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeNone;
  parameter_info.mpUserData = pUserData;

  return CONFIG_SUCCESS;
}

int ConfigurationInterface::addParameter(
                                          double* pMember,
                                          std::string name,
                                          double defaultValue,
                                          UserData *pUserData
                                        )
{
  mParameterInfos.push_back(ParameterInfo());
  ParameterInfo& parameter_info = mParameterInfos.back();
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = this->toString(defaultValue);
  parameter_info.mMembersType = Setting::TypeFloat;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeNone;
  parameter_info.mpUserData = pUserData;

  return CONFIG_SUCCESS;
}

int ConfigurationInterface::addParameter(
                                          std::string* pMember,
                                          std::string name,
                                          std::string defaultValue,
                                          UserData*
                                        )
{
  mParameterInfos.push_back(ParameterInfo());
  ParameterInfo& parameter_info = mParameterInfos.back();
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = defaultValue;
  parameter_info.mMembersType = Setting::TypeString;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeNone;
  return CONFIG_SUCCESS;
}

int ConfigurationInterface::addParameter(vector<bool>* pMember,
    std::string name, bool defaultValue)
{
  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  std::stringstream input_stream;
  if (defaultValue)
  {
    input_stream << std::setprecision(16) << 1;
  } else
  {
    input_stream << std::setprecision(16) << 0;
  }
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = input_stream.str();
  parameter_info.mMembersType = Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeBoolean;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int ConfigurationInterface::addParameter(vector<bool>* pMember,
    std::string name, std::vector<bool> defaultValues)
{
  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.clear();
  for(unsigned int i = 0; i < defaultValues.size(); i++)
  {
    std::stringstream input_stream;
    if (defaultValues.at(i))
    {
      input_stream << std::setprecision(16) << 1;
    } else
    {
      input_stream << std::setprecision(16) << 0;
    }
    parameter_info.mDefaultValues.push_back(input_stream.str());
  }
  parameter_info.mMembersType = Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeBoolean;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int ConfigurationInterface::addParameter(vector<int>* pMember,
    std::string name, int defaultValue)
{
  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  std::stringstream input_stream;
  input_stream << std::setprecision(16) << defaultValue;
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = input_stream.str();
  parameter_info.mMembersType = Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeInt;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int ConfigurationInterface::addParameter(vector<int>* pMember,
    std::string name, std::vector<int> defaultValues)
{
  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.clear();
  for(unsigned int i = 0; i < defaultValues.size(); i++)
  {
    std::stringstream input_stream;
    input_stream << std::setprecision(16) << defaultValues.at(i);
    parameter_info.mDefaultValues.push_back(input_stream.str());
  }
  parameter_info.mMembersType = Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeInt;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int ConfigurationInterface::addParameter(vector<double>* pMember,
    std::string name, double defaultValue)
{
  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  std::stringstream input_stream;
  input_stream << std::setprecision(16) << defaultValue;
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = input_stream.str();
  parameter_info.mMembersType = Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeFloat;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int ConfigurationInterface::addParameter(vector<double>* pMember,
    std::string name, std::vector<double> defaultValues)
{
  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.clear();
  for(unsigned int i = 0; i < defaultValues.size(); i++)
  {
    std::stringstream input_stream;
    input_stream << std::setprecision(16) << defaultValues.at(i);
    parameter_info.mDefaultValues.push_back(input_stream.str());
  }
  parameter_info.mMembersType = Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeFloat;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int ConfigurationInterface::addParameter(vector<std::string>* pMember,
    std::string name, std::string defaultValue)
{
  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = defaultValue;
  parameter_info.mMembersType = Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeString;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int ConfigurationInterface::addParameter(vector<std::string>* pMember,
    std::string name, std::vector<std::string> defaultValues)
{
  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.clear();
  for(unsigned int i = 0; i < defaultValues.size(); i++)
  {
    parameter_info.mDefaultValues.push_back(defaultValues.at(i));
  }
  parameter_info.mMembersType = Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeString;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int ConfigurationInterface::readConfiguration()
{
  int info = CONFIG_SUCCESS;
  // Read the file. If there is an error, report it and exit.
  try
  {
    mConfig.readFile(mConfigFileName.c_str());
  } catch (const FileIOException &fioex) // general IO error, file not readable
  {
    std::cerr << "> error in " << mConfigFileName
        << ": I/O error while reading file." << std::endl;
    for (ParameterInfoVector::iterator iter = this->mParameterInfos.begin(); iter
        != this->mParameterInfos.end(); ++iter)
    {
      setParameterToDefault(*iter);
    }
    return CONFIG_FILE_ERROR;
  } catch (const ParseException &pex) // parser error - some errors can be fixed
  {
    std::cerr << "> error in " << mConfigFileName << ": Parse error at "
        << pex.getFile() << ":" << pex.getLine() << " - " << pex.getError()
        << std::endl;
    std::cerr
        << "> Trying to reset parameters.. If it does not work, please stop messing "
          "around with the config file" << std::endl;
    // reset all parameters (the line is known, but not the parameter)
    for (ParameterInfoVector::iterator iter = this->mParameterInfos.begin(); iter
        != this->mParameterInfos.end(); ++iter)
    {
      setParameterToDefault(*iter);
    }
    Setting &root = mConfig.getRoot();
    for (int i = 0; i < root.getLength(); i++)
    {
      std::string parameter_name = this->mParameterInfos.at(i).mName;
      if (root.exists(parameter_name) && root[parameter_name].getType()
          == Setting::TypeArray)
      {
        Setting &temp = root[parameter_name];
        for (int j = 0; j < root[parameter_name].getLength(); j++)
        {
          temp.remove(j);
        }
      }
      if (root.exists(parameter_name))
      {
        root.remove(parameter_name);
      }
    }
    return CONFIG_FILE_ERROR;
  }

  for (ParameterInfoVector::iterator iter = this->mParameterInfos.begin(); iter
      != this->mParameterInfos.end(); ++iter)
  {
    switch (iter->mMembersType)
    {
    case (Setting::TypeFloat):
    {
      try
      {
        *(static_cast<double*> (iter->mpMember)) = mConfig.lookup(iter->mName);
      } catch (const SettingNotFoundException &nfex)
      {
        info = handleSettingNotFoundException(*iter);
      } catch (const SettingTypeException &nfex)
      {
        info = this->handleTypeException(*iter);
      }
      break;
    } // END case TypeFloat
    case (Setting::TypeBoolean):
    {
      try
      {
        *(static_cast<bool*> (iter->mpMember)) = mConfig.lookup(iter->mName);
      } catch (const SettingNotFoundException &nfex)
      {
        info = handleSettingNotFoundException(*iter);
      } catch (const SettingTypeException &nfex)
      {
        info = this->handleTypeException(*iter);
      }
      break;
    } // END case TypeBoolean
    case (Setting::TypeInt):
    {
      try
      {
        int temp = mConfig.lookup(iter->mName);
        *(static_cast<int*> (iter->mpMember)) = temp;
      } catch (const SettingNotFoundException &nfex)
      {
        info = handleSettingNotFoundException(*iter);
      } catch (const SettingTypeException &nfex)
      {
        info = this->handleTypeException(*iter);
      }
      break;
    } // END case TypeInt
    case (Setting::TypeString):
    {
      try
      {
        string temp = mConfig.lookup(iter->mName);
        *(static_cast<string*> (iter->mpMember)) = temp;
      } catch (const SettingNotFoundException &nfex)
      {
        info = handleSettingNotFoundException(*iter);
      } catch (const SettingTypeException &nfex)
      {
        info = this->handleTypeException(*iter);
      }
      break;
    } // END case TypeString
    case (Setting::TypeArray):
    {
      switch (iter->mIsVectorOfType)
      {
      case (Setting::TypeInt):
      {
        try
        {
          Setting &root = mConfig.getRoot();
          if (root[iter->mName].getType() != Setting::TypeArray)
          {
            throw 20;
          }
          std::vector<int>* p_vector =
              static_cast<std::vector<int>*> (iter->mpMember);
          p_vector->clear();
          for (int j = 0; j < root[iter->mName].getLength(); j++)
          {
            Setting &setting = root[iter->mName][j];
            if (setting.getType() != Setting::TypeInt)
            {
              throw 20;
            }
            p_vector->push_back((int) root[iter->mName][j]);
          }
        } catch (const SettingNotFoundException &nfex)
        {
          info = handleSettingNotFoundException(*iter);
        } catch (const SettingTypeException &nfex)
        {
          cerr << "> error in " << mConfigFileName << ": Wrong type for '"
              << iter->mName << "' setting in configuration file." << endl;
          std::vector<int>* p_vector =
              static_cast<std::vector<int>*> (iter->mpMember);
          Setting &root = mConfig.getRoot();
          root.remove(iter->mName);
          Setting &array = root.add(iter->mName, iter->mMembersType);
          for (unsigned int j = 0; j < p_vector->size(); j++)
          {
            array.add(Setting::TypeInt);
          }
          setParameterToDefault(*iter);
          info = CONFIG_TYPE_ERROR;
        } catch (int e)
        {
          cerr << "> error in " << mConfigFileName << ": array '"
              << iter->mName << "' is corrupt" << std::endl;
          Setting &root = mConfig.getRoot();
          root.remove(iter->mName);
          Setting &array = root.add(iter->mName, iter->mMembersType);
          array.add(Setting::TypeInt);
          setParameterToDefault(*iter);
          info = CONFIG_TYPE_ERROR;
        }
        break;
      } // END array case int
      case (Setting::TypeFloat):
      {
        try
        {
          Setting &root = mConfig.getRoot();
          if (root[iter->mName].getType() != Setting::TypeArray)
          {
            throw 20;
          }
          std::vector<double>* p_vector =
              static_cast<std::vector<double>*> (iter->mpMember);
          p_vector->clear();
          for (int j = 0; j < root[iter->mName].getLength(); j++)
          {
            Setting &setting = root[iter->mName][j];
            if (setting.getType() != Setting::TypeFloat)
            {
              throw 20;
            }
            p_vector->push_back((double) root[iter->mName][j]);
          }
        } catch (const SettingNotFoundException &nfex)
        {
          info = handleSettingNotFoundException(*iter);
        } catch (const SettingTypeException &nfex)
        {
          cerr << "> error in " << mConfigFileName << ": Wrong type for '"
              << iter->mName << "' setting in configuration file." << endl;
          std::vector<double>* p_vector =
              static_cast<std::vector<double>*> (iter->mpMember);
          Setting &root = mConfig.getRoot();
          root.remove(iter->mName);
          Setting &array = root.add(iter->mName, iter->mMembersType);
          for (unsigned int j = 0; j < p_vector->size(); j++)
          {
            array.add(Setting::TypeFloat);
          }
          setParameterToDefault(*iter);
          info = CONFIG_TYPE_ERROR;
        } catch (int e)
        {
          cerr << "> error in " << mConfigFileName << ": array '"
              << iter->mName << "' is corrupt" << std::endl;
          Setting &root = mConfig.getRoot();
          root.remove(iter->mName);
          Setting &array = root.add(iter->mName, iter->mMembersType);
          array.add(Setting::TypeFloat);
          setParameterToDefault(*iter);
          info = CONFIG_TYPE_ERROR;
        }
        break;
      } // END array case float
      case (Setting::TypeBoolean):
      {
        try
        {
          Setting &root = mConfig.getRoot();
          if (root[iter->mName].getType() != Setting::TypeArray)
          {
            throw 20;
          }
          std::vector<bool>* p_vector =
              static_cast<std::vector<bool>*> (iter->mpMember);
          p_vector->clear();
          for (int j = 0; j < root[iter->mName].getLength(); j++)
          {
            Setting &setting = root[iter->mName][j];
            if (setting.getType() != Setting::TypeBoolean)
            {
              throw 20;
            }
            p_vector->push_back((bool) root[iter->mName][j]);
          }
        } catch (const SettingNotFoundException &nfex)
        {
          info = handleSettingNotFoundException(*iter);
        } catch (const SettingTypeException &nfex)
        {
          cerr << "> error in " << mConfigFileName << ": Wrong type for '"
              << iter->mName << "' setting in configuration file." << endl;
          std::vector<double>* p_vector =
              static_cast<std::vector<double>*> (iter->mpMember);
          Setting &root = mConfig.getRoot();
          root.remove(iter->mName);
          Setting &array = root.add(iter->mName, iter->mMembersType);
          for (unsigned int j = 0; j < p_vector->size(); j++)
          {
            array.add(Setting::TypeBoolean);
          }
          setParameterToDefault(*iter);
          info = CONFIG_TYPE_ERROR;
        } catch (int e)
        {
          cerr << "> error in " << mConfigFileName << ": array '"
              << iter->mName << "' is corrupt" << std::endl;
//          std::vector<bool>* p_vector =
//              static_cast<std::vector<bool>*> (iter->mpMember);
          Setting &root = mConfig.getRoot();
          root.remove(iter->mName);
          Setting &array = root.add(iter->mName, iter->mMembersType);
          array.add(Setting::TypeBoolean);
          setParameterToDefault(*iter);
          info = CONFIG_TYPE_ERROR;
        }
        break;
      } // END array case bool
      case (Setting::TypeString):
      {
        try
        {
          Setting &root = mConfig.getRoot();
          if (root[iter->mName].getType() != Setting::TypeArray)
          {
            throw 20;
          }
          std::vector<std::string>* p_vector = static_cast<std::vector<
              std::string>*> (iter->mpMember);
          p_vector->clear();
          for (int j = 0; j < root[iter->mName].getLength(); j++)
          {
            Setting &setting = root[iter->mName][j];
            if (setting.getType() != Setting::TypeString)
            {
              throw 20;
            }
            std::string temp = root[iter->mName][j];
            p_vector->push_back(temp);
          }
        } catch (const SettingNotFoundException &nfex)
        {
          info = handleSettingNotFoundException(*iter);
        } catch (const SettingTypeException &nfex)
        {
          cerr << "> error in " << mConfigFileName << ": Wrong type for '"
              << iter->mName << "' setting in configuration file." << endl;
          std::vector<std::string>* p_vector = static_cast<std::vector<
              std::string>*> (iter->mpMember);
          Setting &root = mConfig.getRoot();
          root.remove(iter->mName);
          Setting &array = root.add(iter->mName, iter->mMembersType);
          for (unsigned int j = 0; j < p_vector->size(); j++)
          {
            array.add(Setting::TypeString);
          }
          setParameterToDefault(*iter);
          info = CONFIG_TYPE_ERROR;
        } catch (int e)
        {
          cerr << "> error in " << mConfigFileName << ": array '"
              << iter->mName << "' is corrupt" << std::endl;
//          std::vector<std::string>* p_vector = static_cast<std::vector<
//              std::string>*> (iter->mpMember);
          Setting &root = mConfig.getRoot();
          root.remove(iter->mName);
          Setting &array = root.add(iter->mName, iter->mMembersType);
          array.add(Setting::TypeString);
          setParameterToDefault(*iter);
          info = CONFIG_TYPE_ERROR;
        }
        break;
      } // END array case string
      default:
      {
        info = EXIT_FAILURE;
        break;
      } // END array case default
      } // END switch array type
      break;
    } // END case TypeArray
    default:
    {
      info = EXIT_FAILURE;
      break;
    } // END case default
    } // END switch
  }
  return info;
}

int ConfigurationInterface::handleTypeException(
    const ConfigurationInterface::ParameterInfo& info)
{
  cerr << "> error in " << mConfigFileName << ": Wrong type for '"
      << info.mName << "' setting in configuration file." << endl;
  Setting &root = mConfig.getRoot();
  root.remove(info.mName);
  root.add(info.mName, info.mMembersType);
  setParameterToDefault(info);
  return CONFIG_TYPE_ERROR;
}

int ConfigurationInterface::handleSettingNotFoundException(
    const ConfigurationInterface::ParameterInfo& info)
{
  cerr << "> error in " << mConfigFileName << ": No '" << info.mName
      << "' setting in configuration file." << endl;
  setParameterToDefault(info);
  return CONFIG_MISSING_ITEM_ERROR;
}

int ConfigurationInterface::writeConfiguration()
{
  // export current values of the parameters into the config object
  Setting &root = mConfig.getRoot();
  for (ParameterInfoVector::iterator iter = this->mParameterInfos.begin(); iter
      != this->mParameterInfos.end(); ++iter)
  {
    switch (iter->mMembersType)
    // switch for first type - either basic data type or array
    {
    case (Setting::TypeFloat):
    {
      if (!root.exists(iter->mName))
      {
        root.add(iter->mName, iter->mMembersType);
      }
      double temp = *(static_cast<double*> (iter->mpMember));
      mConfig.lookup(iter->mName) = temp;
      break;
    } // END case TypeFloat
    case (Setting::TypeInt):
    {
      if (!root.exists(iter->mName))
      {
        root.add(iter->mName, iter->mMembersType);
      }
      mConfig.lookup(iter->mName) = *(static_cast<int*> (iter->mpMember));
      break;
    } // END case TypeInt
    case (Setting::TypeBoolean):
    {
      if (!root.exists(iter->mName))
      {
        root.add(iter->mName, iter->mMembersType);
      }
      bool temp = *(static_cast<bool*> (iter->mpMember));
      mConfig.lookup(iter->mName) = temp;
      break;
    } // END case TypeBoolean
    case (Setting::TypeString):
    {
      if (!root.exists(iter->mName))
      {
        root.add(iter->mName, iter->mMembersType);
      }
      string temp = *(static_cast<string*> (iter->mpMember));
      mConfig.lookup(iter->mName) = temp;
      break;
    } // END case TypeString
    case (Setting::TypeArray):
    {
      switch (iter->mIsVectorOfType)
      // switch for second type - only available in arrays
      {
      case (Setting::TypeBoolean):
      {
        std::vector<bool>* p_vector =
            static_cast<std::vector<bool>*> (iter->mpMember);
        if (!root.exists(iter->mName))
        {
          Setting &array = root.add(iter->mName, iter->mMembersType);
          for (unsigned int j = 0; j < p_vector->size(); j++)
          {
            array.add(Setting::TypeBoolean);
          }
        }
        adjustVectorSize(iter->mpMember, iter->mIsVectorOfType,
            root[iter->mName]);
        for (unsigned int j = 0; j < p_vector->size(); j++)
        {
          mConfig.lookup(iter->mName)[j] = p_vector->at(j);
        }
        break;
      } // END case array bool
      case (Setting::TypeInt):
      {
        std::vector<int>* p_vector =
            static_cast<std::vector<int>*> (iter->mpMember);
        if (!root.exists(iter->mName))
        {
          Setting &array = root.add(iter->mName, iter->mMembersType);
          for (unsigned int j = 0; j < p_vector->size(); j++)
          {
            array.add(Setting::TypeInt);
          }
        }
        adjustVectorSize(iter->mpMember, iter->mIsVectorOfType,
            root[iter->mName]);
        for (unsigned int j = 0; j < p_vector->size(); j++)
        {
          mConfig.lookup(iter->mName)[j] = p_vector->at(j);
        }
        break;
      }
      case (Setting::TypeFloat):
      {
        std::vector<double>* p_vector =
            static_cast<std::vector<double>*> (iter->mpMember);
        if (!root.exists(iter->mName))
        {
          Setting &array = root.add(iter->mName, iter->mMembersType);
          for (unsigned int j = 0; j < p_vector->size(); j++)
          {
            array.add(Setting::TypeFloat);
          }
        }
        adjustVectorSize(iter->mpMember, iter->mIsVectorOfType,
            root[iter->mName]);
        for (unsigned int j = 0; j < p_vector->size(); j++)
        {
          mConfig.lookup(iter->mName)[j] = p_vector->at(j);
        }
        break;
      }
      case (Setting::TypeString):
      {
        std::vector<std::string>* p_vector = static_cast<std::vector<
            std::string>*> (iter->mpMember);
        if (!root.exists(iter->mName))
        {
          Setting &array = root.add(iter->mName, iter->mMembersType);
          for (unsigned int j = 0; j < p_vector->size(); j++)
          {
            array.add(Setting::TypeString);
          }
        }
        adjustVectorSize(iter->mpMember, iter->mIsVectorOfType,
            root[iter->mName]);
        for (unsigned int j = 0; j < p_vector->size(); j++)
        {
          std::string temp = p_vector->at(j);
          mConfig.lookup(iter->mName)[j] = temp;
        }
        break;
      }
      default:
      {
        std::cerr << "Unsupported data type in: " << (mConfigFileName.c_str())
            << endl;
        break;
      }
      } // END switch array types
      break;
    } // END case TypeArray
    default:
    {
      std::cerr << "Unsupported data type in: " << (mConfigFileName.c_str())
          << endl;
      break;
    }
    } // END switch types
  } // END for all parameters
  // save to file
  try
  {
    mConfig.writeFile(mConfigFileName.c_str());
#ifdef DEBUG
    cout << "New configuration successfully written to: " << (mConfigFileName.c_str()) << endl;
#endif
  } catch (const FileIOException &fioex)
  {
    cerr << "I/O error while writing file: " << (mConfigFileName.c_str())
        << endl;
    return CONFIG_FILE_ERROR;
  }
  return CONFIG_SUCCESS;
}

void ConfigurationInterface::setParameterToDefault(void* pParameter,
    std::vector<std::string> defaultValues, int type, int arrayType)
{
  switch (type)
  // switch for first type (either basic data type or array)
  {
  case (Setting::TypeBoolean):
  {
    int value;
    std::istringstream in_stream(defaultValues.at(0));
    if (!(in_stream >> value))
    {
      *(static_cast<bool*> (pParameter)) = false;
    } else
    {
      if (value == 1)
      {
        *(static_cast<bool*> (pParameter)) = true;
      } else
      {
        *(static_cast<bool*> (pParameter)) = false;
      }
    }
    break;
  }
  case (Setting::TypeInt):
  {
    std::istringstream in_stream(defaultValues.at(0));
    if (!(in_stream >> *(static_cast<int*> (pParameter))))
    {
      *(static_cast<int*> (pParameter)) = 0;
    }
    break;
  }
  case (Setting::TypeFloat):
  {
    std::istringstream in_stream(defaultValues.at(0));
    if (!(in_stream >> *(static_cast<double*> (pParameter))))
    {
      *(static_cast<double*> (pParameter)) = 0;
    }
    break;
  }
  case (Setting::TypeString):
  {
    *(static_cast<std::string*> (pParameter)) = defaultValues.at(0);
    break;
  }//TODO add vector read out here
  case (Setting::TypeArray):
  {
    switch (arrayType)
    // second level switch - array can be of each basic data type
    {
    case (Setting::TypeBoolean):
    {
      std::vector<bool>* p_vector = static_cast<std::vector<bool>*> (pParameter);
      p_vector->clear();
      for(unsigned int i = 0; i < defaultValues.size(); i++)
      {
        int value;
        std::istringstream in_stream(defaultValues.at(i));
        if (!(in_stream >> value))
        {
          p_vector->push_back(false);
        } else
        {
          if (value == 1)
          {
            p_vector->push_back(true);
          } else
          {
            p_vector->push_back(false);
          }
        }
      }
      break;
    }
    case (Setting::TypeInt):
    {
      std::vector<int>* p_vector = static_cast<std::vector<int>*> (pParameter);
      p_vector->clear();
      for(unsigned int i = 0; i < defaultValues.size(); i++)
      {
        int temp;
        std::istringstream in_stream(defaultValues.at(i));
        if (!(in_stream >> temp))
        {
          temp = 0;
        }
        p_vector->push_back(temp);
      }
      break;
    }
    case (Setting::TypeFloat):
    {
      std::vector<double>* p_vector = static_cast<std::vector<double>*> (pParameter);
      p_vector->clear();
      for(unsigned int i = 0; i < defaultValues.size(); i++)
      {
        double temp;
        std::istringstream in_stream(defaultValues.at(i));
        if (!(in_stream >> temp))
        {
          temp = 0;
        }
        p_vector->push_back(temp);
      }
      break;
    }
    case (Setting::TypeString):
    {
      std::vector<std::string>* p_vector = static_cast<std::vector<std::string>*> (pParameter);
      p_vector->clear();
      for(unsigned int i = 0; i < defaultValues.size(); i++)
      {
        std::string temp = defaultValues.at(i);
        p_vector->push_back(temp);
      }
      break;
    }
    } // END switch array type
    break;
  }
  } // END switch first type
  return;
}

void ConfigurationInterface::setParameterToDefault(const ParameterInfo& info)
{
  this->setParameterToDefault(info.mpMember, info.mDefaultValues,
      info.mMembersType, info.mIsVectorOfType);
}

const ConfigurationInterface::ParameterInfoVector& ConfigurationInterface::getParameterList() const
{
  return this->mParameterInfos;
}

const std::string& ConfigurationInterface::getConfigFileName() const
{
  return this->mConfigFileName;
}

void ConfigurationInterface::adjustVectorSize(void* pVector, libconfig::Setting::Type type,
    Setting& element)
{
  int config_length = element.getLength();
  switch (type)
  {
  case (Setting::TypeBoolean):
  {
    std::vector<bool>* p_vector = static_cast<std::vector<bool>*> (pVector);
    int vector_length = p_vector->size();
    if (config_length != vector_length)
    {
      int difference = config_length - vector_length;
      if (difference < 0)
      {
        for (int j = vector_length + difference; j < vector_length; j++)
        {
          element.add(type);
        }
      } else if (difference > 0)
      {
        for (int j = config_length - 1; j >= vector_length; j--)
        {
          element.remove(j);
        }
      }
    }
    break;
  }
  case (Setting::TypeInt):
  {
    std::vector<int>* p_vector = static_cast<std::vector<int>*> (pVector);
    int vector_length = p_vector->size();
    if (config_length != vector_length)
    {
      int difference = config_length - vector_length;
      if (difference < 0)
      {
        for (int j = vector_length + difference; j < vector_length; j++)
        {
          element.add(type);
        }
      } else if (difference > 0)
      {
        for (int j = config_length - 1; j >= vector_length; j--)
        {
          element.remove(j);
        }
      }
    }
    break;
  }
  case (Setting::TypeFloat):
  {
    std::vector<double>* p_vector = static_cast<std::vector<double>*> (pVector);
    int vector_length = p_vector->size();
    if (config_length != vector_length)
    {
      int difference = config_length - vector_length;
      if (difference < 0)
      {
        for (int j = vector_length + difference; j < vector_length; j++)
        {
          element.add(type);
        }
      } else if (difference > 0)
      {
        for (int j = config_length - 1; j >= vector_length; j--)
        {
          element.remove(j);
        }
      }
    }
    break;
  }
  case (Setting::TypeString):
  {
    std::vector<std::string>* p_vector =
        static_cast<std::vector<std::string>*> (pVector);
    int vector_length = p_vector->size();
    if (config_length != vector_length)
    {
      int difference = config_length - vector_length;
      if (difference < 0)
      {
        for (int j = vector_length + difference; j < vector_length; j++)
        {
          element.add(type);
        }
      } else if (difference > 0)
      {
        for (int j = config_length - 1; j >= vector_length; j--)
        {
          element.remove(j);
        }
      }
    }
    break;
  }
  default:
  {
    std::cerr << "Unsupported data type in: " << (mConfigFileName.c_str())
        << endl;
  }
  }
  return;
}

void ConfigurationInterface::readOrDefaultConfiguration()
{
  // try to read in parameters - on fail, try to write all missing parameters
  int result = this->readConfiguration();
  if (result != ConfigurationInterface::CONFIG_SUCCESS)
  {
   std::cerr << "> error (" << mConfigFileName << "): Some parameters missing or corrupt,"
       " writing additional parameters to file." << std::endl;
   result = this->writeConfiguration();
   if(result != ConfigurationInterface::CONFIG_SUCCESS)
   {
     std::cerr << "> error (" << mConfigFileName << "): Could not write config file, "
         "please check file permissions." << std::endl;
   }
  }
}
