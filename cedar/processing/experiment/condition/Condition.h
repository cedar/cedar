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

    File:        Condition.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2014 01 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_CONDITION_H
#define CEDAR_PROC_EXPERIMENT_CONDITION_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/ObjectParameterTemplate.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/Experiment.fwd.h"
#include "cedar/processing/experiment/condition/Condition.fwd.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>


/*!@brief An abstract class for all kinds of conditions.
 */
class cedar::proc::experiment::condition::Condition : public cedar::aux::Configurable
{
public:
	//!@brief a parameter for condition objects
	typedef cedar::aux::ObjectParameterTemplate<cedar::proc::experiment::condition::Condition> ConditionParameter;

	//!@cond SKIPPED_DOCUMENTATION
	CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(ConditionParameter);
	//!@endcond
private:

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The Constructor
  Condition();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! @brief Performs the actual check.
   *
   * @param skipIfFired If true, the condition will return false if it returned true once before.
   */
  bool runCheck(bool skipIfFired = true) const;

  /*! Resets the condition. If it is set to fire only once, this will allow it to fire again.
   */
  void reset();

  //! does this condition fire during initiation of a trial?
  virtual bool initialCheck() const;

  /*! Reimplement this to check the validity of the condition. Return true if the action is valid. If false is returned,
   *  an inforative message should be added to the @em errors vector.
   */
  virtual bool checkValidity(std::vector<std::string>& errors, std::vector<std::string>& warnings) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief This method has to be overriden by all derived classes
   *         It should return true if the condition is fulfilled
   */
  virtual bool check() const = 0;

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
  //! Member for storing if the condition has fired before.
  mutable bool mHasFired;

}; // class cedar::proc::experiment::Condition


#include "cedar/auxiliaries/FactoryManager.h"

CEDAR_PROC_EXPORT_SINGLETON(cedar::aux::FactoryManager<cedar::proc::experiment::condition::ConditionPtr>);

namespace cedar
{
  namespace proc
  {
    namespace experiment
    {
      namespace condition
      {
      //!@brief The singleton instance of the condition factory manager.
      typedef cedar::aux::Singleton< cedar::aux::FactoryManager<cedar::proc::experiment::condition::ConditionPtr>>
              ConditionManagerSingleton;
      }
    }
  }
}

#endif // CEDAR_PROC_EXPERIMENT_CONDITION_H

