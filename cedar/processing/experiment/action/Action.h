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

    File:        Action.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2014 01 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_ACTION_H
#define CEDAR_PROC_EXPERIMENT_ACTION_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.h"
#include "cedar/auxiliaries/ObjectParameterTemplate.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/action/Action.fwd.h"
#include "cedar/processing/experiment/Experiment.fwd.h"

// SYSTEM INCLUDES
#include <vector>


/*!@brief An abstract class for all the derived Action classes.
 *    The derived class has to implement the run() function.
 */
class cedar::proc::experiment::action::Action : public cedar::aux::Configurable
{
public:

	//!@brief a parameter for a list of action objects
	typedef cedar::aux::ObjectListParameterTemplate<cedar::proc::experiment::action::Action> ActionListParameter;
	//!@brief a parameter for action objects
	typedef cedar::aux::ObjectParameterTemplate<cedar::proc::experiment::action::Action> ActionParameter;

	//!@cond SKIPPED_DOCUMENTATION
	CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(ActionListParameter);
	CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(ActionParameter);
	//!@endcond

private:

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Action();

  Action(cedar::proc::experiment::ExperimentPtr experiment);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief This function should implement the things that this Action should do
  virtual void run() = 0;

  //! Called before an experiment is started.
  virtual void preExperiment();

  //! Called after an experiment is stopped.
  virtual void postExperiment();

  void setExperiment(cedar::proc::experiment::ExperimentPtr experiment);

  /*! Reimplement this to check the validity of the action. Return true if the action is valid. If false is returned, an
   *  inforative message should be added to the @em errors vector.
   */
  virtual bool checkValidity(std::vector<std::string>& errors, std::vector<std::string>& warnings) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  cedar::proc::experiment::ExperimentWeakPtr _mExperiment;
private:


}; // class cedar::proc::experiment::Action

#include "cedar/auxiliaries/FactoryManager.h"

CEDAR_PROC_EXPORT_SINGLETON(cedar::aux::FactoryManager<cedar::proc::experiment::action::ActionPtr>);

namespace cedar
{
  namespace proc
  {
    namespace experiment
    {
      namespace action
      {
        //!@brief The singleton instance of the instruction factory manager.
        typedef cedar::aux::Singleton< cedar::aux::FactoryManager<cedar::proc::experiment::action::ActionPtr>>
                ActionManagerSingleton;
      }
    }
  }
}

#endif // CEDAR_PROC_EXPERIMENT_ACTION_H

