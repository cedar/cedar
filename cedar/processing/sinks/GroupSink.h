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

    File:        GroupSink.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2013 11 08

    Description: Header file for the class cedar::proc::sinks::GroupSink.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_SINKS_GROUP_SINK_H
#define CEDAR_PROC_SINKS_GROUP_SINK_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"

// FORWARD DECLARATIONS
#include "cedar/processing/sinks/GroupSink.fwd.h"

// SYSTEM INCLUDES


/*!@brief a sink for data in a group, passed on to the outside
 *
 * @remark this is not meant to be used manually - each group takes care of this when adding/removing connectors
 */
class cedar::proc::sinks::GroupSink : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  GroupSink();

  //!@brief Destructor
  virtual ~GroupSink();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief returns empty data
  cedar::aux::DataPtr getEmptyData();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  void compute(const cedar::proc::Arguments& arguments);
  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);

  void validateName(const std::string& newName) const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::aux::DataPtr mEmptyData;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::sinks::GroupSink

#endif // CEDAR_PROC_SINKS_GROUP_SINK_H

