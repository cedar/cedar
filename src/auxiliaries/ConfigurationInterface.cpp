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
#include "src/auxiliaries/math/Limits.h"

// SYSTEM INCLUDES
#include <libconfig.h++>
#include <sys/types.h> // for checking if directory exists
#include <sys/stat.h> // for checking if directory exists
//#include <process.h> // for portable mkdir
#include <stdio.h>  // FILENAME_MAX needed for char array
#ifdef WINDOWS
    #include <direct.h>
    #define getCurrentDir _getcwd
#else
    #include <unistd.h> // for getcwd
    #define getCurrentDir getcwd
 #endif

using namespace std;
using namespace libconfig;
using namespace cedar::aux;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

// Constructors
ConfigurationInterface::ConfigurationInterface()
:
mConfigFileName("newConfiguration")
{
  mParameterInfos.clear();
  readConfigurationFile();
}

ConfigurationInterface::ConfigurationInterface(const std::string& configFileName)
:
mConfigFileName(configFileName)
{
  mParameterInfos.clear();
  readConfigurationFile();
}

ConfigurationInterface::ConfigurationInterface(const char* pConfigFileName)
:
mConfigFileName(std::string(pConfigFileName))
{
  mParameterInfos.clear();
  readConfigurationFile();
}

// Destructor
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
:
mpUserData(NULL)
{
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
                                          const std::string& name,
                                          const bool& defaultValue,
                                          UserData *pUserData
                                        )
{
  mParameterInfos.push_back(ParameterInfo());
  ParameterInfo& parameter_info = mParameterInfos.back();
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = ConfigurationInterface::toString(defaultValue);
  parameter_info.mMembersType = Setting::TypeBoolean;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeNone;
  parameter_info.mpUserData = pUserData;
  return CONFIG_SUCCESS;
}

int ConfigurationInterface::addParameter(
                                          int* pMember,
                                          const std::string& name,
                                          const int& defaultValue,
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
                                          const std::string& name,
                                          const double& defaultValue,
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
                                          const std::string& name,
                                          const std::string& defaultValue,
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

int ConfigurationInterface::addParameter(
                                          vector<bool>* pMember,
                                          const std::string& name,
                                          const bool& defaultValue
                                        )
{
  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = ConfigurationInterface::toString(defaultValue);
  parameter_info.mMembersType = Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeBoolean;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int ConfigurationInterface::addParameter(
                                          vector<bool>* pMember,
                                          const std::string& name,
                                          const std::vector<bool>& defaultValues
                                        )
{
  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.clear();
  for(unsigned int i = 0; i < defaultValues.size(); i++)
  {
    parameter_info.mDefaultValues.push_back(ConfigurationInterface::toString(defaultValues.at(i)));
  }
  parameter_info.mMembersType = Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeBoolean;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int ConfigurationInterface::addParameter(
                                          vector<int>* pMember,
                                          const std::string& name,
                                          const int& defaultValue
                                        )
{
  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = ConfigurationInterface::toString(defaultValue);
  parameter_info.mMembersType = Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeInt;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int ConfigurationInterface::addParameter(
                                          vector<int>* pMember,
                                          const std::string& name,
                                          const std::vector<int>& defaultValues
                                        )
{
  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.clear();
  for(unsigned int i = 0; i < defaultValues.size(); i++)
  {
    parameter_info.mDefaultValues.push_back(ConfigurationInterface::toString(defaultValues.at(i)));
  }
  parameter_info.mMembersType = Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeInt;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int ConfigurationInterface::addParameter(
                                          vector<double>* pMember,
                                          const std::string& name,
                                          const double& defaultValue
                                        )
{
  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.resize(1);
  parameter_info.mDefaultValues.at(0) = ConfigurationInterface::toString(defaultValue);
  parameter_info.mMembersType = Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeFloat;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int ConfigurationInterface::addParameter(
                                          vector<double>* pMember,
                                          const std::string& name,
                                          const std::vector<double>& defaultValues
                                        )
{
  ParameterInfo parameter_info;
  parameter_info.mpMember = static_cast<void*> (pMember);
  parameter_info.mDefaultValues.clear();
  for(unsigned int i = 0; i < defaultValues.size(); i++)
  {
    parameter_info.mDefaultValues.push_back(ConfigurationInterface::toString(defaultValues.at(i)));
  }
  parameter_info.mMembersType = Setting::TypeArray;
  parameter_info.mName = name;
  parameter_info.mIsVectorOfType = Setting::TypeFloat;
  mParameterInfos.push_back(parameter_info);
  return CONFIG_SUCCESS;
}

int ConfigurationInterface::addParameter(
                                          vector<std::string>* pMember,
                                          const std::string& name,
                                          const std::string& defaultValue
                                        )
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

int ConfigurationInterface::addParameter(
                                          vector<std::string>* pMember,
                                          const std::string& name,
                                          const std::vector<std::string>& defaultValues
                                        )
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

int ConfigurationInterface::readConfigurationFile()
{
  // Read the file. If there is an error, report it and exit.
  try
  {
    mConfig.readFile(mConfigFileName.c_str());
  } catch (const FileIOException &fioex) // general IO error, file not readable
  {
    std::cerr << "> error in " << mConfigFileName << ": I/O error while reading file." << std::endl;
    for (ParameterInfoVector::iterator iter = this->mParameterInfos.begin();
         iter != this->mParameterInfos.end();
         ++iter
        )
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
    for (ParameterInfoVector::iterator iter = this->mParameterInfos.begin();
         iter != this->mParameterInfos.end();
         ++iter
        )
    {
      setParameterToDefault(*iter);
    }
    Setting &root = mConfig.getRoot();
    for (int i = 0; i < root.getLength(); i++)
    {
      std::string parameter_name = this->mParameterInfos.at(i).mName;
      if (root.exists(parameter_name) && root[parameter_name].getType() == Setting::TypeArray)
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

  return CONFIG_SUCCESS;
}

int ConfigurationInterface::readConfiguration()
{
  int info = CONFIG_SUCCESS;

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
        readArray<int>(*iter);
        break;
      } // END array case int
      case (Setting::TypeFloat):
      {
        readArray<double>(*iter);
        break;
      } // END array case float
      case (Setting::TypeBoolean):
      {
        readArray<bool>(*iter);
        break;
      } // END array case bool
      case (Setting::TypeString):
      {
        readArray<std::string>(*iter);
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

int ConfigurationInterface::handleTypeException(const ConfigurationInterface::ParameterInfo& info)
{
  cerr << "> error in " << mConfigFileName << ": Wrong type for '"
      << info.mName << "' setting in configuration file." << endl;
  Setting &root = mConfig.getRoot();
  root.remove(info.mName);
  createNewGroupEntry(info.mName, info.mMembersType, root);
  setParameterToDefault(info);
  return CONFIG_TYPE_ERROR;
}

int ConfigurationInterface::handleSettingNotFoundException(const ConfigurationInterface::ParameterInfo& info)
{
  cerr << "> error in " << mConfigFileName << ": No '" << info.mName << "' setting in configuration file." << endl;
  setParameterToDefault(info);
  return CONFIG_MISSING_ITEM_ERROR;
}

int ConfigurationInterface::writeConfiguration()
{
  // export current values of the parameters into the config object
  Setting &root = mConfig.getRoot();
  for (ParameterInfoVector::iterator iter = this->mParameterInfos.begin(); iter != this->mParameterInfos.end(); ++iter)
  {
    switch (iter->mMembersType)
    // switch for first type - either basic data type or array
    {
    case (Setting::TypeFloat):
    {
      try
      {
        mConfig.lookup(iter->mName) = *(static_cast<double*> (iter->mpMember));
      } catch (const SettingNotFoundException &nfex)
      {
        createNewGroupEntry(iter->mName, iter->mMembersType, root);
        mConfig.lookup(iter->mName) = *(static_cast<double*> (iter->mpMember));
      }
      break;
    } // END case TypeFloat
    case (Setting::TypeInt):
    {
      try
      {
        mConfig.lookup(iter->mName) = *(static_cast<int*> (iter->mpMember));
      } catch (const SettingNotFoundException &nfex)
      {
        createNewGroupEntry(iter->mName, iter->mMembersType, root);
        mConfig.lookup(iter->mName) = *(static_cast<int*> (iter->mpMember));
      }
      break;
    } // END case TypeInt
    case (Setting::TypeBoolean):
    {
      try
      {
        mConfig.lookup(iter->mName) = *(static_cast<bool*> (iter->mpMember));
      } catch (const SettingNotFoundException &nfex)
      {
        createNewGroupEntry(iter->mName, iter->mMembersType, root);
        mConfig.lookup(iter->mName) = *(static_cast<bool*> (iter->mpMember));
      }
      break;
    } // END case TypeBoolean
    case (Setting::TypeString):
    {
      try
      {
        mConfig.lookup(iter->mName) = *(static_cast<string*> (iter->mpMember));
      } catch (const SettingNotFoundException &nfex)
      {
        createNewGroupEntry(iter->mName, iter->mMembersType, root);
        mConfig.lookup(iter->mName) = *(static_cast<string*> (iter->mpMember));
      }
      break;
    } // END case TypeString
    case (Setting::TypeArray):
    {
      switch (iter->mIsVectorOfType)
      // switch for second type - only available in arrays
      {
        case (Setting::TypeBoolean):
        {
          writeArray<bool>(iter->mpMember, iter->mName, iter->mIsVectorOfType);
          break;
        } // END case array bool
      case (Setting::TypeInt):
      {
        writeArray<int>(iter->mpMember, iter->mName, iter->mIsVectorOfType);
        break;
      }
      case (Setting::TypeFloat):
      {
        writeArray<double>(iter->mpMember, iter->mName, iter->mIsVectorOfType);
        break;
      }
      case (Setting::TypeString):
      {
        writeArray<std::string>(iter->mpMember, iter->mName, iter->mIsVectorOfType);
        break;
      }
      default:
      {
        std::cerr << "Unsupported data type in: " << (mConfigFileName.c_str()) << endl;
        break;
      }
      } // END switch array types
      break;
    } // END case TypeArray
    default:
    {
      std::cerr << "Unsupported data type in: " << (mConfigFileName.c_str()) << endl;
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
    // check if path exists - if not, try to fix it
    try
    {
      // first check if the path exists
      char currentPath[FILENAME_MAX];
      std::string fullPath;
      if (!getCurrentDir(currentPath, sizeof(currentPath)))
      {
        cout << "> error (ConfigurationInterface) - cannot determine current path" << std::endl;
      }
      struct stat st;
      if(stat(mConfigFileName.c_str(),&st) != 0) // path does not exist
      {
        // create path
        createConfigPath(mConfigFileName);
        // write file (second try)
        mConfig.writeFile(mConfigFileName.c_str());
        return CONFIG_SUCCESS;
      }
      // else: path already exists, move on to next error (file permissions)
    } catch (const FileIOException &directoryException)
    {
      // we tried to create a directory to store the file, but it failed
      cerr << "I/O error while writing file: " << (mConfigFileName.c_str())
          << ", please check file permissions" << endl;
      return CONFIG_FILE_ERROR;
    }
    cerr << "I/O error while writing file: " << (mConfigFileName.c_str()) << ", please check file permissions" << endl;
    return CONFIG_FILE_ERROR;
  }
  return CONFIG_SUCCESS;
}

void ConfigurationInterface::setParameterToDefault(
                                                    void* pParameter,
                                                    std::vector<std::string> defaultValues,
                                                    int type,
                                                    int arrayType
                                                  )
{
  switch (type)
  // switch for first type (either basic data type or array)
  {
  case (Setting::TypeBoolean):
  {
    *(static_cast<bool*> (pParameter)) = ConfigurationInterface::to<bool>(defaultValues.at(0));
    break;
  }
  case (Setting::TypeInt):
  {
    *(static_cast<int*> (pParameter)) = ConfigurationInterface::to<int>(defaultValues.at(0));
    break;
  }
  case (Setting::TypeFloat):
  {
      *(static_cast<double*> (pParameter)) = ConfigurationInterface::to<double>(defaultValues.at(0));
    break;
  }
  case (Setting::TypeString):
  {
    *(static_cast<std::string*> (pParameter)) = defaultValues.at(0);
    break;
  }
  case (Setting::TypeArray):
  {
    // second level switch - array can be of each basic data type
    switch (arrayType)
    {
    case (Setting::TypeBoolean):
    {
      std::vector<bool>* p_vector = static_cast<std::vector<bool>*> (pParameter);
      p_vector->clear();
      for(unsigned int i = 0; i < defaultValues.size(); i++)
      {
        p_vector->push_back(ConfigurationInterface::to<bool>(defaultValues.at(i)));
      }
      break;
    }
    case (Setting::TypeInt):
    {
      std::vector<int>* p_vector = static_cast<std::vector<int>*> (pParameter);
      p_vector->clear();
      for(unsigned int i = 0; i < defaultValues.size(); i++)
      {
        p_vector->push_back(ConfigurationInterface::to<int>(defaultValues.at(i)));
      }
      break;
    }
    case (Setting::TypeFloat):
    {
      std::vector<double>* p_vector = static_cast<std::vector<double>*> (pParameter);
      p_vector->clear();
      for(unsigned int i = 0; i < defaultValues.size(); i++)
      {
        p_vector->push_back(ConfigurationInterface::to<double>(defaultValues.at(i)));
      }
      break;
    }
    case (Setting::TypeString):
    {
      std::vector<std::string>* p_vector = static_cast<std::vector<std::string>*> (pParameter);
      p_vector->clear();
      for(unsigned int i = 0; i < defaultValues.size(); i++)
      {
        p_vector->push_back(defaultValues.at(i));
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
  this->setParameterToDefault(info.mpMember, info.mDefaultValues, info.mMembersType, info.mIsVectorOfType);
}

const ConfigurationInterface::ParameterInfoVector& ConfigurationInterface::getParameterList() const
{
  return this->mParameterInfos;
}

const std::string& ConfigurationInterface::getConfigFileName() const
{
  return this->mConfigFileName;
}

void ConfigurationInterface::adjustVectorSize(void* pVector, const libconfig::Setting::Type type, Setting& element)
{
  switch (type)
  {
  case (Setting::TypeBoolean):
  {
    adjustVector<bool>(pVector,type,element);
    break;
  }
  case (Setting::TypeInt):
  {
    adjustVector<int>(pVector,type,element);
    break;
  }
  case (Setting::TypeFloat):
  {
    adjustVector<double>(pVector,type,element);
    break;
  }
  case (Setting::TypeString):
  {
    adjustVector<std::string>(pVector,type,element);
    break;
  }
  default:
  {
    std::cerr << "Unsupported data type in: " << (mConfigFileName.c_str()) << endl;
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

/*!Recursively creates a new directory for a new configuration file.
 * First, the given path is cut off after the last slash. If the function
 * reached root, nothing is done. If not, createConfigPath checks if the current path
 * already exists. If not, it is called again (recursively) with
 * the cut-off path. When this call returns, the current path is created with
 * system("mkdir " + cut_off_pathname).
 */
void ConfigurationInterface::createConfigPath(std::string path)
{
  struct stat st;
  unsigned int last_slash = path.rfind("/");
  // check if we have reached root
  if (last_slash == 0)
    return;
  // check if this is the last level in the folder tree (i.e. no slash included)
  if (last_slash == (unsigned int)string::npos)
  {
    int result = system(path.c_str());
    //@todo handle errors with mkdir
    (void)result;
    return;
  }
  // else, chop off another level and create directory
  std::string cut_off = path.substr(0,last_slash);
  if(stat(cut_off.c_str(),&st) != 0) // path does not exist
  {
    // recursively check, if all preceding directories exist
    createConfigPath(cut_off);
    std:: string command = "mkdir " + cut_off;
    int result = system(command.c_str());
    //@todo handle errors with mkdir
    (void)result;
  }
  return;
}

//!Recursively add groups to the setting tree until we reach a leaf. The leaf is created with
void ConfigurationInterface::createNewGroupEntry(
                                                  const std::string& name,
                                                  const libconfig::Setting::Type type,
                                                  libconfig::Setting& tree
                                                )
{
  unsigned int first_dot = name.find(".");
  // check if this is the leaf
  if (first_dot == (unsigned int)string::npos)
  {
    // create an entry
    tree.add(name,type);
    return;
  }
  // else, chop off another level and create group
  std::string cut_off = name.substr(0,first_dot);
  std::string rest = name.substr(first_dot+1, name.length() - first_dot - 1);
  if (tree.exists(cut_off))
  {
    createNewGroupEntry(rest, type, tree[cut_off]);
  }
  else
  {
    Setting& new_group = tree.add(cut_off, Setting::TypeGroup);
    createNewGroupEntry(rest, type, new_group);
  }
  return;
}

template<typename T>
void ConfigurationInterface::writeArray(
                                         void* pVector,
                                         const std::string& name,
                                         const libconfig::Setting::Type vectorType
                                       )
{
  // cast to right type
  std::vector<T>* p_vector = static_cast<std::vector<T>*>(pVector);
  // first, try to write to configuration tree
  try
  {
    adjustVectorSize(pVector, vectorType, mConfig.lookup(name));
    for (unsigned int j = 0; j < p_vector->size(); j++)
    {
      mConfig.lookup(name)[j] = p_vector->at(j);
    }
  }

  catch (const libconfig::SettingNotFoundException &nfex)
  {
    createNewGroupEntry(name, libconfig::Setting::TypeArray, mConfig.getRoot());
    libconfig::Setting &array = mConfig.lookup(name);
    for (unsigned int j = 0; j < p_vector->size(); j++)
    {
      array.add(vectorType);
    }
    adjustVectorSize(pVector, vectorType, mConfig.lookup(name));
    for (unsigned int j = 0; j < p_vector->size(); j++)
    {
      mConfig.lookup(name)[j] = p_vector->at(j);
    }
  }
}

template<typename T>
void ConfigurationInterface::adjustVector(
                                           void* pVector,
                                           const libconfig::Setting::Type& type,
                                           libconfig::Setting& element
                                         )
{
  int config_length = element.getLength();
  std::vector<T>* p_vector = static_cast<std::vector<T>*> (pVector);
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
}

template<typename T>
int ConfigurationInterface::readArray(ParameterInfo& info)
{
  int result = CONFIG_SUCCESS;
  try
  {
    if (mConfig.lookup(info.mName).getType() != Setting::TypeArray)
    {
      throw 20;
    }
    std::vector<T>* p_vector = static_cast<std::vector<T>*> (info.mpMember);
    p_vector->clear();
    for (int j = 0; j < mConfig.lookup(info.mName).getLength(); j++)
    {
      Setting &setting = mConfig.lookup(info.mName)[j];
      if (setting.getType() != info.mIsVectorOfType)
      {
        throw 20;
      }
      p_vector->push_back(mConfig.lookup(info.mName)[j]);
    }
  } catch (const SettingNotFoundException &nfex)
  {
    result = handleSettingNotFoundException(info);
  } catch (const SettingTypeException &nfex)
  {
    cerr << "> error in " << mConfigFileName << ": Wrong type for '"
        << info.mName << "' setting in configuration file." << endl;
    std::vector<T>* p_vector = static_cast<std::vector<T>*> (info.mpMember);
    removeItem(info.mName);
    createNewGroupEntry(info.mName, Setting::TypeArray, mConfig.getRoot());
    Setting &array = mConfig.lookup(info.mName);
    for (unsigned int j = 0; j < p_vector->size(); j++)
    {
      array.add(info.mIsVectorOfType);
    }
    setParameterToDefault(info);
    result = CONFIG_TYPE_ERROR;
  } catch (int e)
  {
    cerr << "> error in " << mConfigFileName << ": array '" << info.mName << "' is corrupt" << std::endl;
    removeItem(info.mName);
    createNewGroupEntry(info.mName, Setting::TypeArray, mConfig.getRoot());
    Setting &array = mConfig.lookup(info.mName);
    array.add(info.mIsVectorOfType);
    setParameterToDefault(info);
    result = CONFIG_TYPE_ERROR;
  }
  return result;
}

void ConfigurationInterface::removeItem(const std::string& name)
{
  // find a dot
  unsigned int last_dot = name.rfind(".");
  // check if there is a group path - if not, delete at root
  if (last_dot == (unsigned int)string::npos)
  {
    mConfig.getRoot().remove(name);
  }
  // else, extract group and leaf name, delete leaf without deleting the group
  else
  {
    std::string group_name = name.substr(0,last_dot);
    std::string leaf_name = name.substr(last_dot+1, name.length() - last_dot - 1);
    Setting& group = mConfig.lookup(group_name);
    group.remove(leaf_name);
  }
}
