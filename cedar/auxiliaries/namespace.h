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

    File:        namespace.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 10 12

    Description: Namespace file for cedar::aux.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_AUX_NAMESPACE_H
#define CEDAR_AUX_NAMESPACE_H

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "namespace.h"

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

    class LoopedThread;
    typedef boost::shared_ptr<LoopedThread> LoopedThreadPtr;

    class ConfigurationInterface;
    typedef boost::shared_ptr<ConfigurationInterface> ConfigurationInterfacePtr;

    class UserData;
    typedef boost::shared_ptr<UserData> UserDataPtr;

    template <typename T>
    class IntervalData;
//    typedef boost::shared_ptr<IntervalData> IntervalDataPtr;

    class LogFile;
    typedef boost::shared_ptr<LogFile> LogFilePtr;

    class Object;
    typedef boost::shared_ptr<Object> ObjectPtr;

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
