/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        ConfigurationInterface.h

 ----- Author:      Stephan Zibner
 ----- Email:       stephan.zibner@rub.de
 ----- Date:        2010 01 07

 ----- Description: Interface for classes with configuration parameters.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_CONFIGURATION_INTERFACE_H
#define CEDAR_AUX_CONFIGURATION_INTERFACE_H

// LOCAL INCLUDES
#include "Namespace.h"
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
 *
 * More detailed description of the class.
 */
class cedar::aux::ConfigurationInterface : public cedar::aux::Base
{
public:
  //!@brief Struct that holds all information about a parameter.
  struct ParameterInfo
  {
    ParameterInfo();
    ~ParameterInfo();

    void* mpMember;
    std::vector<std::string> mDefaultValues;
    libconfig::Setting::Type mMembersType;
    std::string mName;
    libconfig::Setting::Type mIsVectorOfType;

    /*!
     * Contains user defined data. To use this, inherit from cedar::aux::UserDataBase and add
     * your user-data in the derived class.
     *
     * @remarks The user data will be deleted automatically!
     */
    UserData *mpUserData;
  };

  /*!
   * The type used to store the parameters.
   *
   * @remark When iterating, use this to get ParameterInfoVector::iterator.
   */
  typedef std::vector<ParameterInfo> ParameterInfoVector;

  /*!@brief Enum that defines the state of a read or write process.
   *
   */
  enum CONFIG_READ_WRITE_STATE
  {
    CONFIG_SUCCESS,
    CONFIG_FILE_ERROR,
    CONFIG_TYPE_ERROR,
    CONFIG_MISSING_ITEM_ERROR
  };


  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ConfigurationInterface(void);

  //!@brief Constructor that gets a configuration file name.
  ConfigurationInterface(const std::string& configFileName);

  //!@brief Constructor that gets a configuration file name.
  ConfigurationInterface(const char* pConfigFileName);

  //!@brief Destructor
  virtual ~ConfigurationInterface(void);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Adds a parameter to the parameter list.
   *
   * @param pMember pointer to member variable
   * @param name name of variable in the scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue default value for the parameter
   */
  virtual int addParameter(
                    bool* pMember,
                    std::string name,
                    bool defaultValue,
                    UserData *pUserData = NULL
                  );

  /*!@brief Adds a parameter to the parameter list.
   *
   * @param pMember pointer to member variable
   * @param name name of variable in the scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue default value for the parameter
   */
  virtual int addParameter(
                    int* pMember,
                    std::string name,
                    int defaultValue,
                    UserData *pUserData = NULL
                  );

  /*!@brief Adds a parameter to the parameter list.
   *
   * @param pMember pointer to member variable
   * @param name name of variable in the scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue default value for the parameter
   */
  virtual int addParameter(
                    double* pMember,
                    std::string name,
                    double defaultValue,
                    UserData *pUserData = NULL
                  );

  /*!@brief Adds a parameter to the parameter list.
   *
   * @param pMember pointer to member variable
   * @param name name of variable in the scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue default value for the parameter
   */
  virtual int addParameter(
                    std::string* pMember,
                    std::string name,
                    std::string defaultValue,
                    UserData* pUserData = NULL
                  );

  /*!@brief Adds a parameter to the parameter list. Default value for a vector can only be a single element...
   *
   * @param pMember pointer to member variable
   * @param name name of variable in th scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue default value for the parameter
   */
  virtual int addParameter(std::vector<bool>* pMember, std::string name, bool defaultValue);

  /*!@brief Adds a parameter to the parameter list. Default value for a vector can only be a single element...
   *
   * @param pMember pointer to member variable
   * @param name name of variable in th scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue default value for the parameter
   */
  virtual int addParameter(std::vector<int>* pMember, std::string name, int defaultValue);

  /*!@brief Adds a parameter to the parameter list. Default value for a vector can only be a single element...
   *
   * @param pMember pointer to member variable
   * @param name name of variable in th scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue default value for the parameter
   */
  virtual int addParameter(std::vector<double>* pMember, std::string name, double defaultValue);

  /*!@brief Adds a parameter to the parameter list. Default value for a vector can only be a single element...
   *
   * @param pMember pointer to member variable
   * @param name name of variable in th scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue default value for the parameter
   */
  virtual int addParameter(std::vector<std::string>* pMember, std::string name, std::string defaultValue);

  /*!@brief Adds a parameter to the parameter list. Default value for a vector can only be a single element...
   *
   * @param pMember pointer to member variable
   * @param name name of variable in th scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue vector of default values for the parameter
   */
  virtual int addParameter(std::vector<bool>* pMember, std::string name, std::vector<bool> defaultValues);

  /*!@brief Adds a parameter to the parameter list. Default value for a vector can only be a single element...
   *
   * @param pMember pointer to member variable
   * @param name name of variable in th scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue vector of default values for the parameter
   */
  virtual int addParameter(std::vector<int>* pMember, std::string name, std::vector<int> defaultValues);

  /*!@brief Adds a parameter to the parameter list. Default value for a vector can only be a single element...
   *
   * @param pMember pointer to member variable
   * @param name name of variable in th scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue vector of default value for the parameter
   */
  virtual int addParameter(std::vector<double>* pMember, std::string name, std::vector<double> defaultValues);

  /*!@brief Adds a parameter to the parameter list. Default value for a vector can only be a single element...
   *
   * @param pMember pointer to member variable
   * @param name name of variable in th scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue vector of default values for the parameter
   */
  virtual int addParameter(std::vector<std::string>* pMember, std::string name, std::vector<std::string> defaultValues);

  /*!@brief Reads in the given set of parameters.
   *
   * @return CONFIG_SUCCESS if successful, an error of the same enum otherwise
   */
  int readConfiguration();

  /*!@brief Reads in the given set of parameters.
   *
   * @return CONFIG_SUCCESS if successful, an error of the same enum otherwise
   */
  int writeConfiguration();


  template <typename T>
  static std::string toString(const T& value)
  {
    std::stringstream input_stream;
    input_stream << std::setprecision(16) << value;
    return input_stream.str();
  }

  template<typename T>
  static T to(const std::string& value)
  {
    T out;
    std::istringstream in_stream(value);
    if(! (in_stream >> out))
    {
      out = 0;
    }
    return out;
  }

  /*!@brief Returns an iteratable reference to the internal storage.
   *
   */
  const ParameterInfoVector& getParameterList() const;

  const std::string& getConfigFileName() const;

  /*!
   * @brief Tries to read the configuration file.
   *
   * If the configuration file cannot be read the parameter values are set to their defaults.
   */
  void readOrDefaultConfiguration();

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
   * @param pParameter void pointer to data
   * @param defaultValue string containing the default value
   * @param type first type info - either basic data type or array from libconfig::Setting::Type enum
   * @param arrayType second type info - one of the basic data types from libconfig::Setting::Type enum
   */
  void setParameterToDefault(void* pParameter, std::vector<std::string> defaultValues, int type, int arrayType);

  void setParameterToDefault(const cedar::aux::ConfigurationInterface::ParameterInfo& info);

  /*!@brief Adjusts the size of the config file array, if a vector's size changed during run time.
   *
   * @param pVector pointer to a (generic) vector
   * @param type type of vector, taken from libconfig::Setting::Type enum
   * @param element reference to config file array element (read off size and adjust size)
   */
  void adjustVectorSize(void* pVector, libconfig::Setting::Type type, libconfig::Setting& element);

  int handleTypeException( const ParameterInfo& info );

  int handleSettingNotFoundException( const ParameterInfo& info );

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  libconfig::Config mConfig;
  std::string mConfigFileName;
  ParameterInfoVector mParameterInfos;
  int mConfigReadWriteState;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet
protected:
  // none yet
private:
  // none yet

}; // class cedar::dev::robot::Component

#endif // CEDAR_AUX_CONFIGURATION_INTERFACE_H

