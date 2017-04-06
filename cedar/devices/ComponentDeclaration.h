/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ComponentDeclaration.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2014 12 02

    Description: Header file for the class cedar::dev::ComponentDeclaration.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_COMPONENT_DECLARATION_H
#define CEDAR_DEV_COMPONENT_DECLARATION_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/Component.h"
#include "cedar/auxiliaries/PluginDeclaration.h"
#include "cedar/auxiliaries/PluginDeclarationTemplate.h"

// FORWARD DECLARATIONS
#include "cedar/devices/ComponentDeclaration.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::dev::ComponentDeclaration : public cedar::aux::PluginDeclarationBaseTemplate<cedar::dev::ComponentPtr>
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ComponentDeclaration(const std::string& category, const std::string& className = "")
  :
  cedar::aux::PluginDeclarationBaseTemplate<cedar::dev::ComponentPtr>(category, className)
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // this will be overridden in the templated version of the class below
  virtual void declare() const = 0;

  // this is mainly used to group together declarations when displaying them to the user
  std::string getPluginType() const
  {
    return "Component";
  }

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
  // none yet

}; // class cedar::dev::ComponentDeclaration


template <class DerivedClass> // derived class will be filled in the plugins
class cedar::dev::ComponentDeclarationTemplate
:
// this is a bit of a weird mechanism that is here for downwards-compatibility
public cedar::aux::PluginDeclarationTemplate
<
cedar::dev::ComponentPtr, // class that the plugin derives from
boost::shared_ptr<DerivedClass>, // the actual class being declared (given by the template)
cedar::dev::ComponentDeclaration // the PluginDeclarationTemplate will inherit this
>
{
//--------------------------------------------------------------------------------------------------------------------
// nested types
//--------------------------------------------------------------------------------------------------------------------
private:
  typedef
  cedar::aux::PluginDeclarationTemplate
  <
    cedar::dev::ComponentPtr,
    boost::shared_ptr<DerivedClass>,
    cedar::dev::ComponentDeclaration
  >
  DeclarationSuper;
//--------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//--------------------------------------------------------------------------------------------------------------------
public:
ComponentDeclarationTemplate(const std::string& category, const std::string& classId = "")
:
DeclarationSuper
(
  category,
  classId
)
{
}

void declare() const
{
  DeclarationSuper::declare();
}

}; // class cedar::dev::ComponentDeclarationTemplate


#endif // CEDAR_DEV_COMPONENT_DECLARATION_H

