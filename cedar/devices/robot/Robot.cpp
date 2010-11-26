/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        Robot.cpp

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 11 08

 ----- Description: Header for the @em cedar::dev::Robot class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "Robot.h"
#include "ComponentNotAvailableException.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/exceptions/ExceptionBase.h"

// SYSTEM INCLUDES
#include <map>
#include <string>
#include <set>

using namespace cedar::dev::robot;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
Robot::Robot(void)
{
}

//! destructor
Robot::~Robot(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

/*! If the component has not been initialized yet, it will be created using the abstract @em createComponent method.
 *  For that reason, the method is not const.
 */
ComponentPtr& Robot::getComponent(const std::string& rComponentName)
{
  // if the requested component is not available ..
  if(!isComponentAvailable(rComponentName))
  {
    // .. throw an exception
    CEDAR_THROW(cedar::aux::exc::ExceptionBase,
      "Component with name \"" + rComponentName + "\" does not exist in the robot \"" + _mName + "\".\n");
  }

  // if the component has not been initialized yet ..
  if (mComponents[rComponentName].use_count() == 0)
  {
    // .. and add it to the map of all components.
    mComponents[rComponentName] = createComponent(rComponentName);
  }

  return mComponents[rComponentName];
}

bool Robot::isComponentAvailable(const std::string& rComponentName) const
{
  // search for the component name in the map
  std::map<std::string, std::set<std::string> >::const_iterator component_names_it
    = _mSubComponentNames.find(rComponentName);

  // return whether the component name was found
  return component_names_it != _mSubComponentNames.end();
}

bool Robot::isComponentAvailable(
                                  const std::string& rComponentName,
                                  const std::string& rParentComponentName
                                ) const
{
  bool is_available = false;

  // search for the sub components of the parent component
  std::map< std::string, std::set<std::string> >::const_iterator parent_component_it
    = _mSubComponentNames.find(rParentComponentName);

  // if the parent component was not found ..
  if (parent_component_it == _mSubComponentNames.end())
  {
    // .. throw an exception.
    CEDAR_THROW(cedar::aux::exc::ExceptionBase,
      "Parent component with name \"" + rParentComponentName + "\" does not exist in the robot.\n");
  }

  // get the names of all subcomponents of the parent component
  std::set<std::string> sub_component_names = parent_component_it->second;

  // search for the subcomponent
  std::set<std::string>::const_iterator sub_component_name_it
    = sub_component_names.find(rComponentName);

  // check and save whether the component name was found
  if (sub_component_name_it != sub_component_names.end())
  {
    is_available = true;
  }

  return is_available;
}
