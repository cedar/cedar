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

    File:        GroupDeclaration.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2014 02 14

    Description: Header file for the class cedar::proc::GroupDeclaration.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GROUP_DECLARATION_H
#define CEDAR_PROC_GROUP_DECLARATION_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/PluginDeclaration.h"
#include "cedar/auxiliaries/Path.h"

// FORWARD DECLARATIONS
#include "cedar/processing/GroupDeclaration.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/enable_shared_from_this.hpp>
#endif // Q_MOC_RUN
#include <string>


/*!@brief Defines a template group, which can be found in a given file.
 */
class cedar::proc::GroupDeclaration : public cedar::aux::PluginDeclaration, public boost::enable_shared_from_this<cedar::proc::GroupDeclaration>
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  GroupDeclaration
  (
    const std::string& descriptiveName,
    const cedar::aux::Path& fileName,
    const std::string& groupName,
    const std::string& category
  );

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Declares this plugin with the appropriate manager.
   */
  void declare() const;

  //! Returns the class name associated with this declaration.
  std::string getClassName() const;

  //! Returns the file name associated with this declaration.
  std::string getFileName() const;

  //! Returns the group name associated with this declaration.
  std::string getGroupName() const;

  //! Returns a nonsense string :)
  std::string getPluginType() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! the filename where this group is defined
  cedar::aux::Path mFileName;
  //! the name of the group
  std::string mGroupName;

}; // class cedar::proc::GroupDeclaration

#endif // CEDAR_PROC_GROUP_DECLARATION_H

