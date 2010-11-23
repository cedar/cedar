/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        Namespace.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 10 12

 ----- Description: Namespace file for cedar::aux.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_NAMESPACE_H
#define CEDAR_AUX_NAMESPACE_H

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "src/namespace.h"

// SYSTEM INCLUDES
#include <sstream>
#include <boost/smart_ptr.hpp>


namespace cedar
{
  /*@brief Namespace for all aux classes. */
  namespace aux
  {
    class Base;
    typedef boost::shared_ptr<Base> BasePtr;

    class Thread;
    typedef boost::shared_ptr<Thread> ThreadPtr;

    class ConfigurationInterface;
    typedef boost::shared_ptr<ConfigurationInterface> ConfigurationInterfacePtr;

    class UserData;
    typedef boost::shared_ptr<UserData> UserDataPtr;

    class LogFile;
    typedef boost::shared_ptr<LogFile> LogFilePtr;

    /*!@brief Template method that converts simple data types to a string.
     *
     * @param[in] value The data value that will be converted to a string.
     */
    template<typename T>
    std::string toString(const T &value)
    {
      std::ostringstream streamOut;
      streamOut << value;
      return streamOut.str();
    }
  }
}

#endif // CEDAR_AUX_NAMESPACE_H
