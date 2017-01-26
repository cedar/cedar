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

    File:        exceptions.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 17

    Description: Header file for exceptions in the cedar::proc namespace.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXCEPTIONS_H
#define CEDAR_PROC_EXCEPTIONS_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/ExceptionBase.h"

// FORWARD DECLARATIONS
#include "cedar/processing/exceptions.fwd.h"
#include "cedar/processing/Triggerable.fwd.h"

// SYSTEMS INCLUDES
#include <string>
#include <vector>
#include <set>

/*!@brief An exception that occurs when data is not found.
 */
class cedar::proc::DataNotFoundException : public cedar::aux::ExceptionBase
{
}; // class cedar::proc::DataNotFoundException

/*!@brief An exception that occurs when a deadlock may be caused by a user action.
 */
class cedar::proc::DeadlockException : public cedar::aux::ExceptionBase
{
}; // class cedar::proc::DeadlockException

/*!@brief An exception that occurs when an object is not found in a Registry.
 */
class cedar::proc::InvalidObjectException : public cedar::aux::ExceptionBase
{
}; // class cedar::proc::InvalidObjectException

/*!@brief An exception that occurs when a role is used as an index that is not known.
 */
class cedar::proc::InvalidRoleException : public cedar::aux::ExceptionBase
{
}; // class cedar::proc::InvalidRoleException

/*!@brief An exception that occurs when a category is used as an index that is not known.
 */
class cedar::proc::InvalidCategoryException : public cedar::aux::ExceptionBase
{
}; // class cedar::proc::InvalidCategoryException


/*!@brief An exception that occurs when a name is used as an index that is not known.
 */
class cedar::proc::InvalidArgumentsException : public cedar::aux::ExceptionBase
{
}; // class cedar::proc::InvalidArgumentsException


/*!@brief An exception that occurs when a name is used as an index that is not known.
 */
class cedar::proc::DuplicateNameException: public cedar::aux::ExceptionBase
{
}; // class cedar::proc::DuplicateNameException


/*!@brief An exception that is thrown, when a processing step would be executed while some mandatory connections are
 *        still missing.
 */
class cedar::proc::MissingConnectionException: public cedar::aux::ExceptionBase
{
}; // class cedar::proc::MissingConnectionException

/*!@brief An exception that is thrown, when a registry cannot find the declaration to which an object belongs
 */
class cedar::proc::MissingDeclarationException: public cedar::aux::ExceptionBase
{
}; // class cedar::proc::MissingDeclarationException

/*!@brief An exception that is thrown, when a plugin file cannot be found.
 */
class cedar::proc::PluginException: public cedar::aux::ExceptionBase
{
}; // class cedar::proc::PluginException

/*!@brief An exception that is thrown by the Manager, if a connection that is marked for deletion, cannot be found in
 * the list of connections.
 */
class cedar::proc::ConnectionNotFoundException : public cedar::aux::ExceptionBase
{
}; // class cedar::proc::ConnectionNotFoundException

/*!@brief An exception that is thrown by Connection, if a weak-pointer check fails. This means the connected item does
 * not longer exist.
 */
class cedar::proc::ConnectionMemberDeletedException : public cedar::aux::ExceptionBase
{
}; // class cedar::proc::ConnectionMemberDeletedException

/*!@brief An exception that is thrown if a connection is duplicated.
 */
class cedar::proc::DuplicateConnectionException : public cedar::aux::ExceptionBase
{
}; // class cedar::proc::DuplicateConnectionException

/*!@brief An exception that is thrown if the user requests a trigger connection that doesn't make sense.
 */
class cedar::proc::InvalidTriggerConnectionException : public cedar::aux::ExceptionBase
{
}; // class cedar::proc::InvalidTriggerConnectionException

/*!@brief An exception that is thrown by processing steps when no measurements for lock time etc. are present.
 */
class cedar::proc::NoMeasurementException : public cedar::aux::ExceptionBase
{
}; // class cedar::proc::NoMeasurementException

/*!@brief An exception that is thrown by ProjectionMapping when a user wants to look up a mapping that does not exist.
 */
class cedar::proc::NoMappingException : public cedar::aux::ExceptionBase
{
}; // class cedar::proc::NoMappingException

/*!@brief An exception that is thrown by cedar::proc::DataSlot when a type check is requested that is not set.
 */
class cedar::proc::NoCheckException : public cedar::aux::ExceptionBase
{
}; // class cedar::proc::NoCheckException

/*!@brief An exception that is a collection of exception messages that occurred during loading of an architecture.
 */
class cedar::proc::ArchitectureLoadingException : public cedar::aux::ExceptionBase
{
public:
  //! constructor
  ArchitectureLoadingException(const std::vector<std::string>& exception_infos)
  :
  mExceptionInfos(exception_infos)
  {
    std::string message = "One or more exceptions occurred while loading the architecture. They are:";
    for (size_t i = 0; i < this->mExceptionInfos.size(); ++i)
    {
      message += "\n";
      message += this->mExceptionInfos.at(i);
    }
    this->setMessage(message);
  }

  //! destructor that ensure that no exception is thrown
  ~ArchitectureLoadingException() throw ()
  {
  }

  //! get the messages of this exception
  const std::vector<std::string>& getMessages() const
  {
    return this->mExceptionInfos;
  }

private:
  std::vector<std::string> mExceptionInfos;
}; // class cedar::proc::ArchitectureLoadingException

//! Thrown when a cycle is detected between the given triggers.
class cedar::proc::TriggerCycleException : public cedar::aux::ExceptionBase
{
  public:
    //! Constructor. Takes a vector of sets where each set contains the nodes of one trigger cycle.
    TriggerCycleException(const std::vector<std::set<cedar::proc::TriggerablePtr> >& cycles);
}; // cedar::proc::TriggerCycleException

/*!@brief An exception that is thrown if a triggerable has not the right loop state (isLooped()).
 */
class cedar::proc::LoopStateException : public cedar::aux::ExceptionBase
{
}; // class cedar::proc::NoCheckException

#endif // CEDAR_PROC_EXCEPTIONS_H
