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

    File:        IterateDirectoryContents.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 11 17

    Description: Header file for the class cedar::proc::experiment::action::IterateDirectoryContents.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_ACTION_ITERATE_DIRECTORY_CONTENTS_H
#define CEDAR_PROC_EXPERIMENT_ACTION_ITERATE_DIRECTORY_CONTENTS_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/action/SetParameterValue.h"
#include "cedar/processing/experiment/StepPropertyParameter.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/DirectoryParameter.h"
#include "cedar/auxiliaries/FileParameter.h"
#include "cedar/auxiliaries/LockableMember.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/action/IterateDirectoryContents.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#include <vector>


/*!@brief An action that goes through a list of directory entries and applies it to a parameter on successive trials.
 */
class cedar::proc::experiment::action::IterateDirectoryContents
:
QObject,
public cedar::proc::experiment::action::SetParameterValue
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  IterateDirectoryContents();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void preExperiment();

  void run();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private slots:
  void updateDirectoryContents();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::aux::LockableMember<std::vector<cedar::aux::Path>> mDirectoryContents;

  cedar::aux::FileParameterPtr mChangedParameterAsFileParameter;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::aux::StringParameterPtr _mFilter;

  cedar::aux::DirectoryParameterPtr _mDirectory;

}; // class cedar::proc::experiment::action::IterateDirectoryContents

#endif // CEDAR_PROC_EXPERIMENT_ACTION_ITERATE_DIRECTORY_CONTENTS_H

