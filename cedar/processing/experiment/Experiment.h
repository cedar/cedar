/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Experiment.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2014 01 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_proc_EXPERIMENT_EXPERIMENT_H
#define CEDAR_proc_EXPERIMENT_EXPERIMENT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/processing/Network.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/Experiment.fwd.h"

// SYSTEM INCLUDES


/*!@brief
 */
class cedar::proc::experiment::Experiment : public cedar::aux::NamedConfigurable
{
private:

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  Experiment(cedar::proc::NetworkPtr network);
  //!@brief Destructor
  ~Experiment();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  const std::string& getFileName() const;
  void setFileName(const std::string& filename);

  unsigned int getRepetitions() const;
  void setRepetitions(unsigned int repetitions);
  void run();
  void cancel();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void startNetwork();
  void stopNetwork();
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::aux::StringParameterPtr _mFileName;
  cedar::aux::UIntParameterPtr _mRepetitions;
  cedar::proc::NetworkPtr mNetwork;

  //! Used for starting all triggers in a separate thread
  cedar::aux::CallFunctionInThreadPtr mStartThreadsCaller;

  //! Used for stopping all triggers in a separate thread
  cedar::aux::CallFunctionInThreadPtr mStopThreadsCaller;

}; // class cedar::proc::experiment::Experiment

#endif // CEDAR_proc_EXPERIMENT_EXPERIMENT_H

