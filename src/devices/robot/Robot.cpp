/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        Robot.cpp

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 11 08

 ----- Description: Header for the \em cedar::dev::Robot class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "Robot.h"
#include "ComponentNotAvailableException.h"

// PROJECT INCLUDES

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

bool Robot::isComponentAvailable(const std::string& componentName) const
{
  std::map<std::string, std::set<std::string> >::const_iterator component_names_it;

  component_names_it = _mComponentNames.find(componentName);

  return component_names_it != _mComponentNames.end();
}

bool Robot::isComponentAvailable(
                                  const std::string& componentName,
                                  const std::string& parentComponentName
                                ) const
{
  bool isAvailable = false;

  std::map< std::string, std::set<std::string> >::const_iterator parent_component_it
    = _mComponentNames.find(parentComponentName);

  std::set<std::string> sub_component_names = parent_component_it->second;

  std::set<std::string>::const_iterator sub_component_name_it
    = sub_component_names.find(componentName);

  if (sub_component_name_it != sub_component_names.end())
  {
    isAvailable = true;
  }

  return isAvailable;
}

bool Robot::areComponentsAvailable(
                                    const std::vector<std::string>& components,
                                    const std::string& parentComponentName
                                  ) const
{
  bool all_components_available = true;

  for (unsigned int i = 0; i < components.size(); ++i)
  {
    if (!isComponentAvailable(components.at(i), parentComponentName))
    {
      all_components_available = false;
      i = components.size();
    }
  }

  return all_components_available;
}

unsigned int Robot::getNumberOfComponents(const std::string& parentComponentName) const
{
  if (!isComponentAvailable(parentComponentName))
  {
    CEDAR_THROW(ComponentNotAvailableException,
      "Component \"" + parentComponentName + "\" is not available in this robot.");
  }

  std::map<std::string, std::set<std::string> >::const_iterator component_it
    = _mComponentNames.find(parentComponentName);

  return component_it->second.size();
}

const std::set<std::string>& Robot::getComponentNames(const std::string& parentComponentName) const
{
  if (!isComponentAvailable(parentComponentName))
  {
    CEDAR_THROW(ComponentNotAvailableException,
      "Component \"" + parentComponentName + "\" is not available in this robot.");
  }

  std::map<std::string, std::set<std::string> >::const_iterator component_it
    = _mComponentNames.find(parentComponentName);

  return component_it->second;
}
