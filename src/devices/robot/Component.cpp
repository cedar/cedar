/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        Component.cpp

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 11 08

 ----- Description: Header for the \em cedar::dev::Component class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "Component.h"
#include "Robot.h"
#include "ComponentNotAvailableException.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::dev::robot;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
Component::Component(void)
{
}

//! destructor
Component::~Component(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

const std::string& Component::getParentName(void) const
{
  return _mParentName;
}

bool Component::isComponentAvailable(const std::string& rComponentName) const
{
  return mpRobot->isComponentAvailable(rComponentName, _mName);
}

bool Component::areComponentsAvailable(const std::vector<std::string>& rComponents) const
{
  return mpRobot->areComponentsAvailable(rComponents, _mName);
}

unsigned int Component::getNumberOfComponents(void) const
{
  return mpRobot->getNumberOfComponents(_mName);
}

ComponentPtr& Component::getComponent(const std::string& rComponentName)
{
  if (!isComponentAvailable(rComponentName))
  {
    CEDAR_THROW(ComponentNotAvailableException,
      "Component with name \"" + rComponentName + "\" is not available from component \"" + _mName + "\".\n");
  }

  return mpRobot->getComponent(rComponentName);
}

const std::set<std::string>& Component::getComponentNames(void) const
{
  return mpRobot->getComponentNames(_mName);
}

ComponentPtr& Component::getParent(void)
{
  return mpRobot->getComponent(getParentName());
}
