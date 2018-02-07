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

    File:        ChannelDeclaration.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2014 12 02

    Description: Header file for the class cedar::dev::ChannelDeclaration.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_CHANNEL_DECLARATION_H
#define CEDAR_DEV_CHANNEL_DECLARATION_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/PluginDeclaration.h"
#include "cedar/auxiliaries/PluginDeclarationTemplate.h"

// FORWARD DECLARATIONS
#include "cedar/devices/ChannelDeclaration.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::dev::ChannelDeclaration : public cedar::aux::PluginDeclarationBaseTemplate<cedar::dev::ChannelPtr>
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ChannelDeclaration(const std::string& category, const std::string& className = "")
  :
  cedar::aux::PluginDeclarationBaseTemplate<cedar::dev::ChannelPtr>(category, className)
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
    return "Channel";
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

}; // class cedar::dev::ChannelDeclaration


template <class DerivedClass> // derived class will be filled in the plugins
class cedar::dev::ChannelDeclarationTemplate
:
// this is a bit of a weird mechanism that is here for downwards-compatibility
public cedar::aux::PluginDeclarationTemplate
<
cedar::dev::ChannelPtr, // class that the plugin derives from
boost::shared_ptr<DerivedClass>, // the actual class being declared (given by the template)
cedar::dev::ChannelDeclaration // the PluginDeclarationTemplate will inherit this
>
{
//--------------------------------------------------------------------------------------------------------------------
// nested types
//--------------------------------------------------------------------------------------------------------------------
private:
  typedef
  cedar::aux::PluginDeclarationTemplate
  <
    cedar::dev::ChannelPtr,
    boost::shared_ptr<DerivedClass>,
    cedar::dev::ChannelDeclaration
  >
  DeclarationSuper;
//--------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//--------------------------------------------------------------------------------------------------------------------
public:
ChannelDeclarationTemplate(const std::string& category, const std::string& classId = "")
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
  cedar::dev::ChannelManagerSingleton::getInstance()->
      registerType<boost::shared_ptr<DerivedClass> >();
}

}; // class cedar::dev::ChannelDeclarationTemplate


#endif // CEDAR_DEV_CHANNEL_DECLARATION_H

