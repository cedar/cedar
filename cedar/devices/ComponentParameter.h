/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ComponentParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 03 05

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_COMPONENT_PARAMETER_H
#define CEDAR_DEV_COMPONENT_PARAMETER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/namespace.h"
#include "cedar/auxiliaries/Parameter.h"

// SYSTEM INCLUDES
#include <string>


/*!@brief A parameter that holds a pointer to a component of a device.
 */
class cedar::dev::ComponentParameter : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::Parameter Super;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ComponentParameter(cedar::aux::Configurable* pOwner, const std::string& name);

  //!@brief Destructor
  virtual ~ComponentParameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  virtual void readFromNode(const cedar::aux::ConfigurationNode& node);

  virtual void writeToNode(cedar::aux::ConfigurationNode& root) const;

  virtual void makeDefault();

  cedar::dev::ComponentPtr getValue() const;

  void setValue(cedar::dev::ComponentSlotPtr component);

  void setValue(const std::string& componentPath);

  //! Returns the string-representation of the currently selected component.
  std::string getStringRepresentation() const;

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
  cedar::dev::ComponentSlotPtr mComponent;

}; // class cedar::dev::ComponentParameter

#endif // CEDAR_DEV_COMPONENT_PARAMETER_H

