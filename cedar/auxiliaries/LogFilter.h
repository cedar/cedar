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

    File:        LogFilter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 02 15

    Description: Header for the \em cedar::aux::LogFilter class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_LOG_FILTER_H
#define CEDAR_AUX_LOG_FILTER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/LogLevel.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/LogFilter.fwd.h"

// SYSTEM INCLUDES

/*!@brief A class for filtering log messages.
 */
class cedar::aux::LogFilter
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The constructor.
  LogFilter();

  //!@brief Destructor
  virtual ~LogFilter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Checks whether this filter accepts the given message.
   */
  virtual bool acceptsMessage
  (
    cedar::aux::LOG_LEVEL messageLevel,
    const std::string& message,
    const std::string& messageSource,
    const std::string& title
  ) const = 0;
  
  //!@brief set whether a filter removes messages after accepting them, not passing them to other subsequent filters
  void setRemovesMessages(bool remove)
  {
    this->mRemovesMessages = remove;
  }

  //!@brief get whether a filter removes messages after accepting them, not passing them to other subsequent filters
  bool removesMessages() const
  {
    return this->mRemovesMessages;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! If true, then any messages accepted by this filter will not be passed on to any further loggers.
  bool mRemovesMessages;
};

#endif // CEDAR_AUX_LOG_FILTER_H

